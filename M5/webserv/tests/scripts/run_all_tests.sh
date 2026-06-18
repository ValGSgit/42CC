#!/bin/bash
# run_all_tests.sh - Run all WebServ test suites

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
HOST="localhost"
PORT=8080
CONFIG="webserv.conf"

# Parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --host)
            HOST="$2"
            shift 2
            ;;
        --port)
            PORT="$2"
            shift 2
            ;;
        --config)
            CONFIG="$2"
            shift 2
            ;;
        -h|--help)
            echo "Usage: $0 [OPTIONS]"
            echo ""
            echo "Options:"
            echo "  --host HOST      Server hostname (default: localhost)"
            echo "  --port PORT      Server port (default: 8080)"
            echo "  --config FILE    Config file to use (default: webserv.conf)"
            echo "  -h, --help       Show this help message"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

echo -e "${BLUE}======================================${NC}"
echo -e "${BLUE}WebServ Comprehensive Test Suite${NC}"
echo -e "${BLUE}======================================${NC}"
echo ""
echo -e "Server: ${YELLOW}${HOST}:${PORT}${NC}"
echo -e "Config: ${YELLOW}${CONFIG}${NC}"
echo ""

# Check if Python 3 is available
if ! command -v python3 &> /dev/null; then
    echo -e "${RED}Error: Python 3 is required but not found${NC}"
    exit 1
fi

# Check if server is running
echo -e "${BLUE}Checking if server is running...${NC}"
if ! python3 -c "import socket; s = socket.socket(); s.settimeout(2); s.connect(('${HOST}', ${PORT})); s.close()" 2>/dev/null; then
    echo -e "${RED}Error: Server is not running on ${HOST}:${PORT}${NC}"
    echo -e "${YELLOW}Please start your server first:${NC}"
    echo -e "  ./webserv ${CONFIG}"
    exit 1
fi
echo -e "${GREEN}✓ Server is running${NC}"
echo ""

# Track results
TOTAL_SUITES=0
PASSED_SUITES=0

# Function to run a test suite
run_test_suite() {
    local name=$1
    local script=$2
    
    TOTAL_SUITES=$((TOTAL_SUITES + 1))
    
    echo -e "${BLUE}======================================${NC}"
    echo -e "${BLUE}Running: ${name}${NC}"
    echo -e "${BLUE}======================================${NC}"
    echo ""
    
    if [ ! -f "$script" ]; then
        echo -e "${RED}Error: Test script not found: ${script}${NC}"
        return 1
    fi
    
    if python3 "$script" --host "$HOST" --port "$PORT"; then
        echo -e "${GREEN}✓ ${name} completed successfully${NC}"
        PASSED_SUITES=$((PASSED_SUITES + 1))
        return 0
    else
        echo -e "${RED}✗ ${name} failed${NC}"
        return 1
    fi
}

# Run test suites
echo -e "${YELLOW}Starting test execution...${NC}"
echo ""

# 1. Main functionality tests
run_test_suite "Main Functionality Tests" "./tests/scripts/test_webserv.py"
echo ""

# 2. CGI tests
run_test_suite "CGI Tests" "./tests/scripts/test_cgi.py"
echo ""

# 3. Stress tests (optional - can be skipped)
read -p "Run stress tests? This may take several minutes (y/n): " -n 1 -r
echo ""
if [[ $REPLY =~ ^[Yy]$ ]]; then
    run_test_suite "Stress Tests" "./tests/scripts/stress_test.py"
    echo ""
else
    echo -e "${YELLOW}Skipping stress tests${NC}"
    echo ""
fi

# Final summary
echo -e "${BLUE}======================================${NC}"
echo -e "${BLUE}FINAL SUMMARY${NC}"
echo -e "${BLUE}======================================${NC}"
echo ""
echo -e "Test Suites Run: ${TOTAL_SUITES}"
echo -e "${GREEN}Passed: ${PASSED_SUITES}${NC}"
echo -e "${RED}Failed: $((TOTAL_SUITES - PASSED_SUITES))${NC}"
echo ""

if [ $PASSED_SUITES -eq $TOTAL_SUITES ]; then
    echo -e "${GREEN}🎉 All test suites passed! 🎉${NC}"
    exit 0
else
    echo -e "${RED}Some test suites failed. Please review the output above.${NC}"
    exit 1
fi
