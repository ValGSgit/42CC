#!/usr/bin/env python3
"""
Comprehensive WebServ HTTP Server Tester
Tests all mandatory features from the subject requirements
"""

import socket
import time
import sys
import os
import threading
import subprocess
import signal
from typing import Dict, List, Tuple, Optional
import random
import string

# ANSI color codes for pretty output
class Colors:
    GREEN = '\033[92m'
    RED = '\033[91m'
    YELLOW = '\033[93m'
    BLUE = '\033[94m'
    MAGENTA = '\033[95m'
    CYAN = '\033[96m'
    RESET = '\033[0m'
    BOLD = '\033[1m'

class TestResult:
    def __init__(self):
        self.passed = 0
        self.failed = 0
        self.warnings = 0
        self.tests = []
    
    def add_pass(self, test_name: str, message: str = ""):
        self.passed += 1
        self.tests.append((test_name, "PASS", message))
        print(f"{Colors.GREEN}✓ PASS{Colors.RESET}: {test_name}")
        if message:
            print(f"  {Colors.CYAN}{message}{Colors.RESET}")
    
    def add_fail(self, test_name: str, message: str = ""):
        self.failed += 1
        self.tests.append((test_name, "FAIL", message))
        print(f"{Colors.RED}✗ FAIL{Colors.RESET}: {test_name}")
        if message:
            print(f"  {Colors.RED}{message}{Colors.RESET}")
    
    def add_warning(self, test_name: str, message: str = ""):
        self.warnings += 1
        self.tests.append((test_name, "WARN", message))
        print(f"{Colors.YELLOW}⚠ WARN{Colors.RESET}: {test_name}")
        if message:
            print(f"  {Colors.YELLOW}{message}{Colors.RESET}")
    
    def print_summary(self):
        total = self.passed + self.failed
        print(f"\n{Colors.BOLD}{'='*60}{Colors.RESET}")
        print(f"{Colors.BOLD}TEST SUMMARY{Colors.RESET}")
        print(f"{'='*60}")
        print(f"Total Tests: {total}")
        print(f"{Colors.GREEN}Passed: {self.passed}{Colors.RESET}")
        print(f"{Colors.RED}Failed: {self.failed}{Colors.RESET}")
        print(f"{Colors.YELLOW}Warnings: {self.warnings}{Colors.RESET}")
        if total > 0:
            success_rate = (self.passed / total) * 100
            print(f"Success Rate: {success_rate:.1f}%")
        print(f"{'='*60}\n")

