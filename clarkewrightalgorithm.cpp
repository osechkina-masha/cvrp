#include "clarkewrightalgorithm.h"

#include <algorithm>
#include <limits>
#include <iostream>

ClarkeWrightAlgorithm::ClarkeWrightAlgorithm()
{
}

ClarkeWrightAlgorithm::ClarkeWrightAlgorithm(const std::shared_ptr<std::unordered_map<Pair, double, PairHash> > &distances, const std::shared_ptr<RestrictionChecker> &checker, const std::shared_ptr<Progress> &progress) :
		mCostCalculator(distances)
	  , mRestrictionChecker(checker)
	  , mProgress(progress)
{
}


std::unordered_set<std::shared_ptr<Route> > ClarkeWrightAlgorithm::calculate(const std::unordered_set<std::shared_ptr<Route>> &initialRoutes)
{
#ifdef WITH_LOG
	std::cout << "calculate clarke wright" << std::endl;
#endif
	mProgress->clean();
	std::unordered_set<std::shared_ptr<Route>> routes;
	std::unordered_map<Pair, std::shared_ptr<Route>, PairHash> insertPlacesMap;
	std::unordered_map<Pair, std::shared_ptr<Route>, PairHash> endsRouteMap;
	for (std::shared_ptr<Route> const &route: initialRoutes)
	{
		std::pair<Pair, std::shared_ptr<Route> > ends(route->ends(), route);
		endsRouteMap.insert(ends);
		if (route->isInvertible())
		{
			std::pair<Pair, std::shared_ptr<Route> > invertedEnds(route->ends().getInverted(), route);
			endsRouteMap.insert(invertedEnds);
		}
		for (Pair const &pair: route->placesForInsertion())
		{
			std::pair<Pair, std::shared_ptr<Route> > place(pair, route);
			insertPlacesMap.insert(place);
		}
		routes.insert(route);
	}
#ifdef WITH_LOG
	std::cout << "try to init costs \n";
#endif
	mCostCalculator.initCosts(initialRoutes);
	bool canMerge = true;
	while (canMerge)
	{
		RelationPair curRelationPair = mCostCalculator.getMaxCostPair();
#ifdef WITH_LOG
		std::cout << "max is " << curRelationPair.placeForInsertion.startId << " " << curRelationPair.placeForInsertion.finishId
					 << " " << curRelationPair.routeToInsert.startId << " " << curRelationPair.routeToInsert.finishId << std::endl;
#endif
		if (!curRelationPair.isValid() || curRelationPair.cost < 0)
		{
			canMerge = false;
			break;
		}
		Pair const &place = curRelationPair.placeForInsertion;
		Pair const &ends = curRelationPair.routeToInsert;
		std::shared_ptr<Route> route1 = insertPlacesMap.at(place); //curRelationPair.mainRoute;
		std::shared_ptr<Route> route2 = endsRouteMap.at(ends); //curRelationPair.routeToInsert;
		std::shared_ptr<Route> mergedRoute(new Route(*route1, *route2, place, curRelationPair.routeToInsert.isInverted));
		if (!mRestrictionChecker->checkAndUpdateRestrictions(mergedRoute, route1, route2))
		{
			mCostCalculator.removeMax();
			mProgress->setProgress(mCostCalculator.allPairs());
#ifdef WITH_LOG
			std::cout << "bad restriction" << std::endl;
#endif
			continue;
		}
#ifdef WITH_LOG
		std::cout << "good merge" << std::endl;
		std::cout << "endsRouteMap erased " << route2->ends().startId << " " << route2->ends().finishId << std::endl;
#endif
		endsRouteMap.erase(route2->ends());
		if (route2->isInvertible())
		{
			endsRouteMap.erase(route2->ends().getInverted());
		}
		endsRouteMap.erase(route1->ends());
		if (route1->isInvertible())
		{
			endsRouteMap.erase(route1->ends().getInverted());
		}
		endsRouteMap[mergedRoute->ends()] = mergedRoute;
		if (mergedRoute->isInvertible())
		{
			Pair invertedMerge = mergedRoute->ends().getInverted();
			endsRouteMap[invertedMerge] = mergedRoute;
		}
		insertPlacesMap.erase(place);
		for (Pair const &pair: route2->placesForInsertion())
		{
			insertPlacesMap.erase(pair);
			insertPlacesMap.erase(pair.getInverted());
		}
		for (Pair const &pair: mergedRoute->placesForInsertion())
		{
			insertPlacesMap[pair] = mergedRoute;
		}
		routes.erase(route1);
		routes.erase(route2);
		routes.insert(mergedRoute);
		mCostCalculator.update(route1, route2, mergedRoute);
		mProgress->setProgress(mCostCalculator.allPairs());
	}
#ifdef WITH_LOG
	std::cout << "finish calculation \n";
#endif
	return routes;
}

void ClarkeWrightAlgorithm::setWaitTimeParam(double newWaitTime)
{
	mRestrictionChecker->setWaitTimeParams(newWaitTime);
}
