#include "unsplitroute.h"
#include <cmath>

UnsplitRoute::UnsplitRoute(const Customer &customer) :
		mStartWeight(0)
	  , mStartVolume(0)
	  , mFinishWeight(0)
	  , mFinishVolume(0)
	  , mMaxWeight(customer.package.weight)
	  , mMaxVolume(customer.package.volume)
{
	mCustomers.push_back(customer);
	if (customer.type == Customer::DeliveryFromCustomer || customer.type == Customer::DeliveryFromDepot)
	{
		mStartVolume = customer.package.volume;
		mStartWeight = customer.package.weight;
	}
	else if (customer.type == Customer::Reception)
	{
		mFinishVolume = customer.package.volume;
		mFinishWeight = customer.package.weight;
	}
}

UnsplitRoute::UnsplitRoute() :
	mStartWeight(0)
  , mStartVolume(0)
  , mFinishWeight(0)
  , mFinishVolume(0)
  , mMaxWeight(0)
  , mMaxVolume(0)
{

}

unsigned int UnsplitRoute::getFirstId() const
{
	if (mCustomers.empty())
		return 0; // depot id;
	return mCustomers.at(0).id;
}

unsigned int UnsplitRoute::getLastId() const
{
	if (mCustomers.empty())
		return 0; //depot id;
	return mCustomers.back().id;
}

double UnsplitRoute::getMaxWeight() const
{
	return mMaxWeight;
}

double UnsplitRoute::getStartWeight() const
{
	return mStartWeight;
}

double UnsplitRoute::getFinishWeight() const
{
	return mFinishWeight;
}

double UnsplitRoute::getMaxVolume() const
{
	return mMaxVolume;
}

double UnsplitRoute::getStartVolume() const
{
	return mStartVolume;
}

double UnsplitRoute::getFinishVolume() const
{
	return mFinishVolume;
}

const std::vector<Customer> &UnsplitRoute::getAllCustomers() const
{
	return mCustomers;
}

UnsplitRoute UnsplitRoute::merge(const UnsplitRoute &route1, const UnsplitRoute &route2, MergeType mergeType)
{
	UnsplitRoute dirRoute1 = route1;
	UnsplitRoute dirRoute2 = route2;
	switch (mergeType) {
	case EndBegin:
		break;
	case EndEnd:
		dirRoute2 = invert(route2);
		break;
	case BeginBegin:
		dirRoute1 = invert(dirRoute1);
		break;
	case BeginEnd:
		dirRoute1 = invert(route1);
		dirRoute2 = invert(route2);
		break;
	default:
		break;
	}
	std::vector<Customer> customers = dirRoute1.mCustomers;
	customers.insert(customers.end(), dirRoute2.mCustomers.begin(), dirRoute2.mCustomers.end());
	double startWeight = dirRoute1.mStartWeight + dirRoute2.mStartWeight;
	double startVolume = dirRoute1.mStartVolume + dirRoute2.mStartVolume;
	double finishWeight = dirRoute1.mFinishWeight + dirRoute2.mFinishWeight;
	double finishVolume = dirRoute1.mFinishVolume + dirRoute2.mFinishVolume;
	double maxWeight = std::max(dirRoute1.mMaxWeight + dirRoute2.mStartWeight, dirRoute1.mFinishWeight + dirRoute2.mMaxWeight);
	double maxVolume = std::max(dirRoute1.mMaxVolume + dirRoute2.mStartVolume, dirRoute1.mFinishVolume + dirRoute2.mMaxVolume);
	return UnsplitRoute(customers, startWeight, startVolume, finishWeight, finishVolume, maxWeight, maxVolume);
}

UnsplitRoute::UnsplitRoute(const std::vector<Customer> &customers, double startWeight, double startVolume,
						   double finishWeight, double finishVolume, double maxWeight, double maxVolume) :
		mCustomers(customers)
	  , mStartWeight(startWeight)
	  , mStartVolume(startVolume)
	  , mFinishWeight(finishWeight)
	  , mFinishVolume(finishVolume)
	  , mMaxWeight(maxWeight)
	  , mMaxVolume(maxVolume)
{

}

UnsplitRoute UnsplitRoute::invert(const UnsplitRoute &unsplitRoute)
{
	UnsplitRoute route = unsplitRoute;
	for (unsigned int i = 0; i < route.mCustomers.size() / 2; i ++)
	{
		Customer customer = route.mCustomers[i];
		route.mCustomers[i] = route.mCustomers[route.mCustomers.size() - i - 1];
		route.mCustomers[route.mCustomers.size() - i - 1] = customer;
	}
	double curWeight = route.mStartWeight;
	double curVolume = route.mStartVolume;
	route.mMaxWeight = route.mStartWeight;
	route.mMaxVolume = route.mStartVolume;
	for (unsigned int i = 0; i < route.mCustomers.size(); i ++)
	{
		if (route.mCustomers[i].type == Customer::DeliveryFromCustomer || route.mCustomers[i].type == Customer::DeliveryFromDepot)
		{
			curWeight -= route.mCustomers[i].package.weight;
			curVolume -= route.mCustomers[i].package.volume;
		}
		else if (route.mCustomers[i].type == Customer::Reception)
		{
			curWeight += route.mCustomers[i].package.weight;
			curVolume += route.mCustomers[i].package.volume;
		}
		route.mMaxWeight = std::max(route.mMaxWeight, curWeight);
		route.mMaxVolume = std::max(route.mMaxVolume, curVolume);
	}
	return route;
}
