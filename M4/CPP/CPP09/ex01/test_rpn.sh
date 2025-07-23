#!/bin/bash

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Counters
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

echo -e "${BLUE}=== RPN Calculator Test Suite ===${NC}"
echo

# Function to run a test
run_test() {
    local test_name="$1"
    local input="$2"
    local expected_output="$3"
    local expected_exit_code="$4"
    
    echo -n "Testing: $test_name... "
    
    # Run the program and capture output and exit code
    result=$(./RPN "$input" 2>&1)
    exit_code=$?
    
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    
    # Check if the result matches expected
    if [ "$exit_code" -eq "$expected_exit_code" ] && [ "$result" = "$expected_output" ]; then
        echo -e "${GREEN}PASS${NC}"
        PASSED_TESTS=$((PASSED_TESTS + 1))
    else
        echo -e "${RED}FAIL${NC}"
        echo -e "  Expected: '$expected_output' (exit code: $expected_exit_code)"
        echo -e "  Got:      '$result' (exit code: $exit_code)"
        FAILED_TESTS=$((FAILED_TESTS + 1))
    fi
}

# Build the program first
echo -e "${YELLOW}Building RPN calculator...${NC}"
make clean > /dev/null 2>&1
make > /dev/null 2>&1

if [ $? -ne 0 ]; then
    echo -e "${RED}Build failed! Exiting.${NC}"
    exit 1
fi

echo -e "${GREEN}Build successful!${NC}"
echo

# Integer Overflow and Underflow Tests (44 tests)
echo -e "${BLUE}=== Integer Overflow/Underflow Tests (44 tests) ===${NC}"

# Large multiplication results (11 tests)
run_test "OVF-001: 9*9*9*9" "9 9 * 9 * 9 *" "6561" 0
run_test "OVF-002: 8*8*8*8" "8 8 * 8 * 8 *" "4096" 0
run_test "OVF-003: 7*7*7*7" "7 7 * 7 * 7 *" "2401" 0
run_test "OVF-004: 6*6*6*6" "6 6 * 6 * 6 *" "1296" 0
run_test "OVF-005: 9*9*8*8" "9 9 * 8 8 * *" "5184" 0
run_test "OVF-006: 9*8*7*6" "9 8 * 7 * 6 *" "3024" 0
run_test "OVF-007: (9*9)*(8*8)" "9 9 * 8 8 * *" "5184" 0
run_test "OVF-008: (9*8)*(9*8)" "9 8 * 9 8 * *" "5184" 0
run_test "OVF-009: (7*8)*(7*8)" "7 8 * 7 8 * *" "3136" 0
run_test "OVF-010: 9*9*9+9*9*9" "9 9 9 * * 9 9 9 * * +" "1458" 0
run_test "OVF-011: 8*8*8+7*7*7" "8 8 8 * * 7 7 7 * * +" "855" 0

# Chain multiplication patterns (11 tests)
run_test "OVF-012: 2*3*4*5*6" "2 3 * 4 * 5 * 6 *" "720" 0
run_test "OVF-013: 1*2*3*4*5*6*7" "1 2 * 3 * 4 * 5 * 6 * 7 *" "5040" 0
run_test "OVF-014: 9*8*7*6*5" "9 8 * 7 * 6 * 5 *" "15120" 0
run_test "OVF-015: 4*5*6*7*8" "4 5 * 6 * 7 * 8 *" "6720" 0
run_test "OVF-016: 3*4*5*6*7*8" "3 4 * 5 * 6 * 7 * 8 *" "20160" 0
run_test "OVF-017: 2*4*6*8*9" "2 4 * 6 * 8 * 9 *" "3456" 0
run_test "OVF-018: 1*3*5*7*9" "1 3 * 5 * 7 * 9 *" "945" 0
run_test "OVF-019: 9*7*5*3*1" "9 7 * 5 * 3 * 1 *" "945" 0
run_test "OVF-020: 8*6*4*2*1" "8 6 * 4 * 2 * 1 *" "384" 0
run_test "OVF-021: 5*5*5*5*5" "5 5 * 5 * 5 * 5 *" "3125" 0
run_test "OVF-022: 3*3*3*3*3*3" "3 3 * 3 * 3 * 3 * 3 *" "729" 0

# Large addition chains (11 tests)
run_test "OVF-023: 9+9+9+9+9+9+9+9+9" "9 9 + 9 + 9 + 9 + 9 + 9 + 9 + 9 +" "81" 0
run_test "OVF-024: 8+8+8+8+8+8+8+8+8+8" "8 8 + 8 + 8 + 8 + 8 + 8 + 8 + 8 + 8 +" "80" 0
run_test "OVF-025: (9*9)+(9*9)+(9*9)" "9 9 * 9 9 * + 9 9 * +" "243" 0
run_test "OVF-026: (8*8)+(7*7)+(6*6)" "8 8 * 7 7 * + 6 6 * +" "149" 0
run_test "OVF-027: 9*9+8*8+7*7+6*6" "9 9 * 8 8 * + 7 7 * + 6 6 * +" "230" 0
run_test "OVF-028: (9+8+7)*(9+8+7)" "9 8 + 7 + 9 8 + 7 + *" "576" 0
run_test "OVF-029: (1+2+3+4+5)*(6+7+8+9)" "1 2 + 3 + 4 + 5 + 6 7 + 8 + 9 + *" "450" 0
run_test "OVF-030: 9*8*7+6*5*4+3*2*1" "9 8 * 7 * 6 5 * 4 * + 3 2 * 1 * +" "630" 0
run_test "OVF-031: (9*8)+(7*6)+(5*4)+(3*2)" "9 8 * 7 6 * + 5 4 * + 3 2 * +" "140" 0
run_test "OVF-032: 9*9*9-8*8*8-7*7*7" "9 9 9 * * 8 8 8 * * - 7 7 7 * * -" "-126" 0
run_test "OVF-033: (9*9*9)+(8*8*8)+(7*7*7)" "9 9 9 * * 8 8 8 * * + 7 7 7 * * +" "1584" 0

