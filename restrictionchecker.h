#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>

#include "truck.h"
#include "route.h"
#include "velocitycalculator.h"

struct TruckInfo
{
	TruckInfo(Capacity const &curCapacity, int curNum = 0) :
			capacity(curCapacity)
		  , num(curNum)
		  , canOverload(false)
	{

	}

	Capacity capacity;
	std::vector<Truck> trucks;
	int num; // number of trucks with the same capacity
	bool canOverload;
};

struct PassTimeInfo
{
	PassTimeInfo(double curMinBeginTime, double curMaxBeginTime, double curPassTime, unsigned int curPointNum) :
		minBeginTime(curMinBeginTime)
	  , maxBeginTime(curMaxBeginTime)
	  , passTime(curPassTime)
	  , isTWOk(true)
	  , pointNumber(curPointNum)
	{
	}
	double minBeginTime;
	double maxBeginTime;
	double passTime;
	bool isTWOk;
	unsigned int pointNumber;
	static PassTimeInfo getUnsatisfiedInfo()
	{
		return PassTimeInfo();
	}
private:
	PassTimeInfo() :
		minBeginTime(-1.0)
	  , maxBeginTime(-1.0)
	  , passTime(-1.0)
	  , isTWOk(false)
	{
	}

};

typedef double time_type;

bool compareVectors(std::vector<int> const &v1, std::vector<int> const &v2);

struct RouteWithTrucks
{
	RouteWithTrucks(std::shared_ptr<Route> const &curRoute, std::vector<int> const &curTruckNums) :
		route(curRoute)
	  , truckNums(curTruckNums)
	{

	}
	std::shared_ptr<Route> route;
	std::vector<int> truckNums;
	bool operator < (RouteWithTrucks const &other) const;
};


class RestrictionChecker
{
public:
	RestrictionChecker();
	RestrictionChecker(std::vector<Truck> const &trucks, std::shared_ptr<std::unordered_map<Pair, time_type, PairHash> > const &time = NULL, double maxWaitTime = 15.0 * 60.0);
	std::unordered_set<std::shared_ptr<Route>> initByRoutes(const std::unordered_set<std::shared_ptr<Route>> &routes);
	bool checkAndUpdateRestrictions(std::shared_ptr<Route> const &newRoute, std::shared_ptr<Route> const &removed1, std::shared_ptr<Route> const &removed2);
	void setWaitTimeParams(double newWaitTime);
	bool isSatisfy(const std::shared_ptr<Route> &route, const PassTimeInfo &info, const Truck &truck);
	PassTimeInfo getTimeInfo(std::shared_ptr<Route> const &route);
	std::unordered_set<std::shared_ptr<Route>> getMaxRoutesWithTrucks(std::unordered_set<std::shared_ptr<Route>> const &routes);
	bool reinit(std::vector<Truck> const &validTrucks, std::unordered_set<std::shared_ptr<Route>> const &routes);

private:
	std::vector<RouteWithTrucks> createGraph(std::shared_ptr<Route> const &newRoute, std::shared_ptr<Route> const &removed1, std::shared_ptr<Route> const &removed2);
	bool updateMaxValidRelations(const std::vector<RouteWithTrucks> &curGraph, bool &changed); // graph is already sorted!!!
	std::shared_ptr<std::unordered_map<Pair, time_type, PairHash> > mTime;
	std::vector<Truck> mTrucks;
	std::unordered_set<std::shared_ptr<Route>> mRoutes;
	double mMaxWaitTime;
	std::vector<unsigned int> mValidRelations;
	std::unordered_set<std::shared_ptr<Route>> mConflictRoutes;
};
