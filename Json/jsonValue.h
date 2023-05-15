#pragma once

#include <variant>
#include "json.h"
#include "jsonException.h"

namespace Json_space {

	class jsonValue {
	private:
		// 官网variant：https://en.cppreference.com/w/cpp/utility/variant
		std::variant<std::nullptr_t, bool, double, std::string, jsonContext::array, jsonContext::object> val;

	public:
		//每一个替代物对应的构造
		explicit jsonValue(std::nullptr_t) :val(nullptr){} 

		explicit jsonValue(bool v):val(v){}

		explicit jsonValue(double v):val(v){}

		explicit jsonValue(const std::string& v):val(v){}
		explicit jsonValue(std::string&& v) :val(std::move(v)){}

		explicit jsonValue(const jsonContext::array& v):val(v){}
		explicit jsonValue(jsonContext::array&& v):val(std::move(v)){}

		explicit jsonValue(const jsonContext::object& v):val(v){}
		explicit jsonValue(jsonContext::object&& v):val(std::move(v)){}

		~jsonValue();

	public:
		jsonType getType()const noexcept;
		std::nullptr_t getNull()const;
		bool getBool()const;
		double getDouble()const;
		const std::string& getSting()const;
		const jsonContext::array& getArray()const;
		const jsonContext::object& getObject()const;

		size_t size()const;
		const jsonContext& operator[](size_t)const;
		jsonContext& operator[](size_t);
		const jsonContext& operator[](const std::string&)const;
		jsonContext& operator[](const std::string&);
	};

}