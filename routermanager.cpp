#include <algorithm>
#include <iostream>
#include <memory>
#include <thread>

#include "routermanager.h"
#include "adopter.h"

RouterManager::RouterManager(const std::string &folder) :
	mFileReader(folder)
  ,  mFinished(false)
  , mProgress(new Progress())
{
#ifdef WITH_LOG
	std::cout << folder << std::endl;
#endif
	mDistances = mFileReader.getDistances();
	mVelocityCalc = mFileReader.getVelocityRestrictions();
	mRestrictionChecker = std::shared_ptr<RestrictionChecker>(new RestrictionChecker(mFileReader.getTrucks(), mFileReader.getTime()));
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
	std::vector<Truck> trucks = mFileReader.getTrucks();
#ifdef WITH_LOG
	std::cout << "number of trucks " << trucks.size() << std::endl;
#endif
	std::vector<Customer> customers = mFileReader.getCustomers();
	std::unordered_set<std::shared_ptr<Route>> initialRoutes = customersToRoutes(customers);
	std::unordered_set<std::shared_ptr<Route>> badRoutes = mRestrictionChecker->initByRoutes(initialRoutes);
	for (std::shared_ptr<Route> const &badRoute: badRoutes)
	{
		initialRoutes.erase(badRoute);
		mRoutesNoTruck.push_back(badRoute);
	}
	mClarkeWrightAlgorithm = ClarkeWrightAlgorithm(mDistances, std::shared_ptr<RestrictionChecker>(mRestrictionChecker), mProgress);
#ifdef WITH_LOG
	std::cout << "create clark wright ";
#endif
	std::unordered_set<std::shared_ptr<Route> > routes = mClarkeWrightAlgorithm.calculate(initialRoutes);
	std::vector<std::vector<int>> graph;
	std::unordered_map<int, std::shared_ptr<Route>> routesInGraph;
	for (std::shared_ptr<Route> const &route: routes)
	{
		std::vector<int> possible;
		PassTimeInfo info = mRestrictionChecker->getTimeInfo(route);

		for (unsigned int j = 0; j < trucks.size(); j ++)
		{
			if (mRestrictionChecker->isSatisfy(route, info, trucks[j]))
			{
				possible.push_back(j);
			}
		}
		graph.push_back(possible);
		routesInGraph.insert(std::pair<int, std::shared_ptr<Route>>(graph.size() - 1, route));
	}
	std::vector<int> matching = getMatching(graph, trucks.size());
	for (unsigned int i = 0; i < matching.size(); i ++)
	{
		if (matching[i] < 0)
		{
			continue;
		}
		unsigned int id = trucks[i].id;
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
	for (auto pair: mTruckRoute)
	{
		double truckWeight = 0;
		for (Truck const &truck: trucks)
		{
			if (pair.first == truck.id)
			{
				truckWeight = truck.capacity.weightCapacity;
			}
		}
		sum += routeLength(pair.second->customerIds());
		std::cout << "\n TRUCK ID IS " << pair.first << std::endl;
		std::cout << " max route weight " << pair.second->maxWeight() << std::endl;
		std::cout << " truck max weight " << truckWeight << std::endl;
		std::cout << " max route Volume " << pair.second->maxVolume() << std::endl;
		std::cout << "number of customers in route is " << pair.second->allCustomers().size() << " total length "
				  << routeLength(pair.second->customerIds()) << std::endl;
		std::cout << "route zone is " << pair.second->getZone() <<std::endl;
	}
	std::cout << "number of used trucks " << mUsedTrucks.size() << std::endl;
	std::cout << "number of routes without trucks is " << mRoutesNoTruck.size() << std::endl;
	for (unsigned int i = 0; i < mRoutesNoTruck.size(); i ++)
	{
		sum += routeLength(mRoutesNoTruck[i]->customerIds());
		std::cout << "\n number of customers in route without truck is " << mRoutesNoTruck[i]->allCustomers().size() << std::endl;
	}
	std::cout << "sum length " << sum << std::endl;
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
	return routes;
}
