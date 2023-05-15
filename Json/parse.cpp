#include "parse.h"
#include <cassert> //assert
#include <cmath> //Huge_Val
#include <cstdlib> //strtod
#include <cstring> //strncmp
#include <stdexcept> //runtime_error

namespace Json_space {


	jsonContext parser::parse()
	{
		parseWhitespace();
		jsonContext jc = parseValue();
		parseWhitespace();
		if (*curr)//�������ֵ��˵��������json�﷨����
			error("ROOT NOT SINGULAR");
		return jc;
	}
	
	void parser::parseWhitespace() noexcept
	{
		const char* p = curr;
		while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n')
			++p;
		curr = p;
	}

	void parser::error(const std::string& msg) const
	{
		throw jsonException(msg + ": " + curr);
	}

	jsonContext parser::parseValue()
	{
		switch (*curr)
		{
			case 'n':
				return parseLiteral("null");
			case 't':
				return parseLiteral("true");
			case 'f':
				return parseLiteral("false");
			case '\"':
				return parseString();
			case '[':
				return parseArray();
			case '{':
				return parseObject();
			case '\0':
				error("EXPECT VALUE");
			default:
				return parseNumber();
		}
	}

	jsonContext parser::parseLiteral(const std::string& literal)
	{
		assert(*curr == literal[0]);
		++curr;
		size_t i = 0;
		for (i; literal[i + 1]; ++i)
			if (curr[i] != literal[i + 1])
				error("INVALID VALUE");
		curr += i;
		switch (literal[0])
		{
			case 't':
				return jsonContext(true);
			case 'f':
				return jsonContext(false);
			default:
				return jsonContext(nullptr);
		}
	}

	jsonContext parser::parseNumber()
	{
		const char* p = curr;

		if (*p == '-')++p;

		//�ж���������
		if (*p == '0')++p;
		else
		{
			if (!is_1_to_9(*p))error("INVALIE VALUE");
			for (++p; is_0_to_9(*p); ++p);
		}

		//�ж�С������
		if (*p == '.')
		{
			++p;
			if (!is_0_to_9(*p))error("INVALID VALUE");
			for (++p; is_0_to_9(*p); ++p);
		}

		//�жϿ�ѧ����������
		if (*p == 'e' || *p == 'E')
		{
			++p;
			if (*p == '+' || *p == '-')++p;
			if (!is_0_to_9(*p))error("INVALIE VALUE");
			for (++p; is_0_to_9(*p); ++p);
		}

		//�������Ķ��ɹ��ˣ�˵������ת��Ϊ������
		double val = strtod(curr, nullptr);
		if (val == HUGE_VAL || val == -HUGE_VAL)error("NUMBER TOO BIG");
		curr = p;
		return jsonContext(val);
	}

	//�ж���λʮ���������Ƿ�Ϸ�
	unsigned parser::parse4hex()
	{
		unsigned u = 0;
		for (int i = 0; i != 4; ++i)
		{
			auto ch = static_cast<unsigned>(toupper(*++curr));
			u <<= 4;
			if (ch >= '0' && ch <= '9')
			{
				u |= (ch - '0');
			}
			else if (ch >= 'A' && ch <= 'F')
			{
				u |= ch - 'A' + 10;
			}
			else error("INVALIT UNICODE HEX");
		}
		return u;
	}

	//����UTF-8�����ʵ��
	std::string parser::encodeUTF8(unsigned u) noexcept
	{
		std::string utf8;
		if (u <= 0x7F) {  
			utf8.push_back(static_cast<char>(u & 0xff));
		}
		else if (u <= 0x7FF) {
			utf8.push_back(static_cast<char>(0xc0 | ((u >> 6) & 0xff)));
			utf8.push_back(static_cast<char>(0x80 | (u & 0x3f)));
		}
		else if (u <= 0xFFFF) {
			utf8.push_back(static_cast<char>(0xe0 | ((u >> 12) & 0xff)));
			utf8.push_back(static_cast<char>(0x80 | ((u >> 6) & 0x3f)));
			utf8.push_back(static_cast<char>(0x80 | (u & 0x3f)));
		}
		else {
			assert(u <= 0x10FFFF);
			utf8.push_back(static_cast<char>(0xf0 | ((u >> 18) & 0xff)));
			utf8.push_back(static_cast<char>(0x80 | ((u >> 12) & 0x3f)));
			utf8.push_back(static_cast<char>(0x80 | ((u >> 6) & 0x3f)));
			utf8.push_back(static_cast<char>(0x80 | (u & 0x3f)));
		}
		return utf8;
	}

