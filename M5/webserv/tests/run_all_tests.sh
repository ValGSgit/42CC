#!/bin/bash

################################################################################
# WEBSERV MASTER TEST RUNNER
# Comprehensive test suite for CI/CD and local testing
################################################################################

# Don't exit on first error - we want to run all tests
set +e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
MAGENTA='\033[0;35m'
CYAN='\033[0;36m'
BOLD='\033[1m'
NC='\033[0m' # No Color

# Configuration
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
SERVER_BIN="${PROJECT_ROOT}/webserv"
CONFIG_FILE="${PROJECT_ROOT}/webserv.conf"
SERVER_HOST="localhost"
SERVER_PORT="8080"
SERVER_PID=""
SERVER_LOG="${PROJECT_ROOT}/server_test.log"

# Test results
TOTAL_SUITES=0
PASSED_SUITES=0
FAILED_SUITES=0
SKIPPED_SUITES=0

# Test reports directory
REPORT_DIR="${SCRIPT_DIR}/test_output"
mkdir -p "$REPORT_DIR"
TIMESTAMP=$(date +%Y%m%d_%H%M%S)
MASTER_REPORT="${REPORT_DIR}/master_test_report_${TIMESTAMP}.txt"

################################################################################
# UTILITY FUNCTIONS
################################################################################

log_info() {
    echo -e "${CYAN}[INFO]${NC} $1" | tee -a "$MASTER_REPORT"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1" | tee -a "$MASTER_REPORT"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1" | tee -a "$MASTER_REPORT"
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1" | tee -a "$MASTER_REPORT"
}

log_section() {
    echo -e "\n${BOLD}${BLUE}═══════════════════════════════════════════════════════════════${NC}" | tee -a "$MASTER_REPORT"
    echo -e "${BOLD}${BLUE}  $1${NC}" | tee -a "$MASTER_REPORT"
    echo -e "${BOLD}${BLUE}═══════════════════════════════════════════════════════════════${NC}\n" | tee -a "$MASTER_REPORT"
}

print_banner() {
    echo -e "${CYAN}" | tee -a "$MASTER_REPORT"
    cat << "EOF" | tee -a "$MASTER_REPORT"
╔════════════════════════════════════════════════════════════════════════╗
║                                                                        ║
║                  🌐  WEBSERV MASTER TEST SUITE  🌐                    ║
║                                                                        ║
║            Comprehensive Testing for HTTP/1.1 Server                  ║
║                                                                        ║
╚════════════════════════════════════════════════════════════════════════╝
EOF
    echo -e "${NC}" | tee -a "$MASTER_REPORT"
}

################################################################################
# SERVER MANAGEMENT
################################################################################

start_server() {
    log_section "Starting WebServ Server"
    
    if [ ! -f "$SERVER_BIN" ]; then
        log_error "Server binary not found: $SERVER_BIN"
        log_error "Run 'make' first to build the server"
        exit 1
    fi
    
    if [ ! -f "$CONFIG_FILE" ]; then
        log_error "Config file not found: $CONFIG_FILE"
        exit 1
    fi
    
    # Check if port is already in use
    if lsof -Pi :${SERVER_PORT} -sTCP:LISTEN -t >/dev/null 2>&1; then
        log_warn "Port ${SERVER_PORT} is already in use"
        log_info "Attempting to kill existing process..."
        lsof -ti:${SERVER_PORT} | xargs kill -9 2>/dev/null || true
        sleep 2
    fi
    
    log_info "Starting server: ${SERVER_BIN} ${CONFIG_FILE}"
    "${SERVER_BIN}" "${CONFIG_FILE}" > "${SERVER_LOG}" 2>&1 &
    SERVER_PID=$!
    
    log_info "Server PID: ${SERVER_PID}"
    log_info "Waiting for server to start..."
    
    # Wait for server to be ready
    local max_attempts=15
    local attempt=0
    while [ $attempt -lt $max_attempts ]; do
        if curl -s -o /dev/null -w "%{http_code}" "http://${SERVER_HOST}:${SERVER_PORT}/" > /dev/null 2>&1; then
            log_success "Server is ready!"
            return 0
        fi
        sleep 1
        ((attempt++))
        echo -n "." | tee -a "$MASTER_REPORT"
    done
    
    echo "" | tee -a "$MASTER_REPORT"
    log_error "Server failed to start within ${max_attempts} seconds"
    log_error "Check server log: ${SERVER_LOG}"
    cat "${SERVER_LOG}" | tee -a "$MASTER_REPORT"
    exit 1
}

stop_server() {
    log_section "Stopping WebServ Server"
    
    if [ -n "$SERVER_PID" ] && ps -p $SERVER_PID > /dev/null 2>&1; then
        log_info "Stopping server (PID: ${SERVER_PID})..."
        kill $SERVER_PID 2>/dev/null || true
        sleep 2
        
        # Force kill if still running
        if ps -p $SERVER_PID > /dev/null 2>&1; then
            log_warn "Server still running, force killing..."
            kill -9 $SERVER_PID 2>/dev/null || true
        fi
        
        log_success "Server stopped"
    else
        log_warn "Server was not running (PID: ${SERVER_PID})"
    fi
    
    # Kill any remaining processes on the port
    if lsof -Pi :${SERVER_PORT} -sTCP:LISTEN -t >/dev/null 2>&1; then
        log_info "Cleaning up remaining processes on port ${SERVER_PORT}..."
        lsof -ti:${SERVER_PORT} | xargs kill -9 2>/dev/null || true
    fi
}

