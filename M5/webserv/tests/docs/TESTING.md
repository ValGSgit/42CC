# WebServ Testing Suite

A comprehensive testing suite for the WebServ HTTP server project, designed to test all mandatory features from the subject requirements.

## 📋 Test Scripts Overview

### 1. **test_webserv.py** - Main Functionality Tests
Comprehensive test suite covering all HTTP server requirements:

**HTTP Methods:**
- ✓ GET requests
- ✓ POST requests with body
- ✓ DELETE requests
- ✓ Method not allowed (405) for unsupported methods

**Static File Serving:**
- ✓ HTML file serving
- ✓ Directory listing (autoindex)
- ✓ Content-Type headers
- ✓ Image serving (JPEG)

**Status Codes:**
- ✓ 200 OK
- ✓ 404 Not Found
- ✓ 405 Method Not Allowed
- ✓ 413 Payload Too Large
- ✓ Custom error pages

**CGI Support:**
- ✓ CGI GET requests with query strings
- ✓ CGI POST requests with body
- ✓ Environment variables (REQUEST_METHOD, QUERY_STRING, etc.)

**File Upload:**
- ✓ Multipart form data uploads
- ✓ Large file uploads (1MB+)
- ✓ Upload path configuration

**Advanced Features:**
- ✓ HTTP/1.0 and HTTP/1.1 support
- ✓ Keep-alive connections
- ✓ Chunked transfer encoding
- ✓ HTTP redirections (301, 302, etc.)
- ✓ Connection timeout handling
- ✓ Client body size limits

**Edge Cases:**
- ✓ Malformed requests
- ✓ Missing headers
- ✓ Very long URIs
- ✓ URL encoding
- ✓ Special characters
- ✓ Empty requests

### 2. **stress_test.py** - Load and Stability Tests
Stress testing to ensure server remains operational under heavy load:

**Tests Included:**
- ✓ **Concurrent Requests**: 100 threads, 10 requests each (1000 total)
- ✓ **Sustained Load**: Continuous requests over 30-60 seconds
- ✓ **Connection Reuse**: Multiple requests on same keep-alive connection
- ✓ **Large Payloads**: 5MB+ POST requests
- ✓ **Rapid Connect/Disconnect**: 500+ quick connections
- ✓ **Mixed Workload**: Random GET/POST/DELETE requests

**Metrics Tracked:**
- Success rate
- Failure rate
- Timeout rate
- Requests per second
- Response times

### 3. **test_cgi.py** - CGI-Specific Tests
Detailed CGI functionality testing:

**Environment Variables:**
- ✓ REQUEST_METHOD
- ✓ QUERY_STRING
- ✓ CONTENT_TYPE
- ✓ CONTENT_LENGTH
- ✓ REQUEST_URI

**CGI Features:**
- ✓ GET with query strings
- ✓ POST with body data
- ✓ STDIN data passing
- ✓ Large POST bodies
- ✓ URL encoding in queries
- ✓ Empty query strings
- ✓ Response without Content-Length
- ✓ Timeout handling

## 🚀 Usage

### Prerequisites
```bash
# Python 3.6+ required
python3 --version

# Make sure your server is compiled
make

# Start your server
./webserv config/default.conf
```

### Running Tests

#### 1. Main Functionality Tests
```bash
# Basic usage (default: localhost:8080)
python3 test_webserv.py

# Custom host/port
python3 test_webserv.py --host localhost --port 8080

# With specific config
python3 test_webserv.py --config ./config/default.conf
```

#### 2. Stress Tests
```bash
# Run all stress tests
python3 stress_test.py

# Custom configuration
python3 stress_test.py --host localhost --port 8080
```

#### 3. CGI Tests
```bash
# Run CGI-specific tests
python3 test_cgi.py

# Custom server
python3 test_cgi.py --host localhost --port 8080
```

#### 4. Run All Tests
```bash
# Make scripts executable
chmod +x test_webserv.py stress_test.py test_cgi.py

# Run everything
./test_webserv.py && ./stress_test.py && ./test_cgi.py
```

## 📊 Understanding Test Output

### Test Status Indicators
- 🟢 **PASS**: Test passed successfully
- 🔴 **FAIL**: Test failed (requires attention)
- 🟡 **WARN**: Warning (may not be critical)

### Example Output
```
Testing GET Request
✓ PASS: GET /
  Status: 200

Testing POST Request
✓ PASS: POST with body
  Status: 200

Testing 404 Not Found
✓ PASS: 404 Not Found
  Status: 404

========================================================
TEST SUMMARY
========================================================
Total Tests: 45
Passed: 43
Failed: 2
Warnings: 3
Success Rate: 95.6%
========================================================
```

