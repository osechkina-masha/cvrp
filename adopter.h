#pragma once

#include <ctime>
#include <sstream>
#include <vector>
#include <unordered_set>
#include <regex>
#include <iostream>

inline std::time_t getTime(std::string const &str)
{
	std::regex timeReg("(\\d+):(\\d+):(\\d+)");
	std::smatch timeMatch;
	if (!std::regex_match(str, timeMatch, timeReg))
	{
		return 0;
	}
	return std::stoi(timeMatch[1]) * 3600 + std::stoi(timeMatch[2]) * 60 + std::stoi(timeMatch[3]);
}

inline int getIntByBinaryStr(std::string const &str)
{
	std::regex reg("([0-1])(\\|([0-1]))*");
	std::smatch match;
	if (!std::regex_match(str, match, reg))
	{
		return 0;
	}
	int zone  = 0;
	for (std::smatch::iterator it = match.begin() + 1; it!=match.end(); ++it)
	{
		if (*it == "0")
		{
			zone *= 2;
		}
		else if (*it == "1")
		{
			(zone *= 2) ++;
		}
	}
	return zone;
}

bool try_kuhn (int vertex, const std::vector<std::vector<int>> &graph, std::unordered_set<int> &used,
				std::vector<int> &maxMatching);

unsigned int getMaxMatchingSize(std::vector<std::vector<int>> const &graph, int secondPartSize);

std::vector<int> getMatching(std::vector<std::vector<int>> const &graph, int secondPartSize);
