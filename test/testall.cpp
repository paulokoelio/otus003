#include "version.h"
#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <vector>

#include "ip_filter.h"
#include "ipf_basefncl.h"

TEST(testall, fn_version)
{
    const char *first = PROJECT_VERSION;
    const char *second = ipf::version();

    std::string ver(second);

    EXPECT_STREQ(first, second);
    EXPECT_STRCASEEQ(first, second);
    EXPECT_GE(ver.length(), 5);
}

TEST(testall, fn_split)
{
    std::vector<std::string> line{};
    std::vector<std::string> lineres{};
    std::string str1("185.89.100.249\t752\t0");
    std::string str2("185.89.100.249");
    std::string str3("185:89:100:249:752:0");
    std::string res1{};
    std::string res2{};
    std::string res3{};
    std::string res4{};
    std::string res5{};
    char d = '\t';
    size_t parts = 4;

    auto vprint = [](const std::vector<std::string> &vstr, std::string &rstr) {
        rstr.clear();
        for (const auto &it : vstr)
        {
            rstr += it;
        }
        return 0;
    };

    // tests: default value of (parts) =1 and using delimiter = TAB
    line = ipf::split(str1, d);
    vprint(line, res1);
    EXPECT_STREQ(res1.c_str(), "185.89.100.249");

    // tests: when (parts) is bigger than actual number of words
    ASSERT_NO_THROW({ line = ipf::split(str1, d, parts); });
    vprint(line, res2);
    EXPECT_STREQ(res2.c_str(), "185.89.100.2497520");

    // tests: using delimiter '.' and (parts) is equal to number of words
    d = '.';
    line = ipf::split(str2, d, parts);
    vprint(line, res3);
    EXPECT_STREQ(res3.c_str(), "18589100249");

    // tests: using delimiter (':') different from basic proposal
    d = ':';
    line = ipf::split(str3, d, parts);
    vprint(line, res4);
    EXPECT_STREQ(res4.c_str(), "18589100249");

    // tests: behavior of function with extremal values;
    ASSERT_NO_THROW({ line = ipf::split(str3, d, 0); });
    EXPECT_EQ(line.size(), 0);
    d = '\0';
    ASSERT_NO_THROW({ line = ipf::split(str3, d, std::string::npos); });
    vprint(line, res5);
    EXPECT_STREQ(res5.c_str(), "185:89:100:249:752:0");
}

// in structures we test: writing and reading data to/from structures, keeping correct values
// and initialising to default values for some elements

TEST(testall, cl_parsedata)
{
    ipf::parsedata xx;
    xx.original_order = std::string::npos - 1;
    xx.ipstr = "test string";
    xx.ipbytes = {-256, 0, 255, 300};

    EXPECT_EQ(xx.original_order, std::string::npos - 1);
    EXPECT_STREQ(xx.ipstr.c_str(), "test string");
    EXPECT_EQ(xx.ipbytes.at(0), -256);
    EXPECT_EQ(xx.ipbytes.at(3), 300);
    EXPECT_EQ(xx.skipped, false);
    EXPECT_EQ(xx.errorcode, 0);
}

TEST(testall, cl_filteredata)
{
    ipf::filteredata xx;
    xx.mask = {1, -1, 255, 0, -300};
    xx.orig_order = {std::string::npos - 1, 0, 1};

    EXPECT_EQ(xx.orig_order.at(0), std::string::npos - 1);
    EXPECT_EQ(xx.orig_order.at(2), 1);
    EXPECT_EQ(xx.mask.at(1), -1);
    EXPECT_EQ(xx.mask.at(2), 255);
    EXPECT_EQ(xx.mask.at(4), -300);
}

