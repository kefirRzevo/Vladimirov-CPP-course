#include <gtest/gtest.h>

#include "../include/Geometry.hpp"
#include "../include/BoundingBox.hpp"

using namespace triangles;
using namespace compare;

TEST(VectorTest, test0) 
{
	Vector<int> v1{1, 2, 3};
	Vector<int> v2{1, 2, 3};
	EXPECT_EQ(v1, v2);

	Vector<double> v3{-1.0000000001, 2, -1};
	Vector<double> v4{-0.99999999, 2, -1.00000001};
	EXPECT_EQ(v3, v4);

	Vector<double> v5{0.0000001, 0.00000009, 0};
	EXPECT_FALSE(v5);
	EXPECT_TRUE(!v5);

	Vector<float> v6{3., 1., 2.};
	Vector<float> v7{3./std::sqrt(14.), 1./std::sqrt(14.), 2./std::sqrt(14.)};
	EXPECT_EQ(v6.normalize(), v7);

	Vector<float> v8{2., 3., 1.};
	Vector<float> v9{8., -5., 0.};
	EXPECT_TRUE(equals(v8.getDistance(v9), std::sqrt(6.f * 6.f + 8.f * 8.f + 1.f * 1.f)));

	Vector<float> v10{-1., 3., 3.};
	Vector<float> v11{-2., 3., 1.};
	Vector<float> v12{0., 4., 0.};
	EXPECT_TRUE(equals(triple(v10, v11, v12), -20.f));

	Vector<float> v13{3., -3., 1.};
	Vector<float> v14{4., 9., 2.};
	Vector<float> v15{-15., -2., 39.};
	EXPECT_EQ(cross(v13, v14), v15);

	Vector<float> v16{1., 2., 3.};
	Vector<float> v17{4., -5., 6.};
	EXPECT_TRUE(equals(dot(v16, v17), 12.f));

	Vector<float> v18{1., 1., 1.};
	Vector<float> v19{2., 2., 2.};
	Vector<float> v20{2., 2., 2.};
	EXPECT_EQ(v18.project(v19), v20);
}

TEST(LineTest, test1)
{
	Line<float> l1{{-1., 0., 4.}, {-1., 2., 5.}};
	Line<float> l2{{4., -10., -21.}, {-2., 4., 10.}};
	EXPECT_TRUE(l1.equals(l2));

	Line<float> l3{{1., 1., 2.}, {5., 0., 1.}};
	Vector<float> v1{2., 3., 1.};
	EXPECT_TRUE(equals<float>(l3.getDistance(v1), 2.32047740));
	
	Line<float> l4{{2., 1., 4.}, {-1., 1., 0.}};
	Line<float> l5{{-1., 0., 2.}, {5., 1., 2.}};
	EXPECT_TRUE(equals<float>(l4.getDistance(l5), 0.603022));

	Line<float> l6{{-1., 3., 2.}, {-3., 5., 3.}};
	Vector<float> v2{4., -2., 1.};
	Vector<float> v3{2., -2., -1.};
	EXPECT_EQ(l6.project(v2), v3);

	Line<float> l7{{0., 0., 0.}, {1., 1., 1.}};
	Vector<float> v4{0., 0., 0};
	Vector<float> v5{-1., -1., -1.};
	Vector<float> v6{1., -1., -1.};
	auto [flag1, res1] = l7.contains(v4);
	auto [flag2, res2] = l7.contains(v5);
	auto [flag3, res3] = l7.contains(v6);
	EXPECT_TRUE(flag1 == true && equals(res1, 0.f));
	EXPECT_TRUE(flag2 == true && equals(res2, -1.f));
	EXPECT_TRUE(flag3 == false);

	Line<float> l8{{3., 3., 0.}, {1., 1., 0.}};
	Vector<float> v7{3., 3., 0};
	Vector<float> v8{1., 1., 0.};
	Vector<float> v9{2., 2., 0.};
	auto [flag4, res4] = l8.contains(v7);
	auto [flag5, res5] = l8.contains(v8);
	auto [flag6, res6] = l8.contains(v9);
	EXPECT_TRUE(flag4 == true && equals(res4, 0.f));
	EXPECT_TRUE(flag5 == true && equals(res5, -2.f));
	std::cout << flag5 << "\n";
	EXPECT_TRUE(flag6 == true && equals(res6, -1.f));	
}