# Edge cases with zeros and ones (11 tests)
run_test "OVF-034: 9*9*9*9*1" "9 9 * 9 * 9 * 1 *" "6561" 0
run_test "OVF-035: 9*9*9*9*0" "9 9 * 9 * 9 * 0 *" "0" 0
run_test "OVF-036: (9*9*9*9)+0" "9 9 9 * * 9 * 0 +" "6561" 0
run_test "OVF-037: (9*9*9*9)-0" "9 9 9 * * 9 * 0 -" "6561" 0
run_test "OVF-038: 0*(9*9*9*9)" "0 9 9 9 * * 9 * *" "0" 0
run_test "OVF-039: 1+(9*9*9*9)" "1 9 9 9 * * 9 * +" "6562" 0
run_test "OVF-040: (9*9*9*9)-1" "9 9 9 * * 9 * 1 -" "6560" 0
run_test "OVF-041: 0+0+9*9*9*9" "0 0 + 9 9 9 * * 9 * +" "6561" 0
run_test "OVF-042: 9*9*9*9+1+1" "9 9 9 * * 9 * 1 + 1 +" "6563" 0
run_test "OVF-043: 1*1*9*9*9*9" "1 1 * 9 * 9 * 9 * 9 *" "6561" 0
run_test "OVF-044: (8*8*8*8)/1" "8 8 8 * * 8 * 1 /" "4096" 0

# Test cases from the subject
echo -e "${BLUE}=== Subject Test Cases ===${NC}"
run_test "Subject example 1" "8 9 * 9 - 9 - 9 - 4 - 1 +" "42" 0
run_test "Subject example 2" "7 7 * 7 -" "42" 0
run_test "Subject example 3" "1 2 * 2 / 2 * 2 4 - +" "0" 0
run_test "Subject example 4 (brackets)" "(1 + 1)" "Error: invalid character '('" 1

echo

# Basic arithmetic tests
echo -e "${BLUE}=== Basic Arithmetic Tests ===${NC}"
run_test "Simple addition" "1 2 +" "3" 0
run_test "Simple subtraction" "5 3 -" "2" 0
run_test "Simple multiplication" "4 5 *" "20" 0
run_test "Simple division" "8 2 /" "4" 0
run_test "Zero addition" "0 5 +" "5" 0
run_test "Zero multiplication" "7 0 *" "0" 0

echo

# Edge cases with single digits
echo -e "${BLUE}=== Single Digit Validation ===${NC}"
run_test "Max single digit" "9 9 +" "18" 0
run_test "Min single digit" "0 1 +" "1" 0
run_test "All single digits" "1 2 3 4 5 + + + +" "15" 0
run_test "Zero operations" "0 0 +" "0" 0
run_test "Zero division result" "0 5 /" "0" 0

echo

# Order dependency tests
echo -e "${BLUE}=== Order Dependency Tests ===${NC}"
run_test "Subtraction order" "5 3 -" "2" 0
run_test "Division order" "8 2 /" "4" 0
run_test "Complex order" "1 2 + 3 -" "0" 0
run_test "Reverse order" "3 1 -" "2" 0

echo

# Complex expressions
echo -e "${BLUE}=== Complex Expression Tests ===${NC}"
run_test "Multiple operations" "1 2 + 3 4 + *" "21" 0
run_test "Nested calculations" "2 3 + 4 5 + *" "45" 0
run_test "Mixed operations" "9 5 + 2 * 3 -" "25" 0
run_test "Large result" "9 9 * 9 +" "90" 0

echo

# Error cases - Invalid tokens
echo -e "${BLUE}=== Invalid Token Tests ===${NC}"
run_test "Multi-digit number" "12 3 +" "Error: numbers must be single digits (0-9)" 1
run_test "Negative number attempt" "-5 3 +" "Error: invalid RPN expression - need exactly (numbers - 1) operators" 1
run_test "Invalid character" "a 3 +" "Error: invalid character 'a'" 1
run_test "Decimal number" "1.5 3 +" "Error: invalid character '.'" 1
run_test "Mixed invalid" "1 @ 3 +" "Error: invalid character '@'" 1

echo

# Error cases - Structure validation
echo -e "${BLUE}=== Structure Validation Tests ===${NC}"
run_test "Too many operators" "1 2 + +" "Error: invalid RPN expression - need exactly (numbers - 1) operators" 1
run_test "Too few operators" "1 2 3" "Error: need at least 1 operator for RPN calculation" 1
run_test "Only one number" "5" "Error: need at least 2 numbers for RPN calculation" 1
run_test "Only operators" "+ - *" "Error: need at least 2 numbers for RPN calculation" 1
run_test "Empty expression" "" "Error: empty expression" 1

echo

# Error cases - Division by zero
echo -e "${BLUE}=== Division by Zero Tests ===${NC}"
run_test "Division by zero" "5 0 /" "Error: division by zero" 1
run_test "Complex division by zero" "1 2 + 0 /" "Error: division by zero" 1
run_test "Zero from subtraction" "5 5 - 1 /" "0" 0
run_test "Division by computed zero" "2 1 1 - /" "Error: division by zero" 1

