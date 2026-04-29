
#pragma once

#include "common.hxx"

// JS Array -> std::vector<T>
template <typename T, typename Convert>
std::vector<T> toVector(const Array &array, Convert &&convert)
{
	std::vector<T> result;
	result.reserve(array.Length());

	for (uint32_t i = 0; i < array.Length(); i++)
		result.push_back(std::forward<Convert>(convert)(array.Get(i)));

	return result;
}

// std::vector<T> -> JS Array
template <typename T, typename Convert>
Array fromVector(const Env &env, const std::vector<T> &values, Convert &&convert)
{
	Array result = Array::New(env, values.size());

	for (uint32_t i = 0; i < values.size(); i++)
		result.Set(i, std::forward<Convert>(convert)(env, values[i]));

	return result;
}

std::vector<std::string> toStringVector(const Array &array);
Array fromStringVector(const Env &env, const std::vector<std::string> &strings);

Object fromNevra(const Env &env, const libdnf5::rpm::Nevra &nevra);

Object fromPackage(const Env &env, const libdnf5::rpm::Package &pkg);
Object fromTxPackage(const Env &env, const libdnf5::base::TransactionPackage &txpkg);
Object fromTransaction(const Env &env, libdnf5::base::Transaction &transaction);