TEST(testall, fn_convertoip)
{
    std::vector<std::string> xtxt1 = {"100", "1", "1"};
    std::vector<std::string> xtxt2 = {"100", "1", "1", "-3"};
    std::vector<std::string> xtxt3 = {"300", "1", "1", "255"};
    std::vector<std::string> xtxt4 = {"255", "0A", "3", "255"};
    std::vector<std::string> xtxt5 = {"255", "0", "001", "255"};
    std::vector<int> bytes;
    bytes.resize(1);
    size_t res, res1;
    bool fail;
    std::string rstr{};

    auto vprint = [](const std::vector<int> &vstr, std::string &rstr) {
        rstr.clear();
        for (const auto &it : vstr)
        {
            rstr += std::to_string(it);
        }
        return 0;
    };

    EXPECT_NO_THROW(res1 = ipf::convertoip(xtxt1, ipf::ipver::IPV4, bytes, fail, res));
    EXPECT_EQ(fail, true);
    EXPECT_EQ(res, 3);
    EXPECT_EQ(res, res1);

    EXPECT_NO_THROW(res1 = ipf::convertoip(xtxt2, ipf::ipver::IPV4, bytes, fail, res));
    EXPECT_EQ(fail, true);
    EXPECT_EQ(res, 2);
    EXPECT_EQ(res, res1);

    EXPECT_NO_THROW(ipf::convertoip(xtxt3, ipf::ipver::IPV4, bytes, fail, res));
    EXPECT_EQ(fail, true);
    EXPECT_EQ(res, 2);

    EXPECT_NO_THROW(ipf::convertoip(xtxt4, ipf::ipver::IPV4, bytes, fail, res));
    EXPECT_EQ(fail, false);
    EXPECT_EQ(res, 0);
    vprint(bytes, rstr);
    EXPECT_STREQ(rstr.c_str(), "25503255"); // in the future this is abnormal result - conversion must fail with error = 2

    ipf::convertoip(xtxt5, ipf::ipver::IPV4, bytes, fail, res);
    EXPECT_EQ(fail, false);
    EXPECT_EQ(res, 0);
    vprint(bytes, rstr);
    EXPECT_STREQ(rstr.c_str(), "25501255");
}

TEST(testall, fn_maskpassed)
{
    std::vector<int> ip1 = {1, 46, 100, 200};
    std::vector<int> ip2 = {1, 46, 46, 200};
    std::vector<int> ip3 = {46, 72, 100, 200};
    std::vector<int> ip4 = {46, 10, 100, 200};
    std::vector<int> ip5 = {46, 72, 1};
    std::vector<int> mask1 = {-1, 1, 46, 100, 200};
    std::vector<int> mask2 = {1, -1, 46, 100, -1};
    std::vector<int> mask3 = {0, 46, 46, 46, 46};
    std::vector<int> mask4 = {0, 1};
    std::vector<int> mask5 = {1, 46, 72};
    bool res1, res2, res3;

    EXPECT_NO_THROW(res1 = ipf::maskpassed(mask1, ip1));
    EXPECT_EQ(res1, false);

    res1 = ipf::maskpassed(mask2, ip1);
    res2 = ipf::maskpassed(mask2, ip2);
    EXPECT_EQ(res1, true);
    EXPECT_EQ(res2, false);

    res1 = ipf::maskpassed(mask3, ip1);
    res2 = ipf::maskpassed(mask3, ip3);
    EXPECT_EQ(res1, true);
    EXPECT_EQ(res2, true);

    EXPECT_NO_THROW(res1 = ipf::maskpassed(mask4, ip2));
    EXPECT_ANY_THROW(res2 = ipf::maskpassed(mask4, ip3)); // temporarily until function is corrected (see \todo in source code )
    EXPECT_EQ(res1, true);

    EXPECT_NO_THROW(res1 = ipf::maskpassed(mask5, ip1));  // this works only because result is false, do not use
                                                          // short mask! untill corrections
    EXPECT_ANY_THROW(res2 = ipf::maskpassed(mask5, ip3)); // temporarily until function is corrected (see \todo in source code )
    EXPECT_ANY_THROW(res3 = ipf::maskpassed(mask5, ip5)); // temporarily until function is corrected (see \todo in source code )
    EXPECT_EQ(res1, false);
    UNUSED(res3); // just to cancel warning "unused variable"
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}