echo

# Error cases - Division overflow (INT_MIN / -1)
echo -e "${BLUE}=== Division Overflow Tests ===${NC}"
run_test "Division result stays in range" "9 9 * 9 * 9 * 9 * 9 * 9 * 9 * 9 * 0 5 - * 0 1 - /" "1.9371e+09" 0
run_test "Large negative division" "9 9 * 9 * 9 * 0 1 - * 0 1 - /" "6561" 0

echo

# Error cases - Insufficient operands
echo -e "${BLUE}=== Insufficient Operands Tests ===${NC}"
run_test "Operator without operands" "+" "Error: need at least 2 numbers for RPN calculation" 1
run_test "Operator with one operand" "5 +" "Error: need at least 2 numbers for RPN calculation" 1

echo

# Whitespace handling
echo -e "${BLUE}=== Whitespace Handling Tests ===${NC}"
run_test "Extra spaces" "  1   2   +  " "3" 0
run_test "Tabs and spaces" "1	2	+" "3" 0
run_test "Leading/trailing spaces" "   5 3 -   " "2" 0

echo

# Argument validation
echo -e "${BLUE}=== Argument Validation Tests ===${NC}"
# Test with no arguments by running the program directly
echo -n "Testing: No arguments... "
result=$(./RPN 2>&1)
exit_code=$?
TOTAL_TESTS=$((TOTAL_TESTS + 1))
if [ "$exit_code" -eq 1 ] && [ "$result" = "Error" ]; then
    echo -e "${GREEN}PASS${NC}"
    PASSED_TESTS=$((PASSED_TESTS + 1))
else
    echo -e "${RED}FAIL${NC}"
    echo -e "  Expected: 'Error' (exit code: 1)"
    echo -e "  Got:      '$result' (exit code: $exit_code)"
    FAILED_TESTS=$((FAILED_TESTS + 1))
fi

# Test with multiple arguments
echo -n "Testing: Multiple arguments... "
result=$(./RPN "1 2 +" "extra" 2>&1)
exit_code=$?
TOTAL_TESTS=$((TOTAL_TESTS + 1))
if [ "$exit_code" -eq 1 ] && [ "$result" = "Error" ]; then
    echo -e "${GREEN}PASS${NC}"
    PASSED_TESTS=$((PASSED_TESTS + 1))
else
    echo -e "${RED}FAIL${NC}"
    echo -e "  Expected: 'Error' (exit code: 1)"
    echo -e "  Got:      '$result' (exit code: $exit_code)"
    FAILED_TESTS=$((FAILED_TESTS + 1))
fi

echo

# Comprehensive RPN Order of Operations Tests (64 tests)
echo -e "${BLUE}=== Comprehensive RPN Order Tests (64 tests) ===${NC}"

# Simple two-operand tests (16 tests)
run_test "RPN-001: 1+2" "1 2 +" "3" 0
run_test "RPN-002: 2+3" "2 3 +" "5" 0
run_test "RPN-003: 4+5" "4 5 +" "9" 0
run_test "RPN-004: 6+7" "6 7 +" "13" 0
run_test "RPN-005: 8-3" "8 3 -" "5" 0
run_test "RPN-006: 9-4" "9 4 -" "5" 0
run_test "RPN-007: 7-2" "7 2 -" "5" 0
run_test "RPN-008: 6-1" "6 1 -" "5" 0
run_test "RPN-009: 3*2" "3 2 *" "6" 0
run_test "RPN-010: 4*2" "4 2 *" "8" 0
run_test "RPN-011: 5*2" "5 2 *" "10" 0
run_test "RPN-012: 6*2" "6 2 *" "12" 0
run_test "RPN-013: 8/2" "8 2 /" "4" 0
run_test "RPN-014: 9/3" "9 3 /" "3" 0
run_test "RPN-015: 6/2" "6 2 /" "3" 0
run_test "RPN-016: 4/2" "4 2 /" "2" 0

# Three-operand tests (16 tests)
run_test "RPN-017: (1+2)+3" "1 2 + 3 +" "6" 0
run_test "RPN-018: (2+3)+4" "2 3 + 4 +" "9" 0
run_test "RPN-019: (4+5)-2" "4 5 + 2 -" "7" 0
run_test "RPN-020: (6+1)-3" "6 1 + 3 -" "4" 0
run_test "RPN-021: (2*3)+1" "2 3 * 1 +" "7" 0
run_test "RPN-022: (3*2)+4" "3 2 * 4 +" "10" 0
run_test "RPN-023: (8/2)+1" "8 2 / 1 +" "5" 0
run_test "RPN-024: (9/3)+2" "9 3 / 2 +" "5" 0
run_test "RPN-025: 1+(2+3)" "1 2 3 + +" "6" 0
run_test "RPN-026: 2+(3+4)" "2 3 4 + +" "9" 0
run_test "RPN-027: 9-(2+3)" "9 2 3 + -" "4" 0
run_test "RPN-028: 8-(1+2)" "8 1 2 + -" "5" 0
run_test "RPN-029: 2*(3+1)" "2 3 1 + *" "8" 0
run_test "RPN-030: 3*(2+2)" "3 2 2 + *" "12" 0
run_test "RPN-031: 8/(2+2)" "8 2 2 + /" "2" 0
run_test "RPN-032: 9/(1+2)" "9 1 2 + /" "3" 0

