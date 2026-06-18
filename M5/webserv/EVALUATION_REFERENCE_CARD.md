# WebServ - Quick Evaluation Reference Card

## 🚀 STARTUP COMMANDS

```bash
# Compile with bonus features
make bonus

# Run with default config
./webserv

# Run with custom config
./webserv config/simple.conf

# Clean and recompile
make re

# Run with valgrind
make vg
```

---

## 🧪 QUICK MANUAL TESTS

### Browser Tests (Chrome/Firefox)
```
http://localhost:8080/                    # Home page
http://localhost:8080/cgi-demo.html       # CGI demo
http://localhost:8080/upload.html         # File upload
http://localhost:8080/browse              # Directory listing
http://localhost:8080/redirect            # Redirect test
http://localhost:8080/session-demo.html   # Sessions (bonus)
http://localhost:8080/cookies-demo.html   # Cookies (bonus)
http://localhost:8080/nonexistent.html    # 404 error page
```

### Curl Tests
```bash
# GET request
curl -v http://localhost:8080/

# POST request
curl -X POST -d "test=data" http://localhost:8080/cgi-bin/env.sh

# DELETE request
curl -X DELETE http://localhost:8080/uploads/test.txt

# OPTIONS request
curl -X OPTIONS -v http://localhost:8080/

# HEAD request
curl -I http://localhost:8080/

# File upload
curl -X POST -F "file=@test.txt" http://localhost:8080/uploads
```

---

## 📋 SUBJECT REQUIREMENTS CHECKLIST

### Configuration (IV.3)
- [x] Multiple ports (8080, 8081, 8082)
- [x] Server names
- [x] Default error pages
- [x] Client max body size
- [x] Allowed methods per route
- [x] HTTP redirections
- [x] Root directories
- [x] Directory listing
- [x] Default index files
- [x] File upload paths
- [x] CGI execution

### Non-Blocking I/O (IV.1)
- [x] Non-blocking sockets
- [x] Single epoll() instance
- [x] epoll for read AND write
- [x] No read/write without epoll
- [x] No errno after read/write
- [x] Regular files exempt from epoll

### HTTP Methods (IV.1)
- [x] GET - Static files, CGI
- [x] POST - Upload, CGI body
- [x] DELETE - File deletion
- [x] HEAD - Headers only
- [x] OPTIONS - Allowed methods

### Core Features
- [x] Serve static website
- [x] Client file upload
- [x] Accurate status codes
- [x] Custom error pages
- [x] Never hang indefinitely
- [x] Multiple ports
- [x] Stress tested

### CGI (IV.3)
- [x] Extension-based (.py, .pl, .rb, .php, .sh)
- [x] Environment variables (RFC 3875)
- [x] Request body via stdin
- [x] Response via stdout
- [x] Unchunk requests
- [x] EOF handling
- [x] Correct working directory
- [x] Timeout handling

### Fork Usage (IV.1)
- [x] Fork ONLY for CGI
- [x] No fork in main server

### Bonus
- [x] Session management
- [x] Cookie support
- [x] Multiple CGI types (5)

---

## 🔍 CODE EXPLANATION POINTS

### Architecture Overview
```
main.cpp              → Entry point, signal handling
ServerManager.cpp     → Main event loop (epoll)
HttpHandler.cpp       → Request processing
ConfigParser.cpp      → Nginx-style config
CgiHandler.cpp        → CGI execution
SessionManager.cpp    → Session/cookie (bonus)
```

### Epoll Event Loop
```cpp
// ServerManager::run()
while (_running) {
    nfds = epoll_wait(_epoll_fd, _events, MAX_CONNECTIONS, 1000);
    
    for (int i = 0; i < nfds; i++) {
        if (isServerSocket(fd))
            acceptConnection();
        else if (events[i].events & EPOLLIN)
            handleRead(fd);
        else if (events[i].events & EPOLLOUT)
            handleWrite(fd);
    }
}
```

### CGI Execution Flow
```cpp
1. fork() child process
2. Setup stdin/stdout pipes
3. Set environment variables
4. chdir() to script directory
5. execve() CGI interpreter
6. Parent reads stdout via epoll (timeout)
7. Parse CGI output (headers + body)
8. Return HttpResponse
```

### Configuration Inheritance
```
Server level:
  - root, index, autoindex, max_body_size
  ↓ (inherited by all routes)
Route level:
  - Can override any server setting
  - Specific to URL path
```

---

## 🐛 DEBUGGING COMMANDS

```bash
# Check server sockets
ss -tlnp | grep 808

# Monitor server output
./webserv webserv.conf 2>&1 | tee server.log

# Test CGI scripts directly
cd www/cgi-bin
QUERY_STRING="num1=10&num2=5&op=add" ./calculator.py

# Check for memory leaks
valgrind --leak-check=full ./webserv webserv.conf

# Stress test
python3 tests/scripts/stress_test.py --host localhost --port 8080

# Comprehensive tests
python3 tests/scripts/test_webserv.py
```

---

## 📝 COMMON QUESTIONS & ANSWERS

