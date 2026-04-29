import type { Package } from './data.js';
import type { PackageQueryFilter } from './query.js';

export * from './data.js';
export * from './query.js';

declare const dnf5: {
	loadRepos(): void;
	query(...filters: PackageQueryFilter[]): Package[];
};

export default dnf5;
