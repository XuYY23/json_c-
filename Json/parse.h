#pragma once


#include "json.h"
#include "jsonException.h"

namespace Json_space {

	//�����ж��ַ��Ƿ���1-9���ַ�������0-9
	constexpr inline bool is_1_to_9(char ch) { return ch >= '1' && ch <= '9'; }
	constexpr inline bool is_0_to_9(char ch) { return ch >= '0' && ch <= '9'; }

	//������
	class parser {
	private:
		const char* curr; //ָ��ǰ�ı�

	public:
		//���湹��
		explicit parser(const char* cstr)noexcept :curr(cstr) {}
		//����string�Ĺ���
		explicit parser(const std::string& context)noexcept :curr(context.c_str()) {}

		//�����಻Ӧ�ñ�������ֵ
		parser(const parser&) = delete;
		parser& operator=(const parser&) = delete;

	private:
		//�����հ��ַ�
		void parseWhitespace()noexcept;
		//�����׳��쳣��Ϣ�ĺ���
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
		//Ψһ ��parse�����ӿ�
		jsonContext parse();
	};
}