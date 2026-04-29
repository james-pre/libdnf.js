#include "common.hxx"
#include "query.hxx"
#include "js-data.hxx"

std::optional<QueryCmp> toQueryCmp(std::string cmp)
{

	if (cmp == "eq")
		return QueryCmp::EQ;
	else if (cmp == "neq")
		return QueryCmp::NEQ;
	else if (cmp == "gt")
		return QueryCmp::GT;
	else if (cmp == "gte")
		return QueryCmp::GTE;
	else if (cmp == "lt")
		return QueryCmp::LT;
	else if (cmp == "lte")
		return QueryCmp::LTE;
	else if (cmp == "exact")
		return QueryCmp::EXACT;
	else if (cmp == "not_exact")
		return QueryCmp::NOT_EXACT;
	else if (cmp == "iexact")
		return QueryCmp::IEXACT;
	else if (cmp == "not_iexact")
		return QueryCmp::NOT_IEXACT;
	else if (cmp == "contains")
		return QueryCmp::CONTAINS;
	else if (cmp == "not_contains")
		return QueryCmp::NOT_CONTAINS;
	else if (cmp == "icontains")
		return QueryCmp::ICONTAINS;
	else if (cmp == "not_icontains")
		return QueryCmp::NOT_ICONTAINS;
	else if (cmp == "startswith")
		return QueryCmp::STARTSWITH;
	else if (cmp == "istartswith")
		return QueryCmp::ISTARTSWITH;
	else if (cmp == "endswith")
		return QueryCmp::ENDSWITH;
	else if (cmp == "iendswith")
		return QueryCmp::IENDSWITH;
	else if (cmp == "regex")
		return QueryCmp::REGEX;
	else if (cmp == "iregex")
		return QueryCmp::IREGEX;
	else if (cmp == "glob")
		return QueryCmp::GLOB;
	else if (cmp == "not_glob")
		return QueryCmp::NOT_GLOB;
	else if (cmp == "iglob")
		return QueryCmp::IGLOB;
	else if (cmp == "not_iglob")
		return QueryCmp::NOT_IGLOB;
	else
		return {};
}

QueryCmp jsToQueryCmp(const Value &value)
{
	if (value.IsUndefined() || value.IsNull())
		return QueryCmp::EQ;

	std::string cmp = value.ToString().Utf8Value();

	std::optional<QueryCmp> result = toQueryCmp(cmp);

	if (!result)
		throw Error::New(value.Env(), "Invalid comparison operator: " + cmp);

	return *result;
}

libdnf5::advisory::AdvisoryQuery createAdvisoryQuery(const Value &_value)
{

	if (!_value.IsArray())
		throw TypeError::New(_value.Env(), "Invalid advisory filter: expected an array");

	const Array &value = _value.As<Array>();

	libdnf5::advisory::AdvisoryQuery query(base);

	for (unsigned int i = 0; i < value.Length(); i++)
	{
		if (!value[i].IsObject())
			throw TypeError::New(value.Env(), "Invalid advisory filter at index " + std::to_string(i));

		Object filter = value[i].As<Object>();

		std::string field = filter.Get("filter").ToString().Utf8Value();

		Value _value = filter.Get("value");
		std::vector<std::string> value = _value.IsArray() ? toStringVector(_value.As<Array>()) : std::vector<std::string>{_value.ToString().Utf8Value()};

		QueryCmp cmp = jsToQueryCmp(filter.Get("cmp"));

		if (field == "name")
			query.filter_name(value, cmp);
		else if (field == "packages")
		{
			std::vector<libdnf5::rpm::Nevra> nerva;
			nerva.reserve(value.size());
			for (const auto &str : value)
				nerva.append_range(libdnf5::rpm::Nevra::parse(str));

			query.filter_packages(nerva, cmp);
		}
		else if (field == "reference")
			query.filter_reference(value, cmp);
		else if (field == "severity")
			query.filter_severity(value, cmp);
		else if (field == "type")
			query.filter_type(value, cmp);
	}
	return query;
}

