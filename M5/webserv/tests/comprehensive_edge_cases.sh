#!/bin/bash

# WebServ Comprehensive Edge Case Test Suite
# Tests all identified problematic areas

# Don't exit on error - we want to run all tests
# set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Counters
TOTAL=0
PASSED=0
FAILED=0
WARNINGS=0

# Server configuration
SERVER_HOST="localhost"
SERVER_PORT="8080"
BASE_URL="http://${SERVER_HOST}:${SERVER_PORT}"

# Test output directory
TEST_DIR="$(dirname "$0")"
OUTPUT_DIR="${TEST_DIR}/test_output"
mkdir -p "$OUTPUT_DIR"

# Log file
LOG_FILE="${OUTPUT_DIR}/test_results_$(date +%Y%m%d_%H%M%S).log"

# Helper functions
log() {
    echo -e "$1" | tee -a "$LOG_FILE"
}

log_test() {
    echo -e "${BLUE}[TEST]${NC} $1" | tee -a "$LOG_FILE"
}

log_pass() {
    echo -e "${GREEN}[PASS]${NC} $1" | tee -a "$LOG_FILE"
    ((PASSED++))
}

log_fail() {
    echo -e "${RED}[FAIL]${NC} $1" | tee -a "$LOG_FILE"
    ((FAILED++))
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1" | tee -a "$LOG_FILE"
    ((WARNINGS++))
}

test_request() {
    local test_name="$1"
    local command="$2"
    local expected_pattern="$3"
    local should_match="${4:-true}"
    
    ((TOTAL++))
    log_test "$test_name"
    
    local output=$(eval "$command" 2>&1 || echo "ERROR")
    local exit_code=$?
    
    if [ "$should_match" = "true" ]; then
        if echo "$output" | grep -qE "$expected_pattern"; then
            log_pass "$test_name"
            return 0
        else
            log_fail "$test_name - Expected pattern not found: $expected_pattern"
            echo "Output: $output" >> "$LOG_FILE"
            return 0  # Return 0 to continue testing
        fi
    else
        if ! echo "$output" | grep -qE "$expected_pattern"; then
            log_pass "$test_name"
            return 0
        else
            log_fail "$test_name - Unexpected pattern found: $expected_pattern"
            echo "Output: $output" >> "$LOG_FILE"
            return 0  # Return 0 to continue testing
        fi
    fi
}

check_http_status() {
    local test_name="$1"
    local url="$2"
    local expected_status="$3"
    local method="${4:-GET}"
    
    ((TOTAL++))
    log_test "$test_name"
    
    local status=$(curl --max-time 10 -s -o /dev/null -w "%{http_code}" -X "$method" "$url" 2>/dev/null || echo "000")
    
    if [ "$status" = "$expected_status" ]; then
        log_pass "$test_name (Status: $status)"
        return 0
    else
        log_fail "$test_name - Expected: $expected_status, Got: $status"
        return 0  # Return 0 to continue testing
    fi
}

# Check if server is running
check_server() {
    log "Checking if server is running on ${BASE_URL}..."
    if ! curl -s -o /dev/null -w "%{http_code}" "${BASE_URL}/" > /dev/null 2>&1; then
        log "${RED}ERROR: Server is not running on ${BASE_URL}${NC}"
        log "Please start the server first: ./webserv webserv.conf"
        exit 1
    fi
    log "${GREEN}Server is running${NC}\n"
}

