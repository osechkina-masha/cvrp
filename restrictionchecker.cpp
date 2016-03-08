#include "restrictionchecker.h"
#include "adopter.h"

#include <algorithm>
#include <iostream>

bool compareVectors(std::vector<int> const &v1, std::vector<int> const &v2)
{
	if (v1.size() < v2.size())
		return true;
	if (v1.size() > v2.size())
		return false;
	for (unsigned int i= 0; i < v1.size(); i ++)
	{
		if (v1[i] < v2[i])
			return true;
		if (v1[i] > v2[i])
			return false;
	}
	return false;
}

bool RouteWithTrucks::operator <(const RouteWithTrucks &other) const
{
	return compareVectors(truckNums, other.truckNums);
}

RestrictionChecker::RestrictionChecker()
{
}

RestrictionChecker::RestrictionChecker(const std::vector<Truck> &trucks,
			const std::shared_ptr<std::unordered_map<Pair, time_type, PairHash> > &time, double maxWaitTime) :
		mTime(time)
	  , mTrucks(trucks)
	  , mMaxWaitTime(maxWaitTime)
{
}

std::unordered_set<std::shared_ptr<Route>> RestrictionChecker::initByRoutes(const std::unordered_set<std::shared_ptr<Route>> &routes)
{
	mValidRelations.clear();
	std::unordered_set<std::shared_ptr<Route>> badRoutes;
	mRoutes = routes;
	std::vector<RouteWithTrucks> graph;
	for (std::shared_ptr<Route> const &route: routes)
	{
		std::vector<int> possibleTrucks;
		PassTimeInfo info = getTimeInfo(route);
		for (unsigned int i = 0; i < mTrucks.size(); i ++)
		{
			if (isSatisfy(route, info, mTrucks[i]))
			{
				possibleTrucks.push_back(i);
			}
		}
		if (possibleTrucks.empty())
		{
			std::cout << "CANT ASSIGN ANY TRUCK TO CUSTOMER!!!!! " << route->allCustomers()[0].id << std::endl;
			badRoutes.insert(route);
			mRoutes.erase(route);
			continue;
		}
		std::sort(possibleTrucks.begin(), possibleTrucks.end());
		graph.push_back(RouteWithTrucks(route, possibleTrucks));
	}
	bool changed;
	updateMaxValidRelations(graph, changed);
	return badRoutes;
}

bool RestrictionChecker::checkAndUpdateRestrictions(const std::shared_ptr<Route> &newRoute,
								const std::shared_ptr<Route> &removed1, const std::shared_ptr<Route> &removed2)
{
	std::vector<RouteWithTrucks> graph = createGraph(newRoute, removed1, removed2);
	bool changed = true;
	if (!updateMaxValidRelations(graph, changed))
	{
#ifdef WITH_LOG
		std::cout << "bad truck relation!!! " << std::endl;
#endif
		return false;
	}
	if (/*!changed &&*/ mConflictRoutes.find(newRoute) != mConflictRoutes.end())
	{
			return false;
	}
	mRoutes.erase(removed1);
	mRoutes.erase(removed2);
	mRoutes.insert(newRoute);
	return true;
}

void RestrictionChecker::setWaitTimeParams(double newWaitTime)
{
	mMaxWaitTime = newWaitTime;
}

