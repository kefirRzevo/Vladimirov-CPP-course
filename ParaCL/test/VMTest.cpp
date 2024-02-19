#include <gtest/gtest.h>
#include <sstream>
#include "backend/VirtualMachine.hpp"

using namespace paracl;

TEST(VMTest, arithmeticTest) {
    Image i;
    i.addConst<ConstInt>(700);
    i.addConst<ConstInt>(11);
    i.addConst<ConstStr>("alallal");
    i.addInstr<iPushAddr>(i.getConstAddr(0U));
    i.addInstr<iPushVal>(302);
    i.addInstr<iAdd>();
    i.addInstr<iOut>();
    i.addInstr<Hlt>();

    std::stringstream is;
    std::stringstream os;
    VirtualMachine m{is, os};
    m.loadImage(std::move(i));
    m.execute();
    os.seekg(0, std::ios::beg);
    int sum = 0;
    os >> sum;
    EXPECT_TRUE(sum == 1002);
}

TEST(VMTest, unSubTest) {
    Image i;
    i.addInstr<iPushVal>(0);
    i.addInstr<iPushVal>(302);
    i.addInstr<iSub>();
    i.addInstr<iOut>();
    i.addInstr<Hlt>();

    std::stringstream is;
    std::stringstream os;
    VirtualMachine m{is, os};
    m.loadImage(std::move(i));
    m.execute();
    os.seekg(0, std::ios::beg);
    int sum = 0;
    os >> sum;
    EXPECT_TRUE(sum == -302);
}

int main()
{
    try {
        ::testing::InitGoogleTest();
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
    }
    return RUN_ALL_TESTS();
}