### Q: Why epoll instead of poll/select?
**A:** Subject allows "poll() or equivalent". Epoll is more efficient for large numbers of connections and is the recommended approach on Linux.

### Q: How do you handle errno?
**A:** We DON'T use errno to adjust server behavior after read/write. We only use strerror(errno) for error logging, which is allowed.

### Q: How does CGI work?
**A:** 
1. Fork child process
2. Redirect stdin/stdout to pipes
3. Set CGI environment variables
4. Execute CGI script with execve()
5. Parent reads output via epoll with timeout
6. Parse output and return to client

### Q: What if a client disconnects?
**A:** Epoll detects EPOLLHUP/EPOLLERR events, we clean up resources (close fd, erase from maps) in closeConnection().

### Q: How do you prevent hanging?
**A:** Every I/O operation has a timeout:
- Client read: Connection timeout
- CGI execution: 30s epoll_wait timeout
- Client write: Non-blocking write

### Q: Multiple ports/servers?
**A:** ServerManager creates one socket per ServerConfig, binds to different ports, all monitored by single epoll instance.

---

## ⚠️ POTENTIAL EVAL QUESTIONS

### "Show me the non-blocking implementation"
```cpp
// Utils::setNonBlocking()
int flags = fcntl(fd, F_GETFL, 0);
fcntl(fd, F_SETFL, flags | O_NONBLOCK);
```

### "Where do you check errno?"
```cpp
// Line 591 HttpHandler.cpp - LOGGING ONLY
std::cerr << "handleRead failed, " << strerror(errno) << "\n";
// NOT used for control flow - connection stays open
```

### "How do you handle chunked encoding?"
```cpp
// HttpRequest::parseRequest() detects Transfer-Encoding: chunked
// Automatically unchunks before passing to CGI
// CGI receives complete body via stdin
```

### "What if CGI times out?"
```cpp
// CgiHandler.cpp line 138
if (nfds == 0) {  // epoll_wait timeout
    kill(pid, SIGTERM);  // Kill CGI process
    return HTTP_INTERNAL_SERVER_ERROR;
}
```

### "How do you prevent directory traversal?"
```cpp
// Utils::isPathSecure()
std::string canonical_path = Utils::normalizePath(root + "/" + path);
if (canonical_path.find(root) != 0)
    return false;  // Path escapes root
```

---

## 🎯 EVALUATION STRATEGY

### 1. Demonstrate Core Features (10 min)
- Start server: `./webserv`
- Browse to localhost:8080
- Show static files, directory listing
- Upload a file
- Test CGI scripts (cgi-demo.html)
- Show error pages (404, 403, etc.)

### 2. Explain Architecture (10 min)
- Show config file structure
- Explain epoll event loop
- Walk through request flow
- Demonstrate CGI execution

### 3. Code Review (15 min)
- Point out non-blocking I/O
- Show errno (non)usage
- Explain CGI implementation
- Demonstrate fork usage

### 4. Bonus Features (5 min)
- Session demo
- Cookie demo
- Multiple CGI types

### 5. Q&A (10 min)
- Answer technical questions
- Explain design decisions
- Discuss edge cases

---

## 🏆 STRENGTHS TO HIGHLIGHT

1. **Clean C++98**: No warnings, proper RAII
2. **Epoll Architecture**: Efficient event-driven I/O
3. **RFC Compliance**: HTTP/1.1, CGI/1.1
4. **Comprehensive Testing**: Multiple test suites
5. **Error Handling**: Graceful degradation
6. **Security**: Path sanitization, timeouts
7. **Bonus Features**: Sessions, 5 CGI types
8. **Documentation**: Well commented

---

## 📊 EXPECTED QUESTIONS MATRIX

| Topic | Question | Answer File:Line |
|-------|----------|------------------|
| Non-blocking | "Show setNonBlocking" | Utils.cpp:1326 |
| Epoll | "Where's the event loop?" | ServerManager.cpp:173 |
| CGI | "How do you execute CGI?" | CgiHandler.cpp:11 |
| Config | "How do you parse config?" | ConfigParser.cpp:8 |
| Fork | "Where do you use fork?" | CgiHandler.cpp:50 |
| Errno | "Do you check errno?" | HttpHandler.cpp:591 (logging only) |
| Upload | "How do you handle uploads?" | HttpHandler.cpp:402 |
| Methods | "Show DELETE implementation" | HttpHandler.cpp:387 |

---

## ✅ PRE-EVALUATION CHECKLIST

- [ ] Compile: `make bonus` (no errors)
- [ ] Run: `./webserv` (starts successfully)
- [ ] Browser test: All pages load
- [ ] CGI test: All 5 scripts work
- [ ] Upload test: File uploads
- [ ] Error pages: 404, 403, 500 show correctly
- [ ] Memory check: `make vg` (no leaks)
- [ ] Code review: Understand all features
- [ ] Bonus: Sessions and cookies work

---

**Last Updated:** November 11, 2025  
**Status:** ✅ READY FOR EVALUATION  
**Confidence:** 95%
