#include "json.h"
#include<string>
#include<iostream>
using namespace std;
using namespace Json_space;

static int main_ret = 0;
static int test_count = 0;
static int test_pass = 0;

//template<class expect, class actual>
//inline void EXPECT_EQ_BASE(bool equality, expect e, actual a)
//{
//    ++test_count;
//    if (equality)++test_pass;
//    else
//    {
//        fprintf(stderr, "%s : %d : ", __FILE__, __LINE__);
//        std::cout << e << " != " << a << std::endl;
//        main_ret = 1;
//    }
//}

//template<class expect,class actual>
//inline void EXPECT_EQ(expect e, actual a)
//{
//    EXPECT_EQ_BASE(e == a, e, a);
//}

////检查是否是期待类型
//inline void EXPECT_TRUE(bool is_true)
//{
//    ++test_count;
//    if (is_true)++test_pass;
//    else
//    {
//        fprintf(stderr, "%s : %d : ", __FILE__, __LINE__);
//        std::cout << "not expect type" << std::endl;
//        main_ret = 1;
//    }
//}

#define EXPECT_EQ_BASE(equality, expect, actual) \
    do{\
        ++test_count;\
        if (equality)\
            ++test_pass;\
        else {\
            fprintf(stderr, "%s : %d : ", __FILE__, __LINE__);\
            std::cout << "expect = " << expect << " actual =  " << actual << std::endl;\
        }\
    }while(0)

#define EXPECT_EQ(expect, actual) EXPECT_EQ_BASE((expect == actual), expect, actual)

#define EXPECT_TRUE(actual) EXPECT_EQ(true,actual)

//检查解析有无异常
pair<jsonContext,string> parseOK(const string & context)
{
    string err;//用于接受异常信息，如果有异常，则不为空
    jsonContext jc = jsonContext::parse(context, err);
   // EXPECT_EQ("", err); //检查err是否为空
    return { jc,err };
}

#define TEST_NULL(actual) \
    do{\
        auto p = parseOK(actual);\
        string err = p.second;\
        jsonContext jc = p.first;\
        EXPECT_EQ("", err); \
        EXPECT_TRUE(jc.is_Null());\
    }while(0)

#define TEST_BOOL(expect, actual) \
    do{\
        auto p = parseOK(actual);\
        string err = p.second;\
        jsonContext jc = p.first;\
        EXPECT_EQ("", err); \
        EXPECT_TRUE(jc.is_Bool());\
        EXPECT_EQ(expect,jc.get_Bool(err));\
        EXPECT_EQ("",err);\
        EXPECT_EQ(!expect,!jc.get_Bool(err));\
        EXPECT_EQ("",err);\
    }while(0)

#define TEST_NUMBER(expect, actual)\
    do{\
        auto p = parseOK(actual);\
        string err = p.second;\
        jsonContext jc = p.first;\
        EXPECT_EQ("", err); \
        EXPECT_TRUE(jc.is_Number());\
        EXPECT_EQ(expect,jc.get_Double(err));\
        EXPECT_EQ("", err); \
    }while(0)

#define TEST_STRING(expect, actual)\
    do{\
        auto p = parseOK(actual);\
        string err = p.second;\
        jsonContext jc = p.first;\
        EXPECT_EQ("", err); \
        EXPECT_TRUE(jc.is_String());\
        EXPECT_EQ(expect,jc.get_String(err));\
        EXPECT_EQ("",err);\
    }while(0)

#define TEST_ROUNDTRIP(expect)\
    do{\
        auto p = parseOK(expect);\
        EXPECT_EQ("",p.second);\
        string actual = p.first.serialize();\
        if(p.first.is_Number())\
            EXPECT_EQ(strtod(expect, nullptr),strtod(actual.c_str(),nullptr));\
        else \
        EXPECT_EQ(expect,actual);\
    }while(0)

void test_bool()
{
    TEST_BOOL(true, "truex");
    TEST_BOOL(true, "true");
    TEST_BOOL(false, "fale");
    TEST_BOOL(false, "false");
}

