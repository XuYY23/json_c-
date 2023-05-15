#include "json.h"
#include "parse.h"
#include <cstdio>
#include "jsonValue.h"
#include "jsonException.h"

namespace Json_space {
	
	jsonContext::jsonContext(std::nullptr_t) :json_value(std::make_unique<jsonValue>(nullptr)){}

	jsonContext::jsonContext(bool v) : json_value(std::make_unique<jsonValue>(v)) {}

	jsonContext::jsonContext(double v) : json_value(std::make_unique<jsonValue>(v)){}

	jsonContext::jsonContext(const std::string& v):json_value(std::make_unique<jsonValue>(v)){}

	jsonContext::jsonContext(std::string&& v):json_value(std::make_unique<jsonValue>(std::move(v))) {}

	jsonContext::jsonContext(const array& v): json_value(std::make_unique<jsonValue>(v)){}

	jsonContext::jsonContext(array&& v): json_value(std::make_unique<jsonValue>(std::move(v))){}

	jsonContext::jsonContext(const object& v) : json_value(std::make_unique<jsonValue>(v)){}

	jsonContext::jsonContext(object&& v) : json_value(std::make_unique<jsonValue>(std::move(v))){}

	jsonContext::jsonContext(const jsonContext& rhs)
	{
		switch (rhs.Type()) 
		{
			case jsonType::Null: 
			{
				json_value = std::make_unique<jsonValue>(nullptr);
				break;
			}
			case jsonType::Boll: 
			{
				std::string err;
				json_value = std::make_unique<jsonValue>(rhs.get_Bool(err));
				break;
			}
			case jsonType::Number: 
			{
				std::string err;
				json_value = std::make_unique<jsonValue>(rhs.get_Double(err));
				break;
			}
			case jsonType::String:
			{
				std::string err;
				json_value = std::make_unique<jsonValue>(rhs.get_String(err));
				break; 
			}
			case jsonType::Array:
			{
				std::string err;
				json_value = std::make_unique<jsonValue>(rhs.get_Array(err));
				break;
			}
			case jsonType::Object:
			{
				std::string err;
				json_value = std::make_unique<jsonValue>(rhs.get_Object(err));
				break;
			}
			default: { break; }
		}
	}

	jsonContext& jsonContext::operator=(jsonContext&&) noexcept = default;

	jsonContext::jsonContext(jsonContext&&) noexcept = default;

	bool operator==(const jsonContext& lhs, const jsonContext& rhs)
	{
		if (lhs.Type() != rhs.Type())return false;

		switch (lhs.Type())
		{
			case jsonType::Null:
			{
				return true;
			}
			case jsonType::Boll:
			{
				std::string err1, err2;
				return lhs.get_Bool(err1) == rhs.get_Bool(err2);
			}
			case jsonType::Number:
			{
				std::string err1, err2;
				return lhs.get_Double(err1) == rhs.get_Double(err2);
			}
			case jsonType::String:
			{
				std::string err1, err2;
				return lhs.get_String(err1) == rhs.get_String(err2);
			}
			case jsonType::Array:
			{
				std::string err1, err2;
				return lhs.get_Array(err1) == rhs.get_Array(err2);
			}
			case jsonType::Object:
			{
				std::string err1, err2;
				return lhs.get_Object(err1) == rhs.get_Object(err2);
			}
			default:
			{
				return false;
			}
		}
	}

	jsonContext::~jsonContext() = default;

	jsonContext& jsonContext::operator=(const jsonContext& rhs) noexcept
	{
		jsonContext temp(rhs);
		swap(temp);
		return *this;
	}

	void jsonContext::swap(jsonContext& rhs) noexcept
	{
		using std::swap;
		swap(json_value, rhs.json_value);
	}

	std::string jsonContext::serialize_String() const noexcept
	{
		std::string res = "\"";
		for (auto e : json_value->getSting())
		{
			switch (e)
			{
				case '\"':
					res += "\\\"";
					break;
				case '\\':
					res += "\\\\";
					break;
				case '\b':
					res += "\\b";
					break;
				case '\f':
					res += "\\f";
					break;
				case '\n':
					res += "\\n";
					break;
				case '\r':
					res += "\\r";
					break;
				case '\t':
					res += "\\t";
					break;
				default:
					if (static_cast<unsigned char>(e) < 0x20)
					{
						char buf[7];
						sprintf_s(buf, "\\u%04X", e);
						res += buf;
					}
					else res += e;
			}
		}
		return res + '"';
	}

