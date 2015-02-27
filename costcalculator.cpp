#include "costcalculator.h"

#include <iostream>

CostCalculator::CostCalculator()
{

}

CostCalculator::CostCalculator(const std::shared_ptr<std::unordered_map<Pair, double, PairHash> > &distances) :
	mDistances(distances)
{
}

void CostCalculator::initCosts(const std::unordered_set<std::shared_ptr<Route> > &routes)
{
	mCosts.clear();
	mEnds.clear();
	mPlaces.clear();
#ifdef WITH_LOG
	std::cout << "start to init costs " << routes.size();
	std::cout.flush();
#endif
	for (std::shared_ptr<Route> const &route1: routes)
	{
		mEnds.insert(route1->ends());
		if (route1->isInvertible())
		{
			mEnds.insert(route1->ends().getInverted());
		}
		for (Pair const &placeForInsert : route1->placesForInsertion())
		{
			mPlaces.insert(placeForInsert);
			for (std::shared_ptr<Route> const &route2: routes)
			{
				if (route1 == route2)
				{
					continue;
				}
				Pair routeEnds = route2->ends();
				double cost = getCost(placeForInsert, routeEnds);
				if (cost >= 0)
				{
					RelationPair pair(placeForInsert, routeEnds, cost);
					auto pair1 = mCosts.insert(pair);
#ifdef WITH_LOG
					if (pair.placeForInsertion.startId == 132 && pair.placeForInsertion.finishId == 74 &&
							pair.routeToInsert.startId == 132 && pair.routeToInsert.finishId == 74)
					{
						std::cout << "insert bad pair in init function \n";
					}
					if (!pair1.second)
					{
						std::cout << "+++++++++SUCH ELEMENT ALREADY EXISTS!!!!++++++++++++++++\n";
						RelationPair existed = *(pair1.first);
						std::cout << existed.routeToInsert.startId << " " << pair.routeToInsert.startId << "\n"
								  << existed.cost << " " << pair.cost << "\n";
					}
#endif
				}
				if (route2->isInvertible())
				{
					Pair invertedRouteEnds = routeEnds.getInverted();
					double costInvert = getCost(placeForInsert, invertedRouteEnds);
					if (costInvert >= 0)
					{
						RelationPair invertedPair(placeForInsert, invertedRouteEnds, costInvert);
						auto pair2 = mCosts.insert(invertedPair);
#ifdef WITH_LOG
						if (invertedPair.placeForInsertion.startId == 132 && invertedPair.placeForInsertion.finishId == 74 &&
								invertedPair.routeToInsert.startId == 132 && invertedPair.routeToInsert.finishId == 74)
						{
							std::cout << "insert bad inverted pair in init function \n";
						}
						if (!pair2.second)
						{
							std::cout << "+++++++++SUCH ELEMENT ALREADY EXISTS!!!!++++++++++++++++\n";
						}
#endif
					}
				}
			}
		}
	}
#ifdef WITH_LOG
	std::cout << "finish init costs " << mCosts.size() << std::endl;
	std::cout.flush();
#endif
}

RelationPair CostCalculator::getMaxCostPair()
{
	if (mCosts.empty())
	{
		return RelationPair(Pair(0, 0), Pair(0, 0), -1);
	}
	return *mCosts.begin();
}

