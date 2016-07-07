#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <fstream>

#include "filereader.h"
#include "clarkewrightalgorithm.h"
#include "routeinfo.h"
#include "samplesparser.h"

class RouterManager
{
public:
    explicit RouterManager(std::string const &folder);
    explicit RouterManager(std::vector<std::string> const &data, std::ofstream& stream);
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
    SamplesParser mSamplesParser;
    std::vector<Customer> mCustomers;
    std::vector<Truck> mTrucks;
    std::ofstream& mStream;
};
