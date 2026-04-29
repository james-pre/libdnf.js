#pragma once

#include "common.hxx"
#include "js-data.hxx"

class PackageDownloadCallbacks final : public libdnf5::repo::DownloadCallbacks
{
public:
	PackageDownloadCallbacks(const Env &env, const Object &options)
		: env(env)
	{
		setCallback(options, "onDownload", onDownload);
		setCallback(options, "onProgress", onProgress);
		setCallback(options, "onEnd", onEnd);
		setCallback(options, "onMirrorFailure", onMirrorFailure);
	}

private:
	Env env;
	FunctionReference onDownload;
	FunctionReference onProgress;
	FunctionReference onEnd;
	FunctionReference onMirrorFailure;

	static void setCallback(
		const Object &options,
		const char *name,
		FunctionReference &target)
	{
		if (!options.Has(name))
			return;

		Value value = options.Get(name);

		if (value.IsUndefined() || value.IsNull())
			return;

		if (!value.IsFunction())
			throw TypeError::New(options.Env(), std::string(name) + " must be a function");

		target = Persistent(value.As<Function>());
	}

	void *add_new_download(
		void *user_data,
		const char *description,
		double total_to_download) override
	{
		if (!onDownload.IsEmpty())
		{
			onDownload.Call({
				String::New(env, description),
				Number::New(env, total_to_download),
			});
		}

		return nullptr;
	}

	int progress(
		void *user_cb_data,
		double total_to_download,
		double downloaded) override
	{
		if (!onProgress.IsEmpty())
		{
			onProgress.Call({
				Number::New(env, total_to_download),
				Number::New(env, downloaded),
			});
		}

		return 0;
	}

	int end(
		void *user_cb_data,
		TransferStatus status,
		const char *msg) override
	{
		if (!onEnd.IsEmpty())
		{
			onEnd.Call({
				Number::New(env, static_cast<int>(status)),
				String::New(env, msg),
			});
		}

		return 0;
	}

	int mirror_failure(
		void *user_cb_data,
		const char *msg,
		const char *url,
		const char *metadata) override
	{
		if (!onMirrorFailure.IsEmpty())
		{
			onMirrorFailure.Call({
				String::New(env, msg),
				String::New(env, url),
				String::New(env, metadata),
			});
		}

		return 0;
	}
};

class PackageTransactionCallbacks final : public libdnf5::rpm::TransactionCallbacks
{
public:
	PackageTransactionCallbacks(const Env &env, const Object &options)
		: env(env)
	{
		setCallback(options, "onBeforeBegin", onBeforeBegin);
		setCallback(options, "onAfterComplete", onAfterComplete);

		setCallback(options, "onInstallProgress", onInstallProgress);
		setCallback(options, "onInstallStart", onInstallStart);
		setCallback(options, "onInstallStop", onInstallStop);

		setCallback(options, "onTransactionProgress", onTransactionProgress);
		setCallback(options, "onTransactionStart", onTransactionStart);
		setCallback(options, "onTransactionStop", onTransactionStop);

		setCallback(options, "onUninstallProgress", onUninstallProgress);
		setCallback(options, "onUninstallStart", onUninstallStart);
		setCallback(options, "onUninstallStop", onUninstallStop);

		setCallback(options, "onUnpackError", onUnpackError);
		setCallback(options, "onCpioError", onCpioError);

		setCallback(options, "onScriptError", onScriptError);
		setCallback(options, "onScriptStart", onScriptStart);
		setCallback(options, "onScriptStop", onScriptStop);

		setCallback(options, "onElemProgress", onElemProgress);

		setCallback(options, "onVerifyProgress", onVerifyProgress);
		setCallback(options, "onVerifyStart", onVerifyStart);
		setCallback(options, "onVerifyStop", onVerifyStop);
	}

private:
	Env env;

	FunctionReference onBeforeBegin;
	FunctionReference onAfterComplete;

	FunctionReference onInstallProgress;
	FunctionReference onInstallStart;
	FunctionReference onInstallStop;

	FunctionReference onTransactionProgress;
	FunctionReference onTransactionStart;
	FunctionReference onTransactionStop;

	FunctionReference onUninstallProgress;
	FunctionReference onUninstallStart;
	FunctionReference onUninstallStop;

	FunctionReference onUnpackError;
	FunctionReference onCpioError;

	FunctionReference onScriptError;
	FunctionReference onScriptStart;
	FunctionReference onScriptStop;

	FunctionReference onElemProgress;

	FunctionReference onVerifyProgress;
	FunctionReference onVerifyStart;
	FunctionReference onVerifyStop;

	static void setCallback(
		const Object &options,
		const char *name,
		FunctionReference &target)
	{
		if (!options.Has(name))
			return;

		Value value = options.Get(name);

		if (value.IsUndefined() || value.IsNull())
			return;

		if (!value.IsFunction())
			throw TypeError::New(options.Env(), std::string(name) + " must be a function");

		target = Persistent(value.As<Function>());
	}

	Value fromOptionalTxPackage(const libdnf5::base::TransactionPackage *item) const
	{
		if (!item)
			return env.Null();

		return fromTxPackage(env, *item);
	}

	String fromScriptType(ScriptType type) const
	{
		return String::New(
			env,
			libdnf5::rpm::TransactionCallbacks::script_type_to_string(type));
	}