TEST(PlaneTest, test2)
{
	Plane<float> p1{{1., 3., 2.}, {3., 2., 5.}};
	Plane<float> p2{3., 2., 5., -19.};
	EXPECT_TRUE(p1.equals(p2));

	Plane<float> p3{{1., -2., 1.}, {4., -2., -2.}, {4., 1., 4.}};
	Plane<float> p4{{4., 1., 4.}, {1., -2., 1.}};
	EXPECT_TRUE(p3.equals(p4));

	Plane<float> p5{{0, 0, 0}, {1, 1, 1}};
	Vector<float> v1{6, 6, 6};
	Vector<float> v2{0, 0, 0};
	EXPECT_EQ(p5.project(v1), v2);
	
	Plane<float> p6{3., -1., 4., -1.};
	Vector<float> v3{0., 0., 1./4.};
	Vector<float> v4{2., 1., -3.};
	Vector<float> v5{38./13., 9./13., -23./13.};
	Line<float> l1{v3, v4 - v3};
	Line<float> l2{v3, v5 - v3};
	EXPECT_TRUE(p6.project(l1).equals(l2));

	Plane<float> p7{-2., 1., 2., -7.};
	Vector<float> v6{-5., -8., -6.};
	EXPECT_TRUE(equals<float>(p7.getDistance(v6), 17. / 3.));
}

