#include <libdnf5/base/base.hpp>
#include <libdnf5/rpm/package_query.hpp>
#include <napi.h>
#include "common.hxx"
#include "query.hxx"
#include "js-data.hxx"

Value Query(const CallbackInfo &args)
{
	Env env = args.Env();

	libdnf5::rpm::PackageQuery query = createPackageQuery(args);

	Array results = Array::New(env);

	for (const libdnf5::rpm::Package &pkg : query)
		results.Set(results.Length(), fromPackage(env, pkg));

	return results;
}

Value LoadRepos(const CallbackInfo &args)
{
	Env env = args.Env();

	// Load repos
	auto repo_sack = base.get_repo_sack();
	repo_sack->create_repos_from_system_configuration();
	// base.lock_system_repo(libdnf5::utils::LockAccess::WRITE, libdnf5::utils::LockBlocking::BLOCKING);
	repo_sack->load_repos();

	return env.Undefined();
}

Object Init(Env env, Object exports)
{

	base.load_config();
	base.setup();

	exports.Set(String::New(env, "loadRepos"), Function::New(env, LoadRepos));
	exports.Set(String::New(env, "query"), Function::New(env, Query));
	return exports;
}

NODE_API_MODULE(js_libdnf5, Init)
