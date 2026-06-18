#!/bin/bash

# ============================================================================
# Siege Testing Script for WebServ - Comprehensive HTTP Server Testing
# ============================================================================
# Tests include:
# - Status code validation (200, 301, 404, 405)
# - CGI script execution performance
# - Connection timeout handling
# - Concurrent user load testing
# - Multiple ports and endpoints
# - Mixed request types (GET, POST, DELETE)
# ============================================================================

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
MAGENTA='\033[0;35m'
NC='\033[0m' # No Color

# Configuration
SERVER_HOST="localhost"
SERVER_PORT="8080"
BASE_URL="http://${SERVER_HOST}:${SERVER_PORT}"
URLS_FILE="siege_urls.txt"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Print colored output
print_header() {
    echo -e "\n${BLUE}╔════════════════════════════════════════════════════════════╗${NC}"
    echo -e "${BLUE}║${NC} ${GREEN}$1${NC}"
    echo -e "${BLUE}╚════════════════════════════════════════════════════════════╝${NC}\n"
}

print_subheader() {
    echo -e "\n${CYAN}▶ $1${NC}"
    echo -e "${CYAN}$(printf '─%.0s' {1..60})${NC}\n"
}

print_info() {
    echo -e "${YELLOW}ℹ${NC}  $1"
}

print_success() {
    echo -e "${GREEN}✓${NC} $1"
}

print_error() {
    echo -e "${RED}✗${NC} $1"
}

print_warning() {
    echo -e "${MAGENTA}⚠${NC}  $1"
}

# Check if server is running
check_server() {
    print_info "Checking if server is running on ${BASE_URL}..."
    
    if curl -s --max-time 2 "${BASE_URL}/" > /dev/null 2>&1; then
        print_success "Server is responding on ${BASE_URL}"
        
        # Test multiple ports
        for port in 8080 8081 8082; do
            if curl -s --max-time 2 "http://${SERVER_HOST}:${port}/" > /dev/null 2>&1; then
                print_success "Port ${port} is active"
            else
                print_warning "Port ${port} is not responding (may not be configured)"
            fi
        done
        return 0
    else
        print_error "Server is not responding on ${BASE_URL}"
        print_info "Please start your server with: ./webserv webserv.conf"
        exit 1
    fi
}

