#include "include/Utils.hpp"
#include "include/Tree.hpp"

using namespace tree;

int main()
{
    try {
		/*
		auto res = myProcess(std::cin);
		for (size_t i = 0; i < res.size(); ++i) {
			std::cout << res[i] << " ";
		}
		std::cout << std::endl;
		//*/
		RBTree<int> t;
		std::fstream out{"../mylog.dot", std::ios::out};

		
		/*
		std::fstream in{"../tests/001.dat", std::ios::in};
		int key = 0;
		while (in >> key) {
			t.insert(key);
		}
		t.dump(out);
		out.flush();
		//*/
		//*
		t.insert(10);
		t.insert(20);
		t.insert(13);
		t.insert(17);
		t.insert(49);
		t.insert(42);
		t.insert(3);
		t.insert(93);
		t.insert(27);
		t.insert(16);
		t.insert(12);
		//t.dump(out);
		//t.textDump();
		int del = 0;
		std::cin >> del;
		t.erase(del);
		t.dump(out);
		//t.textDump();
		//*/
	} catch (const std::exception& e) {
		std::cout << "Error: " << e.what() << std::endl;
	}
    return 0;
}
