# Siege Testing Guide for WebServ

## What is Siege?

Siege is an HTTP load testing and benchmarking utility. It's designed to test a web server's performance under stress, measuring:
- **Transactions**: Total number of successful requests
- **Availability**: Percentage of successful requests (uptime)
- **Response time**: Average time to receive responses
- **Transaction rate**: Requests per second
- **Throughput**: Data transferred per second
- **Concurrency**: Average number of simultaneous connections

## Quick Start

### 1. Start Your Server
```bash
./webserv config/default.conf
```

### 2. Run Siege Tests
```bash
# Make the script executable
chmod +x test_siege.sh

# Run all tests
./test_siege.sh

# Run specific test
./test_siege.sh quick
./test_siege.sh sustained
./test_siege.sh heavy
```

## Test Suite Breakdown

### Test 1: Quick Benchmark (10s, 10 users)
```bash
./test_siege.sh quick
```
- **Purpose**: Quick health check
- **Users**: 10 concurrent
- **Duration**: 10 seconds
- **Good for**: Basic performance verification

### Test 2: Concurrent Users (25 users, 100 requests)
```bash
./test_siege.sh concurrent
```
- **Purpose**: Test concurrent connection handling
- **Users**: 25 concurrent
- **Requests**: 100 total (4 per user)
- **Good for**: Testing connection pool limits

### Test 3: Multiple URLs
```bash
./test_siege.sh urls
```
- **Purpose**: Test various endpoints
- **Users**: 10 concurrent
- **Requests**: 5 per user per URL
- **URLs**: From `siege_urls.txt`
- **Good for**: Mixed workload testing

### Test 4: Sustained Load (30s, 20 users)
```bash
./test_siege.sh sustained
```
- **Purpose**: Test performance over time
- **Users**: 20 concurrent
- **Duration**: 30 seconds
- **Good for**: Finding memory leaks, stability issues

### Test 5: Heavy Load (50 users, 50 requests each)
```bash
./test_siege.sh heavy
```
- **Purpose**: Stress test
- **Users**: 50 concurrent
- **Requests**: 2,500 total
- **Good for**: Breaking point analysis

### Test 6: Static File Performance
```bash
./test_siege.sh static
```
- **Purpose**: Test static file serving
- **Users**: 15 concurrent
- **Duration**: 15 seconds
- **Good for**: File I/O performance

### Test 7: CGI Performance
```bash
./test_siege.sh cgi
```
- **Purpose**: Test CGI script execution
- **Users**: 5 concurrent (lower due to CGI overhead)
- **Requests**: 10 per user
- **Good for**: CGI handler performance

### Test 8: Internet Simulation
```bash
./test_siege.sh internet
```
- **Purpose**: Simulate real-world conditions
- **Users**: 10 concurrent
- **Duration**: 15 seconds
- **Delay**: 0.5s random delay between requests
- **Good for**: Realistic performance testing

## Manual Siege Commands

### Basic Commands

```bash
# Simple test: 10 users, 10 requests each
siege -c 10 -r 10 http://localhost:8080/

# Timed test: 10 users for 30 seconds
siege -c 10 -t 30S http://localhost:8080/

# Test with URL file
siege -c 10 -r 5 -f siege_urls.txt

# Benchmark mode (no delays, maximum speed)
siege -c 50 -r 100 -b http://localhost:8080/

# Verbose output
siege -c 10 -r 5 -v http://localhost:8080/

# Internet mode (random URLs from file)
siege -c 10 -t 30S -i -f siege_urls.txt
```

### Advanced Commands

```bash
# Test with delays (simulate real users)
siege -c 20 -t 60S --delay=1 http://localhost:8080/

# Specific HTTP method
siege -c 5 -r 10 "http://localhost:8080/uploads/test.txt DELETE"

# POST request
siege -c 5 -r 10 "http://localhost:8080/cgi-bin/test.py POST name=John&age=30"

# With custom headers
siege -c 10 -r 5 --header="Authorization: Bearer token123" http://localhost:8080/

# Log results to file
siege -c 10 -t 30S http://localhost:8080/ > siege_results.txt 2>&1

# Test until failure
siege -c 100 -r 1000 http://localhost:8080/
```

## Siege Options Reference

| Option | Description | Example |
|--------|-------------|---------|
| `-c NUM` | Number of concurrent users | `-c 25` |
| `-r NUM` | Repetitions (requests per user) | `-r 10` |
| `-t TIME` | Time to run (S/M/H) | `-t 30S` |
| `-f FILE` | URL file | `-f urls.txt` |
| `-d NUM` | Delay between requests (sec) | `-d 1` |
| `-b` | Benchmark mode (no delays) | `-b` |
| `-i` | Internet mode (random URLs) | `-i` |
| `-v` | Verbose output | `-v` |
| `-g URL` | GET request (default) | `-g` |
| `-p URL` | POST request | `-p` |
| `--header` | Add HTTP header | `--header="Host: example.com"` |
| `--content-type` | Set Content-Type | `--content-type="application/json"` |

## Understanding Siege Output

```
Transactions:                   1247 hits
Availability:                 100.00 %
Elapsed time:                  29.87 secs
Data transferred:               2.34 MB
Response time:                  0.23 secs
Transaction rate:              41.75 trans/sec
Throughput:                     0.08 MB/sec
Concurrency:                    9.73
Successful transactions:        1247
Failed transactions:               0
Longest transaction:            0.89
Shortest transaction:           0.01
```