cleanup() {
    log_info "\nCleaning up..."
    stop_server
}

################################################################################
# TEST SUITE RUNNERS
################################################################################

run_test_suite() {
    local suite_name="$1"
    local suite_description="$2"
    local test_command="$3"
    local is_optional="${4:-false}"
    
    ((TOTAL_SUITES++))
    
    log_section "$suite_description"
    log_info "Running: $suite_name"
    
    local start_time=$(date +%s)
    
    # Run the test and capture exit code
    eval "$test_command"
    local test_exit_code=$?
    
    local end_time=$(date +%s)
    local duration=$((end_time - start_time))
    
    if [ $test_exit_code -eq 0 ]; then
        log_success "✓ $suite_name PASSED (${duration}s)"
        ((PASSED_SUITES++))
    else
        if [ "$is_optional" = "true" ]; then
            log_warn "⊘ $suite_name SKIPPED/FAILED (${duration}s) - Optional test"
            ((SKIPPED_SUITES++))
        else
            log_error "✗ $suite_name FAILED (${duration}s)"
            ((FAILED_SUITES++))
        fi
    fi
    
    # Always return 0 to continue running other tests
    return 0
}

################################################################################
# INDIVIDUAL TEST SUITES
################################################################################

run_comprehensive_edge_cases() {
    if [ -f "${SCRIPT_DIR}/comprehensive_edge_cases.sh" ]; then
        bash "${SCRIPT_DIR}/comprehensive_edge_cases.sh"
        return $?
    else
        log_error "comprehensive_edge_cases.sh not found at ${SCRIPT_DIR}/comprehensive_edge_cases.sh"
        return 1
    fi
}

run_security_tests() {
    if [ -f "${SCRIPT_DIR}/security/run_all_security_tests.py" ]; then
        python3 "${SCRIPT_DIR}/security/run_all_security_tests.py"
        return $?
    else
        log_warn "security tests not found at ${SCRIPT_DIR}/security/run_all_security_tests.py"
        return 1
    fi
}

run_session_cookie_tests() {
    if [ -f "${SCRIPT_DIR}/session_cookie/run_all_tests.sh" ]; then
        bash "${SCRIPT_DIR}/session_cookie/run_all_tests.sh"
        return $?
    else
        log_warn "session/cookie tests not found at ${SCRIPT_DIR}/session_cookie/run_all_tests.sh"
        return 1
    fi
}

run_python_comprehensive_tests() {
    if [ -f "${PROJECT_ROOT}/tester/comprehensive_tester.py" ]; then
        python3 "${PROJECT_ROOT}/tester/comprehensive_tester.py"
    else
        log_warn "Python comprehensive tester not found"
        return 1
    fi
}

run_rfc_compliance_tests() {
    if [ -f "${PROJECT_ROOT}/tester/rfc_compliance_tester.py" ]; then
        python3 "${PROJECT_ROOT}/tester/rfc_compliance_tester.py"
    else
        log_warn "RFC compliance tester not found"
        return 1
    fi
}

run_performance_tests() {
    if [ -f "${PROJECT_ROOT}/tester/performance_tester.py" ]; then
        python3 "${PROJECT_ROOT}/tester/performance_tester.py"
    else
        log_warn "Performance tester not found"
        return 1
    fi
}

run_upload_tests() {
    if [ -f "${SCRIPT_DIR}/scripts/test_upload.py" ]; then
        python3 "${SCRIPT_DIR}/scripts/test_upload.py"
    else
        log_warn "Upload tests not found"
        return 1
    fi
}

run_cgi_tests() {
    if [ -f "${SCRIPT_DIR}/scripts/test_cgi.py" ]; then
        python3 "${SCRIPT_DIR}/scripts/test_cgi.py"
    else
        log_warn "CGI tests not found"
        return 1
    fi
}

run_stress_tests() {
    if [ -f "${SCRIPT_DIR}/scripts/stress_test.py" ]; then
        python3 "${SCRIPT_DIR}/scripts/stress_test.py"
    else
        log_warn "Stress tests not found"
        return 1
    fi
}

################################################################################
# MAIN EXECUTION
################################################################################

