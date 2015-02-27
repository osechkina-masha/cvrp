#pragma once

class Progress
{
public:
	Progress();
	//void setProgress(int doneWork, int allWork);
	void setProgress(unsigned int allWork);
	int getProgress();
	void clean();

private:
	unsigned int mAllWork;
	unsigned int mMaxWork;
};