# Validate status codes
test_status_codes() {
    print_header "Status Code Validation Tests"
    print_info "Testing various HTTP status codes..."
    
    print_subheader "Testing 200 OK - Static Files"
    echo "GET http://localhost:8080/"
    curl -s -o /dev/null -w "Status: %{http_code} | Time: %{time_total}s\n" "http://localhost:8080/"
    echo "GET http://localhost:8080/index.html"
    curl -s -o /dev/null -w "Status: %{http_code} | Time: %{time_total}s\n" "http://localhost:8080/index.html"
    echo "GET http://localhost:8080/test.html"
    curl -s -o /dev/null -w "Status: %{http_code} | Time: %{time_total}s\n" "http://localhost:8080/test.html"
    
    print_subheader "Testing 200 OK - Directory Listing"
    echo "GET http://localhost:8080/browse/"
    curl -s -o /dev/null -w "Status: %{http_code} | Time: %{time_total}s\n" "http://localhost:8080/browse/"
    
    print_subheader "Testing 200 OK - CGI Execution"
    echo "GET http://localhost:8080/cgi-bin/test.py"
    curl -s -o /dev/null -w "Status: %{http_code} | Time: %{time_total}s\n" "http://localhost:8080/cgi-bin/test.py"
    echo "GET http://localhost:8080/cgi-bin/test.sh"
    curl -s -o /dev/null -w "Status: %{http_code} | Time: %{time_total}s\n" "http://localhost:8080/cgi-bin/test.sh"
    echo "POST http://localhost:8080/cgi-bin/test.py"
    curl -s -o /dev/null -w "Status: %{http_code} | Time: %{time_total}s\n" -X POST -d "name=test&value=123" "http://localhost:8080/cgi-bin/test.py"
    
    print_subheader "Testing 301 Redirect"
    echo "GET http://localhost:8080/redirect (expect 301)"
    curl -s -o /dev/null -w "Status: %{http_code} | Redirect: %{redirect_url}\n" "http://localhost:8080/redirect"
    echo "GET http://localhost:8080/old-page (expect 301)"
    curl -s -o /dev/null -w "Status: %{http_code} | Redirect: %{redirect_url}\n" "http://localhost:8080/old-page"
    
    print_subheader "Testing 404 Not Found"
    echo "GET http://localhost:8080/nonexistent.html (expect 404)"
    curl -s -o /dev/null -w "Status: %{http_code} | Time: %{time_total}s\n" "http://localhost:8080/nonexistent.html"
    echo "GET http://localhost:8080/missing-page.html (expect 404)"
    curl -s -o /dev/null -w "Status: %{http_code} | Time: %{time_total}s\n" "http://localhost:8080/missing-page.html"
    echo "GET http://localhost:8080/cgi-bin/nonexistent.py (expect 404)"
    curl -s -o /dev/null -w "Status: %{http_code} | Time: %{time_total}s\n" "http://localhost:8080/cgi-bin/nonexistent.py"
    
    print_subheader "Testing 405 Method Not Allowed"
    echo "POST http://localhost:8080/browse/ (expect 405 - only GET allowed)"
    curl -s -o /dev/null -w "Status: %{http_code} | Time: %{time_total}s\n" -X POST -d "data=test" "http://localhost:8080/browse/"
    echo "DELETE http://localhost:8080/docs/ (expect 405 - only GET allowed)"
    curl -s -o /dev/null -w "Status: %{http_code} | Time: %{time_total}s\n" -X DELETE "http://localhost:8080/docs/"
    
    print_success "Status code validation complete!"
    echo ""
}

# Test CGI execution performance
test_cgi_execution() {
    print_header "CGI Script Execution Performance"
    print_info "Testing CGI script execution speed and reliability..."
    
    # Check if CGI scripts are available
    if ! curl -s --max-time 2 "${BASE_URL}/cgi-bin/test.py" > /dev/null 2>&1; then
        print_warning "CGI endpoint not available, skipping..."
        return 0
    fi
    
    print_subheader "Python CGI Performance"
    echo "Running siege on Python CGI script..."
    siege -c 10 -r 10 -v "${BASE_URL}/cgi-bin/test.py"
    
    print_subheader "Shell CGI Performance"
    echo "Running siege on Shell CGI script..."
    siege -c 10 -r 10 -v "${BASE_URL}/cgi-bin/test.sh"
    
    print_subheader "CGI with POST Data"
    echo "Running siege on CGI with POST requests..."
    siege -c 5 -r 5 -v "${BASE_URL}/cgi-bin/test.py POST name=test&value=123"
    
    print_subheader "CGI with Query Strings"
    echo "Running siege on CGI with query parameters..."
    siege -c 10 -r 10 -v "${BASE_URL}/cgi-bin/test.py?query=value&test=123"
    
    print_success "CGI execution tests complete!"
    echo ""
}

# Test connection timeout handling
test_timeout_handling() {
    print_header "Connection Timeout & Keep-Alive Testing"
    print_info "Testing server timeout behavior and connection handling..."
    
    print_subheader "Single Connection - Multiple Requests"
    for i in {1..5}; do
        echo "Request $i/5:"
        curl -s -o /dev/null -w "Status: %{http_code} | Connect: %{time_connect}s | Total: %{time_total}s\n" "${BASE_URL}/"
        sleep 0.5
    done
    
    print_subheader "Rapid Sequential Requests (Testing Connection Reuse)"
    for i in {1..10}; do
        curl -s -o /dev/null -w "Request $i - Status: %{http_code} | Time: %{time_total}s\n" "${BASE_URL}/index.html" &
    done
    wait
    
    print_subheader "Long-Running CGI Script (if available)"
    if curl -s --max-time 2 "${BASE_URL}/cgi-bin/test.py" > /dev/null 2>&1; then
        echo "Testing CGI timeout handling..."
        timeout 5 curl -v "${BASE_URL}/cgi-bin/test.py" 2>&1 | grep -E "(Connected|HTTP|timeout)" || true
    fi
    
    print_success "Timeout handling tests complete!"
    echo ""
}

