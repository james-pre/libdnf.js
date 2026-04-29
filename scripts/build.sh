#!/bin/bash

# Credit: Dave Dopson, https://stackoverflow.com/a/246128/17637456
project_dir=$(dirname $( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd ))

set -euo pipefail

npx cmake-js -d "$project_dir" clean
npx cmake-js -d "$project_dir" build

source "$project_dir/node.env"