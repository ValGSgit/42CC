#!/bin/bash

################################################################################
# CI/CD PRE-FLIGHT VALIDATION SCRIPT
# Run this before committing to ensure GitHub Actions will work
################################################################################

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
BOLD='\033[1m'
NC='\033[0m'

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

FAILED=0

echo -e "${BOLD}${BLUE}╔════════════════════════════════════════════════════════════════╗${NC}"
echo -e "${BOLD}${BLUE}║                                                                ║${NC}"
echo -e "${BOLD}${BLUE}║          CI/CD Pre-Flight Validation Check                     ║${NC}"
echo -e "${BOLD}${BLUE}║                                                                ║${NC}"
echo -e "${BOLD}${BLUE}╚════════════════════════════════════════════════════════════════╝${NC}"
echo ""

################################################################################
# Check 1: Required Files Exist
################################################################################
echo -e "${BOLD}[1/10]${NC} Checking required files..."

REQUIRED_FILES=(
    "run_tests.sh"
    "tests/run_all_tests.sh"
    "tests/comprehensive_edge_cases.sh"
    "Makefile"
    "webserv.conf"
    ".github/workflows/ci.yml"
)

for file in "${REQUIRED_FILES[@]}"; do
    if [ ! -f "$file" ]; then
        echo -e "${RED}  ✗ Missing: $file${NC}"
        ((FAILED++))
    fi
done

if [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}  ✓ All required files present${NC}"
fi
echo ""

################################################################################
# Check 2: Executable Permissions
################################################################################
echo -e "${BOLD}[2/10]${NC} Checking executable permissions..."

EXEC_FILES=(
    "run_tests.sh"
    "tests/run_all_tests.sh"
    "tests/comprehensive_edge_cases.sh"
)

for file in "${EXEC_FILES[@]}"; do
    if [ ! -x "$file" ]; then
        echo -e "${RED}  ✗ Not executable: $file${NC}"
        echo -e "${YELLOW}     Fix: chmod +x $file${NC}"
        ((FAILED++))
    fi
done

if [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}  ✓ All scripts have execute permissions${NC}"
fi
echo ""

################################################################################
# Check 3: Bash Syntax Validation
################################################################################
echo -e "${BOLD}[3/10]${NC} Validating bash script syntax..."

BASH_SCRIPTS=(
    "run_tests.sh"
    "tests/run_all_tests.sh"
    "tests/comprehensive_edge_cases.sh"
)

for script in "${BASH_SCRIPTS[@]}"; do
    if ! bash -n "$script" 2>/dev/null; then
        echo -e "${RED}  ✗ Syntax error in: $script${NC}"
        bash -n "$script"
        ((FAILED++))
    fi
done

if [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}  ✓ All bash scripts have valid syntax${NC}"
fi
echo ""

################################################################################
# Check 4: Python Syntax Validation
################################################################################
echo -e "${BOLD}[4/10]${NC} Validating Python script syntax..."

if command -v python3 &> /dev/null; then
    PYTHON_SCRIPTS=(
        "tests/security/run_all_security_tests.py"
        "tester/comprehensive_tester.py"
    )
    
    for script in "${PYTHON_SCRIPTS[@]}"; do
        if [ -f "$script" ]; then
            if ! python3 -m py_compile "$script" 2>/dev/null; then
                echo -e "${RED}  ✗ Syntax error in: $script${NC}"
                python3 -m py_compile "$script"
                ((FAILED++))
            fi
        fi
    done
    
    if [ $FAILED -eq 0 ]; then
        echo -e "${GREEN}  ✓ All Python scripts have valid syntax${NC}"
    fi
else
    echo -e "${YELLOW}  ⊘ Python3 not found, skipping Python validation${NC}"
fi
echo ""

################################################################################
# Check 5: YAML Syntax Validation
################################################################################
echo -e "${BOLD}[5/10]${NC} Validating GitHub Actions workflow YAML..."

if command -v python3 &> /dev/null; then
    if python3 -c "import yaml; yaml.safe_load(open('.github/workflows/ci.yml'))" 2>/dev/null; then
        echo -e "${GREEN}  ✓ GitHub Actions workflow YAML is valid${NC}"
    else
        echo -e "${RED}  ✗ GitHub Actions workflow YAML has syntax errors${NC}"
        python3 -c "import yaml; yaml.safe_load(open('.github/workflows/ci.yml'))"
        ((FAILED++))
    fi
else
    echo -e "${YELLOW}  ⊘ Python3 not found, skipping YAML validation${NC}"
fi
echo ""

################################################################################
# Check 6: Build Test
################################################################################
echo -e "${BOLD}[6/10]${NC} Testing clean build..."

if make fclean > /dev/null 2>&1 && make > /dev/null 2>&1; then
    echo -e "${GREEN}  ✓ Project builds successfully${NC}"
else
    echo -e "${RED}  ✗ Build failed${NC}"
    echo -e "${YELLOW}     Run 'make' to see errors${NC}"
    ((FAILED++))
fi
echo ""

################################################################################
# Check 7: Server Startup Test
################################################################################
echo -e "${BOLD}[7/10]${NC} Testing server startup..."

