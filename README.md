# libdnf5 JavaScript bindings

This library is a best-effort set of JS bindings for libdnf5.

## Installation

```sh
npm install libdnf
```

## Usage Example

```ts
import * as dnf5 from 'libdnf';
import { createInterface } from 'node:readline/promises';

using rl = createInterface({ input: process.stdin, output: process.stdout });

process.stdout.write('Loading repositories... ');
dnf5.loadRepos();
console.log('done.');

const packages = dnf5.query({ type: 'name', value: 'kernel' }, 'installed');
console.log('Installed kernels:', packages.map(pkg => pkg.nevra).join(', '));

const tx = dnf5.transaction({
	operations: [{ type: 'install', spec: 'chromium' }],
});

console.log('Installing:');
for (const pkg of tx.packages) {
	console.log(pkg.action, pkg.package.nevra, pkg.reason);
}

const result = await rl.question('Is this okay [y/N]: ').catch(() => null);
if (result != 'y') {
	console.log('Aborted.');
	process.exit(0);
}

process.stdout.write('Downloading... ');
tx.download();
console.log('done.');

process.stdout.write('Running... ');
tx.run();
console.log('done.');
```

## Frequent errors

#### `TypeError [ERR_UNKNOWN_FILE_EXTENSION]: Unknown file extension ".node" for .../dist/libdnf5.node`

You need to enable `--experimental-addon-modules`, this can be done via an env option.

#### `Error: Failed to open lock file "/run/dnf/rpmtransaction.lock": (13) - Permission denied`

Running a transaction requires root privileges.
