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

	/**
	 * Download the needed packages
	 */
	download(options?: TransactionDownloadOptions): void;

	/**
	 * Run the transaction
	 */
	run(options?: TransactionRunOptions): void;

	/**
	 * Set the description to be used in the transaction history
	 */
	setDescription(description: string): void;
}

declare const dnf5: {
	Transaction: typeof Transaction;

	/**
	 * Load default repositories
	 */
	loadRepos(this: void): void;

	/**
	 * Query the loaded repositories using the given filters
	 */
	query(this: void, ...filters: PackageQueryFilter[]): Package[];

	/**
	 * Create a new transaction
	 */
	transaction(this: void, init: TransactionInit): Transaction;
};

export default dnf5;