### Metrics Explained:

- **Transactions**: Total successful HTTP requests
- **Availability**: `(successful / total) * 100`
  - 100% = Perfect (no errors)
  - <100% = Some requests failed
- **Elapsed time**: Total test duration
- **Data transferred**: Total bytes received
- **Response time**: Average time per request
  - Lower is better
  - <0.5s = Excellent
  - 0.5-1s = Good
  - 1-2s = Acceptable
  - >2s = Needs optimization
- **Transaction rate**: Requests per second
  - Higher is better
  - >100 = Excellent
  - 50-100 = Good
  - 20-50 = Acceptable
  - <20 = Needs optimization
- **Throughput**: MB per second
- **Concurrency**: Average simultaneous connections
  - Should be close to `-c` value
  - Much lower = bottleneck
- **Longest/Shortest transaction**: Min/max response times

## Performance Benchmarks

### Excellent Performance
- Availability: 100%
- Response time: <0.5s
- Transaction rate: >100 req/s
- Concurrency: ~90% of requested users

### Good Performance
- Availability: >99%
- Response time: 0.5-1s
- Transaction rate: 50-100 req/s
- Concurrency: ~75% of requested users

### Needs Improvement
- Availability: <99%
- Response time: >2s
- Transaction rate: <20 req/s
- Concurrency: <50% of requested users

## Customizing URL File

Edit `siege_urls.txt` to test your specific endpoints:

```bash
# GET requests
http://localhost:8080/
http://localhost:8080/index.html
http://localhost:8080/uploads/test.txt

# POST requests
http://localhost:8080/cgi-bin/test.py POST name=John&age=30
http://localhost:8080/cgi-bin/test.py POST data=value

# DELETE requests
http://localhost:8080/uploads/test.txt DELETE

# Different ports
http://localhost:8081/
http://localhost:8082/
```

## Troubleshooting

### Issue: "siege: could not open file"
**Solution**: Create the URL file:
```bash
echo "http://localhost:8080/" > siege_urls.txt
```

### Issue: "Connection refused"
**Solution**: Make sure your server is running:
```bash
./webserv config/default.conf
```

### Issue: Low availability (<100%)
**Possible causes**:
- Server crashes under load
- Connection limit reached
- Request timeout
- Method not allowed (405 errors)

**Debug**:
```bash
# Run with verbose output
siege -c 10 -r 5 -v http://localhost:8080/

# Check server logs
# Look for error patterns
```

### Issue: Very slow response times
**Possible causes**:
- CPU bottleneck
- Memory issues
- Slow CGI scripts
- Inefficient file I/O

**Debug**:
```bash
# Monitor server resources
top -p $(pgrep webserv)

# Test with static files only
siege -c 10 -t 10S http://localhost:8080/index.html
```

### Issue: Failed transactions
**Debug**:
```bash
# Run single request to see error
curl -v http://localhost:8080/

# Check specific URL
siege -c 1 -r 1 -v http://localhost:8080/problematic-url
```

## Comparing with Your Python Tests

| Aspect | Python Test Suite | Siege |
|--------|------------------|-------|
| **Purpose** | Functional testing | Load/performance testing |
| **Tests** | Correctness of responses | Server under stress |
| **Output** | Pass/fail per feature | Performance metrics |
| **Use case** | Development/debugging | Benchmarking |
| **Best for** | Finding bugs | Finding bottlenecks |

**Recommended workflow**:
1. Run Python tests first (`./run_all_tests.sh`) - Fix any failures
2. Run siege tests - Check performance
3. Optimize based on siege results
4. Re-run both test suites

## Integration with Your Test Suite

Add to your `run_all_tests.sh`:

```bash
# Run functional tests
echo "Running functional tests..."
./test_webserv.py

# Run performance tests
echo "Running performance tests..."
./test_siege.sh quick
```

## Performance Tips for WebServ

Based on siege results, optimize:

1. **Low transaction rate**:
   - Use non-blocking I/O (epoll/kqueue)
   - Optimize request parsing
   - Reduce string copies

2. **High response time**:
   - Profile with `gprof` or `valgrind --tool=callgrind`
   - Cache file reads
   - Optimize CGI execution

3. **Memory issues**:
   - Check with `valgrind --leak-check=full`
   - Reuse buffers
   - Limit request body sizes

4. **Connection issues**:
   - Increase connection limit in config
   - Implement connection pooling
   - Fix keep-alive handling

## Siege Configuration File

Create `~/.siege/siege.conf` for default settings:

```bash
# Create config
mkdir -p ~/.siege
cat > ~/.siege/siege.conf << 'EOF'
# Siege configuration
verbose = false
color = on
quiet = false
show-logfile = true
logging = false
protocol = HTTP/1.1
chunked = true
cache = false
connection = close
concurrent = 25
time = 1M
EOF
```

## Resources

- Siege documentation: `man siege`
- Siege website: https://www.joedog.org/siege-home/
- Your test suite: `./test_webserv.py` (functional tests)
- This guide: `SIEGE_GUIDE.md`
- Quick reference: Run `./test_siege.sh help`

## Example Session

```bash
# 1. Start server
./webserv config/default.conf &

# 2. Quick test
./test_siege.sh quick

# 3. Check results, adjust server

# 4. Full test suite
./test_siege.sh all

# 5. Custom heavy test
./test_siege.sh custom -c 100 -r 50 -b

# 6. Stop server
killall webserv
```

Happy load testing! 🚀
