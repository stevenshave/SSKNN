#include <iostream>
#include <thread>
#include <random>
#include <cmath>
#include "SSKNN.hpp"

int main(int argc, char *argv[]) {
	std::random_device devr;
	std::default_random_engine randomengine(devr());
	std::uniform_real_distribution<float> uniformdist(1, 100);

	std::cerr << "N threads supported = " << std::thread::hardware_concurrency() << "\n";

	std::vector<std::vector<float> > data;

	//for(unsigned i=0;i<16;i++)data.push_back(std::vector<float>() = { uniformdist(randomengine), uniformdist(randomengine), uniformdist(randomengine) });
	data.push_back(std::vector<float>() = { 1,1,1,1 });
	data.push_back(std::vector<float>() = { 2,2,2,2 });
	data.push_back(std::vector<float>() = { 3,3,3,3 });
	data.push_back(std::vector<float>() = { 4,4,4,4 });
	data.push_back(std::vector<float>() = { 5,5,5,5 });
	data.push_back(std::vector<float>() = { 6,6,6,6 });
	data.push_back(std::vector<float>() = { 7,7,7,7 });
	data.push_back(std::vector<float>() = { 8,8,8,8 });

	std::cerr << "Populated data\n";

	SSKNN<std::vector<float> >  knn;
	knn.PopulateUsingMove(data);
	std::vector<float> mypoints = { 0,2,2,2 };
	mypoints[0]=knn.Query(mypoints, 4, 0);
	for (auto i : mypoints)std::cout << i << ",";
	mypoints = { 0,2,2,2 };
	std::cout << "\n";

	mypoints[0] = knn.QueryParallel(mypoints, 4, 0, 2);
	for (auto i : mypoints)std::cout << i << ",";


	






}