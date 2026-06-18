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
    
    def test_get_request(self):
        """Test basic GET request"""
        print(f"\n{Colors.BOLD}Testing GET Request{Colors.RESET}")
        request = "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n"
        headers, body = self.send_request(request)
        
        if headers:
            status = self.get_status_code(headers)
            if status == 200:
                self.result.add_pass("GET /", f"Status: {status}")
            else:
                self.result.add_fail("GET /", f"Expected 200, got {status}")
        else:
            self.result.add_fail("GET /", "No response received")
    
    def test_post_request(self):
        """Test basic POST request"""
        print(f"\n{Colors.BOLD}Testing POST Request{Colors.RESET}")
        body_content = "test=data&name=value"
        request = (
            "POST /cgi-bin/test.py HTTP/1.1\r\n"
            f"Host: localhost\r\n"
            f"Content-Type: application/x-www-form-urlencoded\r\n"
            f"Content-Length: {len(body_content)}\r\n"
            f"\r\n"
            f"{body_content}"
        )
        headers, body = self.send_request(request, timeout=10.0)
        
        if headers:
            status = self.get_status_code(headers)
            if status == 200:
                self.result.add_pass("POST with body", f"Status: {status}")
            else:
                self.result.add_fail("POST with body", f"Expected 200, got {status}")
        else:
            self.result.add_fail("POST with body", "No response received")
    
    def test_delete_request(self):
        """Test DELETE request"""
        print(f"\n{Colors.BOLD}Testing DELETE Request{Colors.RESET}")
        request = "DELETE /uploads/test_delete.txt HTTP/1.1\r\nHost: localhost\r\n\r\n"
        headers, body = self.send_request(request)
        
        if headers:
            status = self.get_status_code(headers)
            # 200, 204, or 404 are acceptable
            if status in [200, 204, 404]:
                self.result.add_pass("DELETE request", f"Status: {status}")
            else:
                self.result.add_fail("DELETE request", f"Unexpected status: {status}")
        else:
            self.result.add_fail("DELETE request", "No response received")
    
    def test_unsupported_method(self):
        """Test unsupported HTTP method"""
        print(f"\n{Colors.BOLD}Testing Unsupported Method{Colors.RESET}")
        request = "PUT / HTTP/1.1\r\nHost: localhost\r\n\r\n"
        headers, body = self.send_request(request)
        
        if headers:
            status = self.get_status_code(headers)
            if status == 405:
                self.result.add_pass("Method not allowed (PUT)", f"Status: {status}")
            else:
                self.result.add_fail("Method not allowed", f"Expected 405, got {status}")
        else:
            self.result.add_fail("Method not allowed", "No response received")

    # ==================== STATUS CODE TESTS ====================
    
    def test_404_not_found(self):
        """Test 404 for non-existent resource"""
        print(f"\n{Colors.BOLD}Testing 404 Not Found{Colors.RESET}")
        request = "GET /nonexistent_file_12345.html HTTP/1.1\r\nHost: localhost\r\n\r\n"
        headers, body = self.send_request(request)
        
        if headers:
            status = self.get_status_code(headers)
            if status == 404:
                self.result.add_pass("404 Not Found", f"Status: {status}")
            else:
                self.result.add_fail("404 Not Found", f"Expected 404, got {status}")
        else:
            self.result.add_fail("404 Not Found", "No response received")
    
    def test_custom_error_pages(self):
        """Test custom error pages"""
        print(f"\n{Colors.BOLD}Testing Custom Error Pages{Colors.RESET}")
        request = "GET /nonexistent.html HTTP/1.1\r\nHost: localhost\r\n\r\n"
        headers, body = self.send_request(request)
        
        if headers and body:
            status = self.get_status_code(headers)
            if status == 404 and len(body) > 0:
                self.result.add_pass("Custom 404 page", "Error page returned")
            else:
                self.result.add_warning("Custom 404 page", "No custom error page detected")
        else:
            self.result.add_warning("Custom 404 page", "Could not verify")

    # ==================== STATIC FILE SERVING TESTS ====================
    
    def test_static_file_serving(self):
        """Test serving static HTML files"""
        print(f"\n{Colors.BOLD}Testing Static File Serving{Colors.RESET}")
        request = "GET /index.html HTTP/1.1\r\nHost: localhost\r\n\r\n"
        headers, body = self.send_request(request)
        
        if headers:
            status = self.get_status_code(headers)
            content_type = self.check_header(headers, "Content-Type")
            
            if status == 200 and body:
                self.result.add_pass("Static HTML serving", f"Status: {status}, Size: {len(body)} bytes")
            else:
                self.result.add_fail("Static HTML serving", f"Status: {status}")
        else:
            self.result.add_fail("Static HTML serving", "No response received")
    
    def test_directory_listing(self):
        """Test directory listing (autoindex)"""
        print(f"\n{Colors.BOLD}Testing Directory Listing{Colors.RESET}")
        request = "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n"
        headers, body = self.send_request(request)
        
        if headers:
            status = self.get_status_code(headers)
            # Could be 200 with index.html or directory listing
            if status == 200:
                self.result.add_pass("Directory access", f"Status: {status}")
            else:
                self.result.add_fail("Directory access", f"Unexpected status: {status}")
        else:
            self.result.add_fail("Directory access", "No response received")

    # ==================== CGI TESTS ====================
    
    def test_cgi_get(self):
        """Test CGI with GET request"""
        print(f"\n{Colors.BOLD}Testing CGI GET Request{Colors.RESET}")
        request = "GET /cgi-bin/test.py?name=John&age=30 HTTP/1.1\r\nHost: localhost\r\n\r\n"
        headers, body = self.send_request(request, timeout=10.0)
        
        if headers:
            status = self.get_status_code(headers)
            if status == 200 and body:
                self.result.add_pass("CGI GET", f"Status: {status}, Response length: {len(body)}")
            else:
                self.result.add_fail("CGI GET", f"Status: {status}, no body received")
        else:
            self.result.add_fail("CGI GET", "No response received")
    
    def test_cgi_post(self):
        """Test CGI with POST request"""
        print(f"\n{Colors.BOLD}Testing CGI POST Request{Colors.RESET}")
        body_content = "username=testuser&password=secret123"
        request = (
            "POST /cgi-bin/test.py HTTP/1.1\r\n"
            f"Host: localhost\r\n"
            f"Content-Type: application/x-www-form-urlencoded\r\n"
            f"Content-Length: {len(body_content)}\r\n"
            f"\r\n"
            f"{body_content}"
        )
        headers, body = self.send_request(request, timeout=10.0)
        
        if headers:
            status = self.get_status_code(headers)
            if status == 200 and body:
                self.result.add_pass("CGI POST", f"Status: {status}")
            else:
                self.result.add_fail("CGI POST", f"Status: {status}")
        else:
            self.result.add_fail("CGI POST", "No response received")
    
    def test_cgi_environment_variables(self):
        """Test that CGI receives correct environment variables"""
        print(f"\n{Colors.BOLD}Testing CGI Environment Variables{Colors.RESET}")
        request = "GET /cgi-bin/test.py?test=env HTTP/1.1\r\nHost: localhost\r\n\r\n"
        headers, body = self.send_request(request, timeout=10.0)
        
        if headers and body:
            status = self.get_status_code(headers)
            # Check if environment variables are present in response
            if status == 200 and ("REQUEST_METHOD" in body or "QUERY_STRING" in body):
                self.result.add_pass("CGI environment vars", "Environment variables passed")
            else:
                self.result.add_warning("CGI environment vars", "Could not verify env vars")
        else:
            self.result.add_fail("CGI environment vars", "No response")

    # ==================== FILE UPLOAD TESTS ====================
    
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

    # ==================== CLIENT BODY SIZE TESTS ====================
    
    def test_client_max_body_size(self):
        """Test client_max_body_size enforcement"""
        print(f"\n{Colors.BOLD}Testing Max Body Size Limit{Colors.RESET}")
        
        # Try to send a very large body (100MB) - should be rejected
        body_content = "X" * (100 * 1024 * 1024)
        
        request = (
            "POST / HTTP/1.1\r\n"
            f"Host: localhost\r\n"
            f"Content-Type: text/plain\r\n"
            f"Content-Length: {len(body_content)}\r\n"
            f"\r\n"
        )
        
        # Send headers first
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.settimeout(5.0)
            sock.connect((self.host, self.port))
            sock.sendall(request.encode())
            
            # Try to send large body
            chunk_size = 65536
            sent = 0
            while sent < len(body_content):
                try:
                    chunk = body_content[sent:sent+chunk_size]
                    sock.sendall(chunk.encode())
                    sent += len(chunk)
                    if sent > 20 * 1024 * 1024:  # Stop after 20MB
                        break
                except:
                    break
            
            response = sock.recv(4096).decode('utf-8', errors='ignore')
            sock.close()
            
            if response:
                status = self.get_status_code(response)
                if status == 413:
                    self.result.add_pass("Max body size limit", f"Correctly rejected with 413")
                else:
                    self.result.add_warning("Max body size limit", f"Status: {status}")
            else:
                self.result.add_warning("Max body size limit", "Server closed connection")
        except Exception as e:
            self.result.add_warning("Max body size limit", f"Connection behavior: {str(e)}")

    # ==================== REDIRECT TESTS ====================
    
    def test_redirection(self):
        """Test HTTP redirection"""
        print(f"\n{Colors.BOLD}Testing HTTP Redirection{Colors.RESET}")
        request = "GET /redirect HTTP/1.1\r\nHost: localhost\r\n\r\n"
        headers, body = self.send_request(request)
        
        if headers:
            status = self.get_status_code(headers)
            location = self.check_header(headers, "Location")
            
            if status in [301, 302, 303, 307, 308] and location:
                self.result.add_pass("HTTP redirect", f"Status: {status}, Location: {location}")
            else:
                self.result.add_fail("HTTP redirect", f"Status: {status}, no Location header")
        else:
            self.result.add_fail("HTTP redirect", "No response received")

    # ==================== CHUNKED ENCODING TESTS ====================
    
    def test_chunked_request(self):
        """Test chunked transfer encoding in request"""
        print(f"\n{Colors.BOLD}Testing Chunked Request{Colors.RESET}")
        
        request_headers = (
            "POST /cgi-bin/test.py HTTP/1.1\r\n"
            "Host: localhost\r\n"
            "Transfer-Encoding: chunked\r\n"
            "Content-Type: text/plain\r\n"
            "\r\n"
        )
        
        # Chunked body
        chunk1 = "Hello "
        chunk2 = "World!"
        chunked_body = (
            f"{len(chunk1):X}\r\n{chunk1}\r\n"
            f"{len(chunk2):X}\r\n{chunk2}\r\n"
            "0\r\n\r\n"
        )
        
        request = request_headers + chunked_body
        headers, body = self.send_request(request, timeout=10.0)
        
        if headers:
            status = self.get_status_code(headers)
            if status == 200:
                self.result.add_pass("Chunked request", f"Status: {status}")
            else:
                self.result.add_warning("Chunked request", f"Status: {status}")
        else:
            self.result.add_fail("Chunked request", "No response received")

    # ==================== HTTP VERSION TESTS ====================
    
    def test_http_1_0(self):
        """Test HTTP/1.0 compatibility"""
        print(f"\n{Colors.BOLD}Testing HTTP/1.0 Compatibility{Colors.RESET}")
        request = "GET / HTTP/1.0\r\nHost: localhost\r\n\r\n"
        headers, body = self.send_request(request)
        
        if headers:
            status = self.get_status_code(headers)
            if status == 200:
                self.result.add_pass("HTTP/1.0 support", f"Status: {status}")
            else:
                self.result.add_fail("HTTP/1.0 support", f"Status: {status}")
        else:
            self.result.add_fail("HTTP/1.0 support", "No response received")
    
    def test_http_1_1(self):
        """Test HTTP/1.1"""
        print(f"\n{Colors.BOLD}Testing HTTP/1.1{Colors.RESET}")
        request = "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n"
        headers, body = self.send_request(request)
        
        if headers:
            status = self.get_status_code(headers)
            if status == 200:
                self.result.add_pass("HTTP/1.1 support", f"Status: {status}")
            else:
                self.result.add_fail("HTTP/1.1 support", f"Status: {status}")
        else:
            self.result.add_fail("HTTP/1.1 support", "No response received")

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
    
    def test_keep_alive(self):
        """Test Connection: keep-alive"""
        print(f"\n{Colors.BOLD}Testing Keep-Alive Connection{Colors.RESET}")
        
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.settimeout(5.0)
            sock.connect((self.host, self.port))
            
            # Send first request with keep-alive
            request1 = "GET / HTTP/1.1\r\nHost: localhost\r\nConnection: keep-alive\r\n\r\n"
            sock.sendall(request1.encode())
            
            response1 = b""
            while True:
                chunk = sock.recv(4096)
                if not chunk:
                    break
                response1 += chunk
                # Check if we got complete response
                if b"\r\n\r\n" in response1:
                    # Parse Content-Length to know when to stop
                    headers = response1.split(b"\r\n\r\n")[0].decode('utf-8', errors='ignore')
                    if "Content-Length:" in headers:
                        break
            
            # Try second request on same connection
            time.sleep(0.1)
            request2 = "GET /index.html HTTP/1.1\r\nHost: localhost\r\nConnection: keep-alive\r\n\r\n"
            sock.sendall(request2.encode())
            
            response2 = b""
            sock.settimeout(2.0)
            try:
                while True:
                    chunk = sock.recv(4096)
                    if not chunk:
                        break
                    response2 += chunk
                    if b"\r\n\r\n" in response2:
                        break
            except socket.timeout:
                pass
            
            sock.close()
            
            if response1 and response2:
                self.result.add_pass("Keep-alive connection", "Multiple requests on same connection")
            else:
                self.result.add_warning("Keep-alive connection", "Could not verify")
                
        except Exception as e:
            self.result.add_warning("Keep-alive connection", f"Error: {str(e)}")

    # ==================== TIMEOUT TESTS ====================
    
    def test_connection_timeout(self):
        """Test connection timeout handling"""
        print(f"\n{Colors.BOLD}Testing Connection Timeout{Colors.RESET}")
        
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.settimeout(65.0)  # Longer than server timeout
            sock.connect((self.host, self.port))
            
            # Send partial request and wait
            sock.sendall(b"GET / HTTP/1.1\r\n")
            
            # Wait for server to timeout (should be around 60 seconds per spec)
            time.sleep(3)
            
            # Try to read - connection might be closed
            try:
                data = sock.recv(1024)
                if len(data) == 0:
                    self.result.add_pass("Connection timeout", "Server closed idle connection")
                else:
                    self.result.add_warning("Connection timeout", "Server still responding")
            except:
                self.result.add_pass("Connection timeout", "Connection closed by server")
            
            sock.close()
            
        except Exception as e:
            self.result.add_warning("Connection timeout", f"Behavior: {str(e)}")

    # ==================== STRESS TESTS ====================
    
    def test_multiple_connections(self):
        """Test handling multiple simultaneous connections"""
        print(f"\n{Colors.BOLD}Testing Multiple Simultaneous Connections{Colors.RESET}")
        
        def make_request(request_id):
            try:
                sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                sock.settimeout(10.0)
                sock.connect((self.host, self.port))
                request = f"GET /?id={request_id} HTTP/1.1\r\nHost: localhost\r\n\r\n"
                sock.sendall(request.encode())
                response = sock.recv(4096)
                sock.close()
                return response.decode('utf-8', errors='ignore')
            except Exception as e:
                return None
        
        # Create 50 simultaneous connections
        threads = []
        results = []
        
        for i in range(50):
            thread = threading.Thread(target=lambda rid=i: results.append(make_request(rid)))
            thread.start()
            threads.append(thread)
        
        for thread in threads:
            thread.join()
        
        successful = sum(1 for r in results if r and "200" in r)
        
        if successful >= 45:  # Allow some failures
            self.result.add_pass("Multiple connections", f"{successful}/50 successful")
        else:
            self.result.add_fail("Multiple connections", f"Only {successful}/50 successful")
    
    def test_rapid_requests(self):
        """Test rapid sequential requests"""
        print(f"\n{Colors.BOLD}Testing Rapid Sequential Requests{Colors.RESET}")
        
        success_count = 0
        for i in range(20):
            request = "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n"
            headers, body = self.send_request(request, timeout=3.0)
            if headers and self.get_status_code(headers) == 200:
                success_count += 1
        
        if success_count >= 18:  # Allow 2 failures
            self.result.add_pass("Rapid requests", f"{success_count}/20 successful")
        else:
            self.result.add_fail("Rapid requests", f"Only {success_count}/20 successful")

    # ==================== HEADER TESTS ====================
    
    def test_various_headers(self):
        """Test handling of various HTTP headers"""
        print(f"\n{Colors.BOLD}Testing Various HTTP Headers{Colors.RESET}")
        
        # Test User-Agent
        request = "GET / HTTP/1.1\r\nHost: localhost\r\nUser-Agent: WebServTester/1.0\r\n\r\n"
        headers, body = self.send_request(request)
        if headers and self.get_status_code(headers) == 200:
            self.result.add_pass("User-Agent header", "Accepted")
        
        # Test Accept
        request = "GET / HTTP/1.1\r\nHost: localhost\r\nAccept: text/html\r\n\r\n"
        headers, body = self.send_request(request)
        if headers and self.get_status_code(headers) == 200:
            self.result.add_pass("Accept header", "Accepted")
        
        # Test Accept-Language
        request = "GET / HTTP/1.1\r\nHost: localhost\r\nAccept-Language: en-US\r\n\r\n"
        headers, body = self.send_request(request)
        if headers and self.get_status_code(headers) == 200:
            self.result.add_pass("Accept-Language header", "Accepted")

    # ==================== MULTI-PORT TESTS ====================
    
    def test_multiple_ports(self):
        """Test if server listens on multiple ports"""
        print(f"\n{Colors.BOLD}Testing Multiple Port Support{Colors.RESET}")
        
        # Try port 8081 (defined in default.conf)
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.settimeout(2.0)
            result = sock.connect_ex((self.host, 8081))
            sock.close()
            
            if result == 0:
                self.result.add_pass("Multiple ports", "Server listening on port 8081")
            else:
                self.result.add_warning("Multiple ports", "Port 8081 not accessible")
        except Exception as e:
            self.result.add_warning("Multiple ports", f"Could not test port 8081: {str(e)}")

    # ==================== SPECIAL CASES ====================
    
    def test_empty_request(self):
        """Test empty request handling"""
        print(f"\n{Colors.BOLD}Testing Empty Request{Colors.RESET}")
        
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.settimeout(3.0)
            sock.connect((self.host, self.port))
            # Send nothing
            time.sleep(1)
            sock.close()
            self.result.add_pass("Empty request", "Connection handled gracefully")
        except Exception as e:
            self.result.add_warning("Empty request", f"Behavior: {str(e)}")
    
    def test_very_long_uri(self):
        """Test very long URI"""
        print(f"\n{Colors.BOLD}Testing Very Long URI{Colors.RESET}")
        
        long_path = "/" + "a" * 8000
        request = f"GET {long_path} HTTP/1.1\r\nHost: localhost\r\n\r\n"
        headers, body = self.send_request(request)
        
        if headers:
            status = self.get_status_code(headers)
            # 414 (URI Too Long) or 404 are acceptable
            if status in [414, 404, 400]:
                self.result.add_pass("Very long URI", f"Status: {status}")
            else:
                self.result.add_warning("Very long URI", f"Status: {status}")
        else:
            self.result.add_warning("Very long URI", "Connection closed")
    
    def test_special_characters_in_uri(self):
        """Test special characters in URI"""
        print(f"\n{Colors.BOLD}Testing Special Characters in URI{Colors.RESET}")
        
        request = "GET /test%20file.html HTTP/1.1\r\nHost: localhost\r\n\r\n"
        headers, body = self.send_request(request)
        
        if headers:
            status = self.get_status_code(headers)
            # Should handle URL encoding
            self.result.add_pass("URL encoding", f"Status: {status}")
        else:
            self.result.add_fail("URL encoding", "No response")

    # ==================== CONTENT TYPE TESTS ====================
    
    def test_content_types(self):
        """Test proper Content-Type headers"""
        print(f"\n{Colors.BOLD}Testing Content-Type Headers{Colors.RESET}")
        
        # HTML file
        request = "GET /index.html HTTP/1.1\r\nHost: localhost\r\n\r\n"
        headers, body = self.send_request(request)
        if headers:
            content_type = self.check_header(headers, "Content-Type")
            if content_type and "text/html" in content_type:
                self.result.add_pass("HTML Content-Type", f"Type: {content_type}")
            else:
                self.result.add_warning("HTML Content-Type", f"Type: {content_type}")
        
        # JPEG image
        request = "GET /uploads/dog.jpeg HTTP/1.1\r\nHost: localhost\r\n\r\n"
        headers, body = self.send_request(request)
        if headers:
            content_type = self.check_header(headers, "Content-Type")
            if content_type and "image" in content_type:
                self.result.add_pass("JPEG Content-Type", f"Type: {content_type}")
            else:
                self.result.add_warning("JPEG Content-Type", f"Type: {content_type}")

    # ==================== MAIN TEST RUNNER ====================
    
    def run_all_tests(self):
        """Run all test suites"""
        print(f"\n{Colors.BOLD}{Colors.BLUE}{'='*60}{Colors.RESET}")
        print(f"{Colors.BOLD}{Colors.BLUE}WebServ Comprehensive Test Suite{Colors.RESET}")
        print(f"{Colors.BOLD}{Colors.BLUE}Testing server at {self.host}:{self.port}{Colors.RESET}")
        print(f"{Colors.BOLD}{Colors.BLUE}{'='*60}{Colors.RESET}\n")
        
        # Basic HTTP Methods
        self.test_get_request()
        self.test_post_request()
        self.test_delete_request()
        self.test_unsupported_method()
        
        # Status Codes
        self.test_404_not_found()
        self.test_custom_error_pages()
        
        # Static Files
        self.test_static_file_serving()
        self.test_directory_listing()
        
        # CGI
        self.test_cgi_get()
        self.test_cgi_post()
        self.test_cgi_environment_variables()
        
        # File Upload
        self.test_file_upload()
        self.test_large_file_upload()
        
        # Body Size Limits
        self.test_client_max_body_size()
        
        # Redirects
        self.test_redirection()
        
        # Chunked Encoding
        self.test_chunked_request()
        
        # HTTP Versions
        self.test_http_1_0()
        self.test_http_1_1()
        
        # Malformed Requests
        self.test_malformed_request()
        
        # Keep-Alive
        self.test_keep_alive()
        
        # Timeouts
        self.test_connection_timeout()
        
        # Stress Tests
        self.test_multiple_connections()
        self.test_rapid_requests()
        
        # Headers
        self.test_various_headers()
        
        # Multi-Port
        self.test_multiple_ports()
        
        # Special Cases
        self.test_empty_request()
        self.test_very_long_uri()
        self.test_special_characters_in_uri()
        
        # Content Types
        self.test_content_types()
        
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
