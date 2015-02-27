#include "progress.h"

#include <iostream>

const int FullWorkPercent = 100;

Progress::Progress() :
	mAllWork(0)
  , mMaxWork(0)
{
}

void Progress::setProgress(unsigned int allWork)
{
	mMaxWork = std::max(mMaxWork, allWork);
	mAllWork = allWork;
}

int Progress::getProgress()
{
	if (mMaxWork == 0)
	{
		return 0;
	}
	return FullWorkPercent * (mMaxWork - mAllWork) / mMaxWork;
}

void Progress::clean()
{
	mMaxWork = 0;
	mAllWork = 0;
}
