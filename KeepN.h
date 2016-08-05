//#KeepN - working version 1
#pragma once
#include <list>
#include <utility>
#include <cfloat>

template<class T>
class KeepNAscending
{
private:
	unsigned int sortsize;
public:
	float cutoff = FLT_MAX;
	std::list < std::pair<float, T> > best;
	explicit KeepNAscending(const unsigned int ssize) {
		sortsize = ssize;
	};
	void insert(const T input, const float score) {
		if (score > cutoff) return;
		for (auto&& k = best.begin(); k != best.end(); k++) {
			if (score < std::get<0>(*k)) {
				best.insert(k, std::make_pair(score, input));
				if (best.size() > sortsize)best.pop_back();
				if (best.size() == sortsize)cutoff = std::get<0>(best.back());
				return;
			}
		}
		best.push_back(std::make_pair(score, input));
		if (best.size() > sortsize) best.pop_back();
		if (best.size() == sortsize) cutoff = std::get<0>(best.back());
	};
};

template<class T>
class KeepNDescending
{
private:
	unsigned int sortsize;
public:
	float cutoff = -FLT_MAX;
	std::list < std::pair<float, T> > best;
	explicit KeepNDescending(const unsigned int ssize) {
		sortsize = ssize;
	};
	void insert(const T input, const float score) {
		if (score < cutoff) return;
		for (auto&& k = best.begin(); k != best.end(); k++) {
			if (score > std::get<0>(*k)) {
				best.insert(k, std::make_pair(score, input));
				if (best.size() > sortsize)best.pop_back();
				if (best.size() == sortsize)cutoff = std::get<0>(best.back());
				return;
			}
		}
		best.push_back(std::make_pair(score, input));
		if (best.size() > sortsize) best.pop_back();
		if (best.size() == sortsize) cutoff = std::get<0>(best.back());
	};
};