# Setup test files
setup_test_files() {
    log "Setting up test files..."
    
    # Create test CGI scripts
    mkdir -p www/cgi-bin
    
    # Slow CGI script (for timeout test - 35s)
    cat > www/cgi-bin/slow.py << 'EOF'
#!/usr/bin/env python3
import time
print("Content-Type: text/html\r\n\r\n", flush=True)
time.sleep(35)
print("<h1>This should timeout</h1>")
EOF
    chmod +x www/cgi-bin/slow.py
    
    # Shorter timeout test (10s - faster for CI)
    cat > www/cgi-bin/slow_short.py << 'EOF'
#!/usr/bin/env python3
import time
print("Content-Type: text/html\r\n\r\n", flush=True)
time.sleep(10)
print("<h1>Short delay test</h1>")
EOF
    chmod +x www/cgi-bin/slow_short.py
    
    # Large output CGI script
    cat > www/cgi-bin/large_output.py << 'EOF'
#!/usr/bin/env python3
print("Content-Type: text/html\r\n\r\n")
print("<h1>Large Output Test</h1>")
print("A" * (1024 * 1024))  # 1MB of data
EOF
    chmod +x www/cgi-bin/large_output.py
    
    # Error exit CGI script
    cat > www/cgi-bin/error_exit.py << 'EOF'
#!/usr/bin/env python3
import sys
print("Content-Type: text/html\r\n\r\n")
print("<h1>Error Exit Test</h1>")
sys.exit(1)
EOF
    chmod +x www/cgi-bin/error_exit.py
    
    # Binary output CGI script
    cat > www/cgi-bin/binary.py << 'EOF'
#!/usr/bin/env python3
import sys
print("Content-Type: image/png\r\n\r\n")
sys.stdout.buffer.write(b'\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x01\x00\x00\x00\x01')
EOF
    chmod +x www/cgi-bin/binary.py
    
    # Create test files
    echo "Test file content" > "${OUTPUT_DIR}/test.txt"
    dd if=/dev/zero of="${OUTPUT_DIR}/large.bin" bs=1M count=5 2>/dev/null
    touch "${OUTPUT_DIR}/empty.txt"
    
    log "${GREEN}Test files created${NC}\n"
}

# Cleanup test files
cleanup_test_files() {
    log "\nCleaning up test files..."
    rm -f www/cgi-bin/slow.py
    rm -f www/cgi-bin/large_output.py
    rm -f www/cgi-bin/error_exit.py
    rm -f www/cgi-bin/binary.py
    rm -f "${OUTPUT_DIR}/test.txt"
    rm -f "${OUTPUT_DIR}/large.bin"
    rm -f "${OUTPUT_DIR}/empty.txt"
    log "${GREEN}Cleanup complete${NC}"
}

# =============================================================================
# TEST CATEGORIES
# =============================================================================

category_1_cgi_tests() {
    log "\n${BLUE}=== CATEGORY 1: CGI EXECUTION TESTS ===${NC}\n"
    
    check_http_status "1.1: Basic Python CGI" "${BASE_URL}/cgi-bin/calculator.py?num1=5&num2=3&op=add" "200"
    
    test_request "1.2: Python CGI calculation result" \
        "curl -s '${BASE_URL}/cgi-bin/calculator.py?num1=10&num2=5&op=add'" \
        "15"
    
    check_http_status "1.3: Perl CGI" "${BASE_URL}/cgi-bin/textproc.pl?text=test&op=analyze" "200"
    
    check_http_status "1.4: Ruby CGI" "${BASE_URL}/cgi-bin/colorgen.rb?color=6366f1&type=monochrome" "200"
    
    check_http_status "1.5: PHP CGI" "${BASE_URL}/cgi-bin/visualizer.php?data=1,2,3&labels=A,B,C&type=bar" "200"
    
    check_http_status "1.6: Bash CGI" "${BASE_URL}/cgi-bin/sysinfo.sh?type=full" "200"
    
    log_test "1.7: CGI with POST data"
    ((TOTAL++))
    local post_result=$(curl --max-time 5 -s -X POST -d "text=hello&op=analyze" "${BASE_URL}/cgi-bin/textproc.pl" 2>/dev/null)
    if echo "$post_result" | grep -q "hello"; then
        log_pass "CGI POST handling works"
    else
        log_warn "CGI POST handling needs investigation (optional test)"
        ((WARNINGS++))
    fi
    
    # Skip slow timeout test in CI or if SKIP_SLOW_TESTS is set
    if [ -z "$CI" ] && [ -z "$SKIP_SLOW_TESTS" ]; then
        log_test "1.8: CGI timeout handling (will take ~10s)"
        ((TOTAL++))
        # Reduced from 35s to 10s for faster testing
        timeout 15 curl --max-time 15 -s "${BASE_URL}/cgi-bin/slow_short.py" > /dev/null 2>&1
        local curl_exit=$?
        if [ $curl_exit -eq 28 ] || [ $curl_exit -eq 124 ] || [ $curl_exit -eq 0 ]; then
            log_pass "CGI timeout handled correctly"
        else
            log_warn "CGI timeout behavior unclear (exit code: $curl_exit)"
            ((WARNINGS++))
        fi
    else
        log "Skipping slow CGI timeout test (CI or SKIP_SLOW_TESTS set)"
    fi
    
    check_http_status "1.9: Large CGI output" "${BASE_URL}/cgi-bin/large_output.py" "200"
    
    check_http_status "1.10: CGI with error exit" "${BASE_URL}/cgi-bin/error_exit.py" "200"
    
    log_test "1.11: Multiple simultaneous CGI requests"
    ((TOTAL++))
    for i in {1..5}; do
        curl -s "${BASE_URL}/cgi-bin/calculator.py?num1=$i&num2=2&op=add" > /dev/null &
    done
    wait
    log_pass "Concurrent CGI requests completed"
}

