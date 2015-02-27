#include "velocitycalculator.h"

#include <regex>

VelocityCalculator::VelocityCalculator(const std::vector<SpeedDistance> &restrictions) :
	mRestrictions(restrictions)
{
	if (!mRestrictions.empty())
	{
		return;
	}
	SpeedDistance pair1(50.0, 0);
	SpeedDistance pair2(80.0, 50.0);
	SpeedDistance pair3(90.0, 100.0);
	mRestrictions.push_back(pair1);
	mRestrictions.push_back(pair2);
	mRestrictions.push_back(pair3);
}

double VelocityCalculator::getTime(double dist)
{
	double time = 0;
	for (unsigned int i = 0; i < mRestrictions.size() - 1; i ++)
	{
		double curLength = mRestrictions[i + 1].distance - mRestrictions[i].distance;
		time += std::min(dist, curLength) / mRestrictions[i].speed;
		dist -= curLength;
		if (dist <= 0)
		{
			break;
		}
	}
	if (dist > 0)
	{
		time += dist / mRestrictions.back().speed;
	}
	return time * 3600;
}


SpeedDistance::SpeedDistance(double curSpeed, double curDistance) :
		speed(curSpeed)
	  , distance(curDistance)
{
}

SpeedDistance SpeedDistance::parse(const std::string &str)
{
	std::regex infoRegex("(\\d+(.\\d+)*),(\\d+(.\\d+)*)");
	std::smatch infoMatch;
	if (!std::regex_match(str, infoMatch, infoRegex))
	{
		return SpeedDistance(0, 0);
	}
	return SpeedDistance(std::stod(infoMatch[1].str()), std::stod(infoMatch[3].str()));
}
