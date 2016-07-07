#include <algorithm>
#include <iostream>
#include <memory>
#include <thread>

#include "routermanager.h"
#include "adopter.h"

static std::ofstream STR;

RouterManager::RouterManager(const std::string &folder) :
	mFileReader(folder)
  , mFinished(false)
  , mProgress(new Progress())
  , mStream(STR)
{
#ifdef WITH_LOG
	std::cout << folder << std::endl;
#endif
	mDistances = mFileReader.getDistances();
	mVelocityCalc = mFileReader.getVelocityRestrictions();
	mRestrictionChecker = std::shared_ptr<RestrictionChecker>(
				new RestrictionChecker(mFileReader.getTrucks(), mFileReader.getTime()));
	mCustomers = mFileReader.getCustomers();
	mTrucks = mFileReader.getTrucks();
}

RouterManager::RouterManager(const std::vector<std::string> &data, std::ofstream &stream) :
	  mFinished(false)
	, mProgress(new Progress())
	, mSamplesParser(data)
    , mStream(stream)
{
	mDistances = mSamplesParser.getDistances();
	mCustomers = mSamplesParser.getCustomers();
	mTrucks = mSamplesParser.getTrucks();
	mRestrictionChecker = std::shared_ptr<RestrictionChecker>(
				new RestrictionChecker(mTrucks));

}

void RouterManager::calculate()
{
	mFinished = false;
#ifdef WITH_DEBUG
	operator ()();
#else
	std::thread calculateThread(std::ref(*this));
	calculateThread.detach();
#endif
}

std::vector<unsigned int> RouterManager::getUsedTrucks()
{
	return mUsedTrucks;
}

std::vector<unsigned int> RouterManager::getRouteByTruck(unsigned int truckId)
{
	return mTruckRoute.at(truckId)->customerIds();
}

int RouterManager::routeNoTruckNum()
{
	return mRoutesNoTruck.size();
}

std::vector<unsigned int> RouterManager::getRouteNoTruck(int index)
{
	return mRoutesNoTruck[index]->customerIds();
}

double RouterManager::routeLengthByTruck(unsigned int truckId)
{
	return routeLength(mTruckRoute.at(truckId)->customerIds());
}

double RouterManager::routeLength(const std::vector<unsigned int> &customerIds)
{
	if (customerIds.empty())
	{
		return 0;
	}
	double totalDist = mDistances->at(Pair(0, customerIds[0])) + mDistances->at(Pair(customerIds.back(), 0));
	for (unsigned int i = 1; i < customerIds.size(); i ++)
	{
		totalDist += mDistances->at(Pair(customerIds[i - 1], customerIds[i]));
	}
	return totalDist;
}

double RouterManager::routeBeginTimeByTruck(unsigned int truckId)
{
	if (mTruckRoute.at(truckId)->allCustomers().empty())
	{
		return 0;
	}
	Customer customer = mTruckRoute.at(truckId)->allCustomers()[0];
	double time = customer.timeRestriction.arrivalTime;
	double dist = mDistances->at(Pair(0, customer.id));
	return time - mVelocityCalc.getTime(dist);
}

double RouterManager::routeEndTimeByTruck(unsigned int truckId)
{
	std::vector<Customer> const &customers = mTruckRoute.at(truckId)->allCustomers();
	if (customers.empty())
	{
		return 0;
	}
	Customer curCustomer = customers.at(0);
	time_t time = curCustomer.timeRestriction.arrivalTime + curCustomer.timeRestriction.loadTime;
	for (unsigned int i = 1; i < customers.size(); i ++)
	{
		curCustomer = customers[i];
		time += mVelocityCalc.getTime(mDistances->at(Pair(customers[i - 1].id, curCustomer.id)));
		time = std::max(curCustomer.timeRestriction.arrivalTime, time);
		time += curCustomer.timeRestriction.loadTime;
	}
	time += mVelocityCalc.getTime(mDistances->at(Pair(customers.back().id, 0)));
	return time;
}

