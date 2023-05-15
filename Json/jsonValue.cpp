#include "jsonValue.h"
#include "jsonException.h"

namespace Json_space {

	jsonValue::~jsonValue() = default;

	jsonType jsonValue::getType() const noexcept
	{
		if (std::holds_alternative<std::nullptr_t>(val))
			return jsonType::Null;
		else if (std::holds_alternative<bool>(val))
			return jsonType::Boll;
		else if (std::holds_alternative<double>(val))
			return jsonType::Number;
		else if (std::holds_alternative<std::string>(val))
			return jsonType::String;
		else if (std::holds_alternative<jsonContext::array>(val))
			return jsonType::Array;
		else 
			return jsonType::Object;
	}

	std::nullptr_t jsonValue::getNull() const
	{
		try
		{
			return std::get<std::nullptr_t>(val);
		}
		catch (const std::bad_variant_access&)
		{
			throw jsonException("NOT A NULL");
		}
	}

	bool jsonValue::getBool() const
	{
		try
		{
			return std::get<bool>(val);
		}
		catch (const std::bad_variant_access&)
		{
			throw jsonException("NOT A BOOL");
		}
	}

	double jsonValue::getDouble() const
	{
		try
		{
			return std::get<double>(val);
		}
		catch (const std::bad_variant_access&)
		{
			throw jsonException("NOT A DOUBLE");
		}
	}

	const std::string& jsonValue::getSting() const
	{
		try
		{
			return std::get<std::string>(val);
		}
		catch (const std::bad_variant_access&)
		{
			throw jsonException("NOT A STRING");
		}
	}

	const jsonContext::array& jsonValue::getArray() const
	{
		try
		{
			return std::get<jsonContext::array>(val);
		}
		catch (const std::bad_variant_access&)
		{
			throw jsonException("NOT A ARRAY");
		}
	}

	const jsonContext::object& jsonValue::getObject() const
	{
		try
		{
			return std::get<jsonContext::object>(val);
		}
		catch (const std::bad_variant_access&)
		{
			throw jsonException("NOT A OBJECT");
		}
	}

	size_t jsonValue::size() const
	{
		if (std::holds_alternative<jsonContext::array>(val))
		{
			return std::get<jsonContext::array>(val).size();
		}
		else if (std::holds_alternative<jsonContext::object>(val))
		{
			return std::get<jsonContext::object>(val).size();
		}
		else
		{
			throw jsonException("NOT A ARRAY OR OBJECT");
		}
	}

	const jsonContext& jsonValue::operator[](size_t pos) const
	{
		if (std::holds_alternative<jsonContext::array>(val))
		{
			return std::get<jsonContext::array>(val)[pos];
		}
		else
		{
			throw jsonException("NOT A ARRAY");
		}
	}

	jsonContext& jsonValue::operator[](size_t pos)
	{
		return const_cast<jsonContext&>(static_cast<const jsonValue&>(*this)[pos]);
	}

	const jsonContext& jsonValue::operator[](const std::string& key) const
	{
		if (std::holds_alternative<jsonContext::object>(val))
		{
			return std::get<jsonContext::object>(val).at(key);
		}
		else
		{
			throw jsonException("NOT A OBJECT");
		}
	}

	jsonContext& jsonValue::operator[](const std::string& key)
	{
		return const_cast<jsonContext&>(static_cast<const jsonValue&>(*this)[key]);
	}

}