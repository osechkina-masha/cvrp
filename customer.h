#pragma once
#include <ctime>
#include <string>

struct TimeWindow {
	TimeWindow() :
		arrivalTime(0)
	  , departureTime(0)
	  , loadTime(0)
	{
	}

	TimeWindow(std::time_t arrival, std::time_t departure, std::time_t load):
		arrivalTime(arrival)
	  , departureTime(departure)
	  , loadTime(load)
	{

	}
	std::time_t  arrivalTime;
	std::time_t departureTime;
	std::time_t loadTime;
	bool isValid() const
	{
		return arrivalTime > 0 || departureTime > 0 || loadTime > 0;
	}
};

struct Package
{
	Package(double curWeight, double curVolume, unsigned int curId = 0) :
		weight(curWeight)
	  , volume(curVolume)
	  , id(curId)
	{

	}
	double weight;
	double volume;
	unsigned int id;
};

//how to add depot???
struct Customer {
	enum CustomerType {
		DeliveryFromDepot // client takes package
	  , DeliveryFromCustomer // client takes package
	  , Reception // client gives package
	  , DepotLoad //only depot station!!!
	};

	unsigned int id;
	unsigned int relationCustomerID;
	CustomerType type;
	Package package;
	TimeWindow timeRestriction;
	int zone;
	int loadType;
	Customer(unsigned int curId, const Package &curPackage, CustomerType curType = DeliveryFromDepot,
			 unsigned int curRelationID = 0, int curZone = 0, int curLoadType = 0);
	Customer(unsigned int curId, const Package &curPackage, const TimeWindow &curTimeRestriction,
			 CustomerType curType = DeliveryFromDepot, unsigned int curRelationID = 0, int curZone = 0, int curLoadType = 0);
	bool hasTimeRestrictions() const;
	bool operator ==(const Customer &customer) const;
	bool operator !=(const Customer &customer) const;
	static Customer parse(const std::string &str);
};
