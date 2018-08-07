
#include "stdafx.h"
#include "ReferenceClass.h"


string Stos(String^ k) { return  msclr::interop::marshal_as< std::string >(k); }
String^ stoS(string k) { return  msclr::interop::marshal_as<String^>(k); }


std::string replace_all(
	__in const std::string &message,
	__in const std::string &pattern,
	__in const std::string &replace
) {

	std::string result = message;
	std::string::size_type pos = 0;
	std::string::size_type offset = 0;

	while ((pos = result.find(pattern, offset)) != std::string::npos)
	{
		result.replace(result.begin() + pos, result.begin() + pos + pattern.size(), replace);
		offset = pos + replace.size();
	}

	return result;
}