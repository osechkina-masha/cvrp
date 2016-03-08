#pragma once
#include <string>
#include "vector"
#include <unordered_map>
#include <memory>
#include "customerPairInfo.h"
#include "truck.h"
#include "customer.h"

class SamplesParser
{
public:
	SamplesParser(std::vector<std::string> const & filesPath);
	SamplesParser();
	std::shared_ptr<std::unordered_map<Pair, double, PairHash>> getDistances() const;
	std::vector<Customer> getCustomers() const;
	std::vector<Truck> getTrucks() const;

private:
	void parseFiles(std::vector<std::string> const & filesPath);
	std::vector<Customer> mCustomers;
	std::vector<Truck> mTrucks;
	std::shared_ptr<std::unordered_map<Pair, double, PairHash>> mDistances;
};
