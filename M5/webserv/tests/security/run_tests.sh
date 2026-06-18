#!/bin/bash

# Security Test Suite Runner
# Convenient wrapper for running WebServ security tests

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Check if Python 3 is installed
if ! command -v python3 &> /dev/null; then
    echo -e "${RED}ERROR: Python 3 is not installed${NC}"
    exit 1
fi

# Check if requests module is installed
if ! python3 -c "import requests" 2>/dev/null; then
    echo -e "${YELLOW}WARNING: requests module not installed${NC}"
    echo "Installing requests module..."
    pip3 install requests
fi

# Check if server is running
if ! curl -s http://localhost:8080 > /dev/null 2>&1; then
    echo -e "${RED}ERROR: WebServ is not running on localhost:8080${NC}"
    echo ""
    echo "Start the server first:"
    echo "  cd ../.."
    echo "  ./webserv webserv.conf"
    echo ""
    exit 1
fi

# Function to run a test
run_test() {
    local test_name=$1
    echo -e "${BLUE}Running $test_name...${NC}"
    python3 "$test_name"
    echo ""
}

# Parse command line arguments
case "${1:-all}" in
    all)
        echo -e "${GREEN}Running ALL security tests...${NC}"
        echo ""
        python3 run_all_security_tests.py
        ;;
    
    path)
        run_test "test_path_traversal.py"
        ;;
    
    file)
        run_test "test_file_operations.py"
        ;;
    
    cgi)
        run_test "test_cgi_injection.py"
        ;;
    
    dos)
        run_test "test_dos_attacks.py"
        ;;
    
    input)
        run_test "test_input_validation.py"
        ;;
    
    smuggling)
        run_test "test_http_smuggling.py"
        ;;
    
    info)
        run_test "test_information_disclosure.py"
        ;;
    
    critical)
        echo -e "${GREEN}Running CRITICAL security tests...${NC}"
        echo ""
        run_test "test_path_traversal.py"
        run_test "test_file_operations.py"
        run_test "test_cgi_injection.py"
        ;;
    
    quick)
        echo -e "${GREEN}Running QUICK security scan...${NC}"
        echo ""
        run_test "test_path_traversal.py"
        run_test "test_cgi_injection.py"
        ;;
    
    help|--help|-h)
        echo "WebServ Security Test Suite"
        echo ""
        echo "Usage: $0 [command]"
        echo ""
        echo "Commands:"
        echo "  all         Run all security tests (default)"
        echo "  critical    Run only critical security tests"
        echo "  quick       Run quick security scan"
        echo ""
        echo "  path        Test path traversal vulnerabilities"
        echo "  file        Test file operation vulnerabilities"
        echo "  cgi         Test CGI injection vulnerabilities"
        echo "  dos         Test DoS resistance"
        echo "  input       Test input validation"
        echo "  smuggling   Test HTTP request smuggling"
        echo "  info        Test information disclosure"
        echo ""
        echo "  help        Show this help message"
        echo ""
        echo "Examples:"
        echo "  $0              # Run all tests"
        echo "  $0 critical     # Run critical tests only"
        echo "  $0 path         # Run path traversal tests"
        echo ""
        exit 0
        ;;
    
    *)
        echo -e "${RED}Unknown command: $1${NC}"
        echo "Use '$0 help' to see available commands"
        exit 1
        ;;
esac

# Show where reports are saved
if [ "$1" = "all" ] || [ -z "$1" ]; then
    echo -e "${BLUE}Reports saved in: results/${NC}"
    if [ -d "results" ]; then
        latest=$(ls -t results/ | head -n 1)
        if [ -n "$latest" ]; then
            echo -e "${BLUE}Latest report: results/$latest${NC}"
        fi
    fi
fi
