import type { Package } from './data.ts';
import type { PackageQueryFilter } from './query.ts';
import type {
	TransactionDownloadOptions,
	TransactionGroup,
	TransactionInit,
	TransactionPackage,
	TransactionRunOptions,
} from './transaction.ts';

declare class Transaction {
	private constructor();

	get packages(): TransactionPackage[];
	get packagesCount(): number;
	get groups(): TransactionGroup[];
	get brokenDependencyPackages(): Package[];
	get conflictingPackages(): Package[];
	get isEmpty(): boolean;

	download(options?: TransactionDownloadOptions): void;
	run(options?: TransactionRunOptions): void;
	setDescription(description: string): void;
}

declare const dnf5: {
	Transaction: typeof Transaction;
	loadRepos(): void;
	query(...filters: PackageQueryFilter[]): Package[];
	transaction(init: TransactionInit): Transaction;
};

export default dnf5;