	void before_begin(uint64_t total) override
	{
		if (!onBeforeBegin.IsEmpty())
			onBeforeBegin.Call({BigInt::New(env, total)});
	}

	void after_complete(bool success) override
	{
		if (!onAfterComplete.IsEmpty())
			onAfterComplete.Call({Boolean::New(env, success)});
	}

	void install_progress(
		const libdnf5::base::TransactionPackage &item,
		uint64_t amount,
		uint64_t total) override
	{
		if (!onInstallProgress.IsEmpty())
		{
			onInstallProgress.Call({
				fromTxPackage(env, item),
				BigInt::New(env, amount),
				BigInt::New(env, total),
			});
		}
	}

	void install_start(
		const libdnf5::base::TransactionPackage &item,
		uint64_t total) override
	{
		if (!onInstallStart.IsEmpty())
		{
			onInstallStart.Call({
				fromTxPackage(env, item),
				BigInt::New(env, total),
			});
		}
	}

	void install_stop(
		const libdnf5::base::TransactionPackage &item,
		uint64_t amount,
		uint64_t total) override
	{
		if (!onInstallStop.IsEmpty())
		{
			onInstallStop.Call({
				fromTxPackage(env, item),
				BigInt::New(env, amount),
				BigInt::New(env, total),
			});
		}
	}

	void transaction_progress(uint64_t amount, uint64_t total) override
	{
		if (!onTransactionProgress.IsEmpty())
		{
			onTransactionProgress.Call({
				BigInt::New(env, amount),
				BigInt::New(env, total),
			});
		}
	}

	void transaction_start(uint64_t total) override
	{
		if (!onTransactionStart.IsEmpty())
			onTransactionStart.Call({BigInt::New(env, total)});
	}

	void transaction_stop(uint64_t total) override
	{
		if (!onTransactionStop.IsEmpty())
			onTransactionStop.Call({BigInt::New(env, total)});
	}

	void uninstall_progress(
		const libdnf5::base::TransactionPackage &item,
		uint64_t amount,
		uint64_t total) override
	{
		if (!onUninstallProgress.IsEmpty())
		{
			onUninstallProgress.Call({
				fromTxPackage(env, item),
				BigInt::New(env, amount),
				BigInt::New(env, total),
			});
		}
	}

	void uninstall_start(
		const libdnf5::base::TransactionPackage &item,
		uint64_t total) override
	{
		if (!onUninstallStart.IsEmpty())
		{
			onUninstallStart.Call({
				fromTxPackage(env, item),
				BigInt::New(env, total),
			});
		}
	}

	void uninstall_stop(
		const libdnf5::base::TransactionPackage &item,
		uint64_t amount,
		uint64_t total) override
	{
		if (!onUninstallStop.IsEmpty())
		{
			onUninstallStop.Call({
				fromTxPackage(env, item),
				BigInt::New(env, amount),
				BigInt::New(env, total),
			});
		}
	}

	void unpack_error(const libdnf5::base::TransactionPackage &item) override
	{
		if (!onUnpackError.IsEmpty())
			onUnpackError.Call({fromTxPackage(env, item)});
	}

	void cpio_error(const libdnf5::base::TransactionPackage &item) override
	{
		if (!onCpioError.IsEmpty())
			onCpioError.Call({fromTxPackage(env, item)});
	}

	void script_error(
		const libdnf5::base::TransactionPackage *item,
		libdnf5::rpm::Nevra nevra,
		ScriptType type,
		uint64_t return_code) override
	{
		if (!onScriptError.IsEmpty())
		{
			onScriptError.Call({
				fromOptionalTxPackage(item),
				fromNevra(env, nevra),
				fromScriptType(type),
				BigInt::New(env, return_code),
			});
		}
	}

	void script_start(
		const libdnf5::base::TransactionPackage *item,
		libdnf5::rpm::Nevra nevra,
		ScriptType type) override
	{
		if (!onScriptStart.IsEmpty())
		{
			onScriptStart.Call({
				fromOptionalTxPackage(item),
				fromNevra(env, nevra),
				fromScriptType(type),
			});
		}
	}

	void script_stop(
		const libdnf5::base::TransactionPackage *item,
		libdnf5::rpm::Nevra nevra,
		ScriptType type,
		uint64_t return_code) override
	{
		if (!onScriptStop.IsEmpty())
		{
			onScriptStop.Call({
				fromOptionalTxPackage(item),
				fromNevra(env, nevra),
				fromScriptType(type),
				BigInt::New(env, return_code),
			});
		}
	}

	void elem_progress(
		const libdnf5::base::TransactionPackage &item,
		uint64_t amount,
		uint64_t total) override
	{
		if (!onElemProgress.IsEmpty())
		{
			onElemProgress.Call({
				fromTxPackage(env, item),
				BigInt::New(env, amount),
				BigInt::New(env, total),
			});
		}
	}

	void verify_progress(uint64_t amount, uint64_t total) override
	{
		if (!onVerifyProgress.IsEmpty())
		{
			onVerifyProgress.Call({
				BigInt::New(env, amount),
				BigInt::New(env, total),
			});
		}
	}

	void verify_start(uint64_t total) override
	{
		if (!onVerifyStart.IsEmpty())
			onVerifyStart.Call({BigInt::New(env, total)});
	}

	void verify_stop(uint64_t total) override
	{
		if (!onVerifyStop.IsEmpty())
			onVerifyStop.Call({BigInt::New(env, total)});
	}
};
