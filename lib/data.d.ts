export interface Reldep {
	id: number;
	name: string;
	relation: string;
	version: string;
	hash: number;
}

export interface Changelog {
	author: string;
	date: Date;
	text: string;
}

export interface Checksum {
	type: 'unknown' | 'md5' | 'sha1' | 'sha224' | 'sha256' | 'sha384' | 'sha512';
	value: string;
}

export interface Package {
	id: number;
	name: string;
	epoch: string;
	version: string;
	release: string;
	arch: string;
	evr: string;
	nevra: string;
	fullNevra: string;
	nameArch: string;
	group: string;
	downloadSize: bigint;
	installSize: bigint;
	license: string;
	sourceName: string;
	debugSourceName: string;
	sourceDebugInfoName: string;
	debugInfoName: string;
	sourceRpm: string;
	buildTime: Date;
	packager: string;
	vendor: string;
	url: string;
	summary: string;
	description: string;
	provides: Reldep[];
	requires: Reldep[];
	requiresPre: Reldep[];
	conflicts: Reldep[];
	obsoletes: Reldep[];
	prereqIgnoreinst: Reldep[];
	regularRequires: Reldep[];
	recommends: Reldep[];
	suggests: Reldep[];
	enhances: Reldep[];
	supplements: Reldep[];
	depends: Reldep[];
	files: string[];
	changelogs: Changelog[];
	baseUrl: string;
	location: string;
	remoteLocations: string[];
	checksum: Checksum;
	headerChecksum: Checksum;
	packagePath?: string;
	isAvailableLocally: boolean;
	isInstalled: boolean;
	isExcluded: boolean;
	originRepoId: string;
	installTime?: Date;
	repoId: string;
	repoName: string;
	reason: string;
	_string: string;
	_stringDescription: string;
	hash: number;
}
