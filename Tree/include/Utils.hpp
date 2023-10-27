#pragma once

/*

This file contains function, that checks end_to_end tests.

*/

#include <set>
#include <vector>
#include "Tree.hpp"

namespace tree
{

inline std::vector<int> myProcess(std::istream& in) {
	RBTree<int> tree;
	std::vector<int> output;

	char cmd = 0;
	while (in >> cmd) {
		if (cmd == 'k') {
			int key = 0;
			in >> key;
			tree.insert(key);
		} else if (cmd == 'q') {
			int lowerBound = 0;
			int upperBound = 0;
			in >> lowerBound >> upperBound;
			if (lowerBound >= upperBound) {
				output.push_back(0);
				continue;
			}
			auto lowIt = tree.lower_bound(lowerBound);
			auto upIt = tree.upper_bound(upperBound);
			output.push_back(tree.distance(lowIt, upIt));
		} else if (cmd == 'e') {
			break;
		} else {
			throw std::out_of_range(std::string("Unknown command ") + cmd);
		}
	}
	return output;
}

inline std::vector<int> stdProcess(std::istream& in) {
	std::set<int> tree;
	std::vector<int> output;

	char cmd = 0;
	while (in >> cmd) {
		if (cmd == 'k') {
			int key = 0;
			in >> key;
			tree.insert(key);
		} else if (cmd == 'q') {
			int lowerBound = 0;
			int upperBound = 0;
			in >> lowerBound >> upperBound;
			if (lowerBound >= upperBound) {
				output.push_back(0);
				continue;
			}
			auto lowIt = tree.lower_bound(lowerBound);
			auto upIt = tree.upper_bound(upperBound);
			output.push_back(std::distance(lowIt, upIt));
		} else if (cmd == 'e') {
			break;
		} else {
			throw std::out_of_range(std::string("Unknown command ") + cmd);
		}
	}
	return output;
}

inline std::vector<int> result(std::istream& in) {
	std::vector<int> output;

	int elem;
	while (in >> elem) {
		output.push_back(elem);
	}
	return output;
}

} //namespace tree