TEST(TriangleTest, test3)
{
	Vector<float> v1{0, 0, 0};
	Vector<float> v2{1, 1, 0};
	Vector<float> v3{1, 0, 0};
	Triangle<float> t1{v1, v2, v3};
	Triangle<float> t2{v2, v1, v3};
	Triangle<float> t3{v3, v1, v2};
	EXPECT_TRUE(t1 == t1);
	EXPECT_TRUE(t1 != t2);
 	EXPECT_TRUE(t3 != t2);


	Segment<float> seg0{{0, 0, 0}, {2, 2, 2}};
	Segment<float> seg1{{1, 1, 0}, {3, 3, 0}};
	Triangle<float> t4{{1, 1, 0}, {2, 2, 0}, {3, 3, 0}};
	auto [flag1, res1] = t4.isSegment();
	EXPECT_TRUE(flag1 == true && res1.equals(seg1));

	Triangle<float> t5{{0, 0, 0}, {2, 2, 2}, {2, 2, 2}};
	auto [flag2, res2] = t5.isSegment();
	EXPECT_TRUE(flag2 == true && res2.equals(seg0));

	Triangle<float> t6{{0, 0, 0}, {2, 2, 2}, {2, 2, 1}};
	auto [flag3, res3] = t6.isSegment();
	EXPECT_TRUE(flag3 == false);

	Vector<float> v4{0, 0, 0};
	Triangle<float> t7{{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
	auto [flag4, res4] = t7.isPoint();
	EXPECT_TRUE(flag4 == true && res4 == v4);

	Triangle<float> t8{{0, 0, 0}, {0, 0, 0}, {0, 1, 0}};
	auto [flag5, res5] = t8.isPoint();
	EXPECT_TRUE(flag5 == false && res5 == Vector<float>{});


	Vector<float> v5{0.5, 0.2, 0};
	Vector<float> v6{0.5, 0.5, 0};
	Vector<float> v7{1, 1, 1};
	Vector<float> v8{1.5, 1, 0};
	EXPECT_TRUE(t1.contains(v3));
	EXPECT_TRUE(t1.contains(v5));
	EXPECT_TRUE(t1.contains(v6));
	EXPECT_FALSE(t1.contains(v7));
	EXPECT_FALSE(t1.contains(v8));

	Segment<float> seg2{{0, 0, 1}, {0, 0, -1}};
	Segment<float> seg3{{2, -1, 1}, {-1, 2, -1}};
	Segment<float> seg4{{1, 1, 1}, {-1, 2, -1}};
	Segment<float> seg5{{0, 0, 1}, {1, 1, 0}};
	Triangle<float> t9{{0, 0, 0}, {1, 0, 0}, {0, 1, 0}};
	EXPECT_TRUE(t9.intersects(seg2));
	EXPECT_TRUE(t9.intersects(seg3));
	EXPECT_TRUE(t9.intersects(seg4));
	EXPECT_TRUE(t9.intersects(seg5));//*/
}

TEST(FiguresIntersectionTest, test4)
{
	Plane<float> p1{2, 1, -1, -3};
	Plane<float> p2{1, -1, 1, -3};
	Plane<float> p3{2, 1, -1, 10};
	Line<float> l1{{2, -1, 0}, {0, -3, -3}};
	auto res1 = p1.intersects(p2);
	auto res2 = p1.intersects(p1);
	auto res3 = p1.intersects(p3);
	EXPECT_EQ(res1.first, IntersectionType::Intersects);
	EXPECT_TRUE(l1.equals(res1.second));
	EXPECT_EQ(res2.first, IntersectionType::Belongs);
	EXPECT_EQ(res3.first, IntersectionType::Parallels);

	Plane<float> p4{4, 2, -1, -8};
	Plane<float> p5{2, -5, 1, -6};
	Plane<float> p6{1, 1, 3, -4};
	Line<float> l2{{3, 1, 2}, {1, -4, -8}};
	Line<float> l3{{2, 2, 9}, {1, 2, 8}};
	Line<float> l4{{3, -2, 1}, {14, -5, -3}};
	Vector<float> v1{2, 5, 10};
	auto res4 = p4.intersects(l2);
	auto res5 = p5.intersects(l3);
	auto res6 = p6.intersects(l4);
	EXPECT_EQ(res4.first, IntersectionType::Intersects);
	EXPECT_EQ(res4.second, v1);
	EXPECT_EQ(res5.first, IntersectionType::Parallels);
	EXPECT_EQ(res6.first, IntersectionType::Belongs);

	Triangle2<float> t1{{0, 0}, {1, 0}, {1, 1}};
	Triangle2<float> t2{{0, 0}, {1, 0}, {1, 1}};
	EXPECT_TRUE(t1.intersects(t2));

	Triangle2<float> t3{{0, 0}, {1, 0}, {1, 1}};
	Triangle2<float> t4{{0, 0}, {-1, 0}, {-1, -1}};
	EXPECT_TRUE(t3.intersects(t4));

	Triangle2<float> t5{{1, 1}, {2, 1}, {2, 2}};
	Triangle2<float> t6{{0, 0}, {0, 0.5}, {0.5, 0.5}};
	EXPECT_FALSE(t5.intersects(t6));

	Triangle2<float> t7{{0, 0}, {4, 4}, {4, 0}};
	Triangle2<float> t8{{1, 0.5}, {3.5, 3}, {3.5, 0.5}};
	EXPECT_FALSE(t7.intersects(t8));
}

TEST(Vector2Test, test5)
{
	Vector2<float> v2_1{2, 3};
	Vector2<float> v2_2{-2, -3};
	EXPECT_EQ(-v2_1, v2_2);

	Vector2<float> v2_3{3, 4};
	Vector2<float> v2_4{0.6, 0.8};
	EXPECT_EQ(v2_3+=Vector2<float>{}, v2_3);
	EXPECT_EQ(v2_3.normalize(), v2_4);

	Vector2<float> v2_5{4, 3};
	Vector2<float> v2_6{3.9999999, 3.000001};
	EXPECT_EQ(v2_5, v2_6);

	Vector2<float> v2_7{0.0000001, -0.00000000099};
	EXPECT_FALSE(v2_7);
	EXPECT_TRUE(!v2_7);

	Vector2<float> v2_8{4, 3};
	EXPECT_EQ(v2_8.length(), 5);

	Vector2<float> v2_9{4, 3};
	Vector2<float> v2_10{0, 0};
	Vector2<float> v2_11{4, 3};
	Vector2<float> v2_12{2, 1.4999999};
	EXPECT_TRUE(v2_9.collinear(v2_10));
	EXPECT_TRUE(v2_11.collinear(v2_12));

	Vector2<float> v2_13{1, 1};
	Vector2<float> v2_14{0, std::sqrt(2)};
	Vector2<float> v2_15{0, 1};
	EXPECT_EQ(v2_14.project(v2_13), v2_15);
}

TEST(CompareTest, test6)
{
	auto max1 = max(1., 2., 1.2, 0.011, -0.0001);
	EXPECT_EQ(max1, 2.);
	auto max2 = max(1.);
	EXPECT_EQ(max2, 1.);

	float val1 = 0.000019;
	float val2 = 0.000018;
	float val3 = 0.3;
	float val4 = 0.4;
	EXPECT_TRUE(equals(val1, val2));
	EXPECT_TRUE(greaterEq(val1, val2));
	EXPECT_TRUE(lessEq(val2, val1));
	EXPECT_TRUE(greater(val4, val3));
	EXPECT_TRUE(less(val3, val4));

	float val5 = 11.5;
	float val6 = 11.7;
	EXPECT_TRUE(greater(val6, val5));
	EXPECT_TRUE(less(val5, val6));
	EXPECT_TRUE(greaterEq(val6, val5));
	EXPECT_TRUE(lessEq(val5, val6));
	EXPECT_TRUE(!isZero(val5) && !isZero(val6));
}

TEST(Segment1Test, test7)
{
	Segment1<float> s1{1, 2.0000001};
	Segment1<float> s2{1, 2};
	EXPECT_TRUE(s1.equals(s2));

	EXPECT_TRUE(s2.contains(1));
	EXPECT_TRUE(s2.contains(1.5));
	EXPECT_FALSE(s2.contains(0.5));

	Segment1<float> s3{4, 0};
	EXPECT_TRUE(s3.intersects(s2));
	EXPECT_TRUE(s3.intersects(s3));

	Segment1<float> s4{0, 1.5};
	Segment1<float> s5{0, 1};
	EXPECT_TRUE(s4.intersects(s2));
	EXPECT_TRUE(s5.intersects(s2));

	Segment1<float> s6{0, 0.5};
	EXPECT_FALSE(s6.intersects(s2));

	Segment1<float> s7{4.24529, 18.4333};
	Segment1<float> s8{24.5367, 48.3879};
	EXPECT_FALSE(s7.intersects(s8));
}

TEST(Segment2Test, test8)
{
	Vector2<float> v1{1, 1};
	Vector2<float> v2{4, 4};
	Vector2<float> v3{3, 2};
	Vector2<float> v4{2, 3};
	Segment2<float> s1{v1, v2};
	Segment2<float> s2{v4, v3};
	EXPECT_TRUE(s1.intersects(s2));

	Vector2<float> v5{2, 2};
	Vector2<float> v6{1, 4};
	Segment2<float> s3{v6, v5};
	EXPECT_TRUE(s1.intersects(s3));

	Vector2<float> v7{1, 0};
	Vector2<float> v8{5, 1};
	Segment2<float> s4{v7, v8};
	EXPECT_FALSE(s1.intersects(s4));

	Vector2<float> v9{2, 2};
	Vector2<float> v10{3, 3};
	Segment2<float> s5{v10, v9};
	EXPECT_TRUE(s1.intersects(s5));

	Vector2<float> v11{1, 2};
	Vector2<float> v12{3, 4};
	Segment2<float> s6{v11, v12};
	EXPECT_FALSE(s1.intersects(s6));

	Vector2<float> v13{0.5, 0.5};
	Vector2<float> v14{1, 1};
	Vector2<float> v15{2, 2};
	Vector2<float> v16{4, 4};
	Segment2<float> s7{v13, v14};
	Segment2<float> s8{v15, v16};
	EXPECT_FALSE(s7.intersects(s8));

	Vector2<float> v17{2, 2};
	Vector2<float> v18{1, 1};
	Vector2<float> v19{0, 0};
	Vector2<float> v20{2, 1};
	EXPECT_TRUE(s1.contains(v17));
	EXPECT_TRUE(s1.contains(v18));
	EXPECT_FALSE(s1.contains(v19));
	EXPECT_FALSE(s1.contains(v20));
}

TEST(SegmentTest, test9)
{
	Vector<float> v1{1, 1, 1};
	Vector<float> v2{2, 2, 2};
	Vector<float> v3{1.5, 1.5, 1.5};
	Vector<float> v4{0, 3, 1};
	Vector<float> v5{0, 0, 0};
	Segment<float> s1{v1, v2};
	Segment<float> s1_{v2, v1};
	EXPECT_TRUE(s1.contains(v3));
	EXPECT_TRUE(s1.contains(v1));
	EXPECT_FALSE(s1.contains(v4));
	EXPECT_FALSE(s1.contains(v5));

	Vector<float> v6{4, 4, 4};
	Segment<float> s2{v3, v6};
	Segment<float> s3{v3, v3};
	EXPECT_TRUE(s1.intersects(s2));
	EXPECT_TRUE(s1.intersects(s3));

	Vector<float> v7{0, 3, 2};
	Segment<float> s4{v2, v7};
	EXPECT_TRUE(s1.intersects(s4));

	Vector<float> v8{0, 1, 1};
	Vector<float> v9{2, 1, 1};
	Segment<float> s5{v8, v9};
	EXPECT_TRUE(s1.intersects(s5));

	Segment<float> s6{v3, v3};
	EXPECT_TRUE(s1.intersects(s6));
	EXPECT_TRUE(s6.intersects(s6));

	Vector<float> v10{0, 1.5, 1.5};
	Vector<float> v11{2, 1.5, 1.5};
	Segment<float> s7{v10, v11};
	EXPECT_TRUE(s1.intersects(s7));
}

TEST(Triangle2Test, test10)
{
	Vector2<float> v1{0, 0};
	Vector2<float> v2{1, 1};
	Vector2<float> v3{1, 0};
	Triangle2<float> t1{v1, v2, v3};
	Triangle2<float> t2{v2, v1, v3};
	Triangle2<float> t3{v3, v1, v2};
	EXPECT_TRUE(t1.equals(t3));
	EXPECT_TRUE(t1.equals(t2));
 	EXPECT_TRUE(t3.equals(t2));

	Triangle2<float> t4{{0, 0}, {1, 1}, {2, 2}};
	EXPECT_FALSE(t4.valid());
}

TEST(TrianglesIntersectionTest1, test11)
{
	//lines & lines
	Triangle<float> t1{{1, 1, 0}, {2, 2, 0}, {3, 3, 0}};
    Triangle<float> t2{{2, 3, 0}, {1, 4, 0}, {3, 2, 0}};
	Triangle<float> t3{{4, 1, 0}, {5, 0, 0}, {3, 2, 0}};
	Triangle<float> t4{{4, 3, 0}, {5, 4, 0}, {3, 2, 0}};
	Triangle<float> t5{{1, 1, 0}, {2, 2, 0}, {3, 3, 0}};
	EXPECT_TRUE(t1.intersects(t2));
	EXPECT_FALSE(t1.intersects(t3));
	EXPECT_FALSE(t1.intersects(t4));
	EXPECT_TRUE(t1.intersects(t5));
}

TEST(TrianglesIntersectionTest2, test12)
{
	//lines
	Triangle<float> t1{{1, 1, 0}, {2, 2, 0}, {3, 3, 0}};
	Triangle<float> t2{{2, 3, 0}, {2, 2, 0}, {3, 2, 0}};
	EXPECT_TRUE(t1.intersects(t2));

	Triangle<float> t3{{4, 3, 0}, {3, 3, 0}, {3, 4, 0}};
	EXPECT_TRUE(t1.intersects(t3));

	Triangle<float> t4{{4, 3, 0}, {4, 4, 0}, {3, 4, 0}};
	EXPECT_FALSE(t1.intersects(t4));

	Triangle<float> t5{{-1, -1, -1}, {2, 2, 2}, {3, 3, 3}};
	Triangle<float> t6{{-1, -1, 0}, {2, -1, 0}, {-1, 2, 0}};
	EXPECT_TRUE(t5.intersects(t6));
	EXPECT_FALSE(t3.intersects(t5));

	Triangle<float> t7{{1, 1, 1}, {2, 2, 2}, {3, 3, 3}};
	EXPECT_FALSE(t7.intersects(t6));

	Triangle<float> t8{{-1, -1, 1}, {2, -1, 1}, {-1, 2, 1}};
	EXPECT_FALSE(t8.intersects(t6));
}

TEST(TrianglesIntersectionTest3, test13)
{
	//points
	Triangle<float> t1{{1.5, -4, 8.25}, {7, 10, -4.25}, {5, 3, -0.15}};
	Vector<float> v1{-11.84074, -72.09926, 30.60363};
	Triangle<float> t2{v1, v1, v1};
	EXPECT_FALSE(t2.intersects(t1));

	Vector<float> v2{4.54335, 2.99349, 1.15753};
	Triangle<float> t3{v2, v2, v2};
	EXPECT_TRUE(t3.intersects(t1));

	Vector<float> v3{4.54335, 10, 1.15753};
	Triangle<float> t4{v3, v3, v3};
	EXPECT_FALSE(t4.intersects(t1));
}

TEST(TrianglesIntersectionTest4, test14)
{
	//3D
	Vector<float> v1{-106.0469443923, 52.5713734098, 1168.62612723};
    Vector<float> v2{-651.061758726, 168.7717457742, 694.2902541365};
    Vector<float> v3{428.5401392144, -85.9438632779, 733.317083182};
    Vector<float> v4{-411.2836818256, -356.8993505802, 1109.2200394315};
    Vector<float> v5{-286.9602404404, 281.1077706209, 901.6945024073};
    Vector<float> v6{-273.4987522435, -732.4222971763, 626.6145537568};
    Triangle<float> t1{v1, v2, v3};
    Triangle<float> t2{v4, v5, v6};
	EXPECT_TRUE(t1.intersects(t2));

    Vector<float> v7{20.58046, -43.99046, 0};
    Vector<float> v8{-10, -30, 0};
    Vector<float> v9{40.70333, -32.11313, 12.51765};
    Vector<float> v10{14.04051, -20.82335, 0};
    Vector<float> v11{30, -40, 11.89029};
    Vector<float> v12{56.74394, -23.46234, 0};
    Triangle<float> t3{v7, v8, v9};
    Triangle<float> t4{v10, v11, v12};
	EXPECT_TRUE(t3.intersects(t4));

    Vector<float> v13(33.22144, -30.78249, 5.73506);
    Triangle<float> t5{v13, v7, v8};
	EXPECT_TRUE(t5.intersects(t4));

    Triangle<float> t6{v12, v7, v8};
	EXPECT_TRUE(t6.intersects(t4));

    Triangle<float> t7{v12, v11, v8};
	EXPECT_TRUE(t7.intersects(t4));

    Vector<float> v14{23.36736, -14.79033, 5.72534};
    Triangle<float> t8{v14, v7, v8};
	EXPECT_TRUE(t8.intersects(t4));

    Vector<float> v15{10.36104, -17.95187, 5.72534};
    Triangle<float> t9{v15, v7, v8};
	EXPECT_FALSE(t9.intersects(t4));
}

TEST(TrianglesIntersectionTest5, test15)
{
	//2D
    Vector<float> v7 {20.58046, -43.99046, 0};
    Vector<float> v8 {-10, -30, 0};
    Vector<float> v9 {15.61002, -12.31086, 0};
    Vector<float> v10{14.04051, -20.82335, 0};
    Vector<float> v11{30, -40, 0};
    Vector<float> v12{56.74394, -23.46234, 0};
    Triangle<float> t3{v7, v8, v9};
    Triangle<float> t4{v10, v11, v12};
	EXPECT_TRUE(t3.intersects(t4));

    Triangle<float> t5{v7, v11, v12};
	EXPECT_TRUE(t3.intersects(t5));

    Triangle<float> t6{v10, v8, v9};
	EXPECT_TRUE(t3.intersects(t6));

    Vector<float> v13{25.08383, -40.98338, 0};
    Triangle<float> t7{v10, v13, v12};
	EXPECT_TRUE(t3.intersects(t7));

    Vector<float> v14{36.36463, -32.43919, 0};
    Vector<float> v15{33.80259, -25.61689, 0};
    Vector<float> v16{47.729, -25.99418, 0};
    Triangle<float> t8{v14, v15, v16};
	EXPECT_TRUE(t8.intersects(t4));
}

TEST(BoundingBoxTest, test16) 
{
    Triangle<float> t1{{0, 0, 0}, {0, 1, 1}, {1, 0, 1}};
    BoundingBox<float> b1{t1};
    Triangle<float> t2{{0, 1, 0}, {1, 0, 0}, {0, 0, 0}};
    BoundingBox<float> b2{t2};
    EXPECT_TRUE(b1.intersects(b2));
    EXPECT_TRUE(t1.intersects(t2));

    Vector<float> v1{0, 0, 0};
    BoundingBox<float> b3{{-1, -1, -1}, {1, 1, 1}};
    auto [flag1, res1] = b3.intersects(v1);
    EXPECT_TRUE(flag1 == true && res1 == Quadrant::count);

    BoundingBox<float> b4{{0, 0, 1}, {1, 1, 1}};
    auto [flag2, res2] = b4.intersects(v1);
    EXPECT_TRUE(flag2 == true && res2 == Quadrant::count);

    BoundingBox<float> b5{{-2, -1, 1}, {-1, -0.5, 2}};
    auto [flag3, res3] = b5.intersects(v1);
    EXPECT_TRUE(flag3 == false && res3 == Quadrant::_x_yz);

    BoundingBox<float> b6{{3, 1, 1}, {3, 2, 4}};
    auto [flag4, res4] = b6.intersects(v1);
    EXPECT_TRUE(flag4 == false && res4 == Quadrant::xyz);

    BoundingBox<float> b7{{0, 0, 0}, {1, 1, 1}};
    Vector<float> v2{0, 1, 0};
    Vector<float> v3{0.5, 0.5, 0.2};
    Vector<float> v4{1.1, 1.1, 1.2};
    EXPECT_TRUE(b7.contains(v2));
    EXPECT_TRUE(b7.contains(v3));
    EXPECT_FALSE(b7.contains(v4));
}

int main()
{
	::testing::InitGoogleTest();
	return RUN_ALL_TESTS();
}