void CostCalculator::update(const std::shared_ptr<Route> &removedRoute1, const std::shared_ptr<Route> &removedRoute2,
							const std::shared_ptr<Route> &newRoute)
{
	std::unordered_set<RelationPair, RelationPairHash> pairsToDelete;
#ifdef WITH_LOG
	std::cout << "update " << " " << removedRoute1->ends().startId << " " << removedRoute1->ends().finishId << " "
				 << " " << removedRoute2->ends().startId << " " << removedRoute2->ends().finishId << "\n";
#endif
	Pair const removedEnds1 = removedRoute1->ends();
	Pair const removedEnds2 = removedRoute2->ends();
	std::unordered_set<Pair, PairHash> const placesForInsertion1 = removedRoute1->placesForInsertion();
	std::unordered_set<Pair, PairHash> const placesForInsertion2 = removedRoute2->placesForInsertion();
	for (Pair const &endPair: mEnds)
	{
		if (endPair != removedEnds1 && endPair != removedEnds1.getInverted())
		{
			for (Pair const &placePair: placesForInsertion1)
			{
				pairsToDelete.insert(RelationPair(placePair, endPair, getCost(placePair, endPair)));
			}
		}
		if (endPair != removedEnds2 && endPair != removedEnds2.getInverted())
		{
			for (Pair const &placePair: placesForInsertion2)
			{
				pairsToDelete.insert(RelationPair(placePair, endPair, getCost(placePair, endPair)));
			}
		}
	}
	for (Pair const &placePair: mPlaces)
	{
		bool isContain1 = placesForInsertion1.find(placePair) != placesForInsertion1.end();
		bool isContain2 = placesForInsertion2.find(placePair) != placesForInsertion2.end();
		if (!isContain1)
		{
			Pair const &endPair1 = removedEnds1;
			pairsToDelete.insert(RelationPair(placePair, endPair1, getCost(placePair, endPair1)));
			if (removedRoute1->isInvertible())
			{
				Pair invertEnds1 = endPair1.getInverted();
				pairsToDelete.insert(RelationPair(placePair, invertEnds1, getCost(placePair, invertEnds1)));

			}
		}
		if (!isContain2)
		{
			Pair const &endPair2 = removedEnds2;
			pairsToDelete.insert(RelationPair(placePair, endPair2, getCost(placePair, endPair2)));
			if (removedRoute2->isInvertible())
			{
				Pair invertEnds2 = endPair2.getInverted();
				pairsToDelete.insert(RelationPair(placePair, invertEnds2, getCost(placePair, invertEnds2)));
			}
		}
	}
	for (Pair const &pair: placesForInsertion1)
	{
		mPlaces.erase(pair);
	}
	for (Pair const &pair: placesForInsertion2)
	{
		mPlaces.erase(pair);
	}
	mEnds.erase(removedEnds1);
	mEnds.erase(removedEnds2);
	if (removedRoute1->isInvertible())
	{
		mEnds.erase(removedEnds1.getInverted());
	}
	if (removedRoute2->isInvertible())
	{
		mEnds.erase(removedEnds2.getInverted());
	}
	std::unordered_set<RelationPair, RelationPairHash> insertPairs;
	for (Pair const &place: newRoute->placesForInsertion())
	{
		for (Pair const& ends: mEnds)
		{
			insertPairs.insert(RelationPair(place, ends, getCost(place, ends)));
		}
	}
	Pair const &newEnds = newRoute->ends();
	for (Pair const &place: mPlaces)
	{
		insertPairs.insert(RelationPair(place, newEnds, getCost(place, newEnds)));
		if (newRoute->isInvertible())
		{
			insertPairs.insert(RelationPair(place, newEnds.getInverted(), getCost(place, newEnds.getInverted())));
		}
	}
	std::unordered_set<RelationPair, RelationPairHash> intersection;
	for (RelationPair const &pair: pairsToDelete)
	{
		if (insertPairs.find(pair) != insertPairs.end())
		{
			intersection.insert(pair);
		}
	}
	for (RelationPair const &pair: intersection)
	{
		pairsToDelete.erase(pair);
		insertPairs.erase(pair);
	}
	for (RelationPair const &relPair: pairsToDelete)
	{
		if (relPair.cost < 0)
		{
			continue;
		}
		unsigned int s = mCosts.erase(relPair);
#ifdef WITH_LOG
		std::cout << "try to erase " << relPair.placeForInsertion.startId << " " << relPair.placeForInsertion.finishId << " "
					 << relPair.routeToInsert.startId << " " << relPair.routeToInsert.finishId << std::endl;
		if (s == 0)
		{
			if (mDeleted.find(relPair) == mDeleted.end())
			{
				std::cout << "TRY TO DELETE SOMETHING NOT INSERTED???";
			}
			std::cout << "-----------------couldnt erase!!!! \n";
		}
#endif
	}
	for (RelationPair const &pair: insertPairs)
	{
		if (pair.cost >= 0)
		{
			mCosts.insert(pair);
		}
	}
	for (Pair const &place: newRoute->placesForInsertion())
	{
		mPlaces.insert(place);
	}
	mEnds.insert(newRoute->ends());
	if (newRoute->isInvertible())
	{
		mEnds.insert(newRoute->ends().getInverted());
	}
}

void CostCalculator::removeMax()
{
	if (!mCosts.empty())
	{
#ifdef WITH_LOG
		mDeleted.insert(*mCosts.begin());
#endif
		mCosts.erase(mCosts.begin());
	}
}

double CostCalculator::getDistance(unsigned int id1, unsigned int id2)
{
	return mDistances->at(Pair(id1, id2));
}

int CostCalculator::allPairs()
{
	return mCosts.size();
}

double CostCalculator::getCost(const Pair &place, const Pair &routeEnds)
{
	double dPlaceBeginRoute = mDistances->at(Pair(place.startId, routeEnds.startId));
	double dRouteEndPlace = mDistances->at(Pair(routeEnds.finishId, place.finishId));
	double dCur = mDistances->at(place);
	double dTerminalSum = mDistances->at(Pair(0, routeEnds.startId)) + mDistances->at(Pair(routeEnds.finishId, 0));
	double cost = dCur + dTerminalSum - dPlaceBeginRoute - dRouteEndPlace;
	return cost;
}
