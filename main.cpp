#include <iostream>
#include <thread>
#include <regex>

#include "truck.h"
#include "filereader.h"
#include "routermanager.h"
#include <iostream>
#include <sstream>
#include <fstream>


using namespace std;

int main()
{
	std::clock_t time = clock();
	//RouterManager rm("\/home\/masha\/manyClients\/");
	//RouterManager rm("\/home\/masha\/baikal\/");
	//RouterManager rm("\/home\/masha\/test14aug\/");
	//RouterManager rm("\/home\/masha\/test18aug\/");

	//RouterManager rm("\/home\/masha\/KalugFolder\/");
	//RouterManager rm("\/home\/masha\/noKalug\/");
	//RouterManager rm("\/home\/masha\/19augWithKalug/");
    RouterManager rm("\/home\/masha\/testVRP\/");
//	RouterManager rm("\/home\/masha\/testT3\/");
//	RouterManager rm("\/home\/masha\/cw22.07.2014\/");
//	RouterManager rm("\/home\/masha\/testLogitera\/");
//	RouterManager rm("\/home\/masha\/cw-s1\/");
//	RouterManager rm("\/home\/masha\/test1\/");
	rm.calculate();
	std::vector<unsigned int> trucks = rm.getUsedTrucks();
	while (!rm.isFinished()) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
		std::cout << "++++++++++++++++++PROGRESS IS " << rm.getProgress() << std::endl;
		trucks = rm.getUsedTrucks();
	}
	cout << "Hello World! " << trucks.size() << endl;
	cout << "calculate time is " << clock() - time << " " << CLOCKS_PER_SEC << std::endl;
	return 0;
}