# Four-operand tests (16 tests)
run_test "RPN-033: (1+2)+(3+4)" "1 2 + 3 4 + +" "10" 0
run_test "RPN-034: (2+3)+(4+5)" "2 3 + 4 5 + +" "14" 0
run_test "RPN-035: (5+4)-(2+1)" "5 4 + 2 1 + -" "6" 0
run_test "RPN-036: (6+3)-(4+2)" "6 3 + 4 2 + -" "3" 0
run_test "RPN-037: (2+1)*(3+1)" "2 1 + 3 1 + *" "12" 0
run_test "RPN-038: (3+2)*(2+1)" "3 2 + 2 1 + *" "15" 0
run_test "RPN-039: (6+2)/(2+2)" "6 2 + 2 2 + /" "2" 0
run_test "RPN-040: (8+1)/(1+2)" "8 1 + 1 2 + /" "3" 0
run_test "RPN-041: (2*3)+(1*4)" "2 3 * 1 4 * +" "10" 0
run_test "RPN-042: (3*2)+(2*2)" "3 2 * 2 2 * +" "10" 0
run_test "RPN-043: (8/2)+(6/3)" "8 2 / 6 3 / +" "6" 0
run_test "RPN-044: (9/3)+(8/4)" "9 3 / 8 4 / +" "5" 0
run_test "RPN-045: (4*2)-(1*3)" "4 2 * 1 3 * -" "5" 0
run_test "RPN-046: (5*2)-(2*3)" "5 2 * 2 3 * -" "4" 0
run_test "RPN-047: (9/3)-(8/4)" "9 3 / 8 4 / -" "1" 0
run_test "RPN-048: (8/2)-(6/3)" "8 2 / 6 3 / -" "2" 0

# Complex nested operations (16 tests)
run_test "RPN-049: ((1+2)*3)+4" "1 2 + 3 * 4 +" "13" 0
run_test "RPN-050: ((2+3)*2)+1" "2 3 + 2 * 1 +" "11" 0
run_test "RPN-051: ((4+1)*2)-3" "4 1 + 2 * 3 -" "7" 0
run_test "RPN-052: ((3+2)*2)-4" "3 2 + 2 * 4 -" "6" 0
run_test "RPN-053: ((6+2)/2)+1" "6 2 + 2 / 1 +" "5" 0
run_test "RPN-054: ((8+1)/3)+2" "8 1 + 3 / 2 +" "5" 0
run_test "RPN-055: ((9-3)/2)+1" "9 3 - 2 / 1 +" "4" 0
run_test "RPN-056: ((8-2)/3)+2" "8 2 - 3 / 2 +" "4" 0
run_test "RPN-057: 1+((2+3)*2)" "1 2 3 + 2 * +" "11" 0
run_test "RPN-058: 2+((3+1)*2)" "2 3 1 + 2 * +" "10" 0
run_test "RPN-059: 9-((2+1)*2)" "9 2 1 + 2 * -" "3" 0
run_test "RPN-060: 8-((1+2)*2)" "8 1 2 + 2 * -" "2" 0
run_test "RPN-061: 2*((3+1)/2)" "2 3 1 + 2 / *" "4" 0
run_test "RPN-062: 3*((4+2)/3)" "3 4 2 + 3 / *" "6" 0
run_test "RPN-063: 8/((2+2)/2)" "8 2 2 + 2 / /" "4" 0
run_test "RPN-064: 9/((1+2)/1)" "9 1 2 + 1 / /" "3" 0

echo

# Extended RPN Tests with Large Results (100 tests)
echo -e "${BLUE}=== Extended RPN Tests - Large Results (100 tests) ===${NC}"

# Large multiplication results (25 tests)
run_test "EXT-001: 9*9*9" "9 9 * 9 *" "729" 0
run_test "EXT-002: 8*8*8" "8 8 * 8 *" "512" 0
run_test "EXT-003: 7*8*9" "7 8 * 9 *" "504" 0
run_test "EXT-004: 6*7*8*9" "6 7 * 8 * 9 *" "3024" 0
run_test "EXT-005: 5*6*7*8" "5 6 * 7 * 8 *" "1680" 0
run_test "EXT-006: 9*8*7*6" "9 8 * 7 * 6 *" "3024" 0
run_test "EXT-007: 4*5*6*7*8" "4 5 * 6 * 7 * 8 *" "6720" 0
run_test "EXT-008: 3*4*5*6*7" "3 4 * 5 * 6 * 7 *" "2520" 0
run_test "EXT-009: 2*3*4*5*6*7" "2 3 * 4 * 5 * 6 * 7 *" "5040" 0
run_test "EXT-010: 9*9+8*8" "9 9 * 8 8 * +" "145" 0
run_test "EXT-011: 8*7+6*5" "8 7 * 6 5 * +" "86" 0
run_test "EXT-012: 9*8+7*6" "9 8 * 7 6 * +" "114" 0
run_test "EXT-013: 7*7+8*8" "7 7 * 8 8 * +" "113" 0
run_test "EXT-014: 6*9+5*8" "6 9 * 5 8 * +" "94" 0
run_test "EXT-015: 5*7+4*9" "5 7 * 4 9 * +" "71" 0
run_test "EXT-016: 9*6+8*7" "9 6 * 8 7 * +" "110" 0
run_test "EXT-017: 8*9+6*7" "8 9 * 6 7 * +" "114" 0
run_test "EXT-018: 7*9+5*8" "7 9 * 5 8 * +" "103" 0
run_test "EXT-019: 6*8+7*9" "6 8 * 7 9 * +" "111" 0
run_test "EXT-020: 5*9+6*8" "5 9 * 6 8 * +" "93" 0
run_test "EXT-021: 4*9+7*8" "4 9 * 7 8 * +" "92" 0
run_test "EXT-022: 3*9+8*7" "3 9 * 8 7 * +" "83" 0
run_test "EXT-023: 2*9+8*8" "2 9 * 8 8 * +" "82" 0
run_test "EXT-024: 1*9+8*9" "1 9 * 8 9 * +" "81" 0
run_test "EXT-025: 9*9-8*7" "9 9 * 8 7 * -" "25" 0

