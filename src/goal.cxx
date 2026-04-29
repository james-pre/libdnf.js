
#include "goal.hxx"
#include "query.hxx"

libdnf5::transaction::TransactionItemReason requireReason(const Object &op)
{

	if (!op.Has("reason"))
		throw Error::New(op.Env(), "'reason' is required");

	const Value reason = op.Get("reason");
	if (!reason.IsString())
		throw TypeError::New(op.Env(), "'reason' must be a string");

	return libdnf5::transaction::transaction_item_reason_from_string(reason.ToString().Utf8Value());
}

void createGoal(libdnf5::Goal &goal, const Object &options)
{
	Env env = options.Env();

	goal.set_allow_erasing(options.Get("allowErasing").ToBoolean());

	const Value _ops = options.Get("operations");
	if (!_ops.IsArray())
		throw TypeError::New(env, "'operations' must be an array");

	const Array ops = _ops.As<Array>();

	for (uint32_t i = 0; i < ops.Length(); ++i)
	{
		const Value _op = ops.Get(i);
		if (!_op.IsObject())
			throw TypeError::New(env, "'operations' must be an array of objects");

		const Object op = _op.As<Object>();

		const std::string name = op.Get("name").ToString().Utf8Value();
		const std::string spec = op.Get("spec").ToString().Utf8Value();

		libdnf5::GoalJobSettings settings = libdnf5::GoalJobSettings();

		if (name == "debug_install")
			goal.add_debug_install(spec, settings);
		else if (name == "downgrade")
			goal.add_downgrade(spec, settings);
		else if (name == "group_install")
			goal.add_group_install(spec, requireReason(op), settings);
		else if (name == "group_remove")
			goal.add_group_remove(spec, requireReason(op), settings);
		else if (name == "group_upgrade")
			goal.add_group_upgrade(spec, settings);
		else if (name == "install")
			goal.add_install(spec, settings);
		else if (name == "module_disable")
			goal.add_module_disable(spec, settings);
		else if (name == "module_enable")
			goal.add_module_enable(spec, settings);
		else if (name == "module_reset")
			goal.add_module_reset(spec, settings);
		else if (name == "provide_install")
			goal.add_provide_install(spec, settings);
		// else if (name == "redo_transaction")
		// 	goal.add_redo_transaction();
		else if (name == "reinstall")
			goal.add_reinstall(spec, settings);
		// else if (name == "revert_transactions")
		// 	goal.add_revert_transactions();
		else if (name == "remove")
			goal.add_remove(spec, settings);
		else if (name == "rpm_distro_sync")
			goal.add_rpm_distro_sync(settings);
		else if (name == "rpm_downgrade")
			goal.add_rpm_downgrade(spec, settings);
		else if (name == "rpm_install")
			goal.add_rpm_install(spec, settings);
		else if (name == "rpm_install_or_reinstall")
		{
			Value packages = op.Get("packages");

			if (!packages.IsArray())
				throw TypeError::New(env, "Invalid value for 'rpm_install_or_reinstall' packages");

			libdnf5::rpm::PackageSet package_set(base);

			for (const auto &pkg : createPackageQuery(packages.As<Array>()))
				package_set.add(pkg);

			goal.add_rpm_install_or_reinstall(package_set, settings);
		}
		else if (name == "rpm_reason_change")
			goal.add_rpm_reason_change(spec, requireReason(op), op.Has("groupId") ? op.Get("groupId").ToString().Utf8Value() : "", settings);
		else if (name == "rpm_reinstall")
			goal.add_rpm_reinstall(spec, settings);
		else if (name == "rpm_remove")
			goal.add_rpm_remove(spec, settings);
		else if (name == "rpm_upgrade")
			goal.add_rpm_upgrade(spec, settings, op.Get("minimal").ToBoolean());
		else if (name == "serialized_transaction")
			goal.add_serialized_transaction(op.Get("path").ToString().Utf8Value(), settings);
		else if (name == "upgrade")
			goal.add_upgrade(spec, settings);
		else
			throw Error::New(env, "Invalid operation: " + name);
	}
}