	std::string parser::parseRawString()
	{ 
		//�����ַ���ʵ�����ǻ�ȡ����˫����֮�������
		std::string str;
		while (true)
		{
			switch (*++curr)
			{
				case '\"': //������һ�����ַ�����ĩβ
					++curr;
					return str;
				case '\0':
					error("MISS QUOTATION MARK");
				default:
					if (static_cast<unsigned char>(*curr) < 0x20)
						error("INVALID STRING CHAR");
					str.push_back(*curr);
					break;
				case '\\':
					switch (*++curr)
					{
						case '\"':
							str.push_back('\"');
							break;
						case '\\':
							str.push_back('\\');
							break;
						case '/':
							str.push_back('/');
							break;
						case 'b':
							str.push_back('\b');
							break;
						case 'f':
							str.push_back('\f');
							break;
						case 'n':
							str.push_back('\n');
							break;
						case 't':
							str.push_back('\t');
							break;
						case 'r':
							str.push_back('\r');
							break;
						case 'u':
						{
							unsigned u1 = parse4hex(); //���ж�֮�����λʮ���������Ƿ�Ϸ�
							if (u1 >= 0xd800 && u1 <= 0xdbff)//�����һ���ߴ��������Ҫ�������Ӧ�ĵʹ�����
							{  
								//�ʹ�����Ҳһ����һ�����
								if (*++curr != '\\')
									error("INVALID UNICODE SURROGATE");
								if (*++curr != 'u')
									error("INVALID UNICODE SURROGATE");
								unsigned u2 = parse4hex();  
								if (u2 < 0xdc00 || u2 > 0xdfff) //������Ǵ��ڵʹ�����ķ�Χ�ͷǷ�
									error("INVALID UNICODE SURROGATE");
								u1 = (((u1 - 0xd800) << 10) | (u2 - 0xdc00)) + 0x10000; //���ù�ʽ�Ѵ����ת��Ϊ��ʵ�����
							}
							str += encodeUTF8(u1);//��󽫸�ת������ת��ΪUTF-8�ı�����ӵĽ��������ַ�����
						}break;
						default:
							error("INVALID STRING ESCAPE");
					}
					break;
			}
		}
	}

	jsonContext parser::parseString()
	{
		return jsonContext(parseRawString());
	}

	jsonContext parser::parseArray()
	{
		jsonContext::array arr;
		++curr;//�������ʼ�� '['
		parseWhitespace();//�������հ�
		if (*curr == ']')//���ж������ǲ���һ����array
		{
			++curr;//��һ����Ϊ��Ҫ�����û����һ�У��������ص�parse������ʱ�򣬻����error("ROOT NOT SINGULAR");ԭ���������û����������
			return jsonContext(arr);
		}
		while (true)
		{
			parseWhitespace();
			arr.push_back(parseValue()); //�������ÿһ��Ԫ�أ������Ԫ��Ҳ��һ��array��ͬ��������������������һ���ݹ�һ��
			parseWhitespace();
			if (*curr == ',')++curr;
			else if (*curr == ']')
			{
				++curr;//��˼������ͬ��
				return jsonContext(arr);
			}
			else error("MISS COMMA OR SQUARE BRACKET");
		}
	}

	jsonContext parser::parseObject()
	{
		jsonContext::object obj;
		++curr;
		parseWhitespace();
		if (*curr == '}')
		{
			++curr;
			return jsonContext(obj);
		}

		while (true)
		{
			parseWhitespace();
			if (*curr != '"')error("MISS KEY");
			std::string key = parseRawString();//�õ�key
			parseWhitespace();
			if (*curr++ != ':')error("MISS COLON");
			parseWhitespace();
			jsonContext j_val = parseValue();
			obj.insert({ key,j_val });
			parseWhitespace();
			if (*curr == ',')++curr;
			else if (*curr == '}')
			{
				++curr;
				return jsonContext(obj);
			}
			else error("MISS COMMA OR CURLY BRACKET");
		}
	}

}