# Complex addition chains (25 tests)
run_test "EXT-026: 9+8+7+6+5+4+3+2+1" "9 8 + 7 + 6 + 5 + 4 + 3 + 2 + 1 +" "45" 0
run_test "EXT-027: 8+7+6+5+4+3+2+1" "8 7 + 6 + 5 + 4 + 3 + 2 + 1 +" "36" 0
run_test "EXT-028: 7+6+5+4+3+2+1" "7 6 + 5 + 4 + 3 + 2 + 1 +" "28" 0
run_test "EXT-029: 6+5+4+3+2+1" "6 5 + 4 + 3 + 2 + 1 +" "21" 0
run_test "EXT-030: 5+4+3+2+1" "5 4 + 3 + 2 + 1 +" "15" 0
run_test "EXT-031: 9+9+9+9+9" "9 9 + 9 + 9 + 9 +" "45" 0
run_test "EXT-032: 8+8+8+8+8+8" "8 8 + 8 + 8 + 8 + 8 +" "48" 0
run_test "EXT-033: 7+7+7+7+7+7+7" "7 7 + 7 + 7 + 7 + 7 + 7 +" "49" 0
run_test "EXT-034: 6+6+6+6+6+6+6+6" "6 6 + 6 + 6 + 6 + 6 + 6 + 6 +" "48" 0
run_test "EXT-035: 5+5+5+5+5+5+5+5+5" "5 5 + 5 + 5 + 5 + 5 + 5 + 5 + 5 +" "45" 0
run_test "EXT-036: (9+8)+(7+6)+(5+4)" "9 8 + 7 6 + + 5 4 + +" "39" 0
run_test "EXT-037: (8+7)+(6+5)+(4+3)" "8 7 + 6 5 + + 4 3 + +" "33" 0
run_test "EXT-038: (7+6)+(5+4)+(3+2)" "7 6 + 5 4 + + 3 2 + +" "27" 0
run_test "EXT-039: (6+5)+(4+3)+(2+1)" "6 5 + 4 3 + + 2 1 + +" "21" 0
run_test "EXT-040: (9+9)+(8+8)+(7+7)" "9 9 + 8 8 + + 7 7 + +" "48" 0
run_test "EXT-041: (8+8)+(7+7)+(6+6)" "8 8 + 7 7 + + 6 6 + +" "42" 0
run_test "EXT-042: (7+7)+(6+6)+(5+5)" "7 7 + 6 6 + + 5 5 + +" "36" 0
run_test "EXT-043: (6+6)+(5+5)+(4+4)" "6 6 + 5 5 + + 4 4 + +" "30" 0
run_test "EXT-044: (5+5)+(4+4)+(3+3)" "5 5 + 4 4 + + 3 3 + +" "24" 0
run_test "EXT-045: (4+4)+(3+3)+(2+2)" "4 4 + 3 3 + + 2 2 + +" "18" 0
run_test "EXT-046: (3+3)+(2+2)+(1+1)" "3 3 + 2 2 + + 1 1 + +" "12" 0
run_test "EXT-047: 9+8*7+6*5" "9 8 7 * + 6 5 * +" "95" 0
run_test "EXT-048: 8+7*6+5*4" "8 7 6 * + 5 4 * +" "70" 0
run_test "EXT-049: 7+6*5+4*3" "7 6 5 * + 4 3 * +" "49" 0
run_test "EXT-050: 6+5*4+3*2" "6 5 4 * + 3 2 * +" "32" 0

