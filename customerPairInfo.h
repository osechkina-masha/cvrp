#pragma once

struct Pair
{
	Pair(unsigned int curStartId, unsigned int curFinishId, bool curIsInverted = false) :
			startId(curStartId)
		  , finishId(curFinishId)
		  , isInverted(curIsInverted)
	{
	}
	unsigned int startId;
	unsigned int finishId;
	bool isInverted;
	Pair getInverted() const
	{
		return finishId == startId ? *this : Pair(finishId, startId, !isInverted);
	}

	bool isMidle() const
	{
		return startId !=0 && finishId != 0;
	}

	bool operator == (Pair const &pair) const
	{
		return startId == pair.startId && finishId == pair.finishId;// && isInverted == pair.isInverted;
	}
	bool operator != (Pair const &pair) const
	{
		return startId != pair.startId || finishId != pair.finishId;
	}
};

class PairHash
{
public:
	PairHash()
	{
	}

	int operator () (Pair const &pair) const
	{
		return pair.finishId ^ pair.startId;
	}
};

struct RelationPair
{
	RelationPair(Pair const &curPlace, Pair const &curRoute, double curCost) :
			placeForInsertion(curPlace)
		  , routeToInsert(curRoute)
		  , cost(curCost)
	{
	}
	Pair placeForInsertion; // customers between wich another route will be inserted
	Pair routeToInsert; // customers - ends of route wich will be inserted, REAL ENDS EVEN IF ISINFERTED TRUE
	double cost;
	bool isValid()
	{
		return placeForInsertion.finishId != 0 || placeForInsertion.startId != 0 || routeToInsert.finishId != 0 || routeToInsert.startId != 0;
	}
	bool operator < (RelationPair const &relationPair) const // ATTENTION!!! SORTING IN REVERSE ORDER!!!
	{
		if (cost > relationPair.cost)
		{
			return true;
		}
		if (cost < relationPair.cost)
		{
			return false;
		}
		return (placeForInsertion.startId > relationPair.placeForInsertion.startId) ||
			(placeForInsertion.startId == relationPair.placeForInsertion.startId &&
			 placeForInsertion.finishId > relationPair.placeForInsertion.finishId) ||
			(placeForInsertion == relationPair.placeForInsertion && routeToInsert.startId > relationPair.routeToInsert.startId) ||
			(placeForInsertion == relationPair.placeForInsertion && routeToInsert.startId == relationPair.routeToInsert.startId
			 && routeToInsert.finishId > relationPair.routeToInsert.finishId);
	}
	bool operator == (RelationPair const &relationPair) const
	{
		return placeForInsertion == relationPair.placeForInsertion && routeToInsert == relationPair.routeToInsert;
	}
};

class RelationPairHash
{
public:
	RelationPairHash()
	{
	}
	int operator() (RelationPair const &pair) const
	{
		return pair.routeToInsert.startId ^ pair.routeToInsert.finishId ^ pair.placeForInsertion.startId ^ pair.placeForInsertion.finishId;
	}
};
