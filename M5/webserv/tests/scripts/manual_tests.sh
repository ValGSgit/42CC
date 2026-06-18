#!/bin/bash
# manual_tests.sh - Quick manual tests using curl and telnet
# Use this for quick verification and debugging

HOST="localhost"
PORT=8080

echo "=================================================="
echo "WebServ Manual Testing Helper"
echo "=================================================="
echo ""
echo "Server: ${HOST}:${PORT}"
echo ""

# Check if curl is available
if ! command -v curl &> /dev/null; then
    echo "Warning: curl not found. Some tests will be skipped."
    echo "Install with: sudo apt-get install curl"
fi

# Function to print section header
print_header() {
    echo ""
    echo "=================================================="
    echo "$1"
    echo "=================================================="
}

# Function to run a curl test
run_curl() {
    local name=$1
    shift
    echo ""
    echo "--- $name ---"
    curl "$@"
    echo ""
}

# Basic GET tests
print_header "1. Basic GET Requests"
run_curl "GET root" -v "http://${HOST}:${PORT}/"
run_curl "GET index.html" -v "http://${HOST}:${PORT}/index.html"
run_curl "GET 404" -v "http://${HOST}:${PORT}/nonexistent.html"

# POST tests
print_header "2. POST Requests"
run_curl "POST with data" -X POST -d "name=John&age=30" "http://${HOST}:${PORT}/cgi-bin/test.py"
run_curl "POST JSON" -X POST -H "Content-Type: application/json" -d '{"key":"value"}' "http://${HOST}:${PORT}/cgi-bin/test.py"

# DELETE test
print_header "3. DELETE Request"
run_curl "DELETE" -X DELETE -v "http://${HOST}:${PORT}/uploads/test.txt"

# CGI tests
print_header "4. CGI Tests"
run_curl "CGI GET with query" -v "http://${HOST}:${PORT}/cgi-bin/test.py?name=Alice&city=Paris"
run_curl "CGI POST" -X POST -d "username=testuser&password=secret" "http://${HOST}:${PORT}/cgi-bin/test.py"

# File upload
print_header "5. File Upload"
echo "test content" > /tmp/webserv_test_upload.txt
run_curl "Upload file" -X POST -F "file=@/tmp/webserv_test_upload.txt" "http://${HOST}:${PORT}/uploads"
rm -f /tmp/webserv_test_upload.txt

# Various headers
print_header "6. Headers and Special Cases"
run_curl "Custom headers" -H "User-Agent: WebServTester/1.0" -H "Accept-Language: en-US" "http://${HOST}:${PORT}/"
run_curl "Keep-alive" -H "Connection: keep-alive" "http://${HOST}:${PORT}/"
run_curl "Range request" -H "Range: bytes=0-100" "http://${HOST}:${PORT}/index.html"

# Stress test (light)
print_header "7. Light Concurrent Test"
echo "Running 10 concurrent requests..."
for i in {1..10}; do
    curl -s "http://${HOST}:${PORT}/?req=$i" > /dev/null &
done
wait
echo "Done!"

# Large file download
print_header "8. Large File Test"
run_curl "Large file" -v "http://${HOST}:${PORT}/uploads/dog.jpeg" -o /tmp/webserv_test_download.jpg
if [ -f /tmp/webserv_test_download.jpg ]; then
    echo "Downloaded $(stat -f%z /tmp/webserv_test_download.jpg 2>/dev/null || stat -c%s /tmp/webserv_test_download.jpg) bytes"
    rm -f /tmp/webserv_test_download.jpg
fi

# Redirect test
print_header "9. Redirect Test"
run_curl "Redirect" -L -v "http://${HOST}:${PORT}/redirect"

# Telnet tests (if available)
if command -v telnet &> /dev/null || command -v nc &> /dev/null; then
    print_header "10. Raw HTTP Tests (Telnet/Netcat)"
    echo "You can manually test with:"
    echo ""
    echo "  telnet ${HOST} ${PORT}"
    echo "  GET / HTTP/1.1"
    echo "  Host: ${HOST}"
    echo "  [press Enter twice]"
    echo ""
    echo "Or with netcat:"
    echo "  echo -e 'GET / HTTP/1.1\r\nHost: ${HOST}\r\n\r\n' | nc ${HOST} ${PORT}"
fi

print_header "Testing Complete"
echo "Check the output above for any errors or unexpected responses."
echo ""
echo "For automated testing, use:"
echo "  ./test_webserv.py"
echo "  ./test_cgi.py"
echo "  ./stress_test.py"
echo ""