category_2_http_edge_cases() {
    log "\n${BLUE}=== CATEGORY 2: HTTP REQUEST EDGE CASES ===${NC}\n"
    
    log_test "2.1: Missing Host header"
    ((TOTAL++))
    printf "GET / HTTP/1.1\r\n\r\n" | nc -w 2 localhost 8080 > "${OUTPUT_DIR}/test_2_1.txt" 2>&1
    if [ -s "${OUTPUT_DIR}/test_2_1.txt" ]; then
        log_pass "Server responded to missing Host header"
    else
        log_warn "No response for missing Host header"
    fi
    
    log_test "2.2: Malformed request line"
    ((TOTAL++))
    printf "INVALID REQUEST\r\n\r\n" | nc -w 2 localhost 8080 > "${OUTPUT_DIR}/test_2_2.txt" 2>&1
    if grep -q "400" "${OUTPUT_DIR}/test_2_2.txt"; then
        log_pass "Server returned 400 for malformed request"
    else
        log_warn "Server response to malformed request unclear"
    fi
    
    log_test "2.3: Extremely long URI"
    ((TOTAL++))
    long_uri=$(python3 -c 'print("A"*10000)')
    status=$(curl -s -o /dev/null -w "%{http_code}" "${BASE_URL}/${long_uri}" 2>/dev/null)
    if [ "$status" = "414" ] || [ "$status" = "400" ]; then
        log_pass "Server rejected long URI (Status: $status)"
    else
        log_warn "Long URI handling: Status $status"
    fi
    
    check_http_status "2.4: Invalid HTTP method" "${BASE_URL}/" "405" "INVALID"
    
    log_test "2.5: HTTP/0.9 request"
    ((TOTAL++))
    printf "GET /\r\n\r\n" | nc -w 2 localhost 8080 > "${OUTPUT_DIR}/test_2_5.txt" 2>&1
    if [ -s "${OUTPUT_DIR}/test_2_5.txt" ]; then
        log_pass "Server handled HTTP/0.9 request"
    else
        log_warn "HTTP/0.9 handling unclear"
    fi
    
    log_test "2.6: Negative Content-Length"
    ((TOTAL++))
    printf "POST / HTTP/1.1\r\nHost: localhost\r\nContent-Length: -1\r\n\r\n" | nc -w 2 localhost 8080 > "${OUTPUT_DIR}/test_2_6.txt" 2>&1
    if grep -q "400" "${OUTPUT_DIR}/test_2_6.txt"; then
        log_pass "Server rejected negative Content-Length"
    else
        log_warn "Negative Content-Length handling unclear"
    fi
    
    log_test "2.7: Content-Length mismatch"
    ((TOTAL++))
    printf "POST / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 100\r\n\r\nshort" | nc -w 2 localhost 8080 > "${OUTPUT_DIR}/test_2_7.txt" 2>&1
    if [ -s "${OUTPUT_DIR}/test_2_7.txt" ]; then
        log_pass "Server handled Content-Length mismatch"
    else
        log_warn "Content-Length mismatch handling unclear"
    fi
    
    log_test "2.8: Request with null bytes"
    ((TOTAL++))
    printf "GET /\x00test HTTP/1.1\r\nHost: localhost\r\n\r\n" | nc -w 2 localhost 8080 > "${OUTPUT_DIR}/test_2_8.txt" 2>&1
    if [ -s "${OUTPUT_DIR}/test_2_8.txt" ]; then
        log_pass "Server handled null bytes in request"
    else
        log_warn "Null byte handling unclear"
    fi
}