	std::string jsonContext::serialize_Array() const noexcept
	{
		std::string res = "[";
		for (size_t i = 0; i != json_value->size(); ++i)
		{
			if (i > 0)
			{
				res += ",";
			}
			//对于每一个元素调用对应的解析
			res += (*this)[i].serialize();
		}
		return res + "]";
	}

	std::string jsonContext::serialize_Object() const noexcept
	{
		std::string res = "{";
		bool first = true;
		for (auto&& p : json_value->getObject())
		{
			if (first)first = false;
			else res += ",";
			res += "\"" + p.first + "\"";
			res += ":";
			res += p.second.serialize();
		}
		return res + "}";
	}

	jsonContext jsonContext::parse(const std::string& context, std::string& errMsg) noexcept
	{
		try
		{
			parser p(context);
			return p.parse();
		}
		catch (const jsonException& e)
		{
			errMsg = e.what();
			return jsonContext(nullptr);
		}
	}

	//该函数为json生成器，就是把json文本解析出来的树型数据机构转换回json文本，这个过程又称为字符串化
	std::string jsonContext::serialize() const noexcept
	{
		switch (json_value->getType())
		{
			case jsonType::Null:
				return "null";
			case jsonType::Boll:
				return json_value->getBool() ? "true" : "false";
			case jsonType::Number:
				char buf[32];
				snprintf(buf, sizeof(buf), "%.17g", json_value->getDouble());
				return std::string(buf);
			case jsonType::String:
				return serialize_String();
			case jsonType::Array:
				return serialize_Array();
			default:
				return serialize_Object();
		}
	}

	jsonType jsonContext::Type() const noexcept
	{
		return json_value->getType();
	}

	bool jsonContext::is_Null() const noexcept
	{
		return Type() == jsonType::Null;
	}

	bool jsonContext::is_Bool() const noexcept
	{
		return Type() == jsonType::Boll;
	}

	bool jsonContext::is_Number() const noexcept
	{
		return Type() == jsonType::Number;
	}

	bool jsonContext::is_String() const noexcept
	{
		return Type() == jsonType::String;
	}

	bool jsonContext::is_Array() const noexcept
	{
		return Type() == jsonType::Array;
	}

	bool jsonContext::is_Object() const noexcept
	{
		return Type() == jsonType::Object;
	}

	bool jsonContext::get_Bool(std::string& err) const
	{
		err.clear();
		try
		{
			return json_value->getBool();
		}
		catch (const jsonException& c)
		{
			err = c.what();
		}
	}

	double jsonContext::get_Double(std::string& err) const
	{
		err.clear();
		try
		{
			return json_value->getDouble();
		}
		catch (const jsonException& c)
		{
			err = c.what();
		}
	}

	const std::string& jsonContext::get_String(std::string& err) const
	{
		err.clear();
		try
		{
			return json_value->getSting();
		}
		catch (const jsonException& c)
		{
			err = c.what();
		}
	}

	const jsonContext::array& jsonContext::get_Array(std::string& err) const
	{
		err.clear();
		try
		{
			return json_value->getArray();
		}
		catch (const jsonException& c)
		{
			err = c.what();
		}
	}

	const jsonContext::object& jsonContext::get_Object(std::string& err) const
	{
		err.clear();
		try
		{
			return json_value->getObject();
		}
		catch (const jsonException& c)
		{
			err = c.what();
		}
	}

	size_t jsonContext::size() const
	{
		return json_value->size();
	}

	//以下是array
	jsonContext& jsonContext::operator[](size_t pos)
	{
		return json_value->operator[](pos);
	}

	const jsonContext& jsonContext::operator[](size_t pos) const
	{
		return json_value->operator[](pos);
	}

	//以下是object
	jsonContext& jsonContext::operator[](const std::string& key)
	{
		return json_value->operator[](key);
	}

	const jsonContext& jsonContext::operator[](const std::string& key) const
	{
		return json_value->operator[](key);
	}

}