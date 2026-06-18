#!/bin/bash

# WebServ Session & Cookie Test Runner
# Runs all test suites and generates combined report

# Color codes
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m'

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo -e "${CYAN}"
echo "╔════════════════════════════════════════════════╗"
echo "║   WebServ Session & Cookie Test Suite         ║"
echo "║   Comprehensive Testing Framework              ║"
echo "╚════════════════════════════════════════════════╝"
echo -e "${NC}"

# Check if server is running
echo -e "${YELLOW}Checking server status...${NC}"
if ! curl -s http://localhost:8080 > /dev/null 2>&1; then
    echo -e "${RED}ERROR: WebServ server is not running on localhost:8080${NC}"
    echo -e "${YELLOW}Please start the server first:${NC} ./webserv config/webserv.conf"
    exit 1
fi
echo -e "${GREEN}✓ Server is running${NC}\n"

# Make scripts executable
chmod +x "$SCRIPT_DIR/test_sessions.sh" "$SCRIPT_DIR/test_cookies.sh" 2>/dev/null

# Track overall results
TOTAL_PASSED=0
TOTAL_FAILED=0
SUITE_FAILURES=0

# Run Session Tests
echo -e "${BLUE}═══════════════════════════════════════════════${NC}"
echo -e "${BLUE}Running Session Management Tests...${NC}"
echo -e "${BLUE}═══════════════════════════════════════════════${NC}\n"

if "$SCRIPT_DIR/test_sessions.sh"; then
    SESSION_RESULT="PASSED"
else
    SESSION_RESULT="FAILED"
    SUITE_FAILURES=$((SUITE_FAILURES + 1))
fi

echo -e "\n"

# Run Cookie Tests
echo -e "${BLUE}═══════════════════════════════════════════════${NC}"
echo -e "${BLUE}Running Cookie Management Tests...${NC}"
echo -e "${BLUE}═══════════════════════════════════════════════${NC}\n"

if "$SCRIPT_DIR/test_cookies.sh"; then
    COOKIE_RESULT="PASSED"
else
    COOKIE_RESULT="FAILED"
    SUITE_FAILURES=$((SUITE_FAILURES + 1))
fi

# Parse results from log files
if [ -f "/tmp/webserv_session_test_results.log" ]; then
    SESSION_PASSED=$(grep -c "PASS" /tmp/webserv_session_test_results.log)
    SESSION_FAILED=$(grep -c "FAIL" /tmp/webserv_session_test_results.log)
    TOTAL_PASSED=$((TOTAL_PASSED + SESSION_PASSED))
    TOTAL_FAILED=$((TOTAL_FAILED + SESSION_FAILED))
fi

if [ -f "/tmp/webserv_cookie_test_results.log" ]; then
    COOKIE_PASSED=$(grep -c "PASS" /tmp/webserv_cookie_test_results.log)
    COOKIE_FAILED=$(grep -c "FAIL" /tmp/webserv_cookie_test_results.log)
    TOTAL_PASSED=$((TOTAL_PASSED + COOKIE_PASSED))
    TOTAL_FAILED=$((TOTAL_FAILED + COOKIE_FAILED))
fi

TOTAL_TESTS=$((TOTAL_PASSED + TOTAL_FAILED))
if [ $TOTAL_TESTS -gt 0 ]; then
    SUCCESS_RATE=$(awk "BEGIN {printf \"%.1f\", ($TOTAL_PASSED/$TOTAL_TESTS)*100}")
else
    SUCCESS_RATE="0.0"
fi

# Generate Final Report
echo -e "\n${CYAN}"
echo "╔════════════════════════════════════════════════╗"
echo "║           FINAL TEST REPORT                    ║"
echo "╚════════════════════════════════════════════════╝"
echo -e "${NC}"

echo -e "${BLUE}Test Suite Results:${NC}"
echo "─────────────────────────────────────────────────"

if [ "$SESSION_RESULT" = "PASSED" ]; then
    echo -e "Session Tests:     ${GREEN}✓ PASSED${NC} ($SESSION_PASSED passed, $SESSION_FAILED failed)"
else
    echo -e "Session Tests:     ${RED}✗ FAILED${NC} ($SESSION_PASSED passed, $SESSION_FAILED failed)"
fi

if [ "$COOKIE_RESULT" = "PASSED" ]; then
    echo -e "Cookie Tests:      ${GREEN}✓ PASSED${NC} ($COOKIE_PASSED passed, $COOKIE_FAILED failed)"
else
    echo -e "Cookie Tests:      ${RED}✗ FAILED${NC} ($COOKIE_PASSED passed, $COOKIE_FAILED failed)"
fi

echo ""
echo -e "${BLUE}Overall Statistics:${NC}"
echo "─────────────────────────────────────────────────"
echo -e "Total Tests Run:   ${TOTAL_TESTS}"
echo -e "Tests Passed:      ${GREEN}${TOTAL_PASSED}${NC}"
echo -e "Tests Failed:      ${RED}${TOTAL_FAILED}${NC}"
echo -e "Success Rate:      ${SUCCESS_RATE}%"
echo ""

# Categorize result
if [ $TOTAL_FAILED -eq 0 ]; then
    echo -e "${GREEN}╔════════════════════════════════════════════════╗"
    echo -e "║         ALL TESTS PASSED! ✓✓✓                 ║"
    echo -e "╚════════════════════════════════════════════════╝${NC}"
    EXIT_CODE=0
elif [ $SUITE_FAILURES -eq 0 ]; then
    echo -e "${YELLOW}╔════════════════════════════════════════════════╗"
    echo -e "║     TESTS COMPLETED WITH MINOR FAILURES        ║"
    echo -e "╚════════════════════════════════════════════════╝${NC}"
    EXIT_CODE=0
else
    echo -e "${RED}╔════════════════════════════════════════════════╗"
    echo -e "║     TESTS COMPLETED WITH FAILURES ✗            ║"
    echo -e "╚════════════════════════════════════════════════╝${NC}"
    EXIT_CODE=1
fi

echo ""
echo -e "${BLUE}Detailed Logs:${NC}"
echo "  Session Tests: /tmp/webserv_session_test_results.log"
echo "  Cookie Tests:  /tmp/webserv_cookie_test_results.log"
echo ""

# Generate timestamp for report
TIMESTAMP=$(date "+%Y-%m-%d %H:%M:%S")
echo -e "${CYAN}Test run completed at: $TIMESTAMP${NC}"

# Save combined report
REPORT_FILE="/tmp/webserv_all_tests_report.txt"
cat > "$REPORT_FILE" << EOF
WebServ Session & Cookie Test Report
Generated: $TIMESTAMP
═════════════════════════════════════════════════

TEST SUITE RESULTS:
─────────────────────────────────────────────────
Session Tests: $SESSION_RESULT ($SESSION_PASSED passed, $SESSION_FAILED failed)
Cookie Tests:  $COOKIE_RESULT ($COOKIE_PASSED passed, $COOKIE_FAILED failed)

OVERALL STATISTICS:
─────────────────────────────────────────────────
Total Tests:   $TOTAL_TESTS
Passed:        $TOTAL_PASSED
Failed:        $TOTAL_FAILED
Success Rate:  $SUCCESS_RATE%

DETAILED LOGS:
─────────────────────────────────────────────────
Session Tests: /tmp/webserv_session_test_results.log
Cookie Tests:  /tmp/webserv_cookie_test_results.log

═════════════════════════════════════════════════
EOF

echo -e "Combined report saved to: ${REPORT_FILE}\n"

exit $EXIT_CODE
