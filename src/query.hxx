#pragma once

#include "common.hxx"
#include "schema.hxx"

using libdnf5::sack::QueryCmp;

namespace s = schema;

struct AdvisoryQueryFilter
{
	std::string filter;
	std::variant<std::vector<std::string>, std::string> value;
	std::string cmp;
};

static const auto AdvisoryQueryFilterSchema = s::array(s::object<AdvisoryQueryFilter>({
	s::field<&AdvisoryQueryFilter::filter>("filter", s::string()),
	s::field<&AdvisoryQueryFilter::value>("value", s::variant(s::array(s::string().coerce()), s::string().coerce().defaultTo(""))),
	s::field<&AdvisoryQueryFilter::cmp>("cmp", s::string().coerce().defaultTo("eq")),
}));

libdnf5::advisory::AdvisoryQuery createAdvisoryQuery(const std::vector<AdvisoryQueryFilter> &filters);

inline libdnf5::advisory::AdvisoryQuery createAdvisoryQuery(const Value &value)
{
	return createAdvisoryQuery(AdvisoryQueryFilterSchema.parse(value));
}

struct PackageQueryFilter
{
	std::string type;
	std::variant<std::vector<std::string>, std::string> value;
	std::string cmp;
	int64_t limit;
	bool exact_evr;
	time_t timestamp;
	std::optional<std::vector<AdvisoryQueryFilter>> advisories;
	std::vector<PackageQueryFilter> na_from;
};

inline const schema::ObjectSchema<PackageQueryFilter> &PackageQueryFilterSchema()
{
	static const auto schema = s::object<PackageQueryFilter>({
		s::field<&PackageQueryFilter::type>("type", s::string()),
		s::field<&PackageQueryFilter::value>("value", s::variant(s::array(s::string().coerce()), s::string().coerce()).defaultTo("")),
		s::field<&PackageQueryFilter::cmp>("cmp", s::string().coerce().defaultTo("eq")),
		s::field<&PackageQueryFilter::limit>("limit", s::number().coerce().defaultTo(1)),
		s::field<&PackageQueryFilter::exact_evr>("exact_evr", s::boolean().coerce().defaultTo(false)),
		s::field<&PackageQueryFilter::timestamp>("timestamp", s::number().coerce().defaultTo(0)),
		s::field<&PackageQueryFilter::advisories>("advisories", AdvisoryQueryFilterSchema.optional()),
		s::field<&PackageQueryFilter::na_from>("na_from", s::array(s::lazy(PackageQueryFilterSchema)).defaultTo(std::vector<PackageQueryFilter>({}))),
	});

	return schema;
}

libdnf5::rpm::PackageQuery createPackageQuery(std::span<const PackageQueryFilter> filters);

inline libdnf5::rpm::PackageQuery createPackageQuery(std::span<const Value> filterValues)
{
	std::vector<PackageQueryFilter> filters;
	filters.reserve(filterValues.size());
	for (unsigned int i = 0; i < filterValues.size(); i++)
	{
		Value _filter = filterValues[i];

		if (_filter.IsString())
		{
			Object newFilter = Object::New(_filter.Env());
			newFilter.Set("type", _filter.As<String>());
			_filter = newFilter;
		}

		PackageQueryFilter filter = PackageQueryFilterSchema().parse(_filter);
		filters.push_back(filter);
	}

	return createPackageQuery(filters);
}

inline libdnf5::rpm::PackageQuery createPackageQuery(const CallbackInfo &args)
{
	std::vector<Value> filters;
	filters.reserve(args.Length());

	for (unsigned int i = 0; i < args.Length(); i++)
		filters.push_back(args[i]);

	return createPackageQuery(filters);
}

inline libdnf5::rpm::PackageQuery createPackageQuery(const Array &array)
{
	std::vector<Value> filters;
	filters.reserve(array.Length());

	for (unsigned int i = 0; i < array.Length(); i++)
		filters.push_back(array[i]);

	return createPackageQuery(filters);
}