bool RestrictionChecker::updateMaxValidRelations(const std::vector<RouteWithTrucks> &curGraph, bool &changed)
{
	mConflictRoutes.clear();
	std::vector<RouteWithTrucks> graph(curGraph);
	std::sort(graph.begin(), graph.end());
	unsigned int count = 0;
	std::vector<std::vector<int>> workGraph;
	while (count < graph.size() && count < mValidRelations.size() && mValidRelations[count] == count + 1)
	{
		workGraph.push_back(graph[count].truckNums);
		count ++;
	}
	unsigned int minMatchingSize = getMaxMatchingSize(workGraph, mTrucks.size());
	if (minMatchingSize < workGraph.size())
	{
		return false;
	}
	bool betterSolution = false;
	std::vector<unsigned int> newValidRelations(mValidRelations.begin(), mValidRelations.begin() + count);
	for (unsigned int i = count;
		 //i < mTrucks.size(); i ++)
		 i < graph.size(); i ++)
	{
		workGraph.push_back(graph[i].truckNums);
		unsigned int matchingSize = getMaxMatchingSize(workGraph, mTrucks.size());
		if (mValidRelations.size() > i && !betterSolution && matchingSize < mValidRelations[i])
		{
			return false;
		}
		newValidRelations.push_back(matchingSize);
		if (mValidRelations.size() <= i || matchingSize > mValidRelations[i])
		{
			betterSolution = true;
		}
	}
	if (betterSolution)
	{
		mValidRelations = newValidRelations;
	}
#ifdef WITH_LOG
	std::cout << "valid relations vector " << std::endl;
	for (unsigned int i = 0; i < mValidRelations.size(); i ++)
	{
		std::cout << mValidRelations[i] << " ";
	}
	std::cout << "\n";
#endif
	changed = betterSolution;
	for (unsigned int i = 0; i < graph.size(); i ++)
	{
		if (newValidRelations[i] >= mTrucks.size() || newValidRelations[i] == newValidRelations.back())
		{
			break;
		}
		if ((i > 0 && newValidRelations[i - 1] == newValidRelations[i]) ||
				(i + 1 < newValidRelations.size() && newValidRelations[i] == newValidRelations[i + 1]))
		{
			mConflictRoutes.insert(graph[i].route);
		}
	}
//	std::cout << " size of conflict routes is " << mConflictRoutes.size() << std::endl;
	return true;
}

bool RestrictionChecker::isSatisfy(const std::shared_ptr<Route> &route, const PassTimeInfo &info, const Truck &truck)
{
	return truck.capacity.weightCapacity >= route->maxWeight();
//    return (truck.finishTime - truck.startTime >= info.passTime)
//            && (info.maxBeginTime >= truck.startTime)
//            && (info.minBeginTime + info.passTime <= truck.finishTime)
//            && info.isTWOk
//            && (route->maxVolume() <= truck.capacity.volumeCapacity)
//            && (route->maxWeight() <= truck.capacity.weightCapacity)
//            && ((route->getZone() & truck.zone) == route->getZone())
//            && ((route->getLoadType() & truck.loadType) == route->getLoadType())
//            && info.pointNumber <= truck.maxClients;
}

PassTimeInfo RestrictionChecker::getTimeInfo(const std::shared_ptr<Route> &route)
{
	std::vector<Customer> customers = route->allCustomers();
	if (customers.empty() || !mTime)
	{
		return PassTimeInfo(0, 0, 0, 0);
	}
	double timeToFirst = mTime->operator [](Pair(0, customers[0].id));
	std::time_t minLastTime = customers[0].timeRestriction.arrivalTime + customers[0].timeRestriction.loadTime;
	std::time_t maxLastTime = customers[0].timeRestriction.departureTime + customers[0].timeRestriction.loadTime;
	std::time_t serviceTime = timeToFirst;
	unsigned int pointNum = 1;
	for (unsigned int i = 1; i < customers.size(); i ++)
	{
		Customer const & curCustomer = customers[i];
		int id1 = customers[i - 1].id;
		int id2 = customers[i].id;
		double time = mTime->operator [](Pair(id1, id2));
		if (time != 0)
		{
			pointNum ++;
		}
		minLastTime += time;
		maxLastTime += time;
		if (!curCustomer.hasTimeRestrictions())
		{
#ifdef WITH_LOG
			std::cout << "hasnt time restrictions!!! \n";
#endif
			continue;
		}
		if (curCustomer.timeRestriction.arrivalTime - maxLastTime > mMaxWaitTime || curCustomer.timeRestriction.departureTime < minLastTime)
		{
			return PassTimeInfo::getUnsatisfiedInfo();
		}
		serviceTime += curCustomer.timeRestriction.loadTime + time;
		if (curCustomer.timeRestriction.arrivalTime - maxLastTime > 0) //if already was waiting????
		{
			serviceTime += curCustomer.timeRestriction.arrivalTime - maxLastTime;
			maxLastTime = curCustomer.timeRestriction.arrivalTime;
		}
		else
		{
			maxLastTime = std::min(maxLastTime, curCustomer.timeRestriction.departureTime);
		}
		//if we waiting if arrive early and doesnt waiting else?
		minLastTime = std::max(minLastTime, curCustomer.timeRestriction.arrivalTime);
		minLastTime += curCustomer.timeRestriction.loadTime;
		maxLastTime += curCustomer.timeRestriction.loadTime;
	}
	std::time_t minBeginingTime = minLastTime - serviceTime;
	std::time_t maxBeginingTime = maxLastTime - serviceTime;
	serviceTime += mTime->operator [](Pair(customers.back().id, 0));
	return PassTimeInfo(minBeginingTime, maxBeginingTime, serviceTime, pointNum);
}

