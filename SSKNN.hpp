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

public:
	std::vector<DataPoint> observations;
	unsigned int sortsize = 10;

	/*
	Find distance between two data points (of equal dimensionality).
	Result invalid if container sizes do not match.
	Dimensionality not checked for speed.
	*/
	template <typename T1, typename T2>
	float distSquared(T1& coords1, T2& coords2) {
		float dsquared = 0;
		for (unsigned i = 0; i < coords1.size(); ++i) {
			dsquared += ((coords1[i] - coords2[i])*(coords1[i] - coords2[i]));
		}
		return dsquared;
	};


	/*
	Find distance between two data points (of equal dimensionality),ignoring
	one of the dimensions.
	Result invalid if container sizes do not match.
	Dimensionality not checked for speed.
	*/
	template <typename T1, typename T2>
	float distSquared(T1& coords1, T2& coords2, unsigned int ignoreDimension) {
		float dsquared = 0;
		for (unsigned i = 0; i < coords1.size(); ++i) {
			if (i == ignoreDimension)continue;
			dsquared += ((coords1[i] - coords2[i])*(coords1[i] - coords2[i]));
		}
		return dsquared;
	};

	/*
	Find distance between two data points (of equal dimensionality), ignoring
	the dimensions within the ignore list;#
	Result invalid if container sizes do not match.
	Dimensionality not checked for speed.
	*/
	template <typename T1, typename T2, typename T3>
	float distSquared(T1& coords1, T2& coords2, T3& ignorelist) {
		float dsquared = 0;
		for (unsigned i = 0; i < coords1.size(); ++i) {
			if (std::find(ignorelist.begin(), ignorelist.end(), i) == ignorelist.end()) continue;
			dsquared += ((coords1[i] - coords2[i])*(coords1[i] - coords2[i]));
		}
		return dsquared;
	};



	//Populate observations with datapoints from a container
	template <typename iterator>
	size_t Populate(iterator begin, iterator end) {
		while (begin != end) {
			observations.push_back(*begin);
			++begin;
		}
		return observations.size();
	};

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
	}

	/*
	template <typename T1, typename Tkeep> 
	inline std::vector<Tkeep> queryPart(DataPoint& dp, unsigned k, T1 beginIterator, T1 endIterator, unsigned int emptyDimension) {
		KeepBest<Tkeep> keep()
		while(beginIterator!=endIterator){
		keeper.insert(distSquared(*beginIterator, dp, emptyDimension), beginIterator);
		++beginIterator;
		}
	}


	void Query_Parallel(DataPoint& dp, unsigned int k, unsigned int emptyDimension, unsigned int nThreads = 0) {
		if (nThreads == 0)nThreads = std::thread::hardware_concurrency();
		//We have nThreads keepns
		std::vector<std::future<KeepBest<typename std::vector<DataPoint>::iterator > > > keepns;
		//And then one into which to combine results
		KeepBest<typename std::vector<DataPoint>::iterator> best(k);
		unsigned int chunksize = observations.size() / nThreads;

		for (unsigned i = 0; i < nThreads - 1; ++i) {
			keepns.push_back(std::async(std::launch::async, &queryPart, dp, keepns[i], observations.begin() + (chunksize*i), observations.begin() + (chunksize*(i + 1)), emptyDimension));
		}
		keepns.push_back(std::async(std::launch::async, &queryPart, dp, keepns[nThreads - 1], observations.begin() + (chunksize*(nThreads - 1)), observations.end(), emptyDimension));

		for (auto k : keepns) {
			for (auto element : k.get()) {
				best.insert(element.first, element.second);
			}
		}

		dp[emptyDimension] = 0;
		for (auto i : best.best)dp[emptyDimension] += i;
		dp[emptyDimension] /= static_cast<float>(best.best.size());
	}
	

	*/

};

