# Session & Cookie Management Tests

This directory contains comprehensive test suites for WebServ's session and cookie management functionality.

## Test Files

### 1. `test_sessions.sh`
Tests the session management system including:
- Session creation (login)
- Session validation
- Authenticated requests
- Session statistics
- Multiple concurrent sessions
- Invalid session handling
- Session persistence
- Session destruction (logout)
- Post-logout validation

### 2. `test_cookies.sh`
Tests cookie handling including:
- Set-Cookie header generation
- Cookie storage and retrieval
- Cookie transmission
- Multiple cookie handling
- Cookie attributes (Path, Domain, Expires)
- Special characters and encoding
- Cookie deletion
- Large cookie values
- Concurrent operations

### 3. `run_all_tests.sh`
Runs both test suites and generates a combined report.

## Requirements

- WebServ server running on `localhost:8080`
- `curl` command-line tool
- Bash shell (v4.0+)

## Usage

### Run All Tests
```bash
./run_all_tests.sh
```

### Run Session Tests Only
```bash
./test_sessions.sh
```

### Run Cookie Tests Only
```bash
./test_cookies.sh
```

### Make Scripts Executable
```bash
chmod +x *.sh
```

## Test Output

Tests provide colored output:
- ✓ **GREEN** - Test passed
- ✗ **RED** - Test failed
- **YELLOW** - Test name/details

Example output:
```
========================================
  WebServ Session Management Tests
========================================

Test 1: Session Creation
✓ PASS - Session Creation

Test 2: Cookie Storage
✓ PASS - Cookie Storage

...

========================================
           Test Summary
========================================
Total Tests: 10
Passed: 10
Failed: 0
Success Rate: 100.0%

All tests passed! ✓
```

## Test Results

Detailed logs are saved to:
- `/tmp/webserv_session_test_results.log` - Session test log
- `/tmp/webserv_cookie_test_results.log` - Cookie test log

## API Endpoints Tested

### Session Management
- `POST /api/session/login` - Create session
- `GET /api/session/profile` - Get user profile
- `POST /api/session/logout` - Destroy session
- `GET /api/session/info` - Get session statistics

### Cookie Handling
Tests are performed on various endpoints to verify cookie:
- Parsing from request headers
- Setting in response headers
- Persistence across requests
- Proper deletion on logout

## Expected Server Behavior

### Successful Session Flow
1. Client sends `POST /api/session/login`
2. Server creates session and returns:
   ```
   Set-Cookie: session_id=<unique_id>; Path=/
   {"success": true, "session_id": "<unique_id>"}
   ```
3. Client includes cookie in subsequent requests
4. Server validates and maintains session
5. Client sends `POST /api/session/logout`
6. Server destroys session and expires cookie

### Cookie Format
```
Set-Cookie: session_id=<timestamp>_<random1>_<random2>; Path=/; HttpOnly
```

## Troubleshooting

### Server Not Running
```
Error: Couldn't connect to server
```
**Solution**: Start WebServ server: `./webserv config/webserv.conf`

### All Tests Failing
```
Multiple FAIL results
```
**Solution**:
1. Check server is running on port 8080
2. Verify API endpoints are configured
3. Check server logs for errors

### Permission Denied
```
bash: ./test_sessions.sh: Permission denied
```
**Solution**: `chmod +x test_sessions.sh test_cookies.sh run_all_tests.sh`

## Test Coverage

### Session Management (10 tests)
- ✓ Session creation and ID generation
- ✓ Cookie-based authentication
- ✓ Session validation
- ✓ Multiple concurrent sessions
- ✓ Invalid session rejection
- ✓ Session persistence
- ✓ Logout and cleanup
- ✓ Post-logout validation

### Cookie Handling (15 tests)
- ✓ Set-Cookie header generation
- ✓ Cookie parsing and storage
- ✓ Multiple cookies
- ✓ Cookie attributes (Path, Domain, Expires)
- ✓ Special character encoding
- ✓ Empty/large values
- ✓ Case sensitivity
- ✓ Concurrent operations

## Integration with CI/CD

These tests can be integrated into your CI/CD pipeline:

```bash
# Start server
./webserv config/webserv.conf &
SERVER_PID=$!

# Wait for server to be ready
sleep 2

# Run tests
./tests/session_cookie/run_all_tests.sh

# Capture exit code
TEST_RESULT=$?

# Cleanup
kill $SERVER_PID

# Exit with test result
exit $TEST_RESULT
```

## Contributing

When adding new tests:
1. Follow the existing test structure
2. Use descriptive test names
3. Include both positive and negative test cases
4. Update this README with new test descriptions
5. Ensure tests clean up after themselves

## License

Part of the WebServ project - 42 School
