#!/usr/bin/env python3
"""
PERFORMANCE AND LOAD TESTER FOR WEBSERV
Tests server performance, concurrency, and resource limits
"""

import socket
import time
import sys
import threading
import statistics
from concurrent.futures import ThreadPoolExecutor, as_completed
from typing import List, Dict, Tuple
import json

class Colors:
    GREEN = '\033[92m'
    RED = '\033[91m'
    YELLOW = '\033[93m'
    BLUE = '\033[94m'
    MAGENTA = '\033[95m'
    CYAN = '\033[96m'
    RESET = '\033[0m'
    BOLD = '\033[1m'
    DIM = '\033[2m'

class PerformanceTester:
    def __init__(self, host='localhost', port=8080):
        self.host = host
        self.port = port
        self.results = []
        
    def send_single_request(self, request: str, timeout=5.0) -> Dict:
        """Send a single request and measure performance"""
        start_time = time.time()
        
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.settimeout(timeout)
            
            connect_start = time.time()
            sock.connect((self.host, self.port))
            connect_time = time.time() - connect_start
            
            send_start = time.time()
            sock.sendall(request.encode())
            send_time = time.time() - send_start
            
            response_start = time.time()
            response = b""
            while True:
                try:
                    chunk = sock.recv(8192)
                    if not chunk:
                        break
                    response += chunk
                    if len(response) > 1024*1024:  # 1MB limit
                        break
                    if b'\r\n\r\n' in response and len(response) > 100:
                        break
                except socket.timeout:
                    break
            
            response_time = time.time() - response_start
            sock.close()
            
            total_time = time.time() - start_time
            
            # Parse status code
            status_code = None
            if response:
                try:
                    status_line = response.decode('utf-8', errors='ignore').split('\n')[0]
                    parts = status_line.split()
                    if len(parts) >= 2 and parts[1].isdigit():
                        status_code = int(parts[1])
                except:
                    pass
            
            return {
                'success': True,
                'status_code': status_code,
                'total_time': total_time,
                'connect_time': connect_time,
                'send_time': send_time,
                'response_time': response_time,
                'response_size': len(response)
            }
            
        except Exception as e:
            return {
                'success': False,
                'error': str(e),
                'total_time': time.time() - start_time
            }
    
    def concurrent_requests_test(self, num_requests=100, num_threads=10):
        """Test concurrent request handling"""
        print(f"\n{Colors.BOLD}Testing Concurrent Requests:{Colors.RESET}")
        print(f"Requests: {num_requests}, Threads: {num_threads}")
        
        request = "GET / HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n"
        results = []
        
        start_time = time.time()
        
        with ThreadPoolExecutor(max_workers=num_threads) as executor:
            futures = [executor.submit(self.send_single_request, request) for _ in range(num_requests)]
            
            for future in as_completed(futures):
                results.append(future.result())
        
        end_time = time.time()
        duration = end_time - start_time
        
        # Analyze results
        successful = [r for r in results if r['success']]
        failed = [r for r in results if not r['success']]
        
        if successful:
            times = [r['total_time'] for r in successful]
            avg_time = statistics.mean(times)
            min_time = min(times)
            max_time = max(times)
            median_time = statistics.median(times)
            
            print(f"\n{Colors.GREEN}Success Rate: {len(successful)}/{num_requests} ({len(successful)/num_requests*100:.1f}%){Colors.RESET}")
            print(f"Total Duration: {duration:.2f}s")
            print(f"Requests/sec: {num_requests/duration:.2f}")
            print(f"\nResponse Times:")
            print(f"  Average: {avg_time*1000:.2f}ms")
            print(f"  Median:  {median_time*1000:.2f}ms")
            print(f"  Min:     {min_time*1000:.2f}ms")
            print(f"  Max:     {max_time*1000:.2f}ms")
            
            if len(times) > 1:
                stdev = statistics.stdev(times)
                print(f"  StdDev:  {stdev*1000:.2f}ms")
        
        if failed:
            print(f"\n{Colors.RED}Failed Requests: {len(failed)}{Colors.RESET}")
            error_types = {}
            for f in failed:
                error = f.get('error', 'Unknown')
                error_types[error] = error_types.get(error, 0) + 1
            for error, count in error_types.items():
                print(f"  {error}: {count}")
        
        return successful, failed
    
    def stress_test(self, duration_seconds=10):
        """Continuous stress test for specified duration"""
        print(f"\n{Colors.BOLD}Stress Test:{Colors.RESET}")
        print(f"Duration: {duration_seconds}s")
        
        request = "GET / HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n"
        results = []
        start_time = time.time()
        count = 0
        
        while time.time() - start_time < duration_seconds:
            result = self.send_single_request(request, timeout=2.0)
            results.append(result)
            count += 1
            
            if count % 10 == 0:
                elapsed = time.time() - start_time
                rate = count / elapsed
                print(f"\rProgress: {count} requests, {rate:.1f} req/s", end='')
        
        print()
        
        # Analyze results
        successful = [r for r in results if r['success']]
        failed = [r for r in results if not r['success']]
        
        print(f"\n{Colors.GREEN}Total Requests: {count}{Colors.RESET}")
        print(f"Successful: {len(successful)} ({len(successful)/count*100:.1f}%)")
        print(f"Failed: {len(failed)} ({len(failed)/count*100:.1f}%)")
        print(f"Average Rate: {count/duration_seconds:.2f} req/s")
        
        return successful, failed
    
    def keep_alive_test(self, num_requests=50):
        """Test persistent connection handling"""
        print(f"\n{Colors.BOLD}Keep-Alive Test:{Colors.RESET}")
        print(f"Sending {num_requests} sequential requests on single connection")
        
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.settimeout(10.0)
            sock.connect((self.host, self.port))
            
            successful = 0
            failed = 0
            times = []
            
            for i in range(num_requests):
                request = f"GET /index.html HTTP/1.1\r\nHost: localhost\r\nConnection: keep-alive\r\n\r\n"
                
                start = time.time()
                try:
                    sock.sendall(request.encode())
                    
                    response = b""
                    while b'\r\n\r\n' not in response:
                        chunk = sock.recv(4096)
                        if not chunk:
                            break
                        response += chunk
                    
                    elapsed = time.time() - start
                    times.append(elapsed)
                    
                    if response:
                        successful += 1
                    else:
                        failed += 1
                        break
                        
                except Exception as e:
                    failed += 1
                    print(f"\nError on request {i+1}: {e}")
                    break
            
            sock.close()
            
            print(f"\n{Colors.GREEN}Successful: {successful}/{num_requests}{Colors.RESET}")
            if failed:
                print(f"{Colors.RED}Failed: {failed}{Colors.RESET}")
            
            if times:
                print(f"Average time: {statistics.mean(times)*1000:.2f}ms")
                print(f"Min time: {min(times)*1000:.2f}ms")
                print(f"Max time: {max(times)*1000:.2f}ms")
            
        except Exception as e:
            print(f"{Colors.RED}Keep-alive test failed: {e}{Colors.RESET}")
    
    def large_file_test(self):
        """Test large file upload/download"""
        print(f"\n{Colors.BOLD}Large File Transfer Test:{Colors.RESET}")
        
        # Test uploads of various sizes
        sizes = [1024, 10*1024, 100*1024, 1024*1024]  # 1KB to 1MB
        
        for size in sizes:
            data = "x" * size
            request = f"POST /uploads/ HTTP/1.1\r\nHost: localhost\r\nContent-Type: application/octet-stream\r\nContent-Length: {size}\r\n\r\n{data}"
            
            start = time.time()
            result = self.send_single_request(request, timeout=30.0)
            elapsed = time.time() - start
            
            if result['success']:
                rate = size / elapsed / 1024  # KB/s
                print(f"Upload {size:>10} bytes: {elapsed:6.2f}s ({rate:8.2f} KB/s) - Status {result.get('status_code', 'N/A')}")
            else:
                print(f"Upload {size:>10} bytes: {Colors.RED}FAILED{Colors.RESET} - {result.get('error', 'Unknown error')}")
    
    def method_variety_test(self):
        """Test various HTTP methods"""
        print(f"\n{Colors.BOLD}HTTP Methods Test:{Colors.RESET}")
        
        methods = [
            ("GET", "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n", [200]),
            ("POST", "POST /uploads/ HTTP/1.1\r\nHost: localhost\r\nContent-Length: 4\r\n\r\ntest", [200, 201, 204]),
            ("DELETE", "DELETE /uploads/test.txt HTTP/1.1\r\nHost: localhost\r\n\r\n", [200, 204, 404]),
            ("HEAD", "HEAD / HTTP/1.1\r\nHost: localhost\r\n\r\n", [200, 501]),
            ("PUT", "PUT /uploads/test.txt HTTP/1.1\r\nHost: localhost\r\nContent-Length: 4\r\n\r\ntest", [200, 201, 204, 405, 501]),
        ]
        
        for method, request, expected_codes in methods:
            result = self.send_single_request(request)
            status = result.get('status_code')
            
            if result['success'] and status in expected_codes:
                print(f"{Colors.GREEN}✓{Colors.RESET} {method:7} - Status {status} ({result['total_time']*1000:.2f}ms)")
            elif result['success']:
                print(f"{Colors.YELLOW}~{Colors.RESET} {method:7} - Status {status} (expected one of {expected_codes})")
            else:
                print(f"{Colors.RED}✗{Colors.RESET} {method:7} - {result.get('error', 'Failed')}")
    
    def run_all_tests(self):
        """Run all performance tests"""
        print(f"\n{Colors.BOLD}{Colors.CYAN}╔{'═'*78}╗{Colors.RESET}")
        print(f"{Colors.BOLD}{Colors.CYAN}║{' '*20}WEBSERV PERFORMANCE & LOAD TESTER{' '*21}║{Colors.RESET}")
        print(f"{Colors.BOLD}{Colors.CYAN}╚{'═'*78}╝{Colors.RESET}\n")
        
        print(f"{Colors.YELLOW}Testing server at {self.host}:{self.port}{Colors.RESET}\n")
        
        # Check if server is running
        try:
            test_result = self.send_single_request("GET / HTTP/1.1\r\nHost: localhost\r\n\r\n", timeout=5.0)
            if not test_result['success']:
                print(f"{Colors.RED}ERROR: Cannot connect to server{Colors.RESET}")
                return 1
        except:
            print(f"{Colors.RED}ERROR: Cannot connect to server{Colors.RESET}")
            return 1
        
        print(f"{Colors.GREEN}✓ Server is running{Colors.RESET}\n")
        
        try:
            # Run tests
            self.method_variety_test()
            self.concurrent_requests_test(num_requests=50, num_threads=10)
            self.concurrent_requests_test(num_requests=100, num_threads=20)
            self.keep_alive_test(num_requests=30)
            self.large_file_test()
            self.stress_test(duration_seconds=10)
            
            print(f"\n{Colors.BOLD}{Colors.GREEN}All performance tests completed!{Colors.RESET}\n")
            
        except KeyboardInterrupt:
            print(f"\n{Colors.YELLOW}Tests interrupted by user{Colors.RESET}")
            return 1
        except Exception as e:
            print(f"\n{Colors.RED}Error during testing: {e}{Colors.RESET}")
            import traceback
            traceback.print_exc()
            return 1
        
        return 0

def main():
    import argparse
    
    parser = argparse.ArgumentParser(description='Performance and Load Tester for WebServ')
    parser.add_argument('--host', default='localhost', help='Server host')
    parser.add_argument('--port', type=int, default=8080, help='Server port')
    
    args = parser.parse_args()
    
    tester = PerformanceTester(args.host, args.port)
    exit_code = tester.run_all_tests()
    sys.exit(exit_code)

if __name__ == '__main__':
    main()