std::unordered_set<std::shared_ptr<Route> > RestrictionChecker::getMaxRoutesWithTrucks(const std::unordered_set<std::shared_ptr<Route> > &routes)
{
	std::vector<RouteWithTrucks> graph;
	for (std::shared_ptr<Route> const &route: routes)
	{
		std::vector<int> possibleTrucks;
		PassTimeInfo info = getTimeInfo(route);
		for (unsigned int i = 0; i < mTrucks.size(); i ++)
		{
			if (isSatisfy(route, info, mTrucks[i]))
			{
				possibleTrucks.push_back(i);
			}
		}
		if (possibleTrucks.empty())
		{
			continue;
		}
		std::sort(possibleTrucks.begin(), possibleTrucks.end());
		graph.push_back(RouteWithTrucks(route, possibleTrucks));
	}
	std::sort(graph.begin(), graph.end());
	std::unordered_set<std::shared_ptr<Route>> routesWithTrucks;
	std::vector<std::vector<int>> curGraph;
	for (unsigned int i = 0; i < graph.size(); i ++)
	{
		curGraph.push_back(graph[i].truckNums);
		unsigned int curSize = getMaxMatchingSize(curGraph, mTrucks.size());
		if (curSize < curGraph.size())
		{
			break;
		}
		routesWithTrucks.insert(graph[i].route);
	}
	return routesWithTrucks;
}

bool RestrictionChecker::reinit(const std::vector<Truck> &validTrucks, const std::unordered_set<std::shared_ptr<Route> > &routes)
{
	mTrucks = validTrucks;
	return initByRoutes(routes).empty();
}

std::vector<RouteWithTrucks> RestrictionChecker::createGraph(const std::shared_ptr<Route> &newRoute, const std::shared_ptr<Route> &removed1, const std::shared_ptr<Route> &removed2)
{
	std::vector<RouteWithTrucks> graph;
	std::vector<int> possibleTrucksNew;
	PassTimeInfo infoNew = getTimeInfo(newRoute);
	for (unsigned int i = 0; i < mTrucks.size(); i ++)
	{
		if (isSatisfy(newRoute, infoNew, mTrucks[i]))
		{
			possibleTrucksNew.push_back(i);
		}
	}
	std::sort(possibleTrucksNew.begin(), possibleTrucksNew.end());
	graph.push_back(RouteWithTrucks(newRoute, possibleTrucksNew));
	for (std::shared_ptr<Route> const &route: mRoutes)
	{
		if (route == removed1 || route == removed2)
		{
			continue;
		}
		std::vector<int> possibleTrucks;
		PassTimeInfo info = getTimeInfo(route);
		for (unsigned int i = 0; i < mTrucks.size(); i ++)
		{
			if (isSatisfy(route, info, mTrucks[i]))
			{
				possibleTrucks.push_back(i);
			}
		}
		std::sort(possibleTrucks.begin(), possibleTrucks.end());
		graph.push_back(RouteWithTrucks(route, possibleTrucks));
	}
	return graph;
}