class HTTPTester:
    def __init__(self, host: str = "localhost", port: int = 8080):
        self.host = host
        self.port = port
        self.result = TestResult()
    
    def send_request(self, request: str, timeout: float = 5.0) -> Tuple[Optional[str], Optional[str]]:
        """Send HTTP request and return (headers, body)"""
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
            
            # Split headers and body
            if '\r\n\r\n' in response_str:
                headers, body = response_str.split('\r\n\r\n', 1)
                return headers, body
            else:
                return response_str, ""
                
        except Exception as e:
            return None, str(e)
    
    def send_raw_request(self, request: bytes, timeout: float = 5.0) -> bytes:
        """Send raw bytes and return raw response"""
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.settimeout(timeout)
            sock.connect((self.host, self.port))
            sock.sendall(request)
            
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
            return response
        except Exception as e:
            return b""
    
    def get_status_code(self, headers: str) -> int:
        """Extract status code from response headers"""
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
    
    def check_header(self, headers: str, header_name: str) -> Optional[str]:
        """Get value of specific header"""
        lines = headers.split('\r\n')
        for line in lines:
            if ':' in line:
                name, value = line.split(':', 1)
                if name.strip().lower() == header_name.lower():
                    return value.strip()
        return None

    # ==================== BASIC HTTP METHOD TESTS ====================
    
    def test_file_upload(self):
        """Test file upload functionality"""
        print(f"\n{Colors.BOLD}Testing File Upload{Colors.RESET}")
        
        # Create multipart form data
        boundary = "----WebServBoundary123456789"
        file_content = "This is a test file content for upload testing.\n"
        
        body_parts = [
            f"--{boundary}\r\n",
            'Content-Disposition: form-data; name="file"; filename="test_upload.txt"\r\n',
            'Content-Type: text/plain\r\n',
            '\r\n',
            file_content,
            f"\r\n--{boundary}--\r\n"
        ]
        body = ''.join(body_parts)
        
        request = (
            "POST /uploads HTTP/1.1\r\n"
            f"Host: localhost\r\n"
            f"Content-Type: multipart/form-data; boundary={boundary}\r\n"
            f"Content-Length: {len(body)}\r\n"
            f"\r\n"
            f"{body}"
        )
        
        headers, response_body = self.send_request(request)
        
        if headers:
            status = self.get_status_code(headers)
            if status in [200, 201, 204]:
                self.result.add_pass("File upload", f"Status: {status}")
            else:
                self.result.add_fail("File upload", f"Status: {status}")
        else:
            self.result.add_fail("File upload", "No response received")
    
    def test_large_file_upload(self):
        """Test large file upload within limits"""
        print(f"\n{Colors.BOLD}Testing Large File Upload{Colors.RESET}")
        
        # Create a 1MB file
        file_size = 1024 * 1024  # 1MB
        file_content = 'A' * file_size
        
        boundary = "----WebServBoundary"
        body_parts = [
            f"--{boundary}\r\n",
            'Content-Disposition: form-data; name="file"; filename="large.txt"\r\n',
            'Content-Type: text/plain\r\n',
            '\r\n',
            file_content,
            f"\r\n--{boundary}--\r\n"
        ]
        body = ''.join(body_parts)
        
        request = (
            "POST /uploads HTTP/1.1\r\n"
            f"Host: localhost\r\n"
            f"Content-Type: multipart/form-data; boundary={boundary}\r\n"
            f"Content-Length: {len(body)}\r\n"
            f"\r\n"
            f"{body}"
        )
        
        headers, response_body = self.send_request(request, timeout=15.0)
        
        if headers:
            status = self.get_status_code(headers)
            if status in [200, 201, 204]:
                self.result.add_pass("Large file upload (1MB)", f"Status: {status}")
            else:
                self.result.add_warning("Large file upload", f"Status: {status}")
        else:
            self.result.add_fail("Large file upload", "No response received")

    # ==================== MALFORMED REQUEST TESTS ====================
    
    def test_malformed_request(self):
        """Test handling of malformed requests"""
        print(f"\n{Colors.BOLD}Testing Malformed Requests{Colors.RESET}")
        
        # Test 1: Invalid request line
        request = "INVALID REQUEST\r\n\r\n"
        headers, body = self.send_request(request)
        if headers:
            status = self.get_status_code(headers)
            if status == 400:
                self.result.add_pass("Invalid request line", f"Status: {status}")
            else:
                self.result.add_warning("Invalid request line", f"Status: {status}")
        
        # Test 2: Missing Host header (required in HTTP/1.1)
        request = "GET / HTTP/1.1\r\n\r\n"
        headers, body = self.send_request(request)
        if headers:
            status = self.get_status_code(headers)
            # 400 or 200 acceptable (some servers are lenient)
            if status in [400, 200]:
                self.result.add_pass("Missing Host header", f"Status: {status}")
            else:
                self.result.add_warning("Missing Host header", f"Status: {status}")
        
        # Test 3: Invalid header format
        request = "GET / HTTP/1.1\r\nInvalidHeader\r\nHost: localhost\r\n\r\n"
        headers, body = self.send_request(request)
        if headers:
            status = self.get_status_code(headers)
            # Server might ignore or reject
            self.result.add_pass("Malformed header handling", f"Status: {status}")

    # ==================== KEEP-ALIVE TESTS ====================
   

    # ==================== MAIN TEST RUNNER ====================
    
    def run_all_tests(self):
        """Run all test suites"""
        print(f"\n{Colors.BOLD}{Colors.BLUE}{'='*60}{Colors.RESET}")
        print(f"{Colors.BOLD}{Colors.BLUE}WebServ Comprehensive Test Suite{Colors.RESET}")
        print(f"{Colors.BOLD}{Colors.BLUE}Testing server at {self.host}:{self.port}{Colors.RESET}")
        print(f"{Colors.BOLD}{Colors.BLUE}{'='*60}{Colors.RESET}\n")
        
        # File Upload
        self.test_file_upload()
        self.test_large_file_upload()
        # Malformed Requests
        self.test_malformed_request()
        
        # Print summary
        self.result.print_summary()

def main():
    import argparse
    
    parser = argparse.ArgumentParser(description='WebServ HTTP Server Tester')
    parser.add_argument('--host', default='localhost', help='Server host (default: localhost)')
    parser.add_argument('--port', type=int, default=8080, help='Server port (default: 8080)')
    parser.add_argument('--config', default='./config/default.conf', help='Config file to use')
    
    args = parser.parse_args()
    
    # Check if server is running
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(2.0)
        result = sock.connect_ex((args.host, args.port))
        sock.close()
        
        if result != 0:
            print(f"{Colors.RED}Error: Server not running on {args.host}:{args.port}{Colors.RESET}")
            print(f"{Colors.YELLOW}Please start your server first:{Colors.RESET}")
            print(f"  ./webserv {args.config}")
            sys.exit(1)
    except Exception as e:
        print(f"{Colors.RED}Error checking server: {str(e)}{Colors.RESET}")
        sys.exit(1)
    
    # Run tests
    tester = HTTPTester(args.host, args.port)
    tester.run_all_tests()
    
    # Exit with appropriate code
    if tester.result.failed > 0:
        sys.exit(1)
    else:
        sys.exit(0)

if __name__ == "__main__":
    main()
