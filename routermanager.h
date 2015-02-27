#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "filereader.h"
#include "clarkewrightalgorithm.h"
#include "routeinfo.h"

class RouterManager
{
public:
	RouterManager(std::string const &folder);
	void calculate();
	std::vector<unsigned int> getUsedTrucks();
	std::vector<unsigned int> getRouteByTruck(unsigned int truckId);
	int routeNoTruckNum();
	std::vector<unsigned int> getRouteNoTruck(int index);
	double routeLengthByTruck(unsigned int truckId);
	double routeLength(std::vector<unsigned int> const &customerIds);
	double routeBeginTimeByTruck(unsigned int truckId);
	double routeEndTimeByTruck(unsigned int truckId);
	bool isFinished();
	void operator () ();
	int getProgress();

private:
	static std::unordered_set<std::shared_ptr<Route>> customersToRoutes(std::vector<Customer> const &customers);
	FileReader mFileReader;
	ClarkeWrightAlgorithm mClarkeWrightAlgorithm;
	std::shared_ptr<RestrictionChecker> mRestrictionChecker;
	std::unordered_map<unsigned int, std::shared_ptr<Route> > mTruckRoute;
	std::vector<unsigned int> mUsedTrucks;
	std::shared_ptr<std::unordered_map<Pair, double, PairHash>> mDistances;
	bool mFinished;
	VelocityCalculator mVelocityCalc;
	std::vector<std::shared_ptr<Route>> mRoutesNoTruck;
	std::shared_ptr<Progress> mProgress;
};