bool RouterManager::isFinished()
{
	return mFinished;
}

void RouterManager::operator ()()
{
	mUsedTrucks.clear();
	mRoutesNoTruck.clear();
	mTruckRoute.clear();

#ifdef WITH_LOG
	std::cout << "number of mTrucks " << mTrucks.size() << std::endl;
#endif

	std::unordered_set<std::shared_ptr<Route>> initialRoutes = customersToRoutes(mCustomers);
	std::unordered_set<std::shared_ptr<Route>> badRoutes = mRestrictionChecker->initByRoutes(initialRoutes);
	for (std::shared_ptr<Route> const &badRoute: badRoutes)
	{
		initialRoutes.erase(badRoute);
		mRoutesNoTruck.push_back(badRoute);
	}
	mClarkeWrightAlgorithm = ClarkeWrightAlgorithm(mDistances, std::shared_ptr<RestrictionChecker>(mRestrictionChecker), mProgress);
#ifdef WITH_LOG
	std::cout << "create clark wright " << std::endl;
#endif
	std::unordered_set<std::shared_ptr<Route> > routes = mClarkeWrightAlgorithm.calculate(initialRoutes);
	unsigned int size1 = routes.size();
	if (routes.size() > mTrucks.size())
	{
		routes = mClarkeWrightAlgorithm.calculate(routes);
	}
#if WITH_LOG
	if (size1 > routes.size())
	{
		std::cout << "routes num became better " << std::endl;
	}
	else
	{
		std::cout << "routes num the same" << std::endl;
	}
#endif
	std::vector<std::vector<int>> graph;
	std::unordered_map<int, std::shared_ptr<Route>> routesInGraph;
	for (std::shared_ptr<Route> const &route: routes)
	{
		std::vector<int> possible;
		PassTimeInfo info = mRestrictionChecker->getTimeInfo(route);
		for (unsigned int j = 0; j < mTrucks.size(); j ++)
		{
			if (mRestrictionChecker->isSatisfy(route, info, mTrucks[j]))
			{
				possible.push_back(j);
			}
		}
		graph.push_back(possible);
		routesInGraph.insert(std::pair<int, std::shared_ptr<Route>>(graph.size() - 1, route));
	}
#ifdef WITH_LOG
	std::cout << "finish check routes \n";
#endif
	std::vector<int> matching = getMatching(graph, mTrucks.size());
#ifdef WITH_LOG
	std::cout << "got matching \n";
#endif
	for (unsigned int i = 0; i < matching.size(); i ++)
	{
		if (matching[i] < 0)
		{
			continue;
		}
		unsigned int id = mTrucks[i].id;
		mUsedTrucks.push_back(id);
		mTruckRoute[id] = routesInGraph[matching[i]];
		routes.erase(routesInGraph[matching[i]]);
	}
	for (auto routeInGraph: routes)
	{
		mRoutesNoTruck.push_back(routeInGraph);
	}

#ifdef WITH_LOG
	double sum = 0;
    double s1 = 0;
    double s2 = 0;
    double s3 = 0;
    double avLength = 0;
    double maxLength = 0;
		for (Customer const &cust : mCustomers)
    {
        if (cust.zone == 0)
        {
            s3 += cust.package.weight;
        }
        else if (cust.zone == 1)
        {
            s1 += cust.package.weight;
        }
        else if (cust.zone == 3)
        {
            s2 += cust.package.weight;
        }
		avLength += mDistances->at(Pair(0, cust.id));
		maxLength = std::max(maxLength, mDistances->at(Pair(0, cust.id)));
    }
    //std::cout
        mStream
            << "sum weight in zone 2 is " << s2 << std::endl;
    //std::cout
        mStream
            << "sum weight in zone 1 is " << s1 << std::endl;
    //std::cout
        mStream
            << "sum weight in zone 3 is " << s3 << std::endl;
    //std::cout
        mStream
            << "average distance is " << avLength / mCustomers.size() << " max dist " << maxLength << std::endl;
	for (auto pair: mTruckRoute)
	{
		double truckWeight = 0;
		for (Truck const &truck: mTrucks)
		{
			if (pair.first == truck.id)
			{
				truckWeight = truck.capacity.weightCapacity;
			}
		}
		sum += routeLength(pair.second->customerIds());
        //std::cout
        mStream
                << "\n TRUCK ID IS " << pair.first << std::endl;
        //std::cout
        mStream
                << " max route weight " << pair.second->maxWeight() << std::endl;
        //std::cout
        mStream
                << " truck max weight " << truckWeight << std::endl;
        //std::cout
        mStream
                << " max route Volume " << pair.second->maxVolume() << std::endl;
        //std::cout
        mStream
                << "number of customers in route is " << pair.second->allCustomers().size() << " total length "
                  << routeLength(pair.second->customerIds()) << std::endl;
        //std::cout
        mStream
                << "route zone is " << pair.second->getZone() <<std::endl;
	}
    //std::cout
    mStream
            << "number of used mTrucks " << mUsedTrucks.size() << std::endl;
    //std::cout
    mStream
            << "number of routes without mTrucks is " << mRoutesNoTruck.size() << std::endl;
	for (unsigned int i = 0; i < mRoutesNoTruck.size(); i ++)
	{
		sum += routeLength(mRoutesNoTruck[i]->customerIds());
        //std::cout
        mStream
                << "\n number of customers in route without truck is " << mRoutesNoTruck[i]->allCustomers().size() << std::endl;
	}
    //std::cout
    mStream
            << "sum length " << sum << std::endl;
#endif
	mFinished = true;
}

