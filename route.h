#pragma once
#include <vector>
#include <memory>
#include <unordered_set>

#include "unsplitroute.h"
#include "customerPairInfo.h"

class Route
{
public:
	Route();
	Route(Customer const &customer, bool isDirected = false);
	Route(Customer const &customerRec, Customer const customerDel);
	Route(Route const &mainRoute, Route const &insertedRoute, const Pair &placeToInsert,
		   bool invertInsertedRoute = false);
	bool isInvertible() const;
	double maxWeight() const;
	double maxVolume() const;
	virtual ~Route();
	int getZone() const;
	int getLoadType() const;
	std::unordered_set<Pair, PairHash> placesForInsertion() const;
	std::vector<Customer> allCustomers() const;
	std::vector<unsigned int> customerIds() const;
	Pair ends() const;
//	bool operator ==(Route const &route) const;

private:
	Route(std::vector<UnsplitRoute> const &customers, bool isDirected = false);
	void merge(Route const &route1, Route const &route2); // merge without insertion and invertion
	unsigned int firstCustomerId() const;
	unsigned int lastCustomerId() const;
	static Route invert(Route const &route);
	std::vector<UnsplitRoute> mCustomers;
	bool mIsDirected;
	int mZone;
	int mLoadType;
};
