
#include "common.hxx"
#include "js-data.hxx"
#include <string>
#include <utility>

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

std::vector<std::string> toStringVector(const Array &array)
{

	std::vector<std::string> result;
	for (unsigned int i = 0; i < array.Length(); i++)
		result.push_back(array.Get(i).ToString().Utf8Value());

	return result;
}

Array fromStringVector(const Env &env, const std::vector<std::string> &strings)
{
	Array result = Array::New(env);
	for (const auto &str : strings)
		result.Set(result.Length(), String::New(env, str));

	return result;
}

Object fromReldep(const Env &env, const libdnf5::rpm::Reldep &reldep)
{
	Object result = Object::New(env);

	result.Set("id", reldep.get_id().id);
	result.Set("name", reldep.get_name());
	result.Set("relation", reldep.get_relation());
	result.Set("version", reldep.get_version());
	result.Set("hash", reldep.get_hash());

	return result;
}

Array fromReldepList(const Env &env, const libdnf5::rpm::ReldepList &reldeps)
{
	Array result = Array::New(env);
	for (const auto &reldep : reldeps)
	{
		result.Set(result.Length(), fromReldep(env, reldep));
	}
	return result;
}

Object fromChangelog(const Env &env, const libdnf5::rpm::Changelog &changelog)
{
	Object result = Object::New(env);
	result.Set("author", changelog.get_author());
	result.Set("date", Date::New(env, changelog.get_timestamp()));
	result.Set("text", changelog.get_text());
	return result;
}

Object fromChecksum(const Env &env, const libdnf5::rpm::Checksum &checksum)
{
	Object result = Object::New(env);
	result.Set("type", checksum.get_type_str());
	result.Set("value", checksum.get_checksum());
	return result;
}

Object fromPackage(const Env &env, const libdnf5::rpm::Package &pkg)
{
	Object result = Object::New(env);

	result.Set("id", pkg.get_id().id);
	result.Set("name", pkg.get_name());
	result.Set("epoch", pkg.get_epoch());
	result.Set("version", pkg.get_version());
	result.Set("release", pkg.get_release());
	result.Set("arch", pkg.get_arch());
	result.Set("evr", pkg.get_evr());
	result.Set("nevra", pkg.get_nevra());
	result.Set("fullNevra", pkg.get_full_nevra());
	result.Set("nameArch", pkg.get_na());
	result.Set("group", pkg.get_group());
	result.Set("downloadSize", pkg.get_download_size());
	result.Set("installSize", pkg.get_install_size());
	result.Set("license", pkg.get_license());
	result.Set("sourceName", pkg.get_source_name());
	result.Set("debugSourceName", pkg.get_debugsource_name());
	result.Set("sourceDebugInfoName", pkg.get_debuginfo_name_of_source());
	result.Set("debugInfoName", pkg.get_debuginfo_name());
	result.Set("sourceRpm", pkg.get_sourcerpm());
	result.Set("buildTime", Date::New(env, pkg.get_build_time()));
	result.Set("packager", pkg.get_packager());
	result.Set("vendor", pkg.get_vendor());
	result.Set("url", pkg.get_url());
	result.Set("summary", pkg.get_summary());
	result.Set("description", pkg.get_description());
	result.Set("provides", fromReldepList(env, pkg.get_provides()));
	result.Set("requires", fromReldepList(env, pkg.get_requires()));
	result.Set("requiresPre", fromReldepList(env, pkg.get_requires_pre()));
	result.Set("conflicts", fromReldepList(env, pkg.get_conflicts()));
	result.Set("obsoletes", fromReldepList(env, pkg.get_obsoletes()));
	result.Set("prereqIgnoreinst", fromReldepList(env, pkg.get_prereq_ignoreinst()));
	result.Set("regularRequires", fromReldepList(env, pkg.get_regular_requires()));
	result.Set("recommends", fromReldepList(env, pkg.get_recommends()));
	result.Set("suggests", fromReldepList(env, pkg.get_suggests()));
	result.Set("enhances", fromReldepList(env, pkg.get_enhances()));
	result.Set("supplements", fromReldepList(env, pkg.get_supplements()));
	result.Set("depends", fromReldepList(env, pkg.get_depends()));
	result.Set("files", fromStringVector(env, pkg.get_files()));
	result.Set("changelogs", fromVector(env, pkg.get_changelogs(), fromChangelog));
	result.Set("baseUrl", pkg.get_baseurl());
	result.Set("location", pkg.get_location());
	result.Set("remoteLocations", fromStringVector(env, pkg.get_remote_locations()));
	result.Set("checksum", fromChecksum(env, pkg.get_checksum()));
	result.Set("headerChecksum", fromChecksum(env, pkg.get_hdr_checksum()));
	result.Set("packagePath", pkg.get_package_path());
	result.Set("isAvailableLocally", pkg.is_available_locally());
	result.Set("isInstalled", pkg.is_installed());
	result.Set("isExcluded", pkg.is_excluded());
	result.Set("originRepoId", pkg.get_from_repo_id());
	unsigned long long installTime = pkg.get_install_time();
	if (installTime)
		result.Set("installTime", Date::New(env, installTime));

	result.Set("repoId", pkg.get_repo_id());
	result.Set("repoName", pkg.get_repo_name());
	result.Set("reason", transaction_item_reason_to_string(pkg.get_reason()));
	result.Set("_string", pkg.to_string());
	result.Set("_stringDescription", pkg.to_string_description());
	result.Set("hash", pkg.get_hash());

	return result;
}