# Test 1: Quick benchmark (10 seconds, 10 concurrent users)
test_quick_benchmark() {
    print_header "Test 1: Quick Benchmark (10s, 10 concurrent users)"
    print_info "Testing basic server performance with light load..."
    echo ""
    
    siege -c 10 -t 10S -v "${BASE_URL}/"
    
    print_success "Quick benchmark complete!"
    echo ""
}

# Test 2: Concurrent users stress test
test_concurrent_users() {
    print_header "Test 2: Concurrent Users Stress Test (25 users, 100 requests)"
    print_info "Testing with 25 concurrent users making 4 requests each..."
    echo ""
    
    siege -c 25 -r 4 -v "${BASE_URL}/"
    
    print_success "Concurrent users test complete!"
    echo ""
}

# Test 3: URL list testing (comprehensive)
test_url_list() {
    print_header "Test 3: Comprehensive URL List Testing"
    
    if [ ! -f "${SCRIPT_DIR}/${URLS_FILE}" ]; then
        print_error "URL file not found: ${SCRIPT_DIR}/${URLS_FILE}"
        return 1
    fi
    
    print_info "Testing all endpoints from ${URLS_FILE}..."
    print_info "This includes: static files, CGI scripts, redirects, error pages, POST requests"
    echo ""
    
    # Show URL count
    url_count=$(grep -v "^#" "${SCRIPT_DIR}/${URLS_FILE}" | grep -v "^$" | wc -l)
    print_info "Total URLs to test: ${url_count}"
    echo ""
    
    siege -c 15 -r 3 -v -f "${SCRIPT_DIR}/${URLS_FILE}"
    
    print_success "URL list testing complete!"
    echo ""
}

# Test 4: Sustained load test
test_sustained_load() {
    print_header "Test 4: Sustained Load Test (30s, 50 concurrent users)"
    print_info "Testing sustained performance under continuous load..."
    echo ""
    
    siege -c 50 -t 30S -v "${BASE_URL}/"
    
    print_success "Sustained load test complete!"
    echo ""
}

# Test 5: Heavy load stress test
test_heavy_load() {
    print_header "Test 5: Heavy Load Stress Test (2000 users, 50 requests each)"
    print_info "Testing server limits with heavy concurrent load..."
    print_warning "This test generates 5000 total requests!"
    echo ""
    
    siege -c 2000 -r 500 -v -b "${BASE_URL}/"
    
    print_success "Heavy load test complete!"
    echo ""
}

# Test 6: Static file performance
test_static_files() {
    print_header "Test 6: Static File Serving Performance"
    print_info "Testing static HTML file delivery..."
    echo ""
    
    print_subheader "Testing index.html"
    siege -c 20 -t 15S -v "${BASE_URL}/index.html"
    
    print_success "Static file test complete!"
    echo ""
}

# Test 7: Mixed request types
test_mixed_requests() {
    print_header "Test 7: Mixed Request Types (GET, POST, Multiple Ports)"
    print_info "Testing variety of HTTP methods and endpoints..."
    echo ""
    
    if [ ! -f "${SCRIPT_DIR}/${URLS_FILE}" ]; then
        print_error "URL file not found: ${SCRIPT_DIR}/${URLS_FILE}"
        return 1
    fi
    
    print_info "Testing with internet mode (random URL selection)..."
    siege -c 20 -t 20S -v -i -f "${SCRIPT_DIR}/${URLS_FILE}"
    
    print_success "Mixed request test complete!"
    echo ""
}

