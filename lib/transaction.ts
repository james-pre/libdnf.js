import type { Group, Nevra, Package } from './data.js';
import type { PackageQueryFilter } from './query.js';

export interface GoalOpPlain {
	type: 'rpm_distro_sync';
}

export interface GoalOpWithSpec {
	type:
		| 'debug_install'
		| 'downgrade'
		| 'group_upgrade'
		| 'install'
		| 'module_disable'
		| 'module_enable'
		| 'module_reset'
		| 'provide_install'
		| 'reinstall'
		| 'remove'
		| 'rpm_downgrade'
		| 'rpm_install'
		| 'rpm_reinstall'
		| 'rpm_remove'
		| 'upgrade';
	spec: string;
}

export interface GoalOpWithSpecReason {
	type: 'group_install' | 'group_remove';
	spec: string;
	reason: string;
}

export interface GoalOpRpmInstallOrReinstall {
	type: 'rpm_install_or_reinstall';
	packages: PackageQueryFilter[];
}

export interface GoalOpRpmReasonChange {
	type: 'rpm_reason_change';
	spec: string;
	reason: string;
	groupId?: string;
}

export interface GoalOpRpmUpgrade {
	type: 'rpm_upgrade';
	spec: string;
	minimal?: boolean;
}

export interface GoalOpSerializedTransaction {
	type: 'serialized_transaction';
	path: string;
}

export type GoalOp =
	| GoalOpPlain
	| GoalOpWithSpec
	| GoalOpWithSpecReason
	| GoalOpRpmInstallOrReinstall
	| GoalOpRpmReasonChange
	| GoalOpRpmUpgrade
	| GoalOpSerializedTransaction;

export interface TransactionInit {
	allowErasing?: boolean;
	operations: GoalOp[];
}

export interface TransactionPackage {
	package: Package;
	action: string;
	state: string;
	reason: string;
	replaces: Package[];
	replacedBy: Package[];
}

export interface TransactionGroup {
	group: Group;
	action: string;
	state: string;
	reason: string;
}

export interface TransactionDownloadOptions {
	onDownload?(description: string, totalToDownload: number): void;
	onProgress?(totalToDownload: number, downloaded: number): void;
	onEnd?(status: number, message: string): void;
	onMirrorFailure?(message: string, url: string, metadata: string): void;
}

export type TransactionPackageCallback = (item: TransactionPackage) => void;
export type TransactionTotalCallback = (total: bigint) => void;
export type TransactionAmountTotalCallback = (amount: bigint, total: bigint) => void;
export type TransactionPackageTotalCallback = (item: TransactionPackage, total: bigint) => void;

export type TransactionPackageAmountTotalCallback = (item: TransactionPackage, amount: bigint, total: bigint) => void;

export type TransactionScriptStopCallback = (
	item: TransactionPackage | null,
	nevra: Nevra,
	type: string,
	returnCode: bigint
) => void;

export interface TransactionRunOptions {
	onBeforeBegin?: TransactionTotalCallback;
	onAfterComplete?: (success: boolean) => void;

	onInstallStart?: TransactionPackageTotalCallback;
	onInstallProgress?: TransactionPackageAmountTotalCallback;
	onInstallStop?: TransactionPackageAmountTotalCallback;

	onTransactionStart?: TransactionTotalCallback;
	onTransactionProgress?: TransactionAmountTotalCallback;
	onTransactionStop?: TransactionTotalCallback;

	onUninstallStart?: TransactionPackageTotalCallback;
	onUninstallProgress?: TransactionPackageAmountTotalCallback;
	onUninstallStop?: TransactionPackageAmountTotalCallback;

	onUnpackError?: TransactionPackageCallback;
	onCpioError?: TransactionPackageCallback;

	onScriptStart?(item: TransactionPackage | null, nevra: Nevra, type: string): void;
	onScriptStop?: TransactionScriptStopCallback;
	onScriptError?: TransactionScriptStopCallback;

	onElemProgress?: TransactionPackageAmountTotalCallback;

	onVerifyStart?: TransactionTotalCallback;
	onVerifyProgress?: TransactionAmountTotalCallback;
	onVerifyStop?: TransactionTotalCallback;
}
