#pragma once

/*

This file contains function, that checks end_to_end tests.

*/

#include "Tree.hpp"

namespace tree
{

inline std::vector<int> process(std::istream& in) {
	in.exceptions(in.failbit);
	RBTree<int> tree;
	//std::fstream log{"../mylog.dot", std::ios::out};
	std::vector<int> output;

	while (!in.eof()) {
		char cmd = 0;
		in >> cmd;
		if (cmd == 'k') {
			int key = 0;
			in >> key;
			tree.insert(key);
		} else if (cmd == 'q') {
			int lowerBound = 0;
			int upperBound = 0;
			in >> lowerBound >> upperBound;
			if (lowerBound > upperBound) {
				throw std::logic_error("Lower bound > upper bound");
			}
			output.push_back(0);
			auto lowIt = tree.upper_bound(lowerBound);
			auto upIt = tree.lower_bound(upperBound);
			output[output.size() - 1] = tree.distance(lowIt, upIt);
		} else if (cmd == 'e') {
			break;
		} else {
			throw std::out_of_range(std::string("Unknown command ") + cmd);
		}
	}
	//tree.dump(log);

	return output;
}

} //namespace tree
