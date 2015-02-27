#pragma once

#include <vector>
#include <unordered_map>
#include <memory>

#include "route.h"
#include "customerPairInfo.h"
#include "costcalculator.h"
#include "restrictionchecker.h"
#include "progress.h"

class ClarkeWrightAlgorithm
{
public:
	ClarkeWrightAlgorithm();
	ClarkeWrightAlgorithm(const std::shared_ptr<std::unordered_map<Pair, double, PairHash> > &distances, std::shared_ptr<RestrictionChecker> const &checker,
						  std::shared_ptr<Progress> const &progress);
	std::unordered_set<std::shared_ptr<Route> > calculate(const std::unordered_set<std::shared_ptr<Route> > &initialRoutes);
	void setWaitTimeParam(double newWaitTime);

private:
	CostCalculator mCostCalculator;
	std::shared_ptr<RestrictionChecker> mRestrictionChecker;
	std::shared_ptr<Progress> mProgress;
};
