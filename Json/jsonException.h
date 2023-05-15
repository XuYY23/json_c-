#pragma once

#include <stdexcept>

namespace Json_space {

	class jsonException :public std::runtime_error {
	public:
		explicit jsonException(const std::string & errMsg):runtime_error(errMsg){}

	public:
		const char* what()const noexcept override { return runtime_error::what(); }
	};

}