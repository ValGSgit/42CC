# WebServ Test Suite

Comprehensive testing framework for the WebServ HTTP/1.1 server implementation.

## 📁 Directory Structure

```
tests/
├── run_all_tests.sh              # Master test runner (runs all suites)
├── comprehensive_edge_cases.sh   # Edge cases and corner scenarios
├── docs/                         # Test documentation
├── scripts/                      # Individual test scripts
│   ├── test_cgi.py              # CGI execution tests
│   ├── test_upload.py           # File upload tests
│   ├── stress_test.py           # Performance/stress tests
│   └── test_webserv.py          # Basic functionality tests
├── security/                     # Security vulnerability tests
│   ├── run_all_security_tests.py
│   ├── test_path_traversal.py
│   ├── test_cgi_injection.py
│   ├── test_dos_attacks.py
│   ├── test_http_smuggling.py
│   ├── test_file_operations.py
│   ├── test_input_validation.py
│   └── test_information_disclosure.py
├── session_cookie/               # Session/cookie management tests (bonus)
│   ├── run_all_tests.sh
│   ├── test_sessions.sh
│   └── test_cookies.sh
├── siege/                        # Load testing with siege
│   ├── test_siege.sh
│   └── siege_urls.txt
└── test_output/                  # Test results and logs
```

## 🚀 Quick Start

### Run All Tests

```bash
# From project root
./run_tests.sh

# Or from tests directory
cd tests
./run_all_tests.sh
```

### Run Specific Test Suites

```bash
# Comprehensive edge cases
./tests/comprehensive_edge_cases.sh

# Security tests
python3 tests/security/run_all_security_tests.py

# Session/Cookie tests (bonus)
bash tests/session_cookie/run_all_tests.sh

# Individual tests
python3 tests/scripts/test_cgi.py
python3 tests/scripts/test_upload.py
python3 tests/scripts/stress_test.py
```

## 📊 Test Categories

### 1. Comprehensive Edge Cases (`comprehensive_edge_cases.sh`)
- **Category 1**: CGI Execution (Python, PHP, Perl, Ruby, Bash)
- **Category 2**: HTTP Request Edge Cases
- **Category 3**: Path Traversal & Security
- **Category 4**: File Upload Tests
- **Category 5**: HTTP Methods (GET, POST, DELETE, HEAD, OPTIONS)
- **Category 6**: Redirects & Error Pages
- **Category 7**: Directory Listing
- **Category 8**: Session Management (Bonus)
- **Category 9**: Concurrency Tests
- **Category 10**: Content Type Handling

### 2. Security Tests (`security/`)
- Path traversal attacks
- CGI injection vulnerabilities
- DoS attack resistance
- HTTP request smuggling
- File operation security
- Input validation
- Information disclosure

### 3. Feature Tests (`scripts/`)
- CGI execution and environment
- File upload functionality
- Stress testing and performance
- Basic HTTP functionality

### 4. Session/Cookie Tests (`session_cookie/`) - Bonus
- Session creation and management
- Cookie handling
- Session persistence

### 5. Python Comprehensive Tests (`../tester/`)
- HTTP/1.1 RFC compliance
- Performance benchmarking
- Comprehensive status code testing

## 🎯 Test Requirements

### System Dependencies
```bash
# Ubuntu/Debian
sudo apt-get install -y \
    build-essential \
    g++ \
    make \
    python3 \
    python3-pip \
    php-cgi \
    perl \
    ruby \
    curl \
    netcat \
    lsof \
    valgrind

# Python packages
pip3 install requests
```

### Build Server
```bash
make          # Build standard version
make bonus    # Build with bonus features (sessions/cookies)
```

## 📝 Test Output

### Test Reports
All test reports are saved to `tests/test_output/`:
- `master_test_report_YYYYMMDD_HHMMSS.txt` - Master test report
- `test_results_YYYYMMDD_HHMMSS.log` - Individual test logs
- Security reports in `security/results/`

### Reading Results
```bash
# View latest master report
ls -t tests/test_output/master_test_report_*.txt | head -1 | xargs cat

# View latest test log
ls -t tests/test_output/test_results_*.log | head -1 | xargs cat

# View latest security report
ls -t tests/security/results/security_report_*.txt | head -1 | xargs cat
```

