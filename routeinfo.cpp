#include "routeinfo.h"

RouteInfo::RouteInfo(unsigned int truckId, std::vector<unsigned int> customersId, bool valid) :
		truck(truckId)
	  , customers(customersId)
	  , isValid(valid)
{
}
