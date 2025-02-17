//
// Created by antimatter on 6/9/20
//

#include "doctest.h"
#include "csys/system.h"
#include <cstring>

TEST_CASE("String Argument")
{
	using namespace csys;
	System s;

// CORRECT USAGE SINGLE WORD
	std::string strt;
    s.RegisterCommand("0", "", [&strt](std::string str)
    {
        strt = str;
//			std::cout << "String -> " << str.m_String << std::endl;
    }, Arg<std::string>(""));

    s.RegisterCommand("2", "", [](std::string str)
    {
                CHECK(str == "Two");
    }, Arg<std::string>(""));

    s.RegisterCommand("3", "", [](std::string str)
    {
                CHECK(str == "");
    }, Arg<std::string>(""));

    s.RunCommand("0 \" \""); // Zero\] -> Zero\]
	CHECK((strt == " "));

	// single word strings
    s.RunCommand("0 Zero\\]"); // Zero\] -> Zero\]
	CHECK((strt == "Zero]"));
	strt.clear();

    s.RunCommand("0 \"Zero\\\"\""); // Zero\] -> Zero\]
  CHECK((strt == "Zero\""));
	strt.clear();

    s.RunCommand("0 \"Zero \\\" \\\\\""); // Zero\] -> Zero\]
					CHECK((strt == "Zero \" \\"));
	strt.clear();

    s.RunCommand("0 \"Zero\"\"One\"\"    #    \""); // Zero\] -> Zero\]
					CHECK((strt == "ZeroOne    #    "));
	strt.clear();

// CORRECT USAGE MANY WORDS
    s.RegisterCommand("0,1", "", [](std::string str, std::string str1)
    {
        bool zero = str == "Zero";
        bool one = str1 == "One";
                CHECK(zero);
                CHECK(one);
    }, Arg<std::string>(""), Arg<std::string>(""));

	// multi word strings
    s.RunCommand("0,1 \"Zero\" \"One\"");
    s.RunCommand("0,1     Zero    One    ");

// CORRECT USAGE VECTOR OF MULTI WORD(S)
    s.RegisterCommand("0,1,2", "", [](std::vector<std::string> strs)
    {
        std::string ar[] = {"Zero", "One", "Two"};
//		for(auto &str: strs) std::cout << str << std::endl;
        for (unsigned i = 0; i < 3; ++i)
            if (strs[i] != ar[i])
            {
//				std::cout << "CHECK: " << strs[i].m_String << " != " << ar[i] << std::endl;
                        CHECK(false);
                return;
            }
                CHECK(true);
    }, Arg<std::vector<std::string>>(""));

	// multi word strings
    s.RunCommand("0,1,2 [  \"Zero\" \"One\" \"Two\"   ]");

// CORRECT USAGE VECTOR OF VECTOR OF MULTI WORD(S)
    s.RegisterCommand("0,1,2,3", "", [](std::vector<std::vector<std::string>>)
    {
        std::vector<std::vector<std::string>> arr = {{"One",    "Two"},
                                                     {" |Three| |Yeet|"},
                                                     {" Four]", "FIVE?"}};
//		CHECK((strs[0][0].m_String == arr[0][0]));
//		CHECK((strs[0][1].m_String == arr[0][1]));
//		CHECK((strs[1][0].m_String == arr[1][0]));
//		CHECK((strs[2][0].m_String == arr[2][0]));
//		CHECK((strs[2][1].m_String == arr[2][1]));
    }, Arg<std::vector<std::vector<std::string>>>(""));

	// multi word strings
    s.RunCommand("0,1,2,3 [ [\"Arg\"] ]");

    s.RegisterCommand("vecvecvec", "", [](std::vector<std::vector<std::vector<std::string>>> strs)
    {
        bool c = strs[0][0][0] == " " && strs[1][0][0] == "Arg";
                CHECK(c);
    }, Arg<std::vector<std::vector<std::vector<std::string>>>>(""));

    s.RunCommand("vecvecvec  [ \
														[ \
															[\" \"] \
														]     \
									  				[ [\"Arg\"] ] ]");
    s.RegisterCommand("char0", "", [](char c)
    { CHECK(c == '"'); }, Arg<char>(""));
    s.RegisterCommand("char1", "", [](char c)
    { CHECK(c == '"'); }, Arg<char>(""));
    s.RegisterCommand("char2", "", [](char c)
    { CHECK(c == '\\'); }, Arg<char>(""));
    s.RegisterCommand("char3", "", [](char c)
    { CHECK(c == 'a'); }, Arg<char>(""));
    s.RegisterCommand("char4", "", [](char c)
    { CHECK(c == 'b'); }, Arg<char>(""));

	// multi word strings
//	s.runCommand("char0 \"");   // \ issue
//	s.runCommand("char0 ");   // \ issue
//	s.runCommand("char0 a\"");  // too many
//	s.runCommand("char0 aa");   // too many
//	s.runCommand("char0 aaa");   // too many
//	s.runCommand("char1 \\\""); // " good
//	s.runCommand("char2 \\\\"); // \ good
//	s.runCommand("char3 a");    // a good
//	s.runCommand("char4 b");    // b good
}