#pragma once


#include "json.h"
#include "jsonException.h"

namespace Json_space {

	//用以判断字符是否是1-9的字符或者是0-9
	constexpr inline bool is_1_to_9(char ch) { return ch >= '1' && ch <= '9'; }
	constexpr inline bool is_0_to_9(char ch) { return ch >= '0' && ch <= '9'; }

	//解析类
	class parser {
	private:
		const char* curr; //指向当前文本

	public:
		//常规构造
		explicit parser(const char* cstr)noexcept :curr(cstr) {}
		//接受string的构造
		explicit parser(const std::string& context)noexcept :curr(context.c_str()) {}

		//解析类不应该被拷贝赋值
		parser(const parser&) = delete;
		parser& operator=(const parser&) = delete;

	private:
		//跳过空白字符
		void parseWhitespace()noexcept;
		//用于抛出异常信息的函数
		void error(const std::string& msg)const;
		unsigned parse4hex();
		std::string encodeUTF8(unsigned u)noexcept;
		std::string parseRawString();

		jsonContext parseValue();
		jsonContext parseLiteral(const std::string& literal);
		jsonContext parseNumber();
		jsonContext parseString();
		jsonContext parseArray();
		jsonContext parseObject();

	public:
		//唯一 的parse公共接口
		jsonContext parse();
	};
}