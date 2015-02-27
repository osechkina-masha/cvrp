#pragma once
#include <vector>

#include "customer.h"

class UnsplitRoute
{
public:
	enum MergeType
	{
		EndBegin, //connect end of first with begin of second
		BeginEnd, // connect begin of first with end of second
		EndEnd, // connect end of first with end of second
		BeginBegin // connect begin of ferst with begin of second
	};

	UnsplitRoute(Customer const &customer);
	UnsplitRoute();
	unsigned int getFirstId() const;
	unsigned int getLastId() const;
	double getMaxWeight() const;
	double getStartWeight() const;
	double getFinishWeight() const;
	double getMaxVolume() const;
	double getStartVolume() const;
	double getFinishVolume() const;
	//bool operator == (UnsplitRoute const &route) const;
	//bool operator != (UnsplitRoute const &route) const;
	const std::vector<Customer> &getAllCustomers() const;
//	void inverse();
	static UnsplitRoute invert(UnsplitRoute const &route);
	static UnsplitRoute merge(UnsplitRoute const &route1, UnsplitRoute const &route2, MergeType mergeType = EndBegin);

private:
	UnsplitRoute(std::vector<Customer> const &customers, double startWeight, double startVolume, double finishWeight,
				 double finishVolume, double maxWeight, double maxVolume);
	std::vector<Customer> mCustomers;
	double mStartWeight;
	double mStartVolume;
	double mFinishWeight;
	double mFinishVolume;
	double mMaxWeight;
	double mMaxVolume;
};