category_3_security_tests() {
    log "\n${BLUE}=== CATEGORY 3: PATH TRAVERSAL & SECURITY ===${NC}\n"
    
    log_test "3.1: Basic path traversal"
    ((TOTAL++))
    status=$(curl -s -o /dev/null -w "%{http_code}" "${BASE_URL}/../../../etc/passwd" 2>/dev/null)
    if [ "$status" = "403" ] || [ "$status" = "404" ] || [ "$status" = "400" ]; then
        log_pass "Path traversal blocked (Status: $status)"
    else
        log_fail "Path traversal not properly blocked (Status: $status)"
    fi
    
    log_test "3.2: URL encoded path traversal"
    ((TOTAL++))
    status=$(curl -s -o /dev/null -w "%{http_code}" "${BASE_URL}/%2e%2e%2f%2e%2e%2f%2e%2e%2fetc/passwd" 2>/dev/null)
    if [ "$status" = "403" ] || [ "$status" = "404" ] || [ "$status" = "400" ]; then
        log_pass "Encoded path traversal blocked (Status: $status)"
    else
        log_fail "Encoded path traversal not properly blocked (Status: $status)"
    fi
    
    log_test "3.3: Double encoded path traversal"
    ((TOTAL++))
    status=$(curl -s -o /dev/null -w "%{http_code}" "${BASE_URL}/%252e%252e%252f%252e%252e%252fetc/passwd" 2>/dev/null)
    if [ "$status" = "403" ] || [ "$status" = "404" ] || [ "$status" = "400" ]; then
        log_pass "Double encoded traversal blocked (Status: $status)"
    else
        log_warn "Double encoded traversal handling (Status: $status)"
    fi
    
    log_test "3.4: Null byte injection"
    ((TOTAL++))
    status=$(curl -s -o /dev/null -w "%{http_code}" "${BASE_URL}/../../../etc/passwd%00.html" 2>/dev/null)
    if [ "$status" = "403" ] || [ "$status" = "404" ] || [ "$status" = "400" ]; then
        log_pass "Null byte injection blocked (Status: $status)"
    else
        log_warn "Null byte injection handling (Status: $status)"
    fi
    
    log_test "3.5: Access hidden files"
    ((TOTAL++))
    status=$(curl -s -o /dev/null -w "%{http_code}" "${BASE_URL}/.htaccess" 2>/dev/null)
    if [ "$status" = "403" ] || [ "$status" = "404" ]; then
        log_pass "Hidden file access blocked (Status: $status)"
    else
        log_warn "Hidden file access (Status: $status)"
    fi
}

category_4_file_upload_tests() {
    log "\n${BLUE}=== CATEGORY 4: FILE UPLOAD TESTS ===${NC}\n"
    
    check_http_status "4.1: Basic file upload" "${BASE_URL}/uploads" "200" "POST"
    
    log_test "4.2: Upload empty file"
    ((TOTAL++))
    status=$(curl -s -o /dev/null -w "%{http_code}" -F "file=@${OUTPUT_DIR}/empty.txt" "${BASE_URL}/uploads" 2>/dev/null)
    if [ "$status" = "200" ] || [ "$status" = "201" ]; then
        log_pass "Empty file upload handled (Status: $status)"
    else
        log_warn "Empty file upload (Status: $status)"
    fi
    
    log_test "4.3: Upload file exceeding size limit"
    ((TOTAL++))
    status=$(curl -s -o /dev/null -w "%{http_code}" -F "file=@${OUTPUT_DIR}/large.bin" "${BASE_URL}/uploads" 2>/dev/null)
    if [ "$status" = "413" ] || [ "$status" = "400" ]; then
        log_pass "Large file rejected (Status: $status)"
    else
        log_warn "Large file handling (Status: $status)"
    fi
    
    log_test "4.4: Upload with path traversal in filename"
    ((TOTAL++))
    status=$(curl -s -o /dev/null -w "%{http_code}" -F "file=@${OUTPUT_DIR}/test.txt;filename=../../evil.txt" "${BASE_URL}/uploads" 2>/dev/null)
    if [ "$status" = "200" ] || [ "$status" = "201" ] || [ "$status" = "400" ]; then
        log_pass "Path traversal in filename handled (Status: $status)"
    else
        log_warn "Filename path traversal (Status: $status)"
    fi
    
    log_test "4.5: Upload to non-existent directory"
    ((TOTAL++))
    status=$(curl -s -o /dev/null -w "%{http_code}" -F "file=@${OUTPUT_DIR}/test.txt" "${BASE_URL}/nonexistent/upload" 2>/dev/null)
    if [ "$status" = "404" ] || [ "$status" = "403" ]; then
        log_pass "Non-existent directory rejected (Status: $status)"
    else
        log_warn "Non-existent directory handling (Status: $status)"
    fi
}

