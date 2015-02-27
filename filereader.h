#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

#include "truck.h"
#include "customer.h"
#include "customerPairInfo.h"
#include "velocitycalculator.h"

class FileReader
{
public:
	explicit FileReader(std::string const &folder = "");
	std::vector<Truck> getTrucks();
	std::vector<Customer> getCustomers();
	std::shared_ptr<std::unordered_map<Pair, double, PairHash> > getDistances();
	std::shared_ptr<std::unordered_map<Pair, double, PairHash> > getTime();
	VelocityCalculator getVelocityRestrictions();

private:
	std::string mFolder;
	std::shared_ptr<std::unordered_map<Pair, double, PairHash> > getMatrix(const std::string &fileName);
};
