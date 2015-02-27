#include "truck.h"
#include "adopter.h"

#include <iostream>
#include <regex>

Truck::Truck(unsigned int curId, double wCapacity, double vCapacity, int curZone, int curLoadType,
		std::time_t curStartTime, std::time_t curFinishTime, int curMaxClients, double koef) :
	id(curId)
  , capacity(wCapacity, vCapacity)
  , startTime(curStartTime)
  , finishTime(curFinishTime)
  , costKoef(koef)
  , zone(curZone)
  , loadType(curLoadType)
  , maxClients(curMaxClients)
{
#ifdef WITH_LOG
	std::cout << "truck capacity is " << capacity.weightCapacity << " volume is " << capacity.volumeCapacity << std::endl;
#endif
}

Truck Truck::parse(std::string const &str)
{
	std::regex truckRegex("(\\d+),(\\d+(.\\d+)*),(\\d+(.\\d+)*),(\\d+:\\d+:\\d+),(\\d+:\\d+:\\d+),([0-1](\\|[0-1])*),([0-1](\\|[0-1])*)(,(\\d+))*");
	std::smatch truckMatch;
	if (!std::regex_match(str, truckMatch, truckRegex))
	{
	}
	int curId = std::stoi(truckMatch[1].str());
	double weight = std::stod(truckMatch[2].str());
	double volume = std::stod(truckMatch[4].str());
	std::time_t startT = getTime(truckMatch[6].str());
	std::time_t finishT = getTime(truckMatch[7].str());
	int curZone = getIntByBinaryStr(truckMatch[8].str());
	int curLoadType = getIntByBinaryStr(truckMatch[10].str()); // TODO::rename function!!!
    int maxClients = 250;//std::stoi(truckMatch[13].str());
//	if (maxClientsStr.empty())
//	{
//		return Truck(curId, weight, volume, curZone, curLoadType, startT, finishT);
//	}
    return Truck(curId, weight, volume, curZone, curLoadType, startT, finishT, maxClients);
}

bool Truck::operator < (const Truck &truck2) const
{
	return capacity.weightCapacity < truck2.capacity.weightCapacity ||
			(capacity.weightCapacity == truck2.capacity.weightCapacity && capacity.volumeCapacity < truck2.capacity.volumeCapacity);
}

bool Truck::operator ==(const Truck &truck) const
{
	return id == truck.id;
}
