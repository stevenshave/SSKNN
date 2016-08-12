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

	for(unsigned i=0;i<100e6;i++)data.push_back(std::vector<float>() = { uniformdist(randomengine), uniformdist(randomengine), uniformdist(randomengine) });
	

	SSKNN<std::vector<float> >  knn;
	knn.PopulateUsingMove(data);
	std::vector<float> mypoints = { 0,2,2,2 };
	mypoints[0]=knn.Query(mypoints, 2, 0);
	for (auto i : mypoints)std::cout << i << ",";


	






}