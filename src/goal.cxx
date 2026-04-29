
#include "goal.hxx"
#include "query.hxx"
#include "schema.hxx"

struct GoalOperation
{
	std::string type;
	std::optional<std::string> spec;
	std::optional<std::string> reason;
	std::optional<std::string> groupId;
	std::optional<std::string> path;
	bool minimal;
	std::optional<std::vector<Value>> packages;
};

struct GoalOptions
{
	bool allowErasing;
	std::vector<GoalOperation> operations;
};

libdnf5::transaction::TransactionItemReason requireReason(const Env &env, const GoalOperation &op)
{

	if (!op.reason.has_value())
		throw Error::New(env, "'reason' is required");

	return libdnf5::transaction::transaction_item_reason_from_string(op.reason.value());
}

namespace s = schema;

auto goalSchema = s::object<GoalOptions>({
	s::field<&GoalOptions::allowErasing>("allowErasing", s::boolean().coerce().defaultTo(false)),
	s::field<&GoalOptions::operations>(
		"operations",
		s::array(s::object<GoalOperation>({
			s::field<&GoalOperation::type>("type", s::string()),
			s::field<&GoalOperation::spec>("spec", s::string().coerce().optional()),
			s::field<&GoalOperation::reason>("reason", s::string().optional()),
			s::field<&GoalOperation::groupId>("groupId", s::string().optional()),
			s::field<&GoalOperation::path>("path", s::string().optional()),
			s::field<&GoalOperation::minimal>("minimal", s::boolean().coerce().defaultTo(false)),
			s::field<&GoalOperation::packages>("packages", s::array(s::js_value()).optional()),
		}))),
});

void createGoal(libdnf5::Goal &goal, const Object &rawOptions)
{
	Env env = rawOptions.Env();
	GoalOptions options = goalSchema.parse(rawOptions);

	goal.set_allow_erasing(options.allowErasing);

	for (const auto &op : options.operations)
	{

		const std::string type = op.type;
		const std::string spec = op.spec.value_or("");

		libdnf5::GoalJobSettings settings = libdnf5::GoalJobSettings();

		if (type == "debug_install")
			goal.add_debug_install(spec, settings);
		else if (type == "downgrade")
			goal.add_downgrade(spec, settings);
		else if (type == "group_install")
			goal.add_group_install(spec, requireReason(env, op), settings);
		else if (type == "group_remove")
			goal.add_group_remove(spec, requireReason(env, op), settings);
		else if (type == "group_upgrade")
			goal.add_group_upgrade(spec, settings);
		else if (type == "install")
			goal.add_install(spec, settings);
		else if (type == "module_disable")
			goal.add_module_disable(spec, settings);
		else if (type == "module_enable")
			goal.add_module_enable(spec, settings);
		else if (type == "module_reset")
			goal.add_module_reset(spec, settings);
		else if (type == "provide_install")
			goal.add_provide_install(spec, settings);
		// else if (type == "redo_transaction")
		// 	goal.add_redo_transaction();
		else if (type == "reinstall")
			goal.add_reinstall(spec, settings);
		// else if (type == "revert_transactions")
		// 	goal.add_revert_transactions();
		else if (type == "remove")
			goal.add_remove(spec, settings);
		else if (type == "rpm_distro_sync")
			goal.add_rpm_distro_sync(settings);
		else if (type == "rpm_downgrade")
			goal.add_rpm_downgrade(spec, settings);
		else if (type == "rpm_install")
			goal.add_rpm_install(spec, settings);
		else if (type == "rpm_install_or_reinstall")
		{
			if (!op.packages.has_value())
				throw Error::New(env, "Missing 'packages' for 'rpm_install_or_reinstall' operation");

			libdnf5::rpm::PackageSet package_set(base);

			for (const auto &pkg : createPackageQuery(env, op.packages.value()))
				package_set.add(pkg);

			goal.add_rpm_install_or_reinstall(package_set, settings);
		}
		else if (type == "rpm_reason_change")
		{
			if (!op.groupId.has_value())
				throw Error::New(env, "Missing 'groupId' for 'rpm_reason_change' operation");

			goal.add_rpm_reason_change(spec, requireReason(env, op), op.groupId.value(), settings);
		}
		else if (type == "rpm_reinstall")
			goal.add_rpm_reinstall(spec, settings);
		else if (type == "rpm_remove")
			goal.add_rpm_remove(spec, settings);
		else if (type == "rpm_upgrade")
			goal.add_rpm_upgrade(spec, settings, op.minimal);
		else if (type == "serialized_transaction")
		{
			if (!op.path.has_value())
				throw Error::New(env, "Missing 'path' for 'serialized_transaction' operation");

			goal.add_serialized_transaction(op.path.value(), settings);
		}
		else if (type == "upgrade")
			goal.add_upgrade(spec, settings);
		else
			throw Error::New(env, "Invalid operation: " + type);
	}
}