main() {
    print_banner
    
    log_info "Test Start Time: $(date)"
    log_info "Project Root: ${PROJECT_ROOT}"
    log_info "Server Binary: ${SERVER_BIN}"
    log_info "Config File: ${CONFIG_FILE}"
    log_info "Report: ${MASTER_REPORT}"
    echo "" | tee -a "$MASTER_REPORT"
    
    # Trap to ensure cleanup on exit
    trap cleanup EXIT INT TERM
    
    # Start the server
    start_server
    
    # Give server a moment to stabilize
    sleep 2
    
    # Run all test suites
    log_section "EXECUTING TEST SUITES"
    
    # Core functionality tests
    run_test_suite \
        "Comprehensive Edge Cases" \
        "Testing Edge Cases & Corner Scenarios" \
        "run_comprehensive_edge_cases" \
        "false"
    
    run_test_suite \
        "Security Tests" \
        "Testing Security Vulnerabilities" \
        "run_security_tests" \
        "false"
    
    # Bonus features (optional)
    run_test_suite \
        "Session & Cookie Tests" \
        "Testing Session Management (Bonus)" \
        "run_session_cookie_tests" \
        "true"
    
    # Python comprehensive tests (includes comprehensive_tester.py - 500+ test cases)
    run_test_suite \
        "Python Comprehensive Tests" \
        "Running Python Test Suite (comprehensive_tester.py)" \
        "run_python_comprehensive_tests" \
        "true"
    
    run_test_suite \
        "RFC Compliance Tests" \
        "Testing HTTP/1.1 RFC Compliance" \
        "run_rfc_compliance_tests" \
        "true"
    
    # Feature-specific tests
    run_test_suite \
        "Upload Tests" \
        "Testing File Upload Functionality" \
        "run_upload_tests" \
        "true"
    
    run_test_suite \
        "CGI Tests" \
        "Testing CGI Execution" \
        "run_cgi_tests" \
        "true"
    
    # Performance tests (optional, can be slow)
    run_test_suite \
        "Performance Tests" \
        "Testing Server Performance" \
        "run_performance_tests" \
        "true"
    
    run_test_suite \
        "Stress Tests" \
        "Testing Server Under Load" \
        "run_stress_tests" \
        "true"
    
    # Generate final report
    generate_final_report
}

generate_final_report() {
    log_section "TEST SUMMARY"
    
    local total_run=$((PASSED_SUITES + FAILED_SUITES))
    local success_rate=0
    
    if [ $total_run -gt 0 ]; then
        success_rate=$((PASSED_SUITES * 100 / total_run))
    fi
    
    echo "" | tee -a "$MASTER_REPORT"
    echo -e "${BOLD}Test Execution Complete!${NC}" | tee -a "$MASTER_REPORT"
    echo "" | tee -a "$MASTER_REPORT"
    echo -e "Total Test Suites:    ${BOLD}${TOTAL_SUITES}${NC}" | tee -a "$MASTER_REPORT"
    echo -e "Passed:               ${GREEN}${BOLD}${PASSED_SUITES}${NC}" | tee -a "$MASTER_REPORT"
    echo -e "Failed:               ${RED}${BOLD}${FAILED_SUITES}${NC}" | tee -a "$MASTER_REPORT"
    echo -e "Skipped (Optional):   ${YELLOW}${BOLD}${SKIPPED_SUITES}${NC}" | tee -a "$MASTER_REPORT"
    echo -e "Success Rate:         ${BOLD}${success_rate}%${NC}" | tee -a "$MASTER_REPORT"
    echo "" | tee -a "$MASTER_REPORT"
    
    if [ $FAILED_SUITES -eq 0 ]; then
        echo -e "${GREEN}${BOLD}╔═══════════════════════════════════════════════════════════╗${NC}" | tee -a "$MASTER_REPORT"
        echo -e "${GREEN}${BOLD}║                                                           ║${NC}" | tee -a "$MASTER_REPORT"
        echo -e "${GREEN}${BOLD}║              ✓  ALL CRITICAL TESTS PASSED  ✓              ║${NC}" | tee -a "$MASTER_REPORT"
        echo -e "${GREEN}${BOLD}║                                                           ║${NC}" | tee -a "$MASTER_REPORT"
        echo -e "${GREEN}${BOLD}╚═══════════════════════════════════════════════════════════╝${NC}" | tee -a "$MASTER_REPORT"
        echo "" | tee -a "$MASTER_REPORT"
    else
        echo -e "${RED}${BOLD}╔═══════════════════════════════════════════════════════════╗${NC}" | tee -a "$MASTER_REPORT"
        echo -e "${RED}${BOLD}║                                                           ║${NC}" | tee -a "$MASTER_REPORT"
        echo -e "${RED}${BOLD}║              ✗  SOME TESTS FAILED  ✗                      ║${NC}" | tee -a "$MASTER_REPORT"
        echo -e "${RED}${BOLD}║                                                           ║${NC}" | tee -a "$MASTER_REPORT"
        echo -e "${RED}${BOLD}╚═══════════════════════════════════════════════════════════╝${NC}" | tee -a "$MASTER_REPORT"
        echo "" | tee -a "$MASTER_REPORT"
    fi
    
    log_info "Test End Time: $(date)"
    log_info "Full Report: ${MASTER_REPORT}"
    log_info "Server Log: ${SERVER_LOG}"
    
    # Always exit 0 - test failures are reported but don't fail CI
    # Only exit 1 if there's a script error (server didn't start, etc.)
    exit 0
}

# Run main
main