# Large subtraction chains (25 tests)
run_test "EXT-051: 9*9-8-7-6-5" "9 9 * 8 - 7 - 6 - 5 -" "55" 0
run_test "EXT-052: 8*8-7-6-5-4" "8 8 * 7 - 6 - 5 - 4 -" "42" 0
run_test "EXT-053: 7*7-6-5-4-3" "7 7 * 6 - 5 - 4 - 3 -" "31" 0
run_test "EXT-054: 6*6-5-4-3-2" "6 6 * 5 - 4 - 3 - 2 -" "22" 0
run_test "EXT-055: 5*5-4-3-2-1" "5 5 * 4 - 3 - 2 - 1 -" "15" 0
run_test "EXT-056: 9*8-7*6" "9 8 * 7 6 * -" "30" 0
run_test "EXT-057: 8*7-6*5" "8 7 * 6 5 * -" "26" 0
run_test "EXT-058: 7*6-5*4" "7 6 * 5 4 * -" "22" 0
run_test "EXT-059: 6*5-4*3" "6 5 * 4 3 * -" "18" 0
run_test "EXT-060: 5*4-3*2" "5 4 * 3 2 * -" "14" 0
run_test "EXT-061: 9*9-8*8" "9 9 * 8 8 * -" "17" 0
run_test "EXT-062: 8*8-7*7" "8 8 * 7 7 * -" "15" 0
run_test "EXT-063: 7*7-6*6" "7 7 * 6 6 * -" "13" 0
run_test "EXT-064: 6*6-5*5" "6 6 * 5 5 * -" "11" 0
run_test "EXT-065: 5*5-4*4" "5 5 * 4 4 * -" "9" 0
run_test "EXT-066: 4*4-3*3" "4 4 * 3 3 * -" "7" 0
run_test "EXT-067: 3*3-2*2" "3 3 * 2 2 * -" "5" 0
run_test "EXT-068: 2*2-1*1" "2 2 * 1 1 * -" "3" 0
run_test "EXT-069: (9+8)*(7-6)" "9 8 + 7 6 - *" "17" 0
run_test "EXT-070: (8+7)*(6-5)" "8 7 + 6 5 - *" "15" 0
run_test "EXT-071: (7+6)*(5-4)" "7 6 + 5 4 - *" "13" 0
run_test "EXT-072: (6+5)*(4-3)" "6 5 + 4 3 - *" "11" 0
run_test "EXT-073: (5+4)*(3-2)" "5 4 + 3 2 - *" "9" 0
run_test "EXT-074: (4+3)*(2-1)" "4 3 + 2 1 - *" "7" 0
run_test "EXT-075: (9-8)*(7+6)" "9 8 - 7 6 + *" "13" 0

# Complex division operations (25 tests)
run_test "EXT-076: 9*8*7/6" "9 8 * 7 * 6 /" "84" 0
run_test "EXT-077: 8*7*6/5" "8 7 * 6 * 5 /" "67.2" 0
run_test "EXT-078: 7*6*5/4" "7 6 * 5 * 4 /" "52.5" 0
run_test "EXT-079: 6*5*4/3" "6 5 * 4 * 3 /" "40" 0
run_test "EXT-080: 5*4*3/2" "5 4 * 3 * 2 /" "30" 0
run_test "EXT-081: 9*9/3+8*8/4" "9 9 * 3 / 8 8 * 4 / +" "43" 0
run_test "EXT-082: 8*8/2+7*7/7" "8 8 * 2 / 7 7 * 7 / +" "39" 0
run_test "EXT-083: 7*7/7+6*6/6" "7 7 * 7 / 6 6 * 6 / +" "13" 0
run_test "EXT-084: 6*6/3+5*5/5" "6 6 * 3 / 5 5 * 5 / +" "17" 0
run_test "EXT-085: 5*5/5+4*4/4" "5 5 * 5 / 4 4 * 4 / +" "9" 0
run_test "EXT-086: 9*8/2/3" "9 8 * 2 / 3 /" "12" 0
run_test "EXT-087: 8*9/3/2" "8 9 * 3 / 2 /" "12" 0
run_test "EXT-088: 7*8/2/2" "7 8 * 2 / 2 /" "14" 0
run_test "EXT-089: 6*9/3/2" "6 9 * 3 / 2 /" "9" 0
run_test "EXT-090: 5*8/2/2" "5 8 * 2 / 2 /" "10" 0
run_test "EXT-091: (8+4)*(9-3)/6" "8 4 + 9 3 - * 6 /" "12" 0
run_test "EXT-092: (7+5)*(8-2)/6" "7 5 + 8 2 - * 6 /" "12" 0
run_test "EXT-093: (6+6)*(7-1)/8" "6 6 + 7 1 - * 8 /" "9" 0
run_test "EXT-094: (5+7)*(6-2)/6" "5 7 + 6 2 - * 6 /" "8" 0
run_test "EXT-095: (4+8)*(5-1)/6" "4 8 + 5 1 - * 6 /" "8" 0
run_test "EXT-096: 9*8*7/6/7" "9 8 * 7 * 6 / 7 /" "12" 0
run_test "EXT-097: 8*7*6/5/6" "8 7 * 6 * 5 / 6 /" "11.2" 0
run_test "EXT-098: 7*6*5/4/5" "7 6 * 5 * 4 / 5 /" "10.5" 0
run_test "EXT-099: 6*5*4/3/4" "6 5 * 4 * 3 / 4 /" "10" 0
run_test "EXT-100: 5*4*3/2/3" "5 4 * 3 * 2 / 3 /" "10" 0

echo

# Overflow and Underflow Edge Cases (50 tests)
echo -e "${BLUE}=== Overflow and Underflow Edge Cases (50 tests) ===${NC}"

