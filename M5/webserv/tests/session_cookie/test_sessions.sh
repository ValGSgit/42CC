#!/bin/bash

# WebServ Session Management Test Suite
# Tests session creation, validation, expiry, and destruction

# Color codes for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
SERVER_URL="http://localhost:8080"
COOKIE_FILE="./webserv_session_cookies.txt"
TEST_RESULTS="./webserv_session_test_results.log"

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
echo -e "${BLUE}  WebServ Session Management Tests${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

# Test 1: Create Session (Login)
echo -e "${YELLOW}Test 1: Session Creation${NC}"
RESPONSE=$(curl -s -w "\n%{http_code}" -X POST \
    -H "Content-Length: 0" \
    -c "$COOKIE_FILE" \
    "$SERVER_URL/api/session/login")

HTTP_CODE=$(echo "$RESPONSE" | tail -n1)
BODY=$(echo "$RESPONSE" | head -n-1)

if [ "$HTTP_CODE" = "200" ] && echo "$BODY" | grep -q "success"; then
    SESSION_ID=$(echo "$BODY" | grep -o '"session_id":"[^"]*"' | cut -d'"' -f4)
    print_test_result "Session Creation" "PASS" "Session created with ID: $SESSION_ID"
else
    print_test_result "Session Creation" "FAIL" "HTTP $HTTP_CODE - $BODY"
fi

sleep 1

# Test 2: Verify Cookie is Set
echo -e "\n${YELLOW}Test 2: Cookie Storage${NC}"
if [ -f "$COOKIE_FILE" ] && grep -q "SESSIONID" "$COOKIE_FILE"; then
    COOKIE_VALUE=$(grep "SESSIONID" "$COOKIE_FILE" | awk '{print $7}')
    print_test_result "Cookie Storage" "PASS" "Cookie stored: $COOKIE_VALUE"
else
    print_test_result "Cookie Storage" "FAIL" "No session cookie found"
fi

sleep 1

# Test 3: Get Profile with Session
echo -e "\n${YELLOW}Test 3: Authenticated Request (Get Profile)${NC}"
RESPONSE=$(curl -s -w "\n%{http_code}" \
    -b "$COOKIE_FILE" \
    "$SERVER_URL/api/session/profile")

HTTP_CODE=$(echo "$RESPONSE" | tail -n1)
BODY=$(echo "$RESPONSE" | head -n-1)

if [ "$HTTP_CODE" = "200" ] && echo "$BODY" | grep -q "success"; then
    USERNAME=$(echo "$BODY" | grep -o '"username":"[^"]*"' | cut -d'"' -f4)
    print_test_result "Authenticated Request" "PASS" "Profile retrieved: $USERNAME"
else
    print_test_result "Authenticated Request" "FAIL" "HTTP $HTTP_CODE - $BODY"
fi

sleep 1

# Test 4: Session Info (Active Sessions Count)
echo -e "\n${YELLOW}Test 4: Session Statistics${NC}"
RESPONSE=$(curl -s -w "\n%{http_code}" \
    "$SERVER_URL/api/session/info")

HTTP_CODE=$(echo "$RESPONSE" | tail -n1)
BODY=$(echo "$RESPONSE" | head -n-1)

if [ "$HTTP_CODE" = "200" ] && echo "$BODY" | grep -q "active_sessions"; then
    ACTIVE_COUNT=$(echo "$BODY" | grep -o '"active_sessions":[0-9]*' | cut -d':' -f2)
    print_test_result "Session Statistics" "PASS" "Active sessions: $ACTIVE_COUNT"
else
    print_test_result "Session Statistics" "FAIL" "HTTP $HTTP_CODE - $BODY"
fi

sleep 1

# Test 5: Multiple Sessions
echo -e "\n${YELLOW}Test 5: Multiple Concurrent Sessions${NC}"
COOKIE_FILE2="/tmp/webserv_session_cookies2.txt"
RESPONSE=$(curl -s -w "\n%{http_code}" -X POST \
    -H "Content-Type: application/json" \
    -d '{"username":"test_user2"}' \
    -c "$COOKIE_FILE2" \
    "$SERVER_URL/api/session/login")

HTTP_CODE=$(echo "$RESPONSE" | tail -n1)
if [ "$HTTP_CODE" = "200" ]; then
    SESSION_ID2=$(echo "$RESPONSE" | head -n-1 | grep -o '"session_id":"[^"]*"' | cut -d'"' -f4)
    if [ "$SESSION_ID" != "$SESSION_ID2" ]; then
        print_test_result "Multiple Sessions" "PASS" "Created distinct sessions: $SESSION_ID vs $SESSION_ID2"
    else
        print_test_result "Multiple Sessions" "FAIL" "Session IDs are identical"
    fi