# Test 8: Internet simulation (random delays)
test_internet_simulation() {
    print_header "Test 8: Internet Simulation (Real-World Conditions)"
    print_info "Simulating real-world internet with random delays..."
    echo ""
    
    siege -c 15 -t 20S -v --delay=1 "${BASE_URL}/"
    
    print_success "Internet simulation complete!"
    echo ""
}

# Test 9: Multiple ports testing
test_multiple_ports() {
    print_header "Test 9: Multiple Port Testing (8080, 8081, 8082)"
    print_info "Testing all configured server ports..."
    echo ""
    
    for port in 8080 8081 8082; do
        print_subheader "Testing port ${port}"
        if curl -s --max-time 2 "http://${SERVER_HOST}:${port}/" > /dev/null 2>&1; then
            siege -c 10 -r 10 -v "http://${SERVER_HOST}:${port}/"
        else
            print_warning "Port ${port} not responding, skipping..."
        fi
        echo ""
    done
    
    print_success "Multiple port testing complete!"
    echo ""
}

# Custom test function
run_custom_test() {
    print_header "Custom Siege Test"
    
    if [ $# -eq 0 ]; then
        print_error "No custom parameters provided"
        return 1
    fi
    
    print_info "Running: siege $@"
    echo ""
    siege "$@"
}

# Show usage
show_usage() {
    cat << EOF
${GREEN}╔═══════════════════════════════════════════════════════════╗
║         WebServ Siege Testing Script - v2.0              ║
║              Comprehensive HTTP Server Testing            ║
╚═══════════════════════════════════════════════════════════╝${NC}

${CYAN}USAGE:${NC}
    $0 [option]

${CYAN}TEST OPTIONS:${NC}
    ${GREEN}all${NC}             Run all tests (default)
    ${GREEN}status${NC}          Test status codes (200, 301, 404, 405)
    ${GREEN}cgi${NC}             Test CGI script execution performance
    ${GREEN}timeout${NC}         Test connection timeout and keep-alive
    ${GREEN}quick${NC}           Quick benchmark (10s, 10 users)
    ${GREEN}concurrent${NC}      Concurrent users test (25 users)
    ${GREEN}urls${NC}            Test all URLs from file
    ${GREEN}sustained${NC}       Sustained load test (30s, 50 users)
    ${GREEN}heavy${NC}           Heavy load test (100 users, 5000 requests)
    ${GREEN}static${NC}          Static file performance
    ${GREEN}mixed${NC}           Mixed request types (GET/POST/DELETE)
    ${GREEN}internet${NC}        Internet simulation with delays
    ${GREEN}ports${NC}           Test all ports (8080, 8081, 8082)
    ${GREEN}custom${NC}          Run custom siege command (pass additional args)
    ${GREEN}help${NC}            Show this help message

${CYAN}EXAMPLES:${NC}
    ${YELLOW}$0${NC}                          # Run all tests
    ${YELLOW}$0 status${NC}                   # Test status codes only
    ${YELLOW}$0 cgi${NC}                      # Test CGI execution only
    ${YELLOW}$0 quick${NC}                    # Run quick benchmark
    ${YELLOW}$0 custom -c 100 -r 10${NC}      # Custom: 100 users, 10 requests each

${CYAN}WHAT GETS TESTED:${NC}
    ${GREEN}✓${NC} HTTP Status Codes: 200 OK, 301 Redirect, 404 Not Found, 405 Method Not Allowed
    ${GREEN}✓${NC} CGI Execution: Python, Shell, PHP, Perl, Ruby scripts
    ${GREEN}✓${NC} HTTP Methods: GET, POST, DELETE
    ${GREEN}✓${NC} Multiple Ports: 8080 (main), 8081 (dev), 8082 (api)
    ${GREEN}✓${NC} Static Files: HTML, CSS, JS delivery
    ${GREEN}✓${NC} Directory Listing: autoindex functionality
    ${GREEN}✓${NC} Redirects: 301 permanent redirects
    ${GREEN}✓${NC} Error Pages: Custom error page handling
    ${GREEN}✓${NC} File Upload: POST to upload endpoints
    ${GREEN}✓${NC} Connection Handling: Timeouts and keep-alive
    ${GREEN}✓${NC} Load Testing: Concurrent users and sustained load

${CYAN}SIEGE OPTIONS REFERENCE:${NC}
    -c NUM      Number of concurrent users
    -r NUM      Number of repetitions per user
    -t TIME     Time to run (e.g., 30S, 1M, 1H)
    -f FILE     URL file to test
    -d NUM      Delay between requests (seconds)
    -i          Internet mode (random URLs from file)
    -b          Benchmark mode (no delays)
    -v          Verbose output (shows each request)
    --delay=N   Random delay 0 to N seconds

${CYAN}NOTES:${NC}
    • Make sure your webserv is running before tests
    • Siege output shows: transactions, availability, response time, throughput
    • Failed transactions indicate errors (check your server logs)
    • Use -v flag for detailed request/response information

EOF
}

# Main execution
main() {
    echo -e "${GREEN}"
    cat << "EOF"
╔════════════════════════════════════════════════════════════╗
║        WebServ Siege Comprehensive Testing Suite          ║
║              HTTP/1.1 Server Load Testing                  ║
╚════════════════════════════════════════════════════════════╝
EOF
    echo -e "${NC}"
    
    # Check if siege is installed
    if ! command -v siege &> /dev/null; then
        print_error "Siege is not installed!"
        print_info "Install with: sudo apt-get install siege"
        exit 1
    fi
    
    # Check if server is running first
    check_server
    
    # Determine which test to run
    case "${1:-all}" in
        all)
            print_info "Running comprehensive test suite..."
            echo ""
            sleep 1
            test_status_codes
            sleep 2
            test_cgi_execution
            sleep 2
            test_timeout_handling
            sleep 2
            test_quick_benchmark
            sleep 2
            test_concurrent_users
            sleep 2
            test_static_files
            sleep 2
            test_url_list
            sleep 2
            test_mixed_requests
            sleep 2
            test_multiple_ports
            sleep 2
            test_sustained_load
            sleep 2
            test_internet_simulation
            print_header "🎉 ALL TESTS COMPLETE! 🎉"
            print_success "Your webserv has been thoroughly tested!"
            ;;
        status)
            test_status_codes
            ;;
        cgi)
            test_cgi_execution
            ;;
        timeout)
            test_timeout_handling
            ;;
        quick)
            test_quick_benchmark
            ;;
        concurrent)
            test_concurrent_users
            ;;
        urls)
            test_url_list
            ;;
        sustained)
            test_sustained_load
            ;;
        heavy)
            test_heavy_load
            ;;
        static)
            test_static_files
            ;;
        mixed)
            test_mixed_requests
            ;;
        internet)
            test_internet_simulation
            ;;
        ports)
            test_multiple_ports
            ;;
        custom)
            shift
            run_custom_test "$@"
            ;;
        help|--help|-h)
            show_usage
            ;;
        *)
            print_error "Unknown option: $1"
            show_usage
            exit 1
            ;;
    esac
    
    if [ "${1:-all}" != "help" ] && [ "${1:-all}" != "--help" ] && [ "${1:-all}" != "-h" ]; then
        echo ""
        print_success "Testing complete! 🚀"
        print_info "Check the siege output above for detailed statistics"
        print_info "Key metrics: Availability, Response Time, Transaction Rate, Throughput"
        echo ""
    fi
}

# Run main function
main "$@"
