# 🚀 WebServ Siege Testing Suite

Comprehensive HTTP server load testing and validation suite for WebServ.

## 📋 Quick Start

```bash
cd tests/siege

# Run all tests
./test_siege.sh

# Run specific test
./test_siege.sh status    # Test status codes only
./test_siege.sh cgi       # Test CGI execution only
./test_siege.sh quick     # Quick performance benchmark
```

## 🧪 Available Tests

### Status Code Validation (`./test_siege.sh status`)
Tests all HTTP status codes your server should return:
- ✅ **200 OK** - Static files, CGI scripts, directory listings
- ✅ **301 Redirect** - Redirection endpoints
- ✅ **404 Not Found** - Non-existent resources
- ✅ **405 Method Not Allowed** - Invalid HTTP methods

### CGI Execution Testing (`./test_siege.sh cgi`)
Comprehensive CGI script testing:
- Python scripts (`.py`)
- Shell scripts (`.sh`)
- PHP scripts (`.php`)
- Perl scripts (`.pl`)
- Ruby scripts (`.rb`)
- Tests with GET, POST, and query parameters

### Timeout & Connection Testing (`./test_siege.sh timeout`)
Connection handling validation:
- Keep-alive connections
- Sequential requests
- Rapid concurrent connections
- Long-running scripts

### Performance Benchmarks
- `quick` - 10 users, 10 seconds
- `concurrent` - 25 users, 100 requests
- `sustained` - 50 users, 30 seconds
- `heavy` - 100 users, 5000 requests
- `static` - Static file serving performance
- `mixed` - Mix of GET/POST/DELETE requests
- `internet` - Real-world simulation with delays
- `ports` - All ports (8080, 8081, 8082)

## 📊 What the Output Shows

Siege provides detailed metrics:

```
Transactions:              1000 hits
Availability:            100.00 %     ← Should be close to 100%
Elapsed time:              29.45 secs
Data transferred:           5.23 MB
Response time:              0.25 secs  ← Lower is better
Transaction rate:          33.95 trans/sec
Throughput:                 0.18 MB/sec
Concurrency:                8.49
Successful transactions:    1000       ← Should match Transactions
Failed transactions:           0       ← Should be 0
Longest transaction:        1.23
Shortest transaction:       0.01
```

### Key Metrics to Watch:
- **Availability**: Should be ≥99% (100% is perfect)
- **Failed transactions**: Should be 0
- **Response time**: Lower is better (<1s is excellent)
- **Transaction rate**: Higher is better
- **Concurrency**: How many simultaneous connections

## 🎯 Test Coverage

### URLs Tested (170+ different requests):
- ✅ Static HTML pages (10+)
- ✅ Directory listings with autoindex
- ✅ CGI scripts (Python, Shell, PHP, Perl, Ruby)
- ✅ CGI with query strings
- ✅ CGI with POST data
- ✅ File uploads
- ✅ Redirects (301)
- ✅ Error pages (404, 405, etc.)
- ✅ Multiple ports (8080, 8081, 8082)
- ✅ API endpoints
- ✅ Documentation pages
- ✅ Various HTTP methods

### Status Codes Validated:
| Code | Description | Test Cases |
|------|-------------|------------|
| 200 | OK | Static files, CGI, directory listing |
| 301 | Moved Permanently | Redirect endpoints |
| 404 | Not Found | Non-existent files |
| 405 | Method Not Allowed | Invalid methods |

## 🔧 Custom Testing

Run custom siege commands:

```bash
# 200 users, 100 requests each (20,000 total)
./test_siege.sh custom -c 200 -r 100 http://localhost:8080/

# 5 minute sustained test with 50 users
./test_siege.sh custom -c 50 -t 5M http://localhost:8080/

# Verbose output with 20 users
./test_siege.sh custom -c 20 -r 10 -v http://localhost:8080/

# Test specific URL file in internet mode (random)
./test_siege.sh custom -c 25 -t 30S -i -f siege_urls.txt
```

## 📝 URL File Format

The `siege_urls.txt` file contains all test URLs:

```
# GET request
http://localhost:8080/index.html

# POST request with data
http://localhost:8080/cgi-bin/test.py POST name=value&key=data

# DELETE request
http://localhost:8080/upload/file.txt DELETE
```

## 🐛 Troubleshooting

### No transactions completed
- ❌ Server not running → Start with `./webserv webserv.conf`
- ❌ Wrong port → Check your config file

### Low availability (<95%)
- ❌ Server crashing → Check server logs
- ❌ Timeout issues → Check connection handling
- ❌ Resource limits → Increase file descriptors

### Failed transactions
- ❌ Check server error logs
- ❌ Verify CGI scripts are executable
- ❌ Ensure all test files exist

### Slow response times
- ❌ Heavy CGI processing → Optimize scripts
- ❌ Large files → Check file sizes
- ❌ Too many concurrent users → Reduce -c parameter

## 🎓 Understanding Results

### Good Results ✅
```
Availability:            100.00 %
Failed transactions:           0
Response time:              0.15 secs
```

### Problems ❌
```
Availability:             85.00 %  ← Too low
Failed transactions:          50  ← Should be 0
Response time:              5.23 secs  ← Too slow
```

## 📚 Siege Command Reference

| Option | Description | Example |
|--------|-------------|---------|
| `-c N` | Concurrent users | `-c 50` (50 users) |
| `-r N` | Repetitions per user | `-r 10` (10 times) |
| `-t TIME` | Duration | `-t 30S` (30 seconds) |
| `-f FILE` | URL file | `-f urls.txt` |
| `-i` | Internet mode (random) | `-i -f urls.txt` |
| `-v` | Verbose output | `-v` |
| `-b` | Benchmark (no delays) | `-b` |
| `-d N` | Delay between requests | `-d 2` (2 seconds) |
| `--delay=N` | Random delay 0-N | `--delay=1` |

## 🎯 Testing Strategy

1. **Start Simple**: Run `./test_siege.sh quick` first
2. **Validate Status Codes**: Run `./test_siege.sh status`
3. **Test CGI**: Run `./test_siege.sh cgi`
4. **Comprehensive Test**: Run `./test_siege.sh all`
5. **Stress Test**: Run `./test_siege.sh heavy` (when ready)

## 📈 Performance Goals

Target metrics for a good webserv:
- ✅ Availability: ≥99%
- ✅ Response time: <1 second
- ✅ Transaction rate: >30/sec
- ✅ Failed transactions: 0
- ✅ Handles 50+ concurrent users

## 🔍 Detailed Monitoring

While tests run, monitor your server in another terminal:

```bash
# Watch server output
./webserv webserv.conf

# Monitor system resources
htop

# Monitor network connections
watch -n 1 'netstat -an | grep 8080'

# Count active connections
watch -n 1 'netstat -an | grep 8080 | wc -l'
```

## ✨ Tips

1. **Run tests multiple times** - Results can vary
2. **Check all ports** - Test 8080, 8081, 8082
3. **Monitor logs** - Watch for errors during tests
4. **Start small** - Begin with quick test, then scale up
5. **Verify results** - 100% availability should be achievable
6. **Test CGI separately** - CGI can be slower
7. **Use -v flag** - See individual request details

---

**Happy Testing! 🎉**

Made with ❤️ for WebServ
