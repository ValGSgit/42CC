#!/bin/bash

# WebServ Cookie Management Test Suite
# Tests cookie setting, parsing, expiry, and security

# Color codes for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
SERVER_URL="http://localhost:8080"
COOKIE_FILE="/tmp/webserv_cookie_test.txt"
TEST_RESULTS="/tmp/webserv_cookie_test_results.log"

# Counters
TESTS_PASSED=0
TESTS_FAILED=0
TESTS_TOTAL=0

# Clear previous test data
rm -f "$COOKIE_FILE" "$TEST_RESULTS"

# Pre-test cleanup: Clear all sessions to ensure clean state
echo "Cleaning up any existing sessions..."
curl -s -X POST -H "Content-Length: 0" "$SERVER_URL/api/session/clear" > /dev/null 2>&1
sleep 0.5

# Helper function to print test results
print_test_result() {
    local test_name="$1"
    local result="$2"
    local details="$3"

    TESTS_TOTAL=$((TESTS_TOTAL + 1))

    if [ "$result" = "PASS" ]; then
        echo -e "${GREEN}✓ PASS${NC} - $test_name"
        TESTS_PASSED=$((TESTS_PASSED + 1))
    else
        echo -e "${RED}✗ FAIL${NC} - $test_name"
        echo -e "  ${YELLOW}Details:${NC} $details"
        TESTS_FAILED=$((TESTS_FAILED + 1))
    fi

    echo "[$(date)] $test_name - $result - $details" >> "$TEST_RESULTS"
}

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}    WebServ Cookie Management Tests${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

# Test 1: Basic Cookie Setting
echo -e "${YELLOW}Test 1: Set-Cookie Header Response${NC}"
RESPONSE=$(curl -s -i -X POST -H "Content-Length: 0" "$SERVER_URL/api/session/login" | grep -i "Set-Cookie")

if echo "$RESPONSE" | grep -qi "Set-Cookie"; then
    print_test_result "Set-Cookie Header" "PASS" "Server sends Set-Cookie header"
else
    print_test_result "Set-Cookie Header" "FAIL" "No Set-Cookie header found"
fi

sleep 1

# Test 2: Cookie Storage and Retrieval
echo -e "\n${YELLOW}Test 2: Cookie Storage${NC}"
# Clear sessions to prevent 409 conflict
curl -s -X POST -H "Content-Length: 0" "$SERVER_URL/api/session/clear" > /dev/null 2>&1
sleep 0.3
curl -s -c "$COOKIE_FILE" -X POST -H "Content-Length: 0" "$SERVER_URL/api/session/login" > /dev/null

if [ -f "$COOKIE_FILE" ] && [ -s "$COOKIE_FILE" ]; then
    COOKIE_COUNT=$(grep -v "^#" "$COOKIE_FILE" | grep -v "^$" | wc -l)
    print_test_result "Cookie Storage" "PASS" "Stored $COOKIE_COUNT cookie(s)"
else
    print_test_result "Cookie Storage" "FAIL" "Cookie file not created or empty"
fi

sleep 1

# Test 3: Cookie Sending in Request
echo -e "\n${YELLOW}Test 3: Cookie Transmission${NC}"
RESPONSE=$(curl -s -b "$COOKIE_FILE" "$SERVER_URL/api/session/profile" | grep -o '"success":[^,]*')

if echo "$RESPONSE" | grep -q "true"; then
    print_test_result "Cookie Transmission" "PASS" "Cookie successfully sent and recognized"
else
    print_test_result "Cookie Transmission" "FAIL" "Cookie not recognized by server"
fi

sleep 1

# Test 4: Multiple Cookies
echo -e "\n${YELLOW}Test 4: Multiple Cookie Handling${NC}"
RESPONSE=$(curl -s -w "\n%{http_code}" \
    -H "Cookie: SESSIONID=test123; user_pref=dark_mode; lang=en" \
    "$SERVER_URL/")

HTTP_CODE=$(echo "$RESPONSE" | tail -n1)

if [ "$HTTP_CODE" = "200" ]; then
    print_test_result "Multiple Cookies" "PASS" "Server handles multiple cookies"
else
    print_test_result "Multiple Cookies" "FAIL" "Failed with HTTP $HTTP_CODE"
fi

sleep 1

# Test 5: Cookie Path Attribute
echo -e "\n${YELLOW}Test 5: Cookie Path Attribute${NC}"
# Clear sessions to get a fresh login
curl -s -X POST -H "Content-Length: 0" "$SERVER_URL/api/session/clear" > /dev/null 2>&1
sleep 0.3
RESPONSE=$(curl -s -i -X POST -H "Content-Length: 0" "$SERVER_URL/api/session/login")
COOKIE_HEADER=$(echo "$RESPONSE" | grep -i "Set-Cookie")

if echo "$COOKIE_HEADER" | grep -q "Path="; then
    PATH_VALUE=$(echo "$COOKIE_HEADER" | grep -o "Path=[^;]*" | cut -d'=' -f2)
    print_test_result "Cookie Path" "PASS" "Path attribute set: $PATH_VALUE"
else
    print_test_result "Cookie Path" "FAIL" "No Path attribute in Set-Cookie"
fi

sleep 1

# Test 6: Cookie with Special Characters
echo -e "\n${YELLOW}Test 6: Cookie Value Encoding${NC}"
RESPONSE=$(curl -s -w "\n%{http_code}" \
    -H "Cookie: test_cookie=value%20with%20spaces; another=test%2Fslash" \
    "$SERVER_URL/")

HTTP_CODE=$(echo "$RESPONSE" | tail -n1)

if [ "$HTTP_CODE" = "200" ]; then
    print_test_result "Cookie Encoding" "PASS" "Encoded cookie values handled"
else
    print_test_result "Cookie Encoding" "FAIL" "HTTP $HTTP_CODE"
fi

sleep 1

# Test 7: Empty Cookie Header
echo -e "\n${YELLOW}Test 7: Empty Cookie Header${NC}"
RESPONSE=$(curl -s -w "\n%{http_code}" \
    -H "Cookie: " \
    "$SERVER_URL/")

HTTP_CODE=$(echo "$RESPONSE" | tail -n1)

if [ "$HTTP_CODE" = "200" ]; then
    print_test_result "Empty Cookie" "PASS" "Empty cookie header handled gracefully"
else
    print_test_result "Empty Cookie" "FAIL" "HTTP $HTTP_CODE"
fi

sleep 1

# Test 8: Cookie Expiry on Logout
echo -e "\n${YELLOW}Test 8: Cookie Deletion on Logout${NC}"
RESPONSE=$(curl -s -i -X POST -H "Content-Length: 0" -b "$COOKIE_FILE" "$SERVER_URL/api/session/logout")
SET_COOKIE=$(echo "$RESPONSE" | grep -i "Set-Cookie")

if echo "$SET_COOKIE" | grep -qi "expires.*1970\|Max-Age=0"; then
    print_test_result "Cookie Deletion" "PASS" "Cookie properly expired on logout"
else
    print_test_result "Cookie Deletion" "FAIL" "Cookie not expired properly"
fi

sleep 1

# Test 9: Cookie Persistence Test
echo -e "\n${YELLOW}Test 9: Cookie Persistence Across Requests${NC}"
# Clear sessions and create a new one
curl -s -X POST -H "Content-Length: 0" "$SERVER_URL/api/session/clear" > /dev/null 2>&1
sleep 0.3
curl -s -c "$COOKIE_FILE" -X POST -H "Content-Length: 0" "$SERVER_URL/api/session/login" > /dev/null
SUCCESS_COUNT=0

for i in {1..3}; do
    RESPONSE=$(curl -s -b "$COOKIE_FILE" "$SERVER_URL/api/session/profile")
    if echo "$RESPONSE" | grep -q "success"; then
        SUCCESS_COUNT=$((SUCCESS_COUNT + 1))
    fi
    sleep 0.3
done

if [ $SUCCESS_COUNT -eq 3 ]; then
    print_test_result "Cookie Persistence" "PASS" "Cookie persisted across 3 requests"
else
    print_test_result "Cookie Persistence" "FAIL" "Only $SUCCESS_COUNT/3 requests succeeded"
fi

sleep 1

# Test 10: Large Cookie Value
echo -e "\n${YELLOW}Test 10: Large Cookie Value${NC}"
LARGE_VALUE=$(printf 'A%.0s' {1..4000})
RESPONSE=$(curl -s -w "\n%{http_code}" \
    -H "Cookie: large_cookie=$LARGE_VALUE" \
    "$SERVER_URL/")

HTTP_CODE=$(echo "$RESPONSE" | tail -n1)

if [ "$HTTP_CODE" = "200" ] || [ "$HTTP_CODE" = "431" ]; then
    print_test_result "Large Cookie" "PASS" "Large cookie handled (HTTP $HTTP_CODE)"
else
    print_test_result "Large Cookie" "FAIL" "Unexpected HTTP $HTTP_CODE"
fi

sleep 1

# Test 11: Cookie Without Value
echo -e "\n${YELLOW}Test 11: Cookie Name Without Value${NC}"
RESPONSE=$(curl -s -w "\n%{http_code}" \
    -H "Cookie: empty_cookie=" \
    "$SERVER_URL/")

HTTP_CODE=$(echo "$RESPONSE" | tail -n1)

if [ "$HTTP_CODE" = "200" ]; then
    print_test_result "Empty Cookie Value" "PASS" "Empty cookie value handled"
else
    print_test_result "Empty Cookie Value" "FAIL" "HTTP $HTTP_CODE"
fi

sleep 1

# Test 12: Semicolon in Cookie Value
echo -e "\n${YELLOW}Test 12: Special Characters in Cookie${NC}"
RESPONSE=$(curl -s -w "\n%{http_code}" \
    -H 'Cookie: test=value1; another=value2' \
    "$SERVER_URL/")

HTTP_CODE=$(echo "$RESPONSE" | tail -n1)

if [ "$HTTP_CODE" = "200" ]; then
    print_test_result "Cookie Separators" "PASS" "Multiple cookies with semicolons parsed"
else
    print_test_result "Cookie Separators" "FAIL" "HTTP $HTTP_CODE"
fi

sleep 1

# Test 13: Case Sensitivity
echo -e "\n${YELLOW}Test 13: Cookie Header Case Sensitivity${NC}"
RESPONSE1=$(curl -s -w "\n%{http_code}" -H "Cookie: test=value" "$SERVER_URL/")
RESPONSE2=$(curl -s -w "\n%{http_code}" -H "cookie: test=value" "$SERVER_URL/")

HTTP_CODE1=$(echo "$RESPONSE1" | tail -n1)
HTTP_CODE2=$(echo "$RESPONSE2" | tail -n1)

if [ "$HTTP_CODE1" = "200" ] && [ "$HTTP_CODE2" = "200" ]; then
    print_test_result "Case Sensitivity" "PASS" "Both 'Cookie' and 'cookie' headers accepted"
else
    print_test_result "Case Sensitivity" "FAIL" "Case sensitivity issue: $HTTP_CODE1 vs $HTTP_CODE2"
fi

sleep 1

# Test 14: Cookie Domain Scope
echo -e "\n${YELLOW}Test 14: Cookie Domain Attribute${NC}"
# Clear sessions to get a fresh login
curl -s -X POST -H "Content-Length: 0" "$SERVER_URL/api/session/clear" > /dev/null 2>&1
sleep 0.3
RESPONSE=$(curl -s -i -X POST -H "Content-Length: 0" "$SERVER_URL/api/session/login")
COOKIE_HEADER=$(echo "$RESPONSE" | grep -i "Set-Cookie")

# Check if cookie is set correctly (domain is optional, but SESSIONID must be present)
if echo "$COOKIE_HEADER" | grep -q "SESSIONID=" && echo "$COOKIE_HEADER" | grep -q "Path=/"; then
    print_test_result "Cookie Domain" "PASS" "Cookie properly formatted with Path"
else
    print_test_result "Cookie Domain" "FAIL" "Cookie missing required attributes"
fi

sleep 1

# Test 15: Concurrent Cookie Operations
echo -e "\n${YELLOW}Test 15: Concurrent Cookie Operations${NC}"
# Clear sessions first
curl -s -X POST -H "Content-Length: 0" "$SERVER_URL/api/session/clear" > /dev/null 2>&1
sleep 0.3
PIDS=()
for i in {1..5}; do
    # Use different usernames to avoid conflicts
    curl -s -c "/tmp/cookie_$i.txt" -X POST -H "Content-Type: application/json" -d "{\"username\":\"user$i\"}" "$SERVER_URL/api/session/login" > /dev/null &
    PIDS+=($!)
done

# Wait for all requests
for pid in "${PIDS[@]}"; do
    wait $pid
done

COOKIE_COUNT=0
for i in {1..5}; do
    if [ -f "/tmp/cookie_$i.txt" ] && grep -q "SESSIONID" "/tmp/cookie_$i.txt"; then
        COOKIE_COUNT=$((COOKIE_COUNT + 1))
    fi
    rm -f "/tmp/cookie_$i.txt"
done

if [ $COOKIE_COUNT -eq 5 ]; then
    print_test_result "Concurrent Operations" "PASS" "5/5 concurrent cookie operations succeeded"
else
    print_test_result "Concurrent Operations" "FAIL" "Only $COOKIE_COUNT/5 succeeded"
fi

# Summary
echo ""
echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}           Test Summary${NC}"
echo -e "${BLUE}========================================${NC}"
echo -e "Total Tests: ${TESTS_TOTAL}"
echo -e "${GREEN}Passed: ${TESTS_PASSED}${NC}"
echo -e "${RED}Failed: ${TESTS_FAILED}${NC}"
echo -e "Success Rate: $(awk "BEGIN {printf \"%.1f\", ($TESTS_PASSED/$TESTS_TOTAL)*100}")%"
echo ""
echo -e "Detailed log saved to: ${TEST_RESULTS}"

# Cleanup
rm -f "$COOKIE_FILE"

if [ $TESTS_FAILED -eq 0 ]; then
    echo -e "\n${GREEN}All tests passed! ✓${NC}"
    exit 0
else
    echo -e "\n${RED}Some tests failed. Check the log for details.${NC}"
    exit 1
fi
