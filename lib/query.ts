export type QueryCmp =
	| 'eq'
	| 'neq'
	| 'gt'
	| 'gte'
	| 'lt'
	| 'lte'
	| 'exact'
	| 'not_exact'
	| 'iexact'
	| 'not_iexact'
	| 'contains'
	| 'not_contains'
	| 'icontains'
	| 'not_icontains'
	| 'startswith'
	| 'istartswith'
	| 'endswith'
	| 'iendswith'
	| 'regex'
	| 'iregex'
	| 'glob'
	| 'not_glob'
	| 'iglob'
	| 'not_iglob';

export type AdvisoryQueryFilterType = 'name' | 'packages' | 'reference' | 'severity' | 'type';

export interface AdvisoryQueryFilterEntry {
	filter: AdvisoryQueryFilterType;
	value: string | string[];
	cmp?: QueryCmp;
}

export interface PackageQueryFilterAdvisories {
	type: 'advisories' | 'latest_unresolved_advisories';
	value: AdvisoryQueryFilterEntry[];
	cmp?: QueryCmp;
}

export interface PackageQueryFilterExtras {
	type: 'extras';
	exact_evr?: boolean;
}

export interface PackageQueryFilterNameArch {
	type: 'name_arch';
	value: PackageQueryFilter[];
	cmp?: QueryCmp;
}

export interface PackageQueryFilterRecent {
	type: 'recent';
	timestamp?: number | bigint | Date;
}

export type PackageQueryFilterNoValueType =
	| 'available'
	| 'downgradable'
	| 'downgrades'
	| 'duplicates'
	| 'installed'
	| 'installonly'
	| 'leaves'
	| 'leaves_groups'
	| 'priority'
	| 'reboot_suggested'
	| 'recent'
	| 'unneeded'
	| 'upgradable'
	| 'upgrades'
	| 'userinstalled'
	| 'versionlock';

export interface PackageQueryFilterNoValue {
	type: PackageQueryFilterNoValueType;
}

export type PackageQueryFilterWithLimitType =
	| 'earliest_evr'
	| 'earliest_evr_any_arch'
	| 'latest_evr'
	| 'latest_evr_any_arch';

export interface PackageQueryFilterWithLimit {
	type: PackageQueryFilterWithLimitType;
	value: string | string[];
	cmp?: QueryCmp;
}

export type PackageQueryFilterWithValueType =
	| 'arch'
	| 'conflicts'
	| 'description'
	| 'enhances'
	| 'epoch'
	| 'evr'
	| 'file'
	| 'from_repo_id'
	| 'location'
	| 'name'
	| 'nevra'
	| 'obsoletes'
	| 'provides'
	| 'recommends'
	| 'release'
	| 'repo_id'
	| 'requires'
	| 'sourcerpm'
	| 'suggests'
	| 'summary'
	| 'supplements'
	| 'url'
	| 'version';

export interface PackageQueryFilterWithValue {
	type: PackageQueryFilterWithValueType;
	value: string | string[];
	cmp?: QueryCmp;
}

export type PackageQueryFilter =
	| PackageQueryFilterNoValueType
	| PackageQueryFilterNoValue
	| PackageQueryFilterWithValue
	| PackageQueryFilterWithLimit
	| PackageQueryFilterAdvisories
	| PackageQueryFilterExtras
	| PackageQueryFilterNameArch
	| PackageQueryFilterRecent;
