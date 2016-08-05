#include <iostream>
#include <thread>
#include "SSKNN.hpp"

int main(int argc, char *argv[]) {
	SSKNN<std::vector<float> > knn;
	std::cerr << "N threads supported = " << std::thread::hardware_concurrency() << "\n";

}