#!/bin/bash

################################################################################
# WEBSERV TEST RUNNER WRAPPER
# Calls the master test suite in tests/run_all_tests.sh
################################################################################

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Call the master test runner
exec "${SCRIPT_DIR}/tests/run_all_tests.sh" "$@"