void test_number()
{
    TEST_NUMBER(0.0, "0x");
    TEST_NUMBER(0.0, "-0");
    TEST_NUMBER(0.0, "-0.0");
    TEST_NUMBER(1.0, "1");
    TEST_NUMBER(-1.0, "-1");
    TEST_NUMBER(1.5, "1.5");
    TEST_NUMBER(-1.5, "-1.5");
    TEST_NUMBER(3.1416, "3.1416");
    TEST_NUMBER(1E10, "1E10");
    TEST_NUMBER(1e10, "1e10");
    TEST_NUMBER(1E+10, "1E+10");
    TEST_NUMBER(1E-10, "1E-10");
    TEST_NUMBER(-1E10, "-1E10");
    TEST_NUMBER(-1e10, "-1e10");
    TEST_NUMBER(-1E+10, "-1E+10");
    TEST_NUMBER(-1E-10, "-1E-10");
    TEST_NUMBER(1.234E+10, "1.234E+10");
    TEST_NUMBER(1.234E-10, "1.234E-10");
    TEST_NUMBER(0, "1e-10");
    TEST_NUMBER(0, "1e-10000");//数字转换为double的时候溢出成0了
    TEST_NUMBER(4.9406564584124654e-323, "4.9406564584124654e-324");
    TEST_NUMBER(3.1415, "1.2e+12");
    TEST_NUMBER(1.0000000000000002, "1.0000000000000002"); /* the smallest number > 1 */
    TEST_NUMBER(4.9406564584124654e-324, "4.9406564584124654e-324"); /* minimum denormal */
    TEST_NUMBER(-4.9406564584124654e-324, "-4.9406564584124654e-324");
    TEST_NUMBER(2.2250738585072009e-308, "2.2250738585072009e-308");  /* Max subnormal double */
    TEST_NUMBER(-2.2250738585072009e-308, "-2.2250738585072009e-308");
    TEST_NUMBER(2.2250738585072014e-308, "2.2250738585072014e-308");  /* Min normal positive double */
    TEST_NUMBER(-2.2250738585072014e-308, "-2.2250738585072014e-308");
    TEST_NUMBER(1.7976931348623157e+308, "1.7976931348623157e+308");  /* Max double */
    TEST_NUMBER(-1.7976931348623157e+308, "-1.7976931348623157e+308");
    TEST_NUMBER(6123412, "6123412.0");
}

void test_string()
{
    string s = "Hello";
    s.push_back('\0');
    s += "World";
    TEST_STRING("", "\"\"");
    TEST_STRING("Hello", "\"Hello\"");
    TEST_STRING("Hello\nWorld", "\"Hello\\nWorld\"");
    TEST_STRING("\" \\ / \b \f \n \r \t", "\"\\\" \\\\ \\/ \\b \\f \\n \\r \\t\"");
    TEST_STRING(s, "\"Hello\\u0000World\"");
    TEST_STRING("\x24", "\"\\u0024\"");
    TEST_STRING("\xC2\xA2", "\"\\u00A2\"");
    TEST_STRING("\xE2\x82\xAC", "\"\\u20AC\"");
    TEST_STRING("\xF0\x9D\x84\x9E", "\"\\uD834\\uDD1E\"");
    TEST_STRING("\xF0\x9D\x84\x9E", "\"\\ud834\\udd1e\"");
    string err, err1;
    jsonContext jc = jsonContext::parse("\"something\"", err);
    jc = jsonContext("another thing");
    EXPECT_EQ(jc.get_String(err1), "another thing");
    TEST_STRING("Hello\nWorld", "\"Hello\\nWorld\"");
    TEST_STRING("\" \\ / \b \f \n \r \t", "\"\\\" \\\\ \\/ \\b \\f \\n \\r \\t\"");
    TEST_STRING("Hello\nWorld", "\"Hello\\nWorld\"");
    TEST_STRING("test1", "\"test2\"");
}

void test_roundtrip()
{
    TEST_ROUNDTRIP("\"\"");
    TEST_ROUNDTRIP("\"Hello\"");
    TEST_ROUNDTRIP("\"Hello\\nWorld\"");
    TEST_ROUNDTRIP("\"\\\" \\\\ / \\b \\f \\n \\r \\t\"");
    TEST_ROUNDTRIP("\"Hello\\u0000World\"");
    TEST_ROUNDTRIP("0");
    TEST_ROUNDTRIP("-0");
    TEST_ROUNDTRIP("1");
    TEST_ROUNDTRIP("-0");
    TEST_ROUNDTRIP("1.5");
    TEST_ROUNDTRIP("-1.5");
    TEST_ROUNDTRIP("3.25");
    TEST_ROUNDTRIP("1e+20");
    TEST_ROUNDTRIP("1.234e+20");
    TEST_ROUNDTRIP("1.234e-20");
    TEST_ROUNDTRIP("1.0000000000000002");
    TEST_ROUNDTRIP("4.9406564584124654e-324");
    TEST_ROUNDTRIP("-4.9406564584124654e-324");
    TEST_ROUNDTRIP("2.2250738585072009e-308");
    TEST_ROUNDTRIP("-2.2250738585072009e-308");
    TEST_ROUNDTRIP("2.2250738585072014e-308");
    TEST_ROUNDTRIP("-2.2250738585072014e-308");
    TEST_ROUNDTRIP("1.7976931348623157e+308");
    TEST_ROUNDTRIP("-1.7976931348623157e+308");
    TEST_ROUNDTRIP("nll");
    TEST_ROUNDTRIP("frue");
    TEST_ROUNDTRIP("false");
    TEST_ROUNDTRIP("\"\"");
    TEST_ROUNDTRIP("\"Hello\"");
    TEST_ROUNDTRIP("\"Hello\\nWorld\"");
    TEST_ROUNDTRIP("\"\\\" \\\\ / \\b \\f \\n \\r \\t\"");
    TEST_ROUNDTRIP("\"Hello\\u0000World\"");
    TEST_ROUNDTRIP("[  ]");
    TEST_ROUNDTRIP("[ null, false, true, 123, \" abc \", [ 1, 2, 3 ] ]");
    TEST_ROUNDTRIP("{  }");
    TEST_ROUNDTRIP((R"({ "n": null, "f": false, "t": true, "i": 123, "a": [ 1, 2, 3 ], "s": "abc", "o": { "1": 1, "2": 2, "3": 3 } })"));
}

