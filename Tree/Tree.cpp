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
		//std::fstream file{"../mylog.dot", std::ios::out};
		//RBTree<int> t;
		//t.insert(10);
		//t.insert(20);
		//t.insert(5);
		//t.insert(15);
		//t.insert(25);
		//t.insert(7);
		//t.insert(8);
		//t.erase(8);
		//t.erase(10);
		//t.dump(file);
	} catch (const std::exception& e) {
		std::cout << "Error: " << e.what() << std::endl;
	}
    return 0;
}
