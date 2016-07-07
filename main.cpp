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


//    filesPath.push_back("\/home\/osechkina\/cvrp_samples\/E022-04g.dat");
//    filesPath.push_back("\/home\/osechkina\/cvrp_samples\/E016-03m.dat");
//    filesPath.push_back("\/home\/osechkina\/cvrp_samples\/E016-05m.dat");


    //filesPath.push_back("\/home\/osechkina\/cvrp_samples\/E481-38k.dat"); //0 unmatched routes

    std::vector<std::string> allFilesPath;
    allFilesPath.push_back("\/home\/osechkina\/cvrp_samples\/E101-08e.dat");  //cap =200 0 unmatched routes
    allFilesPath.push_back("\/home\/osechkina\/cvrp_samples\/E101-10c.dat"); //cap =200 0 unmatched routes
    allFilesPath.push_back("\/home\/osechkina\/cvrp_samples\/E101-14s.dat"); //cap =112 0 unmatched routes
    allFilesPath.push_back("\/home\/osechkina\/cvrp_samples\/E121-07c.dat"); // cap = 200 0 unmatched routes
    allFilesPath.push_back("\/home\/osechkina\/cvrp_samples\/E135-07f.dat"); //cap = 2210  0 unmatched routes
    allFilesPath.push_back("\/home\/osechkina\/cvrp_samples\/E151-12b.dat"); // cap = 200 0 unmatched routes
    allFilesPath.push_back("\/home\/osechkina\/cvrp_samples\/E151-12c.dat"); // cap = 200 0 unmatched routes
    allFilesPath.push_back("\/home\/osechkina\/cvrp_samples\/E101C11r.dat");  //cap =2043 0 unmatched route
    allFilesPath.push_back("\/home\/osechkina\/cvrp_samples\/E151B14r.dat"); //cap = 1918 0 unmatched routes

    std::vector<std::vector<int>> possibleNum;

    for (int i = 0; i < allFilesPath.size(); ++i)
    {
        std::vector<int> num;
        num.push_back(i);
        possibleNum.push_back(num);
    }

    for (int i = 0; i < allFilesPath.size() - 1; ++i)
    {
        for (int j = i + 1; j < allFilesPath.size(); ++j)
        {
            std::vector<int> num;
            num.push_back(i);
            num.push_back(j);
            possibleNum.push_back(num);
        }
    }

    for (int i = 0; i < allFilesPath.size() - 2; ++i)
    {
        for (int j = i + 1; j < allFilesPath.size() - 1; ++j)
        {
            for (int k = j + 1; k < allFilesPath.size(); ++k)
            {
                std::vector<int> num;
                num.push_back(i);
                num.push_back(j);
                possibleNum.push_back(num);
            }
        }
    }

//	filesPath.push_back("\/home\/osechkina\/cvrp_samples\/E101A11r.dat");  //cap =1409 1 unmatched route
//	filesPath.push_back("\/home\/osechkina\/cvrp_samples\/E101B11r.dat");  //cap =1842 1 unmatched route
    //filesPath.push_back("\/home\/osechkina\/cvrp_samples\/E101D11r.dat");  //cap = 1297 1 unmatched route
    //filesPath.push_back("\/home\/osechkina\/cvrp_samples\/E151A15r.dat"); //cap = 1544 1 unmatched route
    //filesPath.push_back("\/home\/osechkina\/cvrp_samples\/E151C14r.dat"); //cap = 1544 1 unmatched route
    //filesPath.push_back("\/home\/osechkina\/cvrp_samples\/E151D14r.dat"); //cap = 1544 2 unmatched routes
    ofstream stream;
    stream.open ("\/home\/osechkina\/cvrp_samples\/example.txt");

    for (int i = 121; i < possibleNum.size(); ++ i)
    {
        std::vector<int> nums = possibleNum.at(i);
        std::vector<std::string> filesPath;
        srand(time(NULL));
        stream << "used files" << std::endl;
        for (int index : nums)
        {
            filesPath.push_back(allFilesPath.at(index));
            stream << allFilesPath.at(index) << std::endl;
        }
        stream << "end of used files" << std::endl;
        RouterManager rm(filesPath, stream);
        std::clock_t time = clock();
        //RouterManager rm("\/home\/osechkina\/manyClients\/");
        //RouterManager rm("\/home\/osechkina\/baikal\/");
        //RouterManager rm("\/home\/osechkina\/test14aug\/");
        //RouterManager rm("\/home\/osechkina\/test18aug\/");

        //RouterManager rm("\/home\/osechkina\/KalugFolder\/");
        //RouterManager rm("\/home\/osechkina\/noKalug\/");
        //RouterManager rm("\/home\/osechkina\/19augWithKalug/");
        //RouterManager rm("\/home\/osechkina\/testVRP\/");
        //	RouterManager rm("\/home\/osechkina\/testT3\/");
        //	RouterManager rm("\/home\/osechkina\/cw22.07.2014\/");
        //	RouterManager rm("\/home\/osechkina\/testLogitera\/");
        //	RouterManager rm("\/home\/osechkina\/cw-s1\/");
        //	RouterManager rm("\/home\/osechkina\/test1\/");
        rm.calculate();
        std::vector<unsigned int> trucks = rm.getUsedTrucks();
        while (!rm.isFinished()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            std::cout << "++++++++++++++++++PROGRESS IS " << rm.getProgress() << std::endl;
            trucks = rm.getUsedTrucks();
        }
        stream << "Hello World! " << trucks.size() << endl;
        stream << "calculate time is " << clock() - time << " " << CLOCKS_PER_SEC << std::endl;
    }
    stream.close();
	return 0;
}