libdnf5::rpm::PackageQuery createPackageQuery(const Env &env, std::span<const Value> filters)
{
	libdnf5::rpm::PackageQuery query(base);

	for (unsigned int i = 0; i < filters.size(); i++)
	{
		Value _filter = filters[i];
		Object filter;

		if (_filter.IsObject())
			filter = _filter.As<Object>();
		else if (_filter.IsString())
		{
			filter = Object::New(env);
			filter.Set("type", _filter.As<String>());
		}
		else
			throw TypeError::New(env, "Invalid filter at index " + std::to_string(i));

		std::string field = filter.Get("type").ToString().Utf8Value();

		Value _value = filter.Get("value");
		std::vector<std::string> value = _value.IsArray() ? toStringVector(_value.As<Array>()) : std::vector<std::string>{_value.ToString().Utf8Value()};

		Value _limit = filter.Get("limit");
		bool _limit_lossless;
		int64_t limit = _limit.IsBigInt() ? _limit.As<BigInt>().Int64Value(&_limit_lossless) : _limit.ToNumber().Int64Value();

		QueryCmp cmp = jsToQueryCmp(filter.Get("cmp"));

		if (field == "advisories")
			query.filter_advisories(createAdvisoryQuery(_value), cmp);
		else if (field == "arch")
			query.filter_arch(value, cmp);
		else if (field == "available")
			query.filter_available();
		else if (field == "conflicts")
			query.filter_conflicts(value, cmp);
		else if (field == "description")
			query.filter_description(value, cmp);
		else if (field == "downgradable")
			query.filter_downgradable();
		else if (field == "downgrades")
			query.filter_downgrades();
		else if (field == "duplicates")
			query.filter_duplicates();
		else if (field == "earliest_evr")
			query.filter_earliest_evr(limit);
		else if (field == "earliest_evr_any_arch")
			query.filter_earliest_evr_any_arch(limit);
		else if (field == "enhances")
			query.filter_enhances(value, cmp);
		else if (field == "epoch")
			query.filter_epoch(value, cmp);
		else if (field == "evr")
			query.filter_evr(value, cmp);
		else if (field == "extras")
			query.filter_extras(filter.Get("exact_evr").ToBoolean());
		else if (field == "file")
			query.filter_file(value, cmp);
		else if (field == "from_repo_id")
			query.filter_from_repo_id(value, cmp);
		else if (field == "installed")
			query.filter_installed();
		else if (field == "installonly")
			query.filter_installonly();
		else if (field == "latest_evr")
			query.filter_latest_evr(limit);
		else if (field == "latest_evr_any_arch")
			query.filter_latest_evr_any_arch(limit);
		else if (field == "latest_unresolved_advisories")
		{
			libdnf5::rpm::PackageQuery installed(base);
			installed.filter_installed();
			query.filter_latest_unresolved_advisories(createAdvisoryQuery(_value), installed, cmp);
		}
		else if (field == "leaves")
			query.filter_leaves();
		else if (field == "leaves_groups")
			query.filter_leaves_groups();
		else if (field == "location")
			query.filter_location(value, cmp);
		else if (field == "name")
			query.filter_name(value, cmp);
		else if (field == "name_arch")
		{
			libdnf5::rpm::PackageSet package_set(base);
			if (!_value.IsArray())
				throw TypeError::New(env, "Invalid value for 'name_arch' filter");

			for (const auto &pkg : createPackageQuery(_value.As<Array>()))
				package_set.add(pkg);

			query.filter_name_arch(package_set, cmp);
		}
		else if (field == "nevra")
			query.filter_nevra(value, cmp);
		else if (field == "obsoletes")
			query.filter_obsoletes(value, cmp);
		else if (field == "priority")
			query.filter_priority();
		else if (field == "provides")
			query.filter_provides(value, cmp);
		else if (field == "reboot_suggested")
			query.filter_reboot_suggested();
		else if (field == "recent")
			query.filter_recent(filter.Get("timestamp").ToNumber());
		else if (field == "recommends")
			query.filter_recommends(value, cmp);
		else if (field == "release")
			query.filter_release(value, cmp);
		else if (field == "repo_id")
			query.filter_repo_id(value, cmp);
		else if (field == "requires")
			query.filter_requires(value, cmp);
		else if (field == "sourcerpm")
			query.filter_sourcerpm(value, cmp);
		else if (field == "suggests")
			query.filter_suggests(value, cmp);
		else if (field == "summary")
			query.filter_summary(value, cmp);
		else if (field == "supplements")
			query.filter_supplements(value, cmp);
		else if (field == "unneeded")
			query.filter_unneeded();
		else if (field == "upgradable")
			query.filter_upgradable();
		else if (field == "upgrades")
			query.filter_upgrades();
		else if (field == "url")
			query.filter_url(value, cmp);
		else if (field == "userinstalled")
			query.filter_userinstalled();
		else if (field == "version")
			query.filter_version(value, cmp);
		else if (field == "versionlock")
			query.filter_versionlock();
		else
			throw Error::New(env, "Invalid filter type: " + field);
	}

	return query;
}
