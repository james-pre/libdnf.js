#pragma once

#include "common.hxx"

using libdnf5::sack::QueryCmp;

libdnf5::advisory::AdvisoryQuery createAdvisoryQuery(const Value &_value);

libdnf5::rpm::PackageQuery createPackageQuery(const Env &env, std::span<const Value> filters);

inline libdnf5::rpm::PackageQuery createPackageQuery(const CallbackInfo &args)
{
	std::vector<Value> filters;
	filters.reserve(args.Length());

	for (unsigned int i = 0; i < args.Length(); i++)
		filters.push_back(args[i]);

	return createPackageQuery(args.Env(), filters);
}

inline libdnf5::rpm::PackageQuery createPackageQuery(const Array &array)
{
	std::vector<Value> filters;
	filters.reserve(array.Length());

	for (unsigned int i = 0; i < array.Length(); i++)
		filters.push_back(array[i]);

	return createPackageQuery(array.Env(), filters);
}
