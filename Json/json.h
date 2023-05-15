#pragma once

#include<iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace Json_space {

	//六种json数据类型
	enum class jsonType { Null, Boll, Number, String, Array, Object };

	class jsonValue;

	class jsonContext final {
	private:
		std::unique_ptr<jsonValue> json_value; //用智能指针来管理资源

	public:
		using array = std::vector<jsonContext>;
		using object = std::unordered_map<std::string, jsonContext>;

	public:
		//默认构造使value为nullptr
		jsonContext() :jsonContext(nullptr) {}

		jsonContext(std::nullptr_t);


		jsonContext(bool);


		jsonContext(double);

		//便于使int型也能转化为jsonvalue
		jsonContext(int val) :jsonContext(1.0 * val) {}


		jsonContext(const char* cstr):jsonContext(std::string(cstr)){}

		jsonContext(const std::string&);

		jsonContext(std::string&&);


		jsonContext(const array&);
		
		jsonContext(array&&);

		//该构造用于判断其他的某些容器能否转换为array
		template<class V,
			typename std::enable_if< //该函数用于选择类型，只有当第一个模板参数为true时，type类型才有效
				std::is_constructible< //该函数用来判断一个类型是否可以被构造
					jsonContext,
					decltype(*std::declval<V>().begin())>::value, //declval 将任何类型转换为引用类型，从而可以在 decltype 说明符的操作数中使用成员函数，而无需通过构造函数
					int>::type = 0>
		jsonContext(const V& v) :jsonContext(array(v.begin(), v.end())) {}


		jsonContext(const object&);

		jsonContext(object&&);

		//与上同理
		template<class M,
			typename std::enable_if<
				std::is_constructible<
					std::string,
					decltype(std::declval<M>()->begin().first)>::value &&
					std::is_constructible<
						jsonContext,decltype(std::declval<M>()->begin().second)>::value,
					int>::type = 0>
		jsonContext(const M& m):jsonContext(object(m.begin(),m.end())){}


		//防止某些奇怪的指针也能转化成jsonvalue
		jsonContext(void*) = delete;

		jsonContext(const jsonContext&);
		jsonContext(jsonContext&&)noexcept;

		jsonContext& operator=(const jsonContext&)noexcept;
		jsonContext& operator=(jsonContext&&)noexcept;

		//注意这里的析构的实现，必须写着cpp文件中，不然会导致找不到实现的定义的情况，因为在头文件中看不到jsonValue的完整定义
		//而cpp文件中可以看到
		~jsonContext(); 

	public:
		//提供解析json的接口
		static jsonContext parse(const std::string& context, std::string& errMsg)noexcept;
		std::string serialize()const noexcept;

	private:
		//在赋值操作中用到
		void swap(jsonContext&)noexcept;
		std::string serialize_String()const noexcept;
		std::string serialize_Array()const noexcept;
		std::string serialize_Object()const noexcept;

	public:
		//获取jsonvalue中保持的类型
		jsonType Type()const noexcept;
		//判断是否为对应的类型
		bool is_Null() const noexcept;
		bool is_Bool() const noexcept;
		bool is_Number() const noexcept;
		bool is_String()const noexcept;
		bool is_Array()const noexcept;
		bool is_Object()const noexcept;

		//获取相应类型 参数用于接受异常信息
		bool get_Bool(std::string&)const;
		double get_Double(std::string&)const;
		const std::string& get_String(std::string&)const;
		const array& get_Array(std::string&)const;
		const object& get_Object(std::string&)const;

		size_t size()const;
		jsonContext& operator[](size_t);
		const jsonContext& operator[](size_t)const;
		jsonContext& operator[](const std::string&);
		const jsonContext& operator[](const std::string&)const;
	};

	inline std::ostream& operator<<(std::ostream& os, const jsonContext& jc)
	{
		return os << jc.serialize();
	}

	bool operator==(const jsonContext&, const jsonContext&);

	inline bool operator!=(const jsonContext& lhs, const jsonContext& rhs)
	{
		return !(lhs == rhs);
	}

}