else
    print_test_result "Multiple Sessions" "FAIL" "Failed to create second session"
fi

rm -f "$COOKIE_FILE2"
sleep 1

# Test 6: Invalid Session ID
echo -e "\n${YELLOW}Test 6: Invalid Session Handling${NC}"
RESPONSE=$(curl -s -w "\n%{http_code}" \
    -H "Cookie: SESSIONID=invalid_session_12345" \
    "$SERVER_URL/api/session/profile")

HTTP_CODE=$(echo "$RESPONSE" | tail -n1)
BODY=$(echo "$RESPONSE" | head -n-1)

if echo "$BODY" | grep -q "error\|invalid\|not found" || [ "$HTTP_CODE" = "401" ] || [ "$HTTP_CODE" = "403" ]; then
    print_test_result "Invalid Session Handling" "PASS" "Invalid session rejected properly"
else
    print_test_result "Invalid Session Handling" "FAIL" "Invalid session not rejected: HTTP $HTTP_CODE"
fi

sleep 1

# Test 7: Request Without Session Cookie
echo -e "\n${YELLOW}Test 7: Unauthenticated Request${NC}"
RESPONSE=$(curl -s -w "\n%{http_code}" \
    "$SERVER_URL/api/session/profile")

HTTP_CODE=$(echo "$RESPONSE" | tail -n1)
BODY=$(echo "$RESPONSE" | head -n-1)

if echo "$BODY" | grep -q "error\|unauthorized\|not found" || [ "$HTTP_CODE" = "401" ] || [ "$HTTP_CODE" = "403" ]; then
    print_test_result "Unauthenticated Request" "PASS" "Request without cookie rejected"
else
    print_test_result "Unauthenticated Request" "FAIL" "Request without cookie not rejected properly"
fi

sleep 1

# Test 8: Session Persistence Across Requests
echo -e "\n${YELLOW}Test 8: Session Persistence${NC}"
SUCCESS_COUNT=0
for i in {1..5}; do
    RESPONSE=$(curl -s -w "\n%{http_code}" \
        -b "$COOKIE_FILE" \
        "$SERVER_URL/api/session/profile")

    HTTP_CODE=$(echo "$RESPONSE" | tail -n1)
    if [ "$HTTP_CODE" = "200" ]; then
        SUCCESS_COUNT=$((SUCCESS_COUNT + 1))
    fi
    sleep 0.5
done

if [ $SUCCESS_COUNT -eq 5 ]; then
    print_test_result "Session Persistence" "PASS" "Session persisted across 5 requests"
else
    print_test_result "Session Persistence" "FAIL" "Only $SUCCESS_COUNT/5 requests succeeded"
fi

sleep 1

# Test 9: Logout (Session Destruction)
echo -e "\n${YELLOW}Test 9: Session Logout${NC}"
RESPONSE=$(curl -s -w "\n%{http_code}" -X POST \
    -H "Content-Length: 0" \
    -b "$COOKIE_FILE" \
    "$SERVER_URL/api/session/logout")

HTTP_CODE=$(echo "$RESPONSE" | tail -n1)
BODY=$(echo "$RESPONSE" | head -n-1)

if [ "$HTTP_CODE" = "200" ] && echo "$BODY" | grep -q "success"; then
    print_test_result "Session Logout" "PASS" "Session destroyed successfully"
else
    print_test_result "Session Logout" "FAIL" "HTTP $HTTP_CODE - $BODY"
fi

sleep 1

# Test 10: Verify Session is Destroyed
echo -e "\n${YELLOW}Test 10: Post-Logout Validation${NC}"
RESPONSE=$(curl -s -w "\n%{http_code}" \
    -b "$COOKIE_FILE" \
    "$SERVER_URL/api/session/profile")

HTTP_CODE=$(echo "$RESPONSE" | tail -n1)
BODY=$(echo "$RESPONSE" | head -n-1)

if echo "$BODY" | grep -q "error\|invalid\|not found" || [ "$HTTP_CODE" = "401" ] || [ "$HTTP_CODE" = "403" ]; then
    print_test_result "Post-Logout Validation" "PASS" "Session properly destroyed"
else
    print_test_result "Post-Logout Validation" "FAIL" "Session still active after logout"
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
