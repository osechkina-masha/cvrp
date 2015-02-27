#pragma once
#include <vector>
#include <string>

struct SpeedDistance
{
	SpeedDistance(double curSpeed, double curDistance);
	double speed;
	double distance;
	static SpeedDistance parse(const std::string &str);
};

class VelocityCalculator
{
public:
	VelocityCalculator(std::vector<SpeedDistance> const &restrictions = std::vector<SpeedDistance>());
	double getTime(double dist);

private:
	std::vector<SpeedDistance> mRestrictions;
};