## 🎯 Test Coverage

### HTTP Methods (Required by Subject)
- [x] GET
- [x] POST
- [x] DELETE

### Configuration Features
- [x] Multiple ports
- [x] Server names
- [x] Root directories
- [x] Index files
- [x] Autoindex (directory listing)
- [x] Client max body size
- [x] Error pages
- [x] Route-specific methods
- [x] HTTP redirections
- [x] Upload directories
- [x] CGI execution

### Server Requirements
- [x] Non-blocking I/O
- [x] Single poll/select/epoll/kqueue
- [x] Client disconnection handling
- [x] Connection timeout (60s)
- [x] Multiple simultaneous connections
- [x] Never hang indefinitely

### CGI Requirements
- [x] Environment variable passing
- [x] STDIN for request body
- [x] EOF handling
- [x] Chunked request un-chunking
- [x] Correct working directory
- [x] Multiple CGI extensions (.php, .py, .pl, etc.)

## 🔧 Troubleshooting

### Common Issues

**1. "Server not running" error**
```bash
# Make sure server is started
./webserv config/default.conf

# Check if port is already in use
lsof -i :8080
```

**2. CGI tests failing**
```bash
# Make sure CGI scripts are executable
chmod +x www/cgi-bin/*.py
chmod +x www/cgi-bin/*.pl
chmod +x www/cgi-bin/*.sh

# Check Python interpreter
which python3
```

**3. Upload tests failing**
```bash
# Ensure upload directory exists and is writable
mkdir -p www/uploads
chmod 755 www/uploads
```

**4. Timeout errors**
```bash
# Server might be overloaded, try reducing concurrent connections
# Or increase timeout in test script
```

## 📝 Creating Custom Tests

You can easily extend the test suite:

```python
#!/usr/bin/env python3
import socket

def custom_test():
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(('localhost', 8080))
    
    request = "GET /custom HTTP/1.1\r\nHost: localhost\r\n\r\n"
    sock.sendall(request.encode())
    
    response = sock.recv(4096)
    sock.close()
    
    print(response.decode())

if __name__ == "__main__":
    custom_test()
```

## 🎓 Subject Requirements Checklist

### Mandatory Requirements
- [x] Configuration file argument
- [x] Cannot execve another web server
- [x] Non-blocking at all times
- [x] Single poll/select for all I/O
- [x] Client disconnection handling
- [x] No errno checking after read/write
- [x] Request never hangs indefinitely
- [x] Compatible with standard browsers
- [x] Accurate HTTP status codes
- [x] Default error pages
- [x] Fork only for CGI
- [x] Serve static website
- [x] File upload capability
- [x] GET, POST, DELETE methods
- [x] Multiple ports support

### Configuration File Features
- [x] Multiple interface:port pairs
- [x] Default error pages setup
- [x] Max body size limit
- [x] Per-route configuration:
  - [x] Accepted methods
  - [x] HTTP redirects
  - [x] Root directory
  - [x] Directory listing
  - [x] Default file
  - [x] Upload location
  - [x] CGI execution

## 💡 Testing Best Practices

1. **Start Fresh**: Restart server between major test runs
2. **Check Logs**: Monitor server output for errors
3. **Use Valgrind**: Check for memory leaks
   ```bash
   valgrind --leak-check=full ./webserv config/default.conf
   ```
4. **Compare with NGINX**: Run same tests against NGINX for reference
5. **Browser Testing**: Also test with real browsers (Firefox, Chrome, curl)
6. **File Permissions**: Ensure correct permissions on www/ directory

## 🔍 Comparing with NGINX

To compare behavior with NGINX:

```bash
# Install NGINX (if not installed)
sudo apt-get install nginx  # Ubuntu/Debian

# Create similar config for NGINX
# Run tests against both servers
python3 test_webserv.py --port 8080  # Your server
python3 test_webserv.py --port 80     # NGINX
```

## 📚 Additional Resources

- **HTTP/1.1 RFC**: https://tools.ietf.org/html/rfc2616
- **HTTP/1.0 RFC**: https://tools.ietf.org/html/rfc1945
- **CGI RFC**: https://tools.ietf.org/html/rfc3CGI
- **NGINX Config**: http://nginx.org/en/docs/

## 🐛 Reporting Issues

If tests reveal bugs in your server:
1. Note the exact test that failed
2. Review test output and error messages
3. Check server logs
4. Use debugging tools (gdb, valgrind)
5. Compare with NGINX behavior

## 📄 License

This testing suite is provided as-is for educational purposes as part of the WebServ project.

---

**Good luck with your WebServ project! 🚀**
