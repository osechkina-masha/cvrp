#pragma once

#include <vector>

struct RouteInfo
{
public:
	RouteInfo(unsigned int truckId, std::vector<unsigned int> customersId, bool valid = true);
	unsigned int truck;
	std::vector<unsigned int> customers;
	bool isValid;
};