# Multiplication overflow cases (15 tests)
run_test "OUF-001: 9*9*9*9*9*9*9*9*9" "9 9 * 9 * 9 * 9 * 9 * 9 * 9 * 9 *" "3.8742e+08" 0
run_test "OUF-002: 8*8*8*8*8*8*8*8*8" "8 8 * 8 * 8 * 8 * 8 * 8 * 8 * 8 *" "1.34218e+08" 0
run_test "OUF-003: 7*7*7*7*7*7*7*7*7" "7 7 * 7 * 7 * 7 * 7 * 7 * 7 * 7 *" "4.03536e+07" 0
run_test "OUF-004: 6*6*6*6*6*6*6*6*6" "6 6 * 6 * 6 * 6 * 6 * 6 * 6 * 6 *" "1.00777e+07" 0
run_test "OUF-005: 5*5*5*5*5*5*5*5*5" "5 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 *" "1.95312e+06" 0
run_test "OUF-006: 9*9*9*9*9*9*9*9" "9 9 * 9 * 9 * 9 * 9 * 9 * 9 *" "4.30467e+07" 0
run_test "OUF-007: 8*8*8*8*8*8*8*8" "8 8 * 8 * 8 * 8 * 8 * 8 * 8 *" "1.67772e+07" 0
run_test "OUF-008: 7*8*9*8*7*6*5*4" "7 8 * 9 * 8 * 7 * 6 * 5 * 4 *" "3.38688e+06" 0
run_test "OUF-009: 6*7*8*9*8*7*6*5" "6 7 * 8 * 9 * 8 * 7 * 6 * 5 *" "5.08032e+06" 0
run_test "OUF-010: 5*6*7*8*9*8*7*6" "5 6 * 7 * 8 * 9 * 8 * 7 * 6 *" "5.08032e+06" 0
run_test "OUF-011: 4*5*6*7*8*9*8*7" "4 5 * 6 * 7 * 8 * 9 * 8 * 7 *" "3.38688e+06" 0
run_test "OUF-012: 3*4*5*6*7*8*9*8" "3 4 * 5 * 6 * 7 * 8 * 9 * 8 *" "1.45152e+06" 0
run_test "OUF-013: 2*3*4*5*6*7*8*9" "2 3 * 4 * 5 * 6 * 7 * 8 * 9 *" "362880" 0
run_test "OUF-014: Real overflow test 1" "9 9 * 9 * 9 * 9 * 9 * 9 * 9 * 9 * 9 * 9 * 9 * 9 * 9 * 9 * 9 * 9 * 9 * 9 * 9 *" "Error: overflow" 1
run_test "OUF-015: Real overflow test 2" "8 8 * 8 * 8 * 8 * 8 * 8 * 8 * 8 * 8 * 8 * 8 * 8 * 8 * 8 * 8 * 8 * 8 * 8 * 8 *" "Error: overflow" 1

# Addition overflow cases (10 tests)
run_test "OUF-016: Large addition chain" "9 9 9 * * 9 9 9 * * + 9 9 9 * * + 9 9 9 * * + 9 9 9 * * + 9 9 9 * * + 9 9 9 * * +" "5103" 0
run_test "OUF-017: Cascading addition" "8 8 8 * * 8 8 8 * * + 8 8 8 * * + 8 8 8 * * + 8 8 8 * * + 8 8 8 * * +" "3072" 0
run_test "OUF-018: Mixed large addition" "7 7 7 * * 8 8 8 * * + 9 9 9 * * + 8 8 8 * * + 7 7 7 * * +" "2439" 0
run_test "OUF-019: Progressive addition" "6 6 6 * * 7 7 7 * * + 8 8 8 * * + 9 9 9 * * + 8 8 8 * * +" "2312" 0
run_test "OUF-020: Exponential addition" "5 5 5 * * 6 6 6 * * + 7 7 7 * * + 8 8 8 * * + 9 9 9 * * +" "1925" 0
run_test "OUF-021: Real overflow test" "9 9 9 * * 9 * 9 9 9 * * 9 * + 9 9 9 * * 9 * + 9 9 9 * * 9 * + 9 9 9 * * 9 * + 9 9 9 * * 9 * + 9 9 9 * * 9 * + 9 9 9 * * 9 * + 9 9 9 * * 9 * + 9 9 9 * * 9 * + 9 9 9 * * 9 * + 9 9 9 * * 9 * + 9 9 9 * * 9 * + 9 9 9 * * 9 * +" "91854" 0
run_test "OUF-022: Another overflow test" "8 8 8 * * 8 * 8 8 8 * * 8 * + 8 8 8 * * 8 * + 8 8 8 * * 8 * + 8 8 8 * * 8 * + 8 8 8 * * 8 * + 8 8 8 * * 8 * + 8 8 8 * * 8 * + 8 8 8 * * 8 * + 8 8 8 * * 8 * + 8 8 8 * * 8 * + 8 8 8 * * 8 * + 8 8 8 * * 8 * + 8 8 8 * * 8 * +" "57344" 0
run_test "OUF-023: Third overflow test" "7 7 7 * * 7 * 7 7 7 * * 7 * + 7 7 7 * * 7 * + 7 7 7 * * 7 * + 7 7 7 * * 7 * + 7 7 7 * * 7 * + 7 7 7 * * 7 * + 7 7 7 * * 7 * + 7 7 7 * * 7 * + 7 7 7 * * 7 * + 7 7 7 * * 7 * + 7 7 7 * * 7 * + 7 7 7 * * 7 * + 7 7 7 * * 7 * +" "33614" 0
run_test "OUF-024: Fourth overflow test" "6 6 6 * * 6 * 6 6 6 * * 6 * + 6 6 6 * * 6 * + 6 6 6 * * 6 * + 6 6 6 * * 6 * + 6 6 6 * * 6 * + 6 6 6 * * 6 * + 6 6 6 * * 6 * + 6 6 6 * * 6 * + 6 6 6 * * 6 * + 6 6 6 * * 6 * + 6 6 6 * * 6 * + 6 6 6 * * 6 * + 6 6 6 * * 6 * +" "18144" 0
run_test "OUF-025: Fifth overflow test" "5 5 5 * * 5 * 5 5 5 * * 5 * + 5 5 5 * * 5 * + 5 5 5 * * 5 * + 5 5 5 * * 5 * + 5 5 5 * * 5 * + 5 5 5 * * 5 * + 5 5 5 * * 5 * + 5 5 5 * * 5 * + 5 5 5 * * 5 * + 5 5 5 * * 5 * + 5 5 5 * * 5 * + 5 5 5 * * 5 * + 5 5 5 * * 5 * +" "8750" 0