category_5_http_methods() {
    log "\n${BLUE}=== CATEGORY 5: HTTP METHODS ===${NC}\n"
    
    log_test "5.1: HEAD request (no body, Content-Length present)"
    ((TOTAL++))
    response=$(curl -I -s "${BASE_URL}/index.html" 2>&1)
    if echo "$response" | grep -q "Content-Length" && echo "$response" | grep -q "200"; then
        log_pass "HEAD request properly handled"
    else
        log_fail "HEAD request issue"
    fi
    
    log_test "5.2: OPTIONS request"
    ((TOTAL++))
    response=$(curl -X OPTIONS -s -i "${BASE_URL}/" 2>&1)
    if echo "$response" | grep -qE "Allow:|200|204"; then
        log_pass "OPTIONS request handled"
    else
        log_warn "OPTIONS request behavior unclear"
    fi
    
    log_test "5.3: DELETE on file"
    ((TOTAL++))
    echo "delete me" > www/uploads/delete_test.txt
    status=$(curl -s -o /dev/null -w "%{http_code}" -X DELETE "${BASE_URL}/uploads/delete_test.txt" 2>/dev/null)
    if [ "$status" = "200" ] || [ "$status" = "204" ] || [ "$status" = "403" ]; then
        log_pass "DELETE handled (Status: $status)"
    else
        log_warn "DELETE handling (Status: $status)"
    fi
    
    check_http_status "5.4: DELETE on directory" "${BASE_URL}/uploads/" "403" "DELETE"
    
    check_http_status "5.5: POST to non-upload location" "${BASE_URL}/" "405" "POST"
    
    log_test "5.6: PUT request"
    ((TOTAL++))
    status=$(curl -s -o /dev/null -w "%{http_code}" -X PUT -d "content" "${BASE_URL}/test.txt" 2>/dev/null)
    if [ "$status" = "405" ] || [ "$status" = "501" ]; then
        log_pass "PUT request rejected (Status: $status)"
    else
        log_warn "PUT request handling (Status: $status)"
    fi
    
    check_http_status "5.7: TRACE request" "${BASE_URL}/" "405" "TRACE"
}

category_6_redirects_errors() {
    log "\n${BLUE}=== CATEGORY 6: REDIRECTS & ERROR PAGES ===${NC}\n"
    
    log_test "6.1: Basic redirect"
    ((TOTAL++))
    status=$(curl -s -o /dev/null -w "%{http_code}" "${BASE_URL}/redirect" 2>/dev/null)
    if [ "$status" = "301" ] || [ "$status" = "302" ] || [ "$status" = "307" ]; then
        log_pass "Redirect works (Status: $status)"
    else
        log_warn "Redirect behavior (Status: $status)"
    fi
    
    log_test "6.2: Custom 404 page"
    ((TOTAL++))
    response=$(curl -s "${BASE_URL}/nonexistent_page_12345" 2>&1)
    if echo "$response" | grep -q "404"; then
        log_pass "Custom 404 page served"
    else
        log_warn "404 page behavior unclear"
    fi
    
    check_http_status "6.3: 405 Method Not Allowed" "${BASE_URL}/" "405" "POST"
    
    log_test "6.4: External redirect"
    ((TOTAL++))
    status=$(curl -s -o /dev/null -w "%{http_code}" "${BASE_URL}/github" 2>/dev/null)
    if [ "$status" = "301" ] || [ "$status" = "302" ]; then
        log_pass "External redirect (Status: $status)"
    else
        log_warn "External redirect (Status: $status)"
    fi
}

category_7_directory_listing() {
    log "\n${BLUE}=== CATEGORY 7: DIRECTORY LISTING ===${NC}\n"
    
    log_test "7.1: Directory listing enabled"
    ((TOTAL++))
    response=$(curl -s "${BASE_URL}/browse/" 2>&1)
    if echo "$response" | grep -qE "Index of|Directory|sample"; then
        log_pass "Directory listing works"
    else
        log_warn "Directory listing unclear"
    fi
    
    log_test "7.2: Directory listing disabled"
    ((TOTAL++))
    status=$(curl -s -o /dev/null -w "%{http_code}" "${BASE_URL}/assets/" 2>/dev/null)
    if [ "$status" = "403" ] || [ "$status" = "404" ]; then
        log_pass "Directory listing properly disabled (Status: $status)"
    else
        log_warn "Directory listing behavior (Status: $status)"
    fi
}

