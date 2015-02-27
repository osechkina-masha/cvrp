#include "customer.h"
#include "adopter.h"

#include<iostream>
#include <regex>

Customer::Customer(unsigned int curId, Package const &curPackage,
	CustomerType curType, unsigned int curRelationID, int curZone, int curLoadType) :
	id (curId)
  , relationCustomerID(curRelationID)
  , type(curType)
  , package(curPackage)
  , timeRestriction()
  , zone(curZone)
  , loadType(curLoadType)
{
}

Customer::Customer(unsigned int curId, Package const &curPackage,
	TimeWindow const &curTimeRestriction, CustomerType curType, unsigned int curRelationID, int curZone, int curLoadType) :
	id (curId)
  , relationCustomerID(curRelationID)
  , type(curType)
  , package(curPackage)
  , timeRestriction(curTimeRestriction)
  , zone(curZone)
  , loadType(curLoadType)
{
#ifdef WITH_LOG
	std::cout << "package size is " << package.weight << " volume is " << package.volume << std::endl;
#endif

}

bool Customer::hasTimeRestrictions() const
{
	return timeRestriction.isValid();
}

bool Customer::operator == (Customer const &customer) const
{
	return id == customer.id;
}

bool Customer::operator != (Customer const &customer) const
{
	return ! operator ==(customer);
}

Customer Customer::parse(std::string const &str)
{
	std::regex truckRegex("(\\d+),(R|D),(\\d+)*,(\\d+(.\\d+)*),(\\d+(.\\d+)*),(\\d+:\\d+:\\d+),(\\d+:\\d+:\\d+),(\\d+:\\d+:\\d+),([0-1](\\|[0-1])*),([0-1](\\|[0-1])*)");
	std::smatch truckMatch;
	if (!std::regex_match(str, truckMatch, truckRegex))
	{
	}
	unsigned int curId = std::stoi(truckMatch[1].str());
	unsigned int relationId = 0;
	Customer::CustomerType cusType = Customer::DeliveryFromDepot;
	std::string typeStr = truckMatch[2].str();
	std::string relId = truckMatch[3].str();
	if (relId == "" && typeStr == "R")
	{
		cusType = Customer::Reception;
	}
	else if (relId == "" && typeStr == "D")
	{
		cusType = Customer::DeliveryFromDepot;
	}
	else if (relId != "" && typeStr == "D")
	{
		cusType = Customer::DeliveryFromCustomer;
		relationId = std::stoi(relId);
	}
	double weight = std::stod(truckMatch[4].str());
	double volume = std::stod(truckMatch[6].str());
	time_t arrTime = getTime(truckMatch[8].str());  // t is now your desired time_t
	time_t depTime = getTime(truckMatch[9].str());  // t is now your desired time_t
	time_t loadTime = getTime(truckMatch[10].str());  // t is now your desired time_t
	if (arrTime > depTime)
	{
		depTime += 24.0 * 60.0 * 60.0;
	}
	TimeWindow timeW(arrTime, depTime, loadTime);
	int curZone = getIntByBinaryStr(truckMatch[11].str());
	int curLoadType = getIntByBinaryStr(truckMatch[13].str());
	return Customer(curId, Package(weight, volume), timeW, cusType, relationId, curZone, curLoadType);
}
