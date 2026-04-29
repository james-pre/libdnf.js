#include "transaction.hxx"

#include "common.hxx"
#include "callbacks.hxx"
#include "js-data.hxx"

FunctionReference Transaction::constructor;
bool Transaction::constructing = false;

static Object getOptionsObject(const CallbackInfo &info)
{
	Env env = info.Env();

	if (info.Length() == 0 || info[0].IsUndefined() || info[0].IsNull())
		return Object::New(env);

	if (!info[0].IsObject())
		throw TypeError::New(env, "Expected an options object");

	return info[0].As<Object>();
}

void Transaction::Init(const Napi::Env &env, Object &exports)
{
	Function func = DefineClass(env, "Transaction", {
														InstanceAccessor("packages", &Transaction::getPackages, nullptr),
														InstanceAccessor("packagesCount", &Transaction::getPackagesCount, nullptr),
														InstanceAccessor("groups", &Transaction::getGroups, nullptr),
														InstanceAccessor("brokenDependencyPackages", &Transaction::getBrokenDependencyPackages, nullptr),
														InstanceAccessor("conflictingPackages", &Transaction::getConflictingPackages, nullptr),
														InstanceAccessor("isEmpty", &Transaction::getIsEmpty, nullptr),

														InstanceMethod("download", &Transaction::download),
														InstanceMethod("run", &Transaction::run),
														InstanceMethod("setDescription", &Transaction::setDescription),
													});

	constructor = Persistent(func);
	constructor.SuppressDestruct();

	exports.Set("Transaction", func);
}

Object Transaction::NewInstance(const Napi::Env &env, std::unique_ptr<libdnf5::base::Transaction> transaction)
{
	constructing = true;
	Object object;

	try
	{
		object = constructor.New({});
	}
	catch (...)
	{
		constructing = false;
		throw;
	}

	constructing = false;

	Transaction *wrapper = ObjectWrap<Transaction>::Unwrap(object);
	wrapper->transaction = std::move(transaction);

	return object;
}

Transaction::Transaction(const CallbackInfo &info)
	: ObjectWrap<Transaction>(info)
{
	if (!constructing)
		throw TypeError::New(info.Env(), "Transaction objects cannot be constructed directly");
}

libdnf5::base::Transaction &Transaction::tx(const Napi::Env &env)
{
	if (!transaction)
		throw Error::New(env, "Transaction has not been initialized");

	return *transaction;
}

Value Transaction::getPackages(const CallbackInfo &info)
{
	Napi::Env env = info.Env();
	return fromVector(env, tx(env).get_transaction_packages(), fromTxPackage);
}

Value Transaction::getPackagesCount(const CallbackInfo &info)
{
	Napi::Env env = info.Env();
	return Number::New(env, tx(env).get_transaction_packages_count());
}

Value Transaction::getGroups(const CallbackInfo &info)
{
	Napi::Env env = info.Env();
	return fromVector(env, tx(env).get_transaction_groups(), fromTxGroup);
}

Value Transaction::getBrokenDependencyPackages(const CallbackInfo &info)
{
	Napi::Env env = info.Env();
	return fromVector(env, tx(env).get_broken_dependency_packages(), fromPackage);
}

Value Transaction::getConflictingPackages(const CallbackInfo &info)
{
	Napi::Env env = info.Env();
	return fromVector(env, tx(env).get_conflicting_packages(), fromPackage);
}

Value Transaction::getIsEmpty(const CallbackInfo &info)
{
	Napi::Env env = info.Env();
	return Boolean::New(env, tx(env).empty());
}

Value Transaction::download(const CallbackInfo &info)
{
	Napi::Env env = info.Env();
	Object options = getOptionsObject(info);

	base.set_download_callbacks(std::make_unique<PackageDownloadCallbacks>(env, options));
	tx(env).download();

	return env.Undefined();
}

Value Transaction::run(const CallbackInfo &info)
{
	Napi::Env env = info.Env();
	Object options = getOptionsObject(info);

	tx(env).set_callbacks(std::make_unique<PackageTransactionCallbacks>(env, options));
	tx(env).run();

	return env.Undefined();
}

Value Transaction::setDescription(const CallbackInfo &info)
{
	Napi::Env env = info.Env();

	if (info.Length() < 1 || !info[0].IsString())
		throw TypeError::New(env, "Expected a string");

	tx(env).set_description(info[0].As<String>().Utf8Value());

	return env.Undefined();
}