timeout 5 ./webserv webserv.conf > /tmp/webserv_preflight.log 2>&1 &
SERVER_PID=$!
sleep 3

if ps -p $SERVER_PID > /dev/null 2>&1; then
    echo -e "${GREEN}  ✓ Server starts successfully${NC}"
    kill $SERVER_PID 2>/dev/null
    wait $SERVER_PID 2>/dev/null || true
else
    echo -e "${RED}  ✗ Server failed to start${NC}"
    echo -e "${YELLOW}     Check log: /tmp/webserv_preflight.log${NC}"
    cat /tmp/webserv_preflight.log
    ((FAILED++))
fi
sleep 1
echo ""

################################################################################
# Check 8: Port Availability
################################################################################
echo -e "${BOLD}[8/10]${NC} Checking port availability..."

if lsof -Pi :8080 -sTCP:LISTEN -t >/dev/null 2>&1; then
    echo -e "${YELLOW}  ⚠ Port 8080 is currently in use${NC}"
    echo -e "${YELLOW}     This is OK if tests are running, but may cause issues${NC}"
else
    echo -e "${GREEN}  ✓ Port 8080 is available${NC}"
fi
echo ""

################################################################################
# Check 9: Dependencies Check
################################################################################
echo -e "${BOLD}[9/10]${NC} Checking required dependencies..."

DEPS_OK=0
DEPS_MISSING=()

# Build tools
command -v make &> /dev/null || DEPS_MISSING+=("make")
command -v g++ &> /dev/null || DEPS_MISSING+=("g++")

# Test tools
command -v curl &> /dev/null || DEPS_MISSING+=("curl")
command -v python3 &> /dev/null || DEPS_MISSING+=("python3")

# CGI interpreters (optional but recommended)
command -v php-cgi &> /dev/null || echo -e "${YELLOW}     Note: php-cgi not found (optional)${NC}"
command -v perl &> /dev/null || echo -e "${YELLOW}     Note: perl not found (optional)${NC}"
command -v ruby &> /dev/null || echo -e "${YELLOW}     Note: ruby not found (optional)${NC}"

if [ ${#DEPS_MISSING[@]} -gt 0 ]; then
    echo -e "${RED}  ✗ Missing dependencies: ${DEPS_MISSING[*]}${NC}"
    ((FAILED++))
else
    echo -e "${GREEN}  ✓ All critical dependencies present${NC}"
fi
echo ""

################################################################################
# Check 10: Test Directory Structure
################################################################################
echo -e "${BOLD}[10/10]${NC} Verifying test directory structure..."

TEST_DIRS=(
    "tests"
    "tests/test_output"
    "tests/scripts"
    "tests/security"
)

for dir in "${TEST_DIRS[@]}"; do
    if [ ! -d "$dir" ]; then
        echo -e "${RED}  ✗ Missing directory: $dir${NC}"
        ((FAILED++))
    fi
done

if [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}  ✓ Test directory structure is correct${NC}"
fi
echo ""

################################################################################
# Summary
################################################################################
echo -e "${BOLD}${BLUE}═══════════════════════════════════════════════════════════════${NC}"
echo -e "${BOLD}${BLUE}  VALIDATION SUMMARY${NC}"
echo -e "${BOLD}${BLUE}═══════════════════════════════════════════════════════════════${NC}"
echo ""

if [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}${BOLD}╔═══════════════════════════════════════════════════════════╗${NC}"
    echo -e "${GREEN}${BOLD}║                                                           ║${NC}"
    echo -e "${GREEN}${BOLD}║         ✓  ALL CHECKS PASSED - READY TO COMMIT  ✓        ║${NC}"
    echo -e "${GREEN}${BOLD}║                                                           ║${NC}"
    echo -e "${GREEN}${BOLD}╚═══════════════════════════════════════════════════════════╝${NC}"
    echo ""
    echo -e "${GREEN}Your CI/CD setup is ready! You can now:${NC}"
    echo ""
    echo -e "  1. Commit your changes:"
    echo -e "     ${BOLD}git add .${NC}"
    echo -e "     ${BOLD}git commit -m \"Add CI/CD infrastructure\"${NC}"
    echo ""
    echo -e "  2. Push to GitHub:"
    echo -e "     ${BOLD}git push origin testing${NC}"
    echo ""
    echo -e "  3. Check GitHub Actions:"
    echo -e "     Go to your repository → Actions tab"
    echo ""
    exit 0
else
    echo -e "${RED}${BOLD}╔═══════════════════════════════════════════════════════════╗${NC}"
    echo -e "${RED}${BOLD}║                                                           ║${NC}"
    echo -e "${RED}${BOLD}║         ✗  ${FAILED} CHECK(S) FAILED - FIX REQUIRED  ✗         ║${NC}"
    echo -e "${RED}${BOLD}║                                                           ║${NC}"
    echo -e "${RED}${BOLD}╚═══════════════════════════════════════════════════════════╝${NC}"
    echo ""
    echo -e "${YELLOW}Please fix the issues above before committing.${NC}"
    echo ""
    exit 1
fi
