#pragma once

#include "common.hxx"

#include <memory>
#include <libdnf5/base/transaction.hpp>

class Transaction final : public ObjectWrap<Transaction>
{

public:
	static FunctionReference constructor;

	static void Init(const Napi::Env &env, Object &exports);
	static Object NewInstance(const Napi::Env &env, std::unique_ptr<libdnf5::base::Transaction> transaction);

	Transaction(const CallbackInfo &info);

private:
	static bool constructing;

	std::unique_ptr<libdnf5::base::Transaction> transaction;

	libdnf5::base::Transaction &tx(const Napi::Env &env);

	Napi::Value getPackages(const CallbackInfo &info);
	Napi::Value getPackagesCount(const CallbackInfo &info);
	Napi::Value getGroups(const CallbackInfo &info);
	Napi::Value getBrokenDependencyPackages(const CallbackInfo &info);
	Napi::Value getConflictingPackages(const CallbackInfo &info);
	Napi::Value getIsEmpty(const CallbackInfo &info);

	Napi::Value download(const CallbackInfo &info);
	Napi::Value run(const CallbackInfo &info);
	Napi::Value setDescription(const CallbackInfo &info);
};