## 🔧 Makefile Integration

```bash
make run_tests        # Run all tests
make test_full        # Run all tests (alias)
make test_edge_cases  # Run edge case tests (server must be running)
make security_audit   # Run security audit
make vg              # Run with valgrind memory checking
```

## 🐙 GitHub Actions CI/CD

The project includes GitHub Actions workflow (`.github/workflows/ci.yml`) that:
1. Builds the server on every push/PR
2. Runs comprehensive test suite
3. Performs valgrind memory checks
4. Runs security audit
5. Uploads test reports as artifacts

### Viewing CI Results
- Go to repository → Actions tab
- Click on latest workflow run
- Download artifacts (test reports, logs)

## 🎨 Test Result Format

```
═══════════════════════════════════════════════════════════════
  Test Category Name
═══════════════════════════════════════════════════════════════

[TEST] Test description
[PASS] ✓ Test passed
[FAIL] ✗ Test failed
[WARN] ⚠ Warning or optional test

═══════════════════════════════════════════════════════════════
  TEST SUMMARY
═══════════════════════════════════════════════════════════════

Total Test Suites:    9
Passed:               7
Failed:               0
Skipped (Optional):   2
Success Rate:         100%

╔═══════════════════════════════════════════════════════════╗
║                                                           ║
║              ✓  ALL CRITICAL TESTS PASSED  ✓              ║
║                                                           ║
╚═══════════════════════════════════════════════════════════╝
```

## 🛠️ Troubleshooting

### Server Won't Start
```bash
# Check if port is in use
lsof -i :8080

# Kill existing process
lsof -ti:8080 | xargs kill -9

# Check server log
cat server_test.log
```

### Tests Failing
```bash
# Run with verbose output
bash -x tests/run_all_tests.sh

# Test server manually
./webserv webserv.conf &
curl -v http://localhost:8080/

# Check specific test category
./tests/comprehensive_edge_cases.sh | grep FAIL
```

### Missing Dependencies
```bash
# Check Python
python3 --version
pip3 --version

# Check CGI interpreters
php-cgi -v
perl -v
ruby -v

# Check tools
curl --version
nc -h
```

## 📚 Adding New Tests

### Add to Edge Cases
Edit `tests/comprehensive_edge_cases.sh`:
```bash
category_11_my_tests() {
    log "\n${BLUE}=== CATEGORY 11: MY TESTS ===${NC}\n"
    
    check_http_status "11.1: My test" "${BASE_URL}/path" "200"
}

# Add to main()
main() {
    # ... existing categories ...
    category_11_my_tests
}
```

### Add Python Test Script
Create `tests/scripts/test_myfeature.py`:
```python
#!/usr/bin/env python3
import requests

def test_my_feature():
    response = requests.get('http://localhost:8080/my-endpoint')
    assert response.status_code == 200
    
if __name__ == '__main__':
    test_my_feature()
    print("✓ All tests passed")
```

Add to `tests/run_all_tests.sh`:
```bash
run_my_feature_tests() {
    if [ -f "${SCRIPT_DIR}/scripts/test_myfeature.py" ]; then
        python3 "${SCRIPT_DIR}/scripts/test_myfeature.py"
    fi
}

# In main()
run_test_suite \
    "My Feature Tests" \
    "Testing My Feature" \
    "run_my_feature_tests" \
    "false"
```

## 🎯 Test Coverage

Current test coverage includes:
- ✅ HTTP/1.1 request parsing
- ✅ All HTTP methods (GET, POST, DELETE, HEAD, OPTIONS)
- ✅ CGI execution (5 interpreters)
- ✅ File uploads
- ✅ Path traversal prevention
- ✅ Security vulnerabilities
- ✅ Error handling
- ✅ Redirects
- ✅ Directory listing
- ✅ Content type detection
- ✅ Session management (bonus)
- ✅ Concurrent connections
- ✅ Performance/stress testing

## 📞 Support

For issues or questions:
1. Check test logs in `tests/test_output/`
2. Review server log: `server_test.log`
3. Run individual test suites to isolate issues
4. Check GitHub Actions workflow results

## 📄 License

Part of the WebServ project - 42 School curriculum.