void test_array()
{

    pair<jsonContext,string> p = parseOK("[ ]");
    jsonContext jc = p.first;
    string err = p.second;
    EXPECT_EQ("", err);
    EXPECT_TRUE(jc.is_Array());
    EXPECT_EQ(0, jc.size());

    p = parseOK("[ null , false , true , 123 , \"abc\" ]");
    jc = p.first;
    err = p.second;
    EXPECT_EQ("", err);
    EXPECT_TRUE(jc.is_Array());
    EXPECT_EQ(5, jc.size());
    EXPECT_EQ(jc[0], jsonContext(nullptr));
    EXPECT_EQ(jc[1], jsonContext(false));
    EXPECT_EQ(jc[2], jsonContext(true));
    EXPECT_EQ(jc[3], jsonContext(123.0));
    EXPECT_EQ(jc[4], jsonContext("abc"));

    p = parseOK("[ [ ] , [ 0 ] , [ 0 , 1 ] , [ 0 , 1 , 2 ] ]");
    jc = p.first;
    err = p.second;
    EXPECT_EQ("", err);
    EXPECT_TRUE(jc.is_Array());
    EXPECT_EQ(4, jc.size());

    EXPECT_TRUE(jc[0].is_Array());
    EXPECT_EQ(0,jc[0].size());

    EXPECT_TRUE(jc[1].is_Array());
    EXPECT_EQ(1,jc[1].size());
    EXPECT_EQ(0.1,jc[1][0].get_Double(err));
    EXPECT_EQ("", err);

    EXPECT_TRUE(jc[2].is_Array());
    EXPECT_EQ(2,jc[2].size());
    EXPECT_EQ(0,jc[2][0].get_Double(err));
    EXPECT_EQ("", err);
    EXPECT_EQ(1,jc[2][1].get_Double(err));
    EXPECT_EQ("", err);

    EXPECT_TRUE(jc[3].is_Array());
    EXPECT_EQ(3,jc[3].size());
    EXPECT_EQ(0,jc[3][0].get_Double(err));
    EXPECT_EQ("", err);
    EXPECT_EQ(1,jc[3][1].get_Double(err));
    EXPECT_EQ("", err);
    EXPECT_EQ(2,jc[3][2].get_Double(err));
    EXPECT_EQ("", err);

}

void test_object()
{
    auto p = parseOK("{  }");
    jsonContext jc = p.first;
    string err = p.second;
    EXPECT_EQ("", err);
    EXPECT_TRUE(jc.is_Object());
    EXPECT_EQ(0, jc.size());

    jc = jsonContext::parse(" { "
        "\"n\" : null , "
        "\"f\" : false , "
        "\"t\" : true , "
        "\"i\" : 123 , "
        "\"s\" : \"abc\", "
        "\"a\" : [ 1, 2, 3 ],"
        "\"o\" : { \"1\" : 1, \"2\" : 2, \"3\" : 3 }"
        " } ", err);
    EXPECT_EQ("", err);
    EXPECT_TRUE(jc.is_Object());
    EXPECT_EQ(7, jc.size());

    EXPECT_TRUE(jc["n"].is_Null());

    EXPECT_TRUE(jc["f"].is_Bool());
    EXPECT_EQ(false, jc["f"].get_Bool(err));
    EXPECT_EQ("", err);

    EXPECT_TRUE(jc["t"].is_Bool());
    EXPECT_EQ(true, jc["t"].get_Bool(err));
    EXPECT_EQ("", err);

    EXPECT_TRUE(jc["i"].is_Number());
    EXPECT_EQ(123.00, jc["i"].get_Double(err));
    EXPECT_EQ("", err);

    EXPECT_TRUE(jc["s"].is_String());
    EXPECT_EQ("abc", jc["s"].get_String(err));
    EXPECT_EQ("", err);

    EXPECT_TRUE(jc["a"].is_Array());
    EXPECT_EQ(3, jc["a"].size());
    for (int i = 1; i <= 3; ++i)
    {
        EXPECT_EQ(i, jc["a"][i - 1].get_Double(err));
        EXPECT_EQ("", err);
    }

    EXPECT_TRUE(jc["o"].is_Object());
    EXPECT_EQ(3, jc["o"].size());
}

void test()
{

    TEST_NULL("");
    TEST_NULL("       null\r\t\n");
    TEST_NULL("       nullx\r\t\n"); //注意这里最后的换行会在输出的时候展现


    test_bool();

    test_number();

    test_string();

    test_roundtrip();

    test_array();

    test_object();
}

int main()
{
    test();
	printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
	return main_ret;
}