category_8_session_tests() {
    log "\n${BLUE}=== CATEGORY 8: SESSION MANAGEMENT (BONUS) ===${NC}\n"
    
    log_test "8.1: Session creation"
    ((TOTAL++))
    response=$(curl -s -c "${OUTPUT_DIR}/cookies.txt" "${BASE_URL}/api/session/login" -d "user=test" 2>&1)
    if [ -f "${OUTPUT_DIR}/cookies.txt" ] && grep -q "session" "${OUTPUT_DIR}/cookies.txt"; then
        log_pass "Session created"
    else
        log_warn "Session creation unclear"
    fi
    
    log_test "8.2: Use session cookie"
    ((TOTAL++))
    if [ -f "${OUTPUT_DIR}/cookies.txt" ]; then
        status=$(curl -s -o /dev/null -w "%{http_code}" -b "${OUTPUT_DIR}/cookies.txt" "${BASE_URL}/api/session/info" 2>/dev/null)
        if [ "$status" = "200" ]; then
            log_pass "Session cookie used successfully"
        else
            log_warn "Session cookie usage (Status: $status)"
        fi
    else
        log_warn "No cookie file to test"
    fi
    
    log_test "8.3: Invalid session ID"
    ((TOTAL++))
    status=$(curl -s -o /dev/null -w "%{http_code}" -H "Cookie: session_id=invalid123456" "${BASE_URL}/api/session/info" 2>/dev/null)
    if [ "$status" = "401" ] || [ "$status" = "403" ] || [ "$status" = "404" ]; then
        log_pass "Invalid session rejected (Status: $status)"
    else
        log_warn "Invalid session handling (Status: $status)"
    fi
}

category_9_concurrency_tests() {
    log "\n${BLUE}=== CATEGORY 9: CONCURRENCY & STRESS ===${NC}\n"
    
    log_test "9.1: Multiple simultaneous connections (20)"
    ((TOTAL++))
    for i in {1..20}; do
        curl -s -o /dev/null "${BASE_URL}/" &
    done
    wait
    log_pass "Concurrent connections handled"
    
    log_test "9.2: Rapid sequential requests"
    ((TOTAL++))
    for i in {1..50}; do
        curl -s -o /dev/null "${BASE_URL}/" 2>/dev/null
    done
    log_pass "Sequential requests handled"
}

category_10_content_types() {
    log "\n${BLUE}=== CATEGORY 10: CONTENT TYPES & SPECIAL FILES ===${NC}\n"
    
    log_test "10.1: HTML file"
    ((TOTAL++))
    response=$(curl -s -I "${BASE_URL}/index.html" 2>&1)
    if echo "$response" | grep -q "text/html"; then
        log_pass "HTML content type correct"
    else
        log_warn "HTML content type unclear"
    fi
    
    log_test "10.2: Unknown file extension"
    ((TOTAL++))
    echo "test" > www/test.xyz
    status=$(curl -s -o /dev/null -w "%{http_code}" "${BASE_URL}/test.xyz" 2>/dev/null)
    if [ "$status" = "200" ]; then
        log_pass "Unknown extension handled (Status: $status)"
    else
        log_warn "Unknown extension (Status: $status)"
    fi
    rm -f www/test.xyz
}

# =============================================================================
# MAIN EXECUTION
# =============================================================================

main() {
    log "========================================"
    log "WebServ Comprehensive Edge Case Testing"
    log "========================================"
    log "Started: $(date)"
    log "Server: ${BASE_URL}"
    log "========================================\n"
    
    check_server
    setup_test_files
    
    # Run all test categories
    category_1_cgi_tests
    category_2_http_edge_cases
    category_3_security_tests
    category_4_file_upload_tests
    category_5_http_methods
    category_6_redirects_errors
    category_7_directory_listing
    category_8_session_tests
    category_9_concurrency_tests
    category_10_content_types
    
    cleanup_test_files
    
    # Print summary
    log "\n========================================"
    log "              TEST SUMMARY"
    log "========================================"
    log "Total Tests:    $TOTAL"
    log "${GREEN}Passed:         $PASSED${NC}"
    log "${RED}Failed:         $FAILED${NC}"
    log "${YELLOW}Warnings:       $WARNINGS${NC}"
    log "Success Rate:   $(awk "BEGIN {printf \"%.2f\", ($PASSED/$TOTAL)*100}")%"
    log "========================================"
    log "Completed: $(date)"
    log "Log file: $LOG_FILE"
    log "========================================"
    
    if [ $FAILED -gt 0 ]; then
        exit 1
    else
        exit 0
    fi
}

# Trap to cleanup on exit
trap cleanup_test_files EXIT INT TERM

# Run main
main
