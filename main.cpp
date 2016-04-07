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
	srand(time(NULL));
	std::vector<std::string> filesPath;


//    filesPath.push_back("\/home\/masha\/cvrp_samples\/E022-04g.dat");
//    filesPath.push_back("\/home\/masha\/cvrp_samples\/E016-03m.dat");
//    filesPath.push_back("\/home\/masha\/cvrp_samples\/E016-05m.dat");


	//filesPath.push_back("\/home\/masha\/cvrp_samples\/E481-38k.dat"); //0 unmatched routes


	//filesPath.push_back("\/home\/masha\/cvrp_samples\/E101-08e.dat");  //cap =200 0 unmatched routes
	//filesPath.push_back("\/home\/masha\/cvrp_samples\/E101-10c.dat"); //cap =200 0 unmatched routes
	filesPath.push_back("\/home\/masha\/cvrp_samples\/E101-14s.dat"); //cap =112 0 unmatched routes
	//filesPath.push_back("\/home\/masha\/cvrp_samples\/E121-07c.dat"); // cap = 200 0 unmatched routes
	filesPath.push_back("\/home\/masha\/cvrp_samples\/E135-07f.dat"); //cap = 2210  0 unmatched routes
	//filesPath.push_back("\/home\/masha\/cvrp_samples\/E151-12b.dat"); // cap = 200 0 unmatched routes
	//filesPath.push_back("\/home\/masha\/cvrp_samples\/E151-12c.dat"); // cap = 200 0 unmatched routes
	//filesPath.push_back("\/home\/masha\/cvrp_samples\/E101C11r.dat");  //cap =2043 0 unmatched route
	//filesPath.push_back("\/home\/masha\/cvrp_samples\/E151B14r.dat"); //cap = 1918 0 unmatched routes


//	filesPath.push_back("\/home\/masha\/cvrp_samples\/E101A11r.dat");  //cap =1409 1 unmatched route
//	filesPath.push_back("\/home\/masha\/cvrp_samples\/E101B11r.dat");  //cap =1842 1 unmatched route
	//filesPath.push_back("\/home\/masha\/cvrp_samples\/E101D11r.dat");  //cap = 1297 1 unmatched route
	//filesPath.push_back("\/home\/masha\/cvrp_samples\/E151A15r.dat"); //cap = 1544 1 unmatched route
	//filesPath.push_back("\/home\/masha\/cvrp_samples\/E151C14r.dat"); //cap = 1544 1 unmatched route
	//filesPath.push_back("\/home\/masha\/cvrp_samples\/E151D14r.dat"); //cap = 1544 2 unmatched routes

	RouterManager rm(filesPath);
	std::clock_t time = clock();
	//RouterManager rm("\/home\/masha\/manyClients\/");
	//RouterManager rm("\/home\/masha\/baikal\/");
	//RouterManager rm("\/home\/masha\/test14aug\/");
	//RouterManager rm("\/home\/masha\/test18aug\/");

	//RouterManager rm("\/home\/masha\/KalugFolder\/");
	//RouterManager rm("\/home\/masha\/noKalug\/");
	//RouterManager rm("\/home\/masha\/19augWithKalug/");
	//RouterManager rm("\/home\/masha\/testVRP\/");
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