int RouterManager::getProgress()
{
	return mProgress->getProgress();
}

std::unordered_set<std::shared_ptr<Route> > RouterManager::customersToRoutes(const std::vector<Customer> &customers)
{
	std::unordered_set<std::shared_ptr<Route>> routes;
	std::unordered_map<unsigned int, Customer> pairCustomers;
	for (Customer const &curCustomer: customers)
	{
		if (curCustomer.type == Customer::Reception)
		{
#ifdef WITH_LOG
			std::cout << "insert to map customer with id " << curCustomer.id << std::endl;
#endif
			std::pair<unsigned int, Customer> pair(curCustomer.id, curCustomer);
			pairCustomers.insert(pair);
		}
	}
	for (Customer const &curCustomer: customers)
	{
		if (curCustomer.type == Customer::DeliveryFromCustomer)
		{
#ifdef WITH_LOG
			std::cout << "relation package " << curCustomer.id << std::endl;
#endif
			if (pairCustomers.find(curCustomer.relationCustomerID) == pairCustomers.end())
			{
#ifdef WITH_LOG
				std::cout << "CANT FIND RELATION ID!!! " << curCustomer.relationCustomerID << std::endl;
#endif
				continue;
			}
			Customer pairCustomer = pairCustomers.at(curCustomer.relationCustomerID);
			pairCustomers.erase(curCustomer.relationCustomerID);
			std::shared_ptr<Route> route = std::shared_ptr<Route>(new Route(curCustomer, pairCustomer));
			routes.insert(std::shared_ptr<Route>(route));
		}
		else if (curCustomer.type == Customer::DeliveryFromDepot)
		{
			std::shared_ptr<Route> route(new Route(curCustomer));
			routes.insert(route);
		}
	}
	for (auto const &customer: pairCustomers)
	{
		Customer const &curCustomer = customer.second;
		std::shared_ptr<Route> route(new Route(curCustomer));
		routes.insert(route);
	}
#ifdef WITH_LOG
	std::cout << "converted customers to routes" << std::endl;
#endif
	return routes;
}
