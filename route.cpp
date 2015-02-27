#include "route.h"

#include <limits>
#include <iostream>

Route::Route() :
	mIsDirected(false)
  , mZone(0)
  , mLoadType(0)
{
}

Route::Route(const Customer &customer, bool isDirected) :
	mIsDirected(isDirected)
  , mZone(customer.zone)
  , mLoadType(customer.loadType)
{
	mCustomers.push_back(UnsplitRoute(customer));
}

Route::Route(const Customer &customerRec, const Customer customerDel) :
	mIsDirected(true)
  , mZone(customerRec.zone | customerDel.zone)
  , mLoadType(customerRec.loadType | customerDel.loadType)
{
	mCustomers.push_back(UnsplitRoute(customerRec));
	mCustomers.push_back(UnsplitRoute(customerDel));
}

Route::Route(const std::vector<UnsplitRoute> &customers, bool isDirected) :
	mCustomers(customers)
  , mIsDirected(isDirected)
  , mZone(0)
  , mLoadType(0)
{
	for (UnsplitRoute const &route: customers)
	{
		for (Customer const &cust: route.getAllCustomers())
		{
			mZone = mZone | cust.zone;
			mLoadType = mLoadType | cust.loadType;
		}
	}
}

void Route::merge(const Route &route1, const Route &route2)
{
	mCustomers.clear();
	for (unsigned int i = 0; i < route1.mCustomers.size() - 1; i ++)
	{
		mCustomers.push_back(route1.mCustomers[i]);
	}
	UnsplitRoute mergedUnsplitRoute = UnsplitRoute::merge(route1.mCustomers.back(), route2.mCustomers[0]);
	mCustomers.push_back(mergedUnsplitRoute);
	for (unsigned int i = 1; i < route2.mCustomers.size(); i ++)
	{
		mCustomers.push_back(route2.mCustomers[i]);
	}
	mIsDirected = route1.mIsDirected || route2.mIsDirected;
}

Route Route::invert(const Route &route)
{
	std::vector<UnsplitRoute> customers;
	for (int i = route.mCustomers.size() - 1; i >= 0; i --)
	{
		const UnsplitRoute curRoute = route.mCustomers[i];
		customers.push_back(UnsplitRoute::invert(curRoute));
	}
	return Route(customers);
}

bool Route::isInvertible() const
{
	return !mIsDirected && ends().finishId != ends().startId;
}

double Route::maxWeight() const
{
	double sum = 0;
	std::vector<Customer> customers = allCustomers();
	for (Customer const &cust: customers)
	{
		if (cust.type == Customer::DeliveryFromDepot)
		{
			sum += cust.package.weight;
		}
	}
	double max = sum;
	for (unsigned int i = 0; i < customers.size(); i ++)
	{
		Customer const &cur = customers[i];
		double curW = cur.package.weight;
		switch (cur.type) {
		case Customer::DeliveryFromDepot:
			sum -= curW;
			break;
		case Customer::DeliveryFromCustomer:
			sum -= curW;
			break;
		case Customer::Reception:
			sum += curW;
			break;
		default:
			break;
		}
		max = std::max(sum, max);
	}
	return max;
}

double Route::maxVolume() const
{
	std::vector<Customer> customers = allCustomers();
	double sum = 0;
	for (Customer const &cust: customers)
	{
		if (cust.type == Customer::DeliveryFromDepot)
		{
			sum += cust.package.volume;
		}
	}
	double max = sum;
	for (unsigned int i = 0; i < customers.size(); i ++)
	{
		Customer const &cur = customers[i];
		double curV = cur.package.volume;
		switch (cur.type) {
		case Customer::DeliveryFromDepot:
			sum -= curV;
			break;
		case Customer::DeliveryFromCustomer:
			sum -= curV;
			break;
		case Customer::Reception:
			sum += curV;
			break;
		default:
			break;
		}
		max = std::max(sum, max);
	}
	return max;
}

Route::~Route()
{
}

