#include <unordered_set>
#include <iostream>
#include <vector>

#include "include/OctroTree.hpp"
#include "include/Triangle.hpp"

using namespace triangles;

int main()
{
	std::vector<Triangle<float>> triangles{};
	size_t numTriangles = 0;
	std::cin >> numTriangles;
	for(size_t i = 0; i < numTriangles; ++i)
	{
		Triangle<float> temp{};
		std::cin >> temp.v1_.x_ >> temp.v1_.y_ >> temp.v1_.z_;
		std::cin >> temp.v2_.x_ >> temp.v2_.y_ >> temp.v2_.z_;
		std::cin >> temp.v3_.x_ >> temp.v3_.y_ >> temp.v3_.z_;
		triangles.push_back(temp);
	}

	OctoTree<float> tree{};
	std::unordered_set<size_t> result{};
	for(size_t i = 0; i < triangles.size(); ++i)
	{
		tree.insert(NodeValue{triangles[i], BoundingBox<float>{triangles[i]}, i});
	}
	tree.getIntersections(result);

	for(const auto& value: result)
	{
		std::cout << value << " ";
	}
	std::cout << std::endl;

    return 0;
}
