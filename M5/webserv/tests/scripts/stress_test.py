#!/usr/bin/env python3
"""
Stress Test Suite for WebServ
Tests server stability under heavy load
"""

import socket
import threading
import time
import random
import string
import sys
from typing import List

class Colors:
    GREEN = '\033[92m'
    RED = '\033[91m'
    YELLOW = '\033[93m'
    BLUE = '\033[94m'
    CYAN = '\033[96m'
    RESET = '\033[0m'
    BOLD = '\033[1m'

class StressTester:
    def __init__(self, host: str = "localhost", port: int = 8080):
        self.host = host
        self.port = port
        self.success_count = 0
        self.failure_count = 0
        self.timeout_count = 0
        self.lock = threading.Lock()
    
    def make_request(self, path: str = "/", method: str = "GET", timeout: float = 5.0) -> bool:
        """Make a single HTTP request and return success status"""
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.settimeout(timeout)
            sock.connect((self.host, self.port))
            
            request = f"{method} {path} HTTP/1.1\r\nHost: {self.host}\r\n\r\n"
            sock.sendall(request.encode())
            
            response = sock.recv(4096)
            sock.close()
            
            if response and b"HTTP" in response:
                return True
            return False
            
        except socket.timeout:
            with self.lock:
                self.timeout_count += 1
            return False
        except Exception as e:
            return False
    
    def concurrent_requests_test(self, num_threads: int = 100, requests_per_thread: int = 10):
        """Test with many concurrent connections"""
        print(f"\n{Colors.BOLD}Concurrent Requests Test{Colors.RESET}")
        print(f"Threads: {num_threads}, Requests per thread: {requests_per_thread}")
        print(f"Total requests: {num_threads * requests_per_thread}")
        
        self.success_count = 0
        self.failure_count = 0
        self.timeout_count = 0
        
        def worker():
            for _ in range(requests_per_thread):
                if self.make_request():
                    with self.lock:
                        self.success_count += 1
                else:
                    with self.lock:
                        self.failure_count += 1
        
        start_time = time.time()
        
        threads = []
        for _ in range(num_threads):
            thread = threading.Thread(target=worker)
            thread.start()
            threads.append(thread)
        
        for thread in threads:
            thread.join()
        
        elapsed = time.time() - start_time
        total = self.success_count + self.failure_count
        
        print(f"\n{Colors.CYAN}Results:{Colors.RESET}")
        print(f"  Successful: {Colors.GREEN}{self.success_count}{Colors.RESET}")
        print(f"  Failed: {Colors.RED}{self.failure_count}{Colors.RESET}")
        print(f"  Timeouts: {Colors.YELLOW}{self.timeout_count}{Colors.RESET}")
        print(f"  Time: {elapsed:.2f}s")
        print(f"  Requests/sec: {total/elapsed:.2f}")
        
        success_rate = (self.success_count / total * 100) if total > 0 else 0
        print(f"  Success rate: {success_rate:.1f}%")
        
        if success_rate >= 95:
            print(f"{Colors.GREEN}✓ PASS{Colors.RESET}: Server handled concurrent load well")
            return True
        else:
            print(f"{Colors.RED}✗ FAIL{Colors.RESET}: High failure rate under load")
            return False
    
    def sustained_load_test(self, duration: int = 60, rps: int = 50):
        """Test sustained load over time"""
        print(f"\n{Colors.BOLD}Sustained Load Test{Colors.RESET}")
        print(f"Duration: {duration}s, Target rate: {rps} req/s")
        
        self.success_count = 0
        self.failure_count = 0
        self.timeout_count = 0
        running = True
        
        def worker():
            while running:
                if self.make_request(timeout=3.0):
                    with self.lock:
                        self.success_count += 1
                else:
                    with self.lock:
                        self.failure_count += 1
                time.sleep(1.0 / rps)
        
        start_time = time.time()
        
        threads = []
        for _ in range(10):  # Use 10 worker threads
            thread = threading.Thread(target=worker)
            thread.daemon = True
            thread.start()
            threads.append(thread)
        
        # Monitor for duration
        try:
            time.sleep(duration)
        except KeyboardInterrupt:
            print(f"\n{Colors.YELLOW}Test interrupted{Colors.RESET}")
        
        running = False
        time.sleep(2)  # Let threads finish
        
        elapsed = time.time() - start_time
        total = self.success_count + self.failure_count
        
        print(f"\n{Colors.CYAN}Results:{Colors.RESET}")
        print(f"  Successful: {Colors.GREEN}{self.success_count}{Colors.RESET}")
        print(f"  Failed: {Colors.RED}{self.failure_count}{Colors.RESET}")
        print(f"  Timeouts: {Colors.YELLOW}{self.timeout_count}{Colors.RESET}")
        print(f"  Actual time: {elapsed:.2f}s")
        print(f"  Actual rate: {total/elapsed:.2f} req/s")
        
        success_rate = (self.success_count / total * 100) if total > 0 else 0
        print(f"  Success rate: {success_rate:.1f}%")
        
        if success_rate >= 95:
            print(f"{Colors.GREEN}✓ PASS{Colors.RESET}: Server maintained stability")
            return True
        else:
            print(f"{Colors.RED}✗ FAIL{Colors.RESET}: Server struggled with sustained load")
            return False
    
    def connection_reuse_test(self, num_requests: int = 100):
        """Test connection reuse with keep-alive"""
        print(f"\n{Colors.BOLD}Connection Reuse Test{Colors.RESET}")
        print(f"Requests: {num_requests}")
        
        success = 0
        failures = 0
        
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.settimeout(5.0)
            sock.connect((self.host, self.port))
            
            for i in range(num_requests):
                request = f"GET /?req={i} HTTP/1.1\r\nHost: {self.host}\r\nConnection: keep-alive\r\n\r\n"
                sock.sendall(request.encode())
                
                # Read response
                response = b""
                try:
                    while True:
                        chunk = sock.recv(4096)
                        if not chunk:
                            break
                        response += chunk
                        if b"\r\n\r\n" in response:
                            # Got headers, check for body
                            if b"Content-Length:" in response:
                                # Parse content length and read body
                                headers = response.split(b"\r\n\r\n")[0]
                                for line in headers.split(b"\r\n"):
                                    if b"Content-Length:" in line:
                                        length = int(line.split(b":")[1].strip())
                                        body_start = response.find(b"\r\n\r\n") + 4
                                        body = response[body_start:]
                                        while len(body) < length:
                                            body += sock.recv(4096)
                                        break
                            break
                except socket.timeout:
                    pass
                
                if response and b"200" in response:
                    success += 1
                else:
                    failures += 1
                
                time.sleep(0.01)  # Small delay between requests
            
            sock.close()
            
        except Exception as e:
            print(f"{Colors.RED}Connection error: {str(e)}{Colors.RESET}")
            failures += num_requests - success
        
        print(f"\n{Colors.CYAN}Results:{Colors.RESET}")
        print(f"  Successful: {Colors.GREEN}{success}{Colors.RESET}")
        print(f"  Failed: {Colors.RED}{failures}{Colors.RESET}")
        
        success_rate = (success / num_requests * 100)
        print(f"  Success rate: {success_rate:.1f}%")
        
        if success_rate >= 90:
            print(f"{Colors.GREEN}✓ PASS{Colors.RESET}: Connection reuse works well")
            return True
        else:
            print(f"{Colors.RED}✗ FAIL{Colors.RESET}: Issues with connection reuse")
            return False
    
    def large_payload_test(self, size_mb: int = 5, num_requests: int = 10):
        """Test with large POST payloads"""
        print(f"\n{Colors.BOLD}Large Payload Test{Colors.RESET}")
        print(f"Payload size: {size_mb}MB, Requests: {num_requests}")
        
        payload = "X" * (size_mb * 1024 * 1024)
        success = 0
        failures = 0
        
        for i in range(num_requests):
            try:
                sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                sock.settimeout(15.0)
                sock.connect((self.host, self.port))
                
                request_header = (
                    f"POST /uploads HTTP/1.1\r\n"
                    f"Host: {self.host}\r\n"
                    f"Content-Type: application/octet-stream\r\n"
                    f"Content-Length: {len(payload)}\r\n"
                    f"\r\n"
                )
                
                sock.sendall(request_header.encode())
                
                # Send payload in chunks
                chunk_size = 65536
                sent = 0
                while sent < len(payload):
                    chunk = payload[sent:sent+chunk_size]
                    sock.sendall(chunk.encode())
                    sent += len(chunk)
                
                response = sock.recv(4096)
                sock.close()
                
                if response and (b"200" in response or b"201" in response or b"413" in response):
                    success += 1
                else:
                    failures += 1
                    
            except Exception as e:
                failures += 1
            
            print(f"  Progress: {i+1}/{num_requests}", end='\r')
        
        print()  # New line after progress
        
        print(f"\n{Colors.CYAN}Results:{Colors.RESET}")
        print(f"  Successful: {Colors.GREEN}{success}{Colors.RESET}")
        print(f"  Failed: {Colors.RED}{failures}{Colors.RESET}")
        
        success_rate = (success / num_requests * 100)
        print(f"  Success rate: {success_rate:.1f}%")
        
        if success_rate >= 80:
            print(f"{Colors.GREEN}✓ PASS{Colors.RESET}: Handled large payloads")
            return True
        else:
            print(f"{Colors.RED}✗ FAIL{Colors.RESET}: Issues with large payloads")
            return False
    
    def rapid_connect_disconnect_test(self, num_connections: int = 500):
        """Test rapid connection establishment and closing"""
        print(f"\n{Colors.BOLD}Rapid Connect/Disconnect Test{Colors.RESET}")
        print(f"Connections: {num_connections}")
        
        success = 0
        failures = 0
        
        start_time = time.time()
        
        for i in range(num_connections):
            try:
                sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                sock.settimeout(2.0)
                sock.connect((self.host, self.port))
                sock.close()
                success += 1
            except:
                failures += 1
            
            if (i + 1) % 50 == 0:
                print(f"  Progress: {i+1}/{num_connections}", end='\r')
        
        elapsed = time.time() - start_time
        print()
        
        print(f"\n{Colors.CYAN}Results:{Colors.RESET}")
        print(f"  Successful: {Colors.GREEN}{success}{Colors.RESET}")
        print(f"  Failed: {Colors.RED}{failures}{Colors.RESET}")
        print(f"  Time: {elapsed:.2f}s")
        print(f"  Connections/sec: {num_connections/elapsed:.2f}")
        
        success_rate = (success / num_connections * 100)
        print(f"  Success rate: {success_rate:.1f}%")
        
        if success_rate >= 95:
            print(f"{Colors.GREEN}✓ PASS{Colors.RESET}: Handled rapid connections")
            return True
        else:
            print(f"{Colors.RED}✗ FAIL{Colors.RESET}: Issues with rapid connections")
            return False
    
    def mixed_workload_test(self, duration: int = 30):
        """Test with mixed GET/POST/DELETE requests"""
        print(f"\n{Colors.BOLD}Mixed Workload Test{Colors.RESET}")
        print(f"Duration: {duration}s")
        
        self.success_count = 0
        self.failure_count = 0
        running = True
        
        paths = ["/", "/index.html", "/test.html", "/cgi-bin/test.py"]
        methods = ["GET", "GET", "GET", "POST"]  # More GETs than POSTs
        
        def worker():
            while running:
                path = random.choice(paths)
                method = random.choice(methods)
                if self.make_request(path, method, timeout=3.0):
                    with self.lock:
                        self.success_count += 1
                else:
                    with self.lock:
                        self.failure_count += 1
                time.sleep(random.uniform(0.01, 0.1))
        
        start_time = time.time()
        
        threads = []
        for _ in range(20):
            thread = threading.Thread(target=worker)
            thread.daemon = True
            thread.start()
            threads.append(thread)
        
        try:
            time.sleep(duration)
        except KeyboardInterrupt:
            print(f"\n{Colors.YELLOW}Test interrupted{Colors.RESET}")
        
        running = False
        time.sleep(2)
        
        elapsed = time.time() - start_time
        total = self.success_count + self.failure_count
        
        print(f"\n{Colors.CYAN}Results:{Colors.RESET}")
        print(f"  Successful: {Colors.GREEN}{self.success_count}{Colors.RESET}")
        print(f"  Failed: {Colors.RED}{self.failure_count}{Colors.RESET}")
        print(f"  Time: {elapsed:.2f}s")
        print(f"  Requests/sec: {total/elapsed:.2f}")
        
        success_rate = (self.success_count / total * 100) if total > 0 else 0
        print(f"  Success rate: {success_rate:.1f}%")
        
        if success_rate >= 95:
            print(f"{Colors.GREEN}✓ PASS{Colors.RESET}: Handled mixed workload well")
            return True
        else:
            print(f"{Colors.RED}✗ FAIL{Colors.RESET}: Issues with mixed workload")
            return False
    
    def run_all_stress_tests(self):
        """Run all stress tests"""
        print(f"\n{Colors.BOLD}{Colors.BLUE}{'='*60}{Colors.RESET}")
        print(f"{Colors.BOLD}{Colors.BLUE}WebServ Stress Test Suite{Colors.RESET}")
        print(f"{Colors.BOLD}{Colors.BLUE}Testing server at {self.host}:{self.port}{Colors.RESET}")
        print(f"{Colors.BOLD}{Colors.BLUE}{'='*60}{Colors.RESET}\n")
        
        results = []
        
        # Run tests
        results.append(("Concurrent Requests (100 threads)", self.concurrent_requests_test(100, 10)))
        results.append(("Sustained Load (30s)", self.sustained_load_test(30, 30)))
        results.append(("Connection Reuse", self.connection_reuse_test(50)))
        results.append(("Large Payloads", self.large_payload_test(1, 5)))
        results.append(("Rapid Connect/Disconnect", self.rapid_connect_disconnect_test(200)))
        results.append(("Mixed Workload", self.mixed_workload_test(20)))
        
        # Print summary
        print(f"\n{Colors.BOLD}{'='*60}{Colors.RESET}")
        print(f"{Colors.BOLD}STRESS TEST SUMMARY{Colors.RESET}")
        print(f"{'='*60}")
        
        passed = sum(1 for _, result in results if result)
        total = len(results)
        
        for test_name, result in results:
            status = f"{Colors.GREEN}PASS{Colors.RESET}" if result else f"{Colors.RED}FAIL{Colors.RESET}"
            print(f"  {test_name}: {status}")
        
        print(f"\nTotal: {passed}/{total} passed")
        print(f"{'='*60}\n")
        
        return passed == total

def main():
    import argparse
    
    parser = argparse.ArgumentParser(description='WebServ Stress Tester')
    parser.add_argument('--host', default='localhost', help='Server host')
    parser.add_argument('--port', type=int, default=8080, help='Server port')
    
    args = parser.parse_args()
    
    # Check if server is running
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(2.0)
        result = sock.connect_ex((args.host, args.port))
        sock.close()
        
        if result != 0:
            print(f"{Colors.RED}Error: Server not running on {args.host}:{args.port}{Colors.RESET}")
            sys.exit(1)
    except Exception as e:
        print(f"{Colors.RED}Error: {str(e)}{Colors.RESET}")
        sys.exit(1)
    
    # Run tests
    tester = StressTester(args.host, args.port)
    success = tester.run_all_stress_tests()
    
    sys.exit(0 if success else 1)

if __name__ == "__main__":
    main()