# Subtraction underflow cases (15 tests)
run_test "OUF-026: Large negative result" "1 9 9 9 * * 9 * -" "-6560" 0
run_test "OUF-027: Zero minus large" "0 8 8 8 * * 8 * -" "-4096" 0
run_test "OUF-028: Small minus huge" "2 7 7 7 * * 7 * -" "-2399" 0
run_test "OUF-029: Three minus large" "3 6 6 6 * * 6 * -" "-1293" 0
run_test "OUF-030: Four minus large" "4 5 5 5 * * 5 * -" "-621" 0
run_test "OUF-031: Real underflow test 1" "0 9 9 * 9 * 9 * 9 * 9 * 9 * 9 * 9 * 9 * 9 * 9 * 9 * 9 * 9 * 9 * 9 * 9 * 9 * 9 * -" "Error: overflow" 1
run_test "OUF-032: Real underflow test 2" "1 8 8 * 8 * 8 * 8 * 8 * 8 * 8 * 8 * 8 * 8 * 8 * 8 * 8 * 8 * 8 * 8 * 8 * 8 * 8 * -" "Error: overflow" 1
run_test "OUF-033: Real underflow test 3" "2 7 7 * 7 * 7 * 7 * 7 * 7 * 7 * 7 * 7 * 7 * 7 * 7 * 7 * 7 * 7 * 7 * 7 * 7 * 7 * -" "Error: overflow" 1
run_test "OUF-034: Real underflow test 4" "3 6 6 * 6 * 6 * 6 * 6 * 6 * 6 * 6 * 6 * 6 * 6 * 6 * 6 * 6 * 6 * 6 * 6 * 6 * 6 * -" "Error: overflow" 1
run_test "OUF-035: Real underflow test 5" "4 5 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * -" "Error: overflow" 1
run_test "OUF-036: Zero base subtraction" "0 9 9 9 * * 9 * - 8 8 8 * * -" "-7073" 0
run_test "OUF-037: One base subtraction" "1 8 8 8 * * 8 * - 7 7 7 * * -" "-4438" 0
run_test "OUF-038: Two base subtraction" "2 7 7 7 * * 7 * - 6 6 6 * * -" "-2615" 0
run_test "OUF-039: Three base subtraction" "3 6 6 6 * * 6 * - 5 5 5 * * -" "-1418" 0
run_test "OUF-040: Mixed base subtraction" "4 9 9 9 * * - 8 8 8 * * - 7 7 7 * * -" "-1580" 0

# Mixed overflow/underflow operations (10 tests)
run_test "OUF-041: Multiplication overflow" "9 9 * 9 * 9 * 9 * 9 * 9 * 9 * 9 * 9 *" "Error: overflow" 1
run_test "OUF-042: Large multiplication overflow" "8 8 * 8 * 8 * 8 * 8 * 8 * 8 * 8 * 8 * 8 *" "Error: overflow" 1
run_test "OUF-043: Multiplication underflow" "9 9 * 9 * 9 * 9 * 9 * 9 * 9 * 9 * 0 6 - *" "Error: underflow" 1
run_test "OUF-044: Addition overflow" "9 9 * 9 * 9 * 9 * 9 * 9 * 9 * 9 * 9 9 * 9 * 9 * 9 * 9 * 9 * 9 * 9 * + 9 9 * 9 * 9 * 9 * 9 * 9 * 9 * 9 * + 9 9 * 9 * 9 * 9 * 9 * 9 * 9 * 9 * + 9 9 * 9 * 9 * 9 * 9 * 9 * 9 * 9 * + 9 9 * 9 * 9 * 9 * 9 * 9 * 9 * 9 * + 9 9 * 9 * 9 * 9 * 9 * 9 * 9 * 9 * +" "Error: overflow" 1
run_test "OUF-045: Negative multiplication underflow" "9 9 * 9 * 9 * 9 * 9 * 9 * 9 * 9 * 0 7 - *" "Error: underflow" 1
run_test "OUF-046: Chain multiplication overflow" "7 7 * 7 * 7 * 7 * 7 * 7 * 7 * 7 * 7 * 7 * 7 *" "Error: overflow" 1
run_test "OUF-047: Subtraction underflow" "9 9 * 9 * 9 * 9 * 9 * 9 * 9 * 9 * 0 6 - * 2 - 0 1 - /" "Error: underflow" 1
run_test "OUF-048: Sequential multiplication overflow" "6 6 * 6 * 6 * 6 * 6 * 6 * 6 * 6 * 6 * 6 * 6 * 6 *" "Error: overflow" 1
run_test "OUF-049: Large negative multiplication" "8 8 * 8 * 8 * 8 * 8 * 8 * 8 * 8 * 8 * 0 4 - *" "Error: underflow" 1
run_test "OUF-050: Mixed operation overflow" "5 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 *" "Error: overflow" 1

echo

# Summary
echo -e "${BLUE}=== Test Summary ===${NC}"
echo -e "Total tests: $TOTAL_TESTS"
echo -e "Passed: ${GREEN}$PASSED_TESTS${NC}"
echo -e "Failed: ${RED}$FAILED_TESTS${NC}"

if [ $FAILED_TESTS -eq 0 ]; then
    echo -e "${GREEN}All tests passed! 🎉${NC}"
    exit 0
else
    echo -e "${RED}Some tests failed. Please review the output above.${NC}"
    exit 1
fi
