#pragma once

#include<iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace Json_space {

	//����json��������
	enum class jsonType { Null, Boll, Number, String, Array, Object };

	class jsonValue;

	class jsonContext final {
	private:
		std::unique_ptr<jsonValue> json_value; //������ָ����������Դ

	public:
		using array = std::vector<jsonContext>;
		using object = std::unordered_map<std::string, jsonContext>;

	public:
		//Ĭ�Ϲ���ʹvalueΪnullptr
		jsonContext() :jsonContext(nullptr) {}

		jsonContext(std::nullptr_t);


		jsonContext(bool);


		jsonContext(double);

		//����ʹint��Ҳ��ת��Ϊjsonvalue
		jsonContext(int val) :jsonContext(1.0 * val) {}


		jsonContext(const char* cstr):jsonContext(std::string(cstr)){}

		jsonContext(const std::string&);

		jsonContext(std::string&&);


		jsonContext(const array&);
		
		jsonContext(array&&);

		//�ù��������ж�������ĳЩ�����ܷ�ת��Ϊarray
		template<class V,
			typename std::enable_if< //�ú�������ѡ�����ͣ�ֻ�е���һ��ģ�����Ϊtrueʱ��type���Ͳ���Ч
				std::is_constructible< //�ú��������ж�һ�������Ƿ���Ա�����
					jsonContext,
					decltype(*std::declval<V>().begin())>::value, //declval ���κ�����ת��Ϊ�������ͣ��Ӷ������� decltype ˵�����Ĳ�������ʹ�ó�Ա������������ͨ�����캯��
					int>::type = 0>
		jsonContext(const V& v) :jsonContext(array(v.begin(), v.end())) {}


		jsonContext(const object&);

		jsonContext(object&&);

		//����ͬ��
		template<class M,
			typename std::enable_if<
				std::is_constructible<
					std::string,
					decltype(std::declval<M>()->begin().first)>::value &&
					std::is_constructible<
						jsonContext,decltype(std::declval<M>()->begin().second)>::value,
					int>::type = 0>
		jsonContext(const M& m):jsonContext(object(m.begin(),m.end())){}


		//��ֹĳЩ��ֵ�ָ��Ҳ��ת����jsonvalue
		jsonContext(void*) = delete;

		jsonContext(const jsonContext&);
		jsonContext(jsonContext&&)noexcept;

		jsonContext& operator=(const jsonContext&)noexcept;
		jsonContext& operator=(jsonContext&&)noexcept;

		//ע�������������ʵ�֣�����д��cpp�ļ��У���Ȼ�ᵼ���Ҳ���ʵ�ֵĶ�����������Ϊ��ͷ�ļ��п�����jsonValue����������
		//��cpp�ļ��п��Կ���
		~jsonContext(); 

	public:
		//�ṩ����json�Ľӿ�
		static jsonContext parse(const std::string& context, std::string& errMsg)noexcept;
		std::string serialize()const noexcept;

	private:
		//�ڸ�ֵ�������õ�
		void swap(jsonContext&)noexcept;
		std::string serialize_String()const noexcept;
		std::string serialize_Array()const noexcept;
		std::string serialize_Object()const noexcept;

	public:
		//��ȡjsonvalue�б��ֵ�����
		jsonType Type()const noexcept;
		//�ж��Ƿ�Ϊ��Ӧ������
		bool is_Null() const noexcept;
		bool is_Bool() const noexcept;
		bool is_Number() const noexcept;
		bool is_String()const noexcept;
		bool is_Array()const noexcept;
		bool is_Object()const noexcept;

		//��ȡ��Ӧ���� �������ڽ����쳣��Ϣ
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