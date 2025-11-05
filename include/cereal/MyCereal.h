#pragma once

#include <cereal/archives/binary.hpp>

#include <string>
#include <sstream>

template<typename T>
std::string Serialize(const T& object)
{
	std::ostringstream oss(std::ios::binary);
	{
	    cereal::BinaryOutputArchive archive(oss);
	    archive(object);
	}

	return oss.str();
}

template<typename T>
T Deserialize(const std::string& data)
{
	T object;

	std::istringstream iss(data, std::ios::binary);
	{
	    cereal::BinaryInputArchive archive(iss);
	    archive(object);
	}

	return object;
}