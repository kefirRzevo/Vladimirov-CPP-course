#include "include/Utils.hpp"
#include "include/Tree.hpp"

using namespace tree;

int main()
{
    try {
		auto res = process(std::cin);
		for (size_t i = 0; i < res.size(); ++i) {
			std::cout << res[i] << " ";
		}
		std::cout << std::endl;
		/*
		RBTree<int> t;
		std::fstream log{"../mylog.dot", std::ios::out};
		t.insert(8);
		t.insert(18);
		t.insert(5);
		t.insert(15);
		t.insert(17);
		t.insert(25);
		t.insert(40);
		t.insert(80);
		t.dump(log);*/

	} catch (const std::exception& e) {
		std::cout << "Error: " << e.what() << std::endl;
	}
    return 0;
}
