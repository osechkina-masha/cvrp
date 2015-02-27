#pragma once

#include <string>
#include <ctime>

struct Capacity
{
	Capacity(double weight, double volume) :
		weightCapacity(weight)
	  , volumeCapacity(volume)
	{
	}
	double weightCapacity;
	double volumeCapacity;
	bool operator == (Capacity const &other) const
	{
		return other.weightCapacity == weightCapacity && other.volumeCapacity == volumeCapacity;
	}
};

struct Truck {

public:
	Truck(unsigned int curId, double wCapacity, double vCapacity, int curZone = 0, int curLoadType = 0
		, std::time_t curStartTime = 9 * 60 * 60, std::time_t curFinishTime = 18 * 60 * 60, int curMaxClients = 255, double koef = 1.0);
	unsigned int id;
	Capacity capacity;
	std::time_t startTime;
	std::time_t finishTime;
	double costKoef;
	int zone;
	int loadType;
	unsigned int maxClients;
	static Truck parse(const std::string &str);
	bool operator <(Truck const &truck2) const;
	bool operator ==(Truck const &truck) const;
};

class TruckHash
{
public:
	TruckHash()
	{
	}

	int operator () (const Truck &truck) const
	{
		return truck.id;
	}
};
