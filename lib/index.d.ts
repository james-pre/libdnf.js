import type { Package } from './data.js';
import type { PackageQueryFilter } from './query.js';
import type { Transaction, TransactionInit } from './transaction.js';

export * from './data.js';
export * from './query.js';

declare const dnf5: {
	loadRepos(): void;
	query(...filters: PackageQueryFilter[]): Package[];
	transaction(init: TransactionInit): Transaction;
};

export default dnf5;