int Route::getZone() const
{
	return mZone;
}

int Route::getLoadType() const
{
	return mLoadType;
}

Route::Route(const Route &mainRoute, const Route &insertedRoute, Pair const &placeToInsert,
				   bool invertInsertedRoute) :
	mZone(mainRoute.mZone | insertedRoute.mZone)
  , mLoadType(mainRoute.mLoadType | insertedRoute.mLoadType)
{
	if ((invertInsertedRoute && !insertedRoute.isInvertible()) || mainRoute.mCustomers.empty()) //TODO:: make smart merge!!!
	{
		mIsDirected = false;
		return;
	}
	if (insertedRoute.mCustomers.empty())
	{
		mCustomers = mainRoute.mCustomers;
		mIsDirected = mainRoute.mIsDirected;
		return;
	}
	mIsDirected = mainRoute.mIsDirected || insertedRoute.mIsDirected;
	mCustomers = mainRoute.mCustomers;
	Route const customers = (!invertInsertedRoute || !insertedRoute.isInvertible()) ? insertedRoute : invert(insertedRoute);
	if (placeToInsert.startId == 0) // is depot?
	{
		mCustomers.insert(mCustomers.begin(), customers.mCustomers.begin(), customers.mCustomers.end());
		//merge(customers, mainRoute);
		return;
	}
	if (placeToInsert.finishId == 0)
	{
		mCustomers.insert(mCustomers.end(), customers.mCustomers.begin(), customers.mCustomers.end());
		//merge(mainRoute, customers);
		return;
	}
	for (unsigned int i = 0; i < mCustomers.size() - 1; i ++)
	{
		if (mCustomers[i].getLastId() == placeToInsert.startId && mCustomers[i + 1].getFirstId() == placeToInsert.finishId) // unsplit routes which have be merged
		{
			mCustomers.insert(mCustomers.begin() + i + 1, customers.mCustomers.begin(), customers.mCustomers.end());
		}
	}
}

std::unordered_set<Pair, PairHash> Route::placesForInsertion() const
{
	std::unordered_set<Pair, PairHash> places;
	if (mCustomers.empty())
	{
		return places;
	}
	places.insert(Pair(0, mCustomers[0].getFirstId()));
	for (unsigned int i = 0; i < mCustomers.size() - 1; i ++)
	{
		places.insert(Pair(mCustomers[i].getLastId(), mCustomers[i + 1].getFirstId()));
	}
	places.insert(Pair(mCustomers.back().getLastId(), 0));
	return places;
}

std::vector<Customer> Route::allCustomers() const
{
	std::vector<Customer> customers;
	for (unsigned int i = 0; i < mCustomers.size(); i ++)
	{
		std::vector<Customer> curCustomers(mCustomers[i].getAllCustomers());
		customers.insert(customers.end(), curCustomers.begin(), curCustomers.end());
	}
	return customers;
}

std::vector<unsigned int> Route::customerIds() const
{
	std::vector<Customer> customers(allCustomers());
	std::vector<unsigned int> ids;
	for (unsigned int i = 0; i < customers.size(); i ++)
	{
		ids.push_back(customers[i].id);
	}
	return ids;
}

Pair Route::ends() const
{
	return Pair(firstCustomerId(), lastCustomerId());
}

//bool Route::operator ==(const Route &route) const
//{
//	if (mIsDirected != route.mIsDirected || mCustomers.size() != route.mCustomers.size())
//	{
//		return false;
//	}
//	for (unsigned int i = 0; i < mCustomers.size(); i ++)
//	{
//		if (mCustomers[i] != route.mCustomers[i])
//		{
//			return false;
//		}
//	}
//	return true;
//}

unsigned int Route::firstCustomerId() const
{
	if (mCustomers.empty())
		return 0;
	return mCustomers[0].getFirstId();
}

unsigned int Route::lastCustomerId() const
{
	if (mCustomers.empty())
		return 0;
	return mCustomers.back().getLastId();
}
