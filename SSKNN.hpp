#pragma once
#include <vector>
#include <iterator>
#include <list>
#include <utility>
#include <cfloat>
#include <thread>
#include <future>



template <class DataPoint>
class SSKNN {
private:
	//Nested KeepBest class
	template<class Tsorter>
	class KeepBest
	{
	private:
		unsigned int sortsize;
	public:
		float cutoff = FLT_MAX; //Initial cutoff is v. high.
		std::list < std::pair<float, Tsorter> > best;
		explicit KeepBest(unsigned int ssize) {
			sortsize = ssize;
		};
		void insert(const float score, const Tsorter input) {
			if (score > cutoff) return; //Obj will not be accepted, so return.
			for (auto&& k = best.begin(); k != best.end(); k++) {//Find insertion point
				if (score < std::get<0>(*k)) {//And insert, removing the last element
					best.insert(k, std::make_pair(score, input));
					if (best.size() > sortsize)best.pop_back();
					if (best.size() == sortsize)cutoff = std::get<0>(best.back());
					return;
				}
			}//Insertion point not found, so push it back.
			best.push_back(std::make_pair(score, input));
			if (best.size() > sortsize) best.pop_back();
			if (best.size() == sortsize) cutoff = std::get<0>(best.back());
		};
	};
	
	//*
	//Find distance between two data points (of equal dimensionality).
	//Result invalid if container sizes do not match.
	//Dimensionality not checked for speed.
	//*/
	//	static float distSquared(DataPoint& coords1, DataPoint& coords2) {
	//	float dsquared = 0;
	//	for (unsigned i = 0; i < coords1.size(); ++i) {
	//		dsquared += ((coords1[i] - coords2[i])*(coords1[i] - coords2[i]));
	//	}
	//	return dsquared;
	//};
/*
	Find distance between two data points (of equal dimensionality),ignoring
	one of the dimensions.
	Result invalid if container sizes do not match.
	Dimensionality not checked for speed.
	*/

	static float distSquared(DataPoint& coords1, DataPoint& coords2, unsigned int ignoreDimension) {
		float dsquared = 0;
		for (unsigned i = 0; i < coords1.size(); ++i) {
			if (i == ignoreDimension)continue;
			dsquared += ((coords1[i] - coords2[i])*(coords1[i] - coords2[i]));
		}
		return dsquared;
	};

public:
	std::vector<DataPoint> observations;

	//Populate observations with datapoints from a container, destroying the input vector.
	template <typename Container>
	size_t PopulateUsingMove(Container& inData) {
		observations.reserve(observations.size() + inData.size()); //Avoid reallocations
		std::move(inData.begin(), inData.end(), std::back_inserter(observations));
		inData.clear();//After move is capacity of inData truely 0 - it has not been reized, should reserve(0) be called for speed?
		return observations.size();
	};

	float Query(DataPoint& dp, unsigned int k, unsigned int emptyDimension) {
		KeepBest<typename std::vector<DataPoint>::iterator> keeper(k);
		for (auto i = observations.begin(); i != observations.end();++i) {
			keeper.insert(distSquared(*i, dp, emptyDimension), i);
		}

		float sum = 0;
		for (auto i : keeper.best)sum += (*(i.second))[emptyDimension];
		return sum / static_cast<float>(k);
	};

	KeepBest<typename std::vector<DataPoint>::iterator > QueryPart(DataPoint& dp, unsigned k, typename std::vector<DataPoint>::iterator beginIterator, typename std::vector<DataPoint>::iterator endIterator, unsigned emptyDimension) {
		KeepBest<typename std::vector<DataPoint>::iterator > keep(k);
		while(beginIterator!=endIterator){
		keep.insert(distSquared(*beginIterator, dp, emptyDimension), beginIterator);
		++beginIterator;
		}
		return keep;
	};
	
	float QueryParallel(DataPoint& dp, unsigned int k, unsigned int emptyDimension, unsigned int nThreads = 0) {
		if (nThreads == 0)nThreads = std::thread::hardware_concurrency();
		//We have nThreads keepns
		std::vector<std::future<KeepBest<typename std::vector<DataPoint>::iterator > > > futures;

		//And then one into which to combine results
		KeepBest<typename std::vector<DataPoint>::iterator> best(k);
		unsigned int chunksize = observations.size() / nThreads;
		std::cerr << "Chunksize = " << chunksize << "\n";
		

		for (unsigned i = 0; i < nThreads - 1; ++i) {
			std::cerr << "Spawning t=" << i << ", from" << (chunksize*i) << " to " << (chunksize*(i + 1)) - 1 << "\n";
			futures.push_back(std::async(std::launch::async, [&] {return QueryPart(dp, k, observations.begin() + (chunksize*i), observations.begin() + (chunksize*(i + 1)), emptyDimension); }));
		}
	
		std::cerr << "Spawning t=" << nThreads-1 << ", from" << (chunksize*(nThreads - 1)) << " to " <<observations.size()-1 << "\n";
		futures.push_back(std::async(std::launch::async, [&] {return QueryPart(dp, k, observations.begin() + (chunksize*(nThreads - 1)), observations.end(), emptyDimension); }));
		for (auto&& i: futures) {
			auto f = i.get();
			std::cerr << "future" << f.best.size() << "\n";
			for (auto element : f.best) {
				best.insert(element.first, element.second);
			}
		}



		dp[emptyDimension] = 0;
		for (auto i : best.best)dp[emptyDimension] += (*i.second)[emptyDimension];
		dp[emptyDimension] /= static_cast<float>(best.best.size());
		return dp[emptyDimension];
	};
	

};

