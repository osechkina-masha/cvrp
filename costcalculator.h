#pragma once

#include <unordered_map>
#include <set>
#include <vector>
#include <memory>
#include <unordered_set>

#include "customerPairInfo.h"
#include "route.h"

class CostCalculator
{
public:
	CostCalculator();
	CostCalculator(const std::shared_ptr<std::unordered_map<Pair, double, PairHash> > &distances);
	void initCosts(const std::unordered_set<std::shared_ptr<Route> > &routes);
	RelationPair getMaxCostPair();
	void update(std::shared_ptr<Route> const &removedRoute1, std::shared_ptr<Route> const &removedRoute2,
				std::shared_ptr<Route> const &newRoute);
	void removeMax();
	double getDistance(unsigned int id1, unsigned int id2);
	int allPairs();

private:
	std::shared_ptr<std::unordered_map<Pair, double, PairHash> > mDistances;
	std::unordered_set<Pair, PairHash> mEnds;
	std::unordered_set<Pair, PairHash> mPlaces;
	std::set<RelationPair> mCosts;
#ifdef WITH_LOG
	std::unordered_set<RelationPair, RelationPairHash> mDeleted;
#endif
	double getCost(const Pair &place, const Pair &routeEnds);
};
