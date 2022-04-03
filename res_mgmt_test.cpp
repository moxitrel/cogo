#include "res_mgmt.h"
#include "gtest/gtest.h"

TEST(With, CreateFailed)
{
    res_mgmt_leaks_cnt = 0;

    bool doInit = false;
    bool doExit = false;
    bool sk = false;
    bool fk = false;

    WITH(doInit = true, false, doExit = true) {
        sk = true;
    } else {
        fk = true;
    }

    // run fk block
    EXPECT_TRUE (doInit);
    EXPECT_FALSE(doExit);
    EXPECT_FALSE(sk);
    EXPECT_TRUE (fk);
    // no leak
    EXPECT_EQ(RES_MGMT_LEAKS()[0], nullptr);
}

TEST(With, CreateSucceed)
{
    res_mgmt_leaks_cnt = 0;

    bool doInit = false;
    bool doExit = false;
    bool sk = false;
    bool fk = false;

    WITH(doInit = true, true, doExit = true) {
        sk = true;
    } else {
        fk = true;
    }

    // run sk block
    EXPECT_TRUE (doInit);
    EXPECT_TRUE (doExit);
    EXPECT_TRUE (sk);
    EXPECT_FALSE(fk);
    // no leak
    EXPECT_EQ(RES_MGMT_LEAKS()[0], nullptr);
}

TEST(With, Nested)
{
    res_mgmt_leaks_cnt = 0;

    bool doInit1 = false;
    bool doExit1 = false;
    bool sk1 = false;
    bool fk1 = false;
    bool doInit2 = false;
    bool doExit2 = false;
    bool sk2 = false;
    bool fk2 = false;

    WITH(doInit1 = true, true, doExit1 = true) {
        sk1 = true;
        WITH(doInit2 = true, true, doExit2 = true) {
            sk2 = true;
        } else {
            fk2 = true;
        }
    } else {
        fk1 = true;
    }

    // run sk block
    EXPECT_TRUE (doInit1);
    EXPECT_TRUE (doExit1);
    EXPECT_TRUE (doInit2);
    EXPECT_TRUE (doExit2);
    EXPECT_TRUE (sk1);
    EXPECT_TRUE (sk2);
    EXPECT_FALSE(fk2);
    EXPECT_FALSE(fk1);
    // no leak
    EXPECT_EQ(RES_MGMT_LEAKS()[0], nullptr);
}


TEST(With, JumpOutBlockLeak)
{
    res_mgmt_leaks_cnt = 0;

    bool doInit = false;
    bool doExit = false;
    bool sk = false;
    bool fk = false;
    bool afterGoto = false;


    WITH(doInit = true, true, doExit = true) {
        sk = true;
        goto jumpOutBlock;
        afterGoto = true;
    } else {
        fk = true;
    }
jumpOutBlock:

    EXPECT_TRUE (doInit);
    EXPECT_FALSE(doExit);
    EXPECT_TRUE (sk);
    EXPECT_FALSE(afterGoto);
    EXPECT_FALSE(fk);
    // 1 leak
    EXPECT_NE(RES_MGMT_LEAKS()[0], nullptr);
    EXPECT_EQ(RES_MGMT_LEAKS()[1], nullptr);
}

TEST(With, BreakNoLeak)
{
    res_mgmt_leaks_cnt = 0;

    bool doInit = false;
    bool doExit = false;
    bool sk = false;
    bool fk = false;
    bool afterBreak = false;

    WITH(doInit = true, true, doExit = true) {
        sk = true;
        break;
        afterBreak = true;
    } else {
        fk = true;
    }

    EXPECT_TRUE (doInit);
    EXPECT_TRUE (doExit);
    EXPECT_TRUE (sk);
    EXPECT_FALSE(afterBreak);
    EXPECT_FALSE(fk);
    // no leak
    EXPECT_EQ(RES_MGMT_LEAKS()[0], nullptr);
}

TEST(With, EmptyCheck)
{
    res_mgmt_leaks_cnt = 0;

    bool doInit = false;
    bool doExit = false;
    bool sk = false;
    bool fk = false;

    WITH(doInit = true, , doExit = true) {
        sk = true;
    } else {
        fk = true;
    }

    EXPECT_TRUE (doInit);
    EXPECT_TRUE (doExit);
    EXPECT_TRUE (sk);   // empty CHECK is true
    EXPECT_FALSE(fk);
    // no leak
    EXPECT_EQ(RES_MGMT_LEAKS()[0], nullptr);
}

TEST(Defer, Success)
{
    res_mgmt_leaks_cnt = 0;

    bool defered = false;

    DEFER(defered = true) {
        EXPECT_FALSE(defered);
    }
    EXPECT_TRUE(defered);
    // no leak
    EXPECT_EQ(RES_MGMT_LEAKS()[0], nullptr);
}

TEST(Defer, JumpOutBlockLeak)
{
    res_mgmt_leaks_cnt = 0;

    bool defered = false;
    bool afterGoto = false;

    DEFER(defered = true) {
        EXPECT_FALSE(defered);
        goto JumpOutBlock;
        afterGoto = true;
    }
JumpOutBlock:

    EXPECT_FALSE(defered);
    EXPECT_FALSE(afterGoto);
    // 1 leak
    EXPECT_NE(RES_MGMT_LEAKS()[0], nullptr);
    EXPECT_EQ(RES_MGMT_LEAKS()[1], nullptr);
}

TEST(Defer, BreakNoLeak)
{
    res_mgmt_leaks_cnt = 0;

    bool defered = false;
    bool afterBreak = false;

    DEFER(defered = true) {
        EXPECT_FALSE(defered);
        break;
        afterBreak = true;
    }
    EXPECT_TRUE (defered);
    EXPECT_FALSE(afterBreak);
    // no leak
    EXPECT_EQ(RES_MGMT_LEAKS()[0], nullptr);
}
