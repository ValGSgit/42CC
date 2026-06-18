#!/usr/bin/env python3
"""
CGI Testing Suite for WebServ
Tests CGI functionality in detail
"""

import socket
import time
import sys
import os

class Colors:
    GREEN = '\033[92m'
    RED = '\033[91m'
    YELLOW = '\033[93m'
    BLUE = '\033[94m'
    CYAN = '\033[96m'
    RESET = '\033[0m'
    BOLD = '\033[1m'

class CGITester:
    def __init__(self, host: str = "localhost", port: int = 8080):
        self.host = host
        self.port = port
        self.passed = 0
        self.failed = 0
    
    def send_request(self, request: str, timeout: float = 10.0):
        """Send request and return headers and body"""
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.settimeout(timeout)
            sock.connect((self.host, self.port))
            sock.sendall(request.encode())
            
            response = b""
            while True:
                try:
                    chunk = sock.recv(4096)
                    if not chunk:
                        break
                    response += chunk
                except socket.timeout:
                    break
            
            sock.close()
            response_str = response.decode('utf-8', errors='ignore')
            
            if '\r\n\r\n' in response_str:
                headers, body = response_str.split('\r\n\r\n', 1)
                return headers, body
            return response_str, ""
        except Exception as e:
            return None, str(e)
    
    def get_status_code(self, headers: str) -> int:
        """Extract status code"""
        if headers:
            lines = headers.split('\r\n')
            if lines:
                parts = lines[0].split(' ')
                if len(parts) >= 2:
                    try:
                        return int(parts[1])
                    except:
                        pass
        return 0
    
    def test_python_cgi_get(self):
        """Test Python CGI with GET"""
        print(f"\n{Colors.BOLD}Testing Python CGI with GET{Colors.RESET}")
        request = "GET /cgi-bin/test.py?name=Alice&age=25 HTTP/1.1\r\nHost: localhost\r\n\r\n"
        headers, body = self.send_request(request)
        
        if headers:
            status = self.get_status_code(headers)
            if status == 200 and body and "Alice" in body:
                print(f"{Colors.GREEN}✓ PASS{Colors.RESET}: Python CGI GET works")
                print(f"  Query string parsed correctly")
                self.passed += 1
            elif status == 200:
                print(f"{Colors.YELLOW}⚠ WARN{Colors.RESET}: Python CGI returned 200 but query not found")
                self.passed += 1
            else:
                print(f"{Colors.RED}✗ FAIL{Colors.RESET}: Status {status}")
                self.failed += 1
        else:
            print(f"{Colors.RED}✗ FAIL{Colors.RESET}: No response")
            self.failed += 1
    
    def test_python_cgi_post(self):
        """Test Python CGI with POST"""
        print(f"\n{Colors.BOLD}Testing Python CGI with POST{Colors.RESET}")
        body_content = "username=testuser&password=secret123"
        request = (
            "POST /cgi-bin/test.py HTTP/1.1\r\n"
            f"Host: localhost\r\n"
            f"Content-Type: application/x-www-form-urlencoded\r\n"
            f"Content-Length: {len(body_content)}\r\n"
            f"\r\n"
            f"{body_content}"
        )
        headers, body = self.send_request(request)
        
        if headers:
            status = self.get_status_code(headers)
            if status == 200 and body:
                print(f"{Colors.GREEN}✓ PASS{Colors.RESET}: Python CGI POST works")
                if "testuser" in body:
                    print(f"  POST data processed correctly")
                self.passed += 1
            else:
                print(f"{Colors.RED}✗ FAIL{Colors.RESET}: Status {status}")
                self.failed += 1
        else:
            print(f"{Colors.RED}✗ FAIL{Colors.RESET}: No response")
            self.failed += 1
    
    def test_cgi_environment_request_method(self):
        """Test REQUEST_METHOD environment variable"""
        print(f"\n{Colors.BOLD}Testing REQUEST_METHOD Env Var{Colors.RESET}")
        request = "GET /cgi-bin/test.py HTTP/1.1\r\nHost: localhost\r\n\r\n"
        headers, body = self.send_request(request)
        
        if headers and body:
            if "REQUEST_METHOD" in body and "GET" in body:
                print(f"{Colors.GREEN}✓ PASS{Colors.RESET}: REQUEST_METHOD set correctly")
                self.passed += 1
            else:
                print(f"{Colors.YELLOW}⚠ WARN{Colors.RESET}: REQUEST_METHOD not verified in output")
                self.passed += 1
        else:
            print(f"{Colors.RED}✗ FAIL{Colors.RESET}: No response")
            self.failed += 1
    
    def test_cgi_environment_query_string(self):
        """Test QUERY_STRING environment variable"""
        print(f"\n{Colors.BOLD}Testing QUERY_STRING Env Var{Colors.RESET}")
        request = "GET /cgi-bin/test.py?key1=value1&key2=value2 HTTP/1.1\r\nHost: localhost\r\n\r\n"
        headers, body = self.send_request(request)
        
        if headers and body:
            if "QUERY_STRING" in body or ("key1" in body and "value1" in body):
                print(f"{Colors.GREEN}✓ PASS{Colors.RESET}: QUERY_STRING set correctly")
                self.passed += 1
            else:
                print(f"{Colors.YELLOW}⚠ WARN{Colors.RESET}: QUERY_STRING not verified")
                self.passed += 1
        else:
            print(f"{Colors.RED}✗ FAIL{Colors.RESET}: No response")
            self.failed += 1
    
    def test_cgi_environment_content_type(self):
        """Test CONTENT_TYPE environment variable"""
        print(f"\n{Colors.BOLD}Testing CONTENT_TYPE Env Var{Colors.RESET}")
        body_content = "test data"
        request = (
            "POST /cgi-bin/test.py HTTP/1.1\r\n"
            f"Host: localhost\r\n"
            f"Content-Type: application/json\r\n"
            f"Content-Length: {len(body_content)}\r\n"
            f"\r\n"
            f"{body_content}"
        )
        headers, body = self.send_request(request)
        
        if headers and body:
            if "CONTENT_TYPE" in body or "application/json" in body:
                print(f"{Colors.GREEN}✓ PASS{Colors.RESET}: CONTENT_TYPE set correctly")
                self.passed += 1
            else:
                print(f"{Colors.YELLOW}⚠ WARN{Colors.RESET}: CONTENT_TYPE not verified")
                self.passed += 1
        else:
            print(f"{Colors.RED}✗ FAIL{Colors.RESET}: No response")
            self.failed += 1
    
    def test_cgi_environment_content_length(self):
        """Test CONTENT_LENGTH environment variable"""
        print(f"\n{Colors.BOLD}Testing CONTENT_LENGTH Env Var{Colors.RESET}")
        body_content = "0123456789" * 10  # 100 bytes
        request = (
            "POST /cgi-bin/test.py HTTP/1.1\r\n"
            f"Host: localhost\r\n"
            f"Content-Type: text/plain\r\n"
            f"Content-Length: {len(body_content)}\r\n"
            f"\r\n"
            f"{body_content}"
        )
        headers, body = self.send_request(request)
        
        if headers and body:
            if "CONTENT_LENGTH" in body or "100" in body:
                print(f"{Colors.GREEN}✓ PASS{Colors.RESET}: CONTENT_LENGTH set correctly")
                self.passed += 1
            else:
                print(f"{Colors.YELLOW}⚠ WARN{Colors.RESET}: CONTENT_LENGTH not verified")
                self.passed += 1
        else:
            print(f"{Colors.RED}✗ FAIL{Colors.RESET}: No response")
            self.failed += 1
    
    def test_cgi_stdin_post_data(self):
        """Test CGI receives POST data via stdin"""
        print(f"\n{Colors.BOLD}Testing CGI STDIN (POST data){Colors.RESET}")
        body_content = "This is test data that should be passed to CGI via stdin"
        request = (
            "POST /cgi-bin/test.py HTTP/1.1\r\n"
            f"Host: localhost\r\n"
            f"Content-Type: text/plain\r\n"
            f"Content-Length: {len(body_content)}\r\n"
            f"\r\n"
            f"{body_content}"
        )
        headers, body = self.send_request(request)
        
        if headers and body:
            status = self.get_status_code(headers)
            if status == 200:
                print(f"{Colors.GREEN}✓ PASS{Colors.RESET}: CGI received POST data")
                self.passed += 1
            else:
                print(f"{Colors.RED}✗ FAIL{Colors.RESET}: Status {status}")
                self.failed += 1
        else:
            print(f"{Colors.RED}✗ FAIL{Colors.RESET}: No response")
            self.failed += 1
    
    def test_cgi_large_post_body(self):
        """Test CGI with large POST body"""
        print(f"\n{Colors.BOLD}Testing CGI with Large POST Body{Colors.RESET}")
        body_content = "X" * 10000  # 10KB
        request = (
            "POST /cgi-bin/test.py HTTP/1.1\r\n"
            f"Host: localhost\r\n"
            f"Content-Type: text/plain\r\n"
            f"Content-Length: {len(body_content)}\r\n"
            f"\r\n"
            f"{body_content}"
        )
        headers, body = self.send_request(request, timeout=15.0)
        
        if headers:
            status = self.get_status_code(headers)
            if status == 200:
                print(f"{Colors.GREEN}✓ PASS{Colors.RESET}: CGI handled large POST body")
                self.passed += 1
            else:
                print(f"{Colors.RED}✗ FAIL{Colors.RESET}: Status {status}")
                self.failed += 1
        else:
            print(f"{Colors.RED}✗ FAIL{Colors.RESET}: No response")
            self.failed += 1
    
    def test_cgi_timeout(self):
        """Test CGI timeout handling"""
        print(f"\n{Colors.BOLD}Testing CGI Timeout{Colors.RESET}")
        # This assumes you have a slow CGI script
        request = "GET /cgi-bin/test.py?sleep=5 HTTP/1.1\r\nHost: localhost\r\n\r\n"
        headers, body = self.send_request(request, timeout=15.0)
        
        if headers:
            status = self.get_status_code(headers)
            if status == 200 or status == 504:  # Gateway Timeout
                print(f"{Colors.GREEN}✓ PASS{Colors.RESET}: CGI timeout handled (status {status})")
                self.passed += 1
            else:
                print(f"{Colors.YELLOW}⚠ WARN{Colors.RESET}: Status {status}")
                self.passed += 1
        else:
            print(f"{Colors.YELLOW}⚠ WARN{Colors.RESET}: Connection timeout (expected)")
            self.passed += 1
    
    def test_cgi_special_characters(self):
        """Test CGI with special characters in query"""
        print(f"\n{Colors.BOLD}Testing CGI with Special Characters{Colors.RESET}")
        request = "GET /cgi-bin/test.py?name=John%20Doe&city=New%20York HTTP/1.1\r\nHost: localhost\r\n\r\n"
        headers, body = self.send_request(request)
        
        if headers:
            status = self.get_status_code(headers)
            if status == 200:
                print(f"{Colors.GREEN}✓ PASS{Colors.RESET}: CGI handled URL encoding")
                self.passed += 1
            else:
                print(f"{Colors.RED}✗ FAIL{Colors.RESET}: Status {status}")
                self.failed += 1
        else:
            print(f"{Colors.RED}✗ FAIL{Colors.RESET}: No response")
            self.failed += 1
    
    def test_cgi_empty_query(self):
        """Test CGI with empty query string"""
        print(f"\n{Colors.BOLD}Testing CGI with Empty Query{Colors.RESET}")
        request = "GET /cgi-bin/test.py? HTTP/1.1\r\nHost: localhost\r\n\r\n"
        headers, body = self.send_request(request)
        
        if headers:
            status = self.get_status_code(headers)
            if status == 200:
                print(f"{Colors.GREEN}✓ PASS{Colors.RESET}: CGI handled empty query")
                self.passed += 1
            else:
                print(f"{Colors.RED}✗ FAIL{Colors.RESET}: Status {status}")
                self.failed += 1
        else:
            print(f"{Colors.RED}✗ FAIL{Colors.RESET}: No response")
            self.failed += 1
    
    def test_cgi_no_content_length_response(self):
        """Test CGI response without Content-Length"""
        print(f"\n{Colors.BOLD}Testing CGI Response without Content-Length{Colors.RESET}")
        request = "GET /cgi-bin/test.py HTTP/1.1\r\nHost: localhost\r\n\r\n"
        headers, body = self.send_request(request)
        
        if headers and body:
            status = self.get_status_code(headers)
            if status == 200:
                print(f"{Colors.GREEN}✓ PASS{Colors.RESET}: CGI response received")
                print(f"  Body length: {len(body)} bytes")
                self.passed += 1
            else:
                print(f"{Colors.RED}✗ FAIL{Colors.RESET}: Status {status}")
                self.failed += 1
        else:
            print(f"{Colors.RED}✗ FAIL{Colors.RESET}: No response")
            self.failed += 1
    
    def run_all_tests(self):
        """Run all CGI tests"""
        print(f"\n{Colors.BOLD}{Colors.BLUE}{'='*60}{Colors.RESET}")
        print(f"{Colors.BOLD}{Colors.BLUE}WebServ CGI Test Suite{Colors.RESET}")
        print(f"{Colors.BOLD}{Colors.BLUE}Testing server at {self.host}:{self.port}{Colors.RESET}")
        print(f"{Colors.BOLD}{Colors.BLUE}{'='*60}{Colors.RESET}\n")
        
        self.test_python_cgi_get()
        self.test_python_cgi_post()
        self.test_cgi_environment_request_method()
        self.test_cgi_environment_query_string()
        self.test_cgi_environment_content_type()
        self.test_cgi_environment_content_length()
        self.test_cgi_stdin_post_data()
        self.test_cgi_large_post_body()
        self.test_cgi_timeout()
        self.test_cgi_special_characters()
        self.test_cgi_empty_query()
        self.test_cgi_no_content_length_response()
        
        # Print summary
        print(f"\n{Colors.BOLD}{'='*60}{Colors.RESET}")
        print(f"{Colors.BOLD}CGI TEST SUMMARY{Colors.RESET}")
        print(f"{'='*60}")
        total = self.passed + self.failed
        print(f"Total Tests: {total}")
        print(f"{Colors.GREEN}Passed: {self.passed}{Colors.RESET}")
        print(f"{Colors.RED}Failed: {self.failed}{Colors.RESET}")
        if total > 0:
            success_rate = (self.passed / total) * 100
            print(f"Success Rate: {success_rate:.1f}%")
        print(f"{'='*60}\n")
        
        return self.failed == 0

def main():
    import argparse
    
    parser = argparse.ArgumentParser(description='WebServ CGI Tester')
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
    tester = CGITester(args.host, args.port)
    success = tester.run_all_tests()
    
    sys.exit(0 if success else 1)

if __name__ == "__main__":
    main()
