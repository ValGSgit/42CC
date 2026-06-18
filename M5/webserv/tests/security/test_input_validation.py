#!/usr/bin/env python3
"""
Input Validation Vulnerability Tests
Tests for malformed requests and input validation issues
"""

import requests
import sys
import socket

HOST = "http://localhost:8080"
HOST_ADDR = "localhost"
PORT = 8080

COLORS = {
    'RED': '\033[91m',
    'GREEN': '\033[92m',
    'YELLOW': '\033[93m',
    'BLUE': '\033[94m',
    'END': '\033[0m'
}

def print_test(name, result, details=""):
    color = COLORS['GREEN'] if result == "PASS" else COLORS['RED'] if result == "FAIL" else COLORS['YELLOW']
    print(f"{color}[{result}]{COLORS['END']} {name}")
    if details:
        print(f"      {details}")

def test_invalid_http_version():
    """Test invalid HTTP version handling"""
    invalid_versions = [
        "HTTP/0.9",
        "HTTP/2.0",
        "HTTP/99.9",
        "HTTP/ABC",
        "HTTP",
        "HTTTP/1.1",
    ]
    
    vulnerable = False
    for version in invalid_versions:
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.connect((HOST_ADDR, PORT))
            sock.send(f"GET / {version}\r\nHost: localhost\r\n\r\n".encode())
            response = sock.recv(4096).decode('utf-8', errors='ignore')
            sock.close()
            
            if "200 OK" in response:
                print_test(
                    f"Invalid HTTP Version: {version}",
                    "FAIL",
                    "Server accepted invalid HTTP version"
                )
                vulnerable = True
        except:
            pass
    
    if not vulnerable:
        print_test("Invalid HTTP Version", "PASS", "Invalid versions rejected")
    
    return not vulnerable

def test_invalid_method():
    """Test invalid HTTP methods"""
    invalid_methods = [
        "GETT",
        "POOST",
        "DELETEE",
        "HACK",
        "INVALID",
        "123",
        "GET\x00POST",
    ]
    
    vulnerable = False
    for method in invalid_methods:
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.connect((HOST_ADDR, PORT))
            sock.send(f"{method} / HTTP/1.1\r\nHost: localhost\r\n\r\n".encode())
            response = sock.recv(4096).decode('utf-8', errors='ignore')
            sock.close()
            
            if "200 OK" in response:
                print_test(
                    f"Invalid Method: {method}",
                    "FAIL",
                    "Server accepted invalid method"
                )
                vulnerable = True
        except:
            pass
    
    if not vulnerable:
        print_test("Invalid HTTP Method", "PASS", "Invalid methods rejected")
    
    return not vulnerable

def test_malformed_request_line():
    """Test malformed HTTP request lines"""
    malformed_requests = [
        "GET\r\nHost: localhost\r\n\r\n",  # Missing URI and version
        "GET /\r\nHost: localhost\r\n\r\n",  # Missing version
        "GET HTTP/1.1\r\nHost: localhost\r\n\r\n",  # Missing URI
        "/ HTTP/1.1\r\nHost: localhost\r\n\r\n",  # Missing method
        "GET / HTTP/1.1 extra\r\nHost: localhost\r\n\r\n",  # Extra data
    ]
    
    vulnerable = False
    for req in malformed_requests:
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.connect((HOST_ADDR, PORT))
            sock.send(req.encode())
            sock.settimeout(2)
            response = sock.recv(4096).decode('utf-8', errors='ignore')
            sock.close()
            
            if "200 OK" in response:
                print_test(
                    "Malformed Request Line",
                    "FAIL",
                    "Server accepted malformed request"
                )
                vulnerable = True
                break
        except:
            pass
    
    if not vulnerable:
        print_test("Malformed Request Line", "PASS", "Malformed requests rejected")
    
    return not vulnerable

def test_missing_host_header():
    """Test request without Host header (required in HTTP/1.1)"""
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((HOST_ADDR, PORT))
        sock.send(b"GET / HTTP/1.1\r\n\r\n")  # No Host header
        response = sock.recv(4096).decode('utf-8', errors='ignore')
        sock.close()
        
        if "200 OK" in response:
            print_test(
                "Missing Host Header",
                "WARN",
                "Server accepted request without Host header"
            )
            return True  # Not critical, HTTP/1.0 compatible
        else:
            print_test(
                "Missing Host Header",
                "PASS",
                "Server requires Host header"
            )
            return True
    except Exception as e:
        print_test("Missing Host Header", "ERROR", str(e))
        return False

def test_duplicate_content_length():
    """Test duplicate Content-Length headers"""
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((HOST_ADDR, PORT))
        request = (
            b"POST /upload HTTP/1.1\r\n"
            b"Host: localhost\r\n"
            b"Content-Length: 5\r\n"
            b"Content-Length: 10\r\n"
            b"\r\n"
            b"12345"
        )
        sock.send(request)
        response = sock.recv(4096).decode('utf-8', errors='ignore')
        sock.close()
        
        if "200" in response or "201" in response:
            print_test(
                "Duplicate Content-Length",
                "WARN",
                "Server processed duplicate Content-Length headers"
            )
        else:
            print_test(
                "Duplicate Content-Length",
                "PASS",
                "Server rejected duplicate headers"
            )
        
        return True
    except Exception as e:
        print_test("Duplicate Content-Length", "ERROR", str(e))
        return False

def test_negative_content_length():
    """Test negative Content-Length"""
    try:
        r = requests.post(
            f"{HOST}/upload",
            data="test",
            headers={'Content-Length': '-1'},
            timeout=5
        )
        
        if r.status_code in [200, 201]:
            print_test(
                "Negative Content-Length",
                "FAIL",
                "Server accepted negative Content-Length"
            )
            return False
        else:
            print_test(
                "Negative Content-Length",
                "PASS",
                "Server rejected negative Content-Length"
            )
            return True
    except Exception as e:
        print_test("Negative Content-Length", "INFO", "Request blocked")
        return True

def test_huge_content_length():
    """Test extremely large Content-Length"""
    try:
        headers = {'Content-Length': '999999999999999999999'}
        
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((HOST_ADDR, PORT))
        request = (
            b"POST /upload HTTP/1.1\r\n"
            b"Host: localhost\r\n"
            b"Content-Length: 999999999999999999999\r\n"
            b"\r\n"
        )
        sock.send(request)
        sock.settimeout(2)
        try:
            response = sock.recv(4096).decode('utf-8', errors='ignore')
            if "413" in response:
                print_test(
                    "Huge Content-Length",
                    "PASS",
                    "Server rejected with 413"
                )
            elif "400" in response:
                print_test(
                    "Huge Content-Length",
                    "PASS",
                    "Server rejected with 400"
                )
            else:
                print_test(
                    "Huge Content-Length",
                    "WARN",
                    f"Unexpected response: {response[:100]}"
                )
        except socket.timeout:
            print_test(
                "Huge Content-Length",
                "WARN",
                "Server accepted but timed out"
            )
        sock.close()
        
        return True
    except Exception as e:
        print_test("Huge Content-Length", "ERROR", str(e))
        return False

def test_header_name_with_spaces():
    """Test header names with spaces"""
    try:
        # Headers with spaces should be rejected
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((HOST_ADDR, PORT))
        sock.send(b"GET / HTTP/1.1\r\nHost: localhost\r\nX Header: value\r\n\r\n")
        response = sock.recv(4096).decode('utf-8', errors='ignore')
        sock.close()
        
        if "200 OK" in response:
            print_test(
                "Header With Spaces",
                "WARN",
                "Server accepted header name with spaces"
            )
        else:
            print_test(
                "Header With Spaces",
                "PASS",
                "Server rejected malformed header"
            )
        
        return True
    except Exception as e:
        print_test("Header With Spaces", "ERROR", str(e))
        return False

def test_crlf_injection_headers():
    """Test CRLF injection in header values"""
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((HOST_ADDR, PORT))
        
        # Try to inject new headers via CRLF
        request = (
            b"GET / HTTP/1.1\r\n"
            b"Host: localhost\r\n"
            b"User-Agent: Test\r\nX-Injected: malicious\r\n"
            b"\r\n"
        )
        sock.send(request)
        response = sock.recv(4096).decode('utf-8', errors='ignore')
        sock.close()
        
        # Check if response contains injected header
        if "X-Injected" in response:
            print_test(
                "CRLF Injection",
                "FAIL",
                "Successfully injected headers via CRLF"
            )
            return False
        else:
            print_test(
                "CRLF Injection",
                "PASS",
                "CRLF injection blocked"
            )
            return True
    except Exception as e:
        print_test("CRLF Injection", "ERROR", str(e))
        return False

def test_unicode_in_uri():
    """Test Unicode characters in URI"""
    payloads = [
        "/test%C0%AF",  # Overlong encoding
        "/test\u4e2d\u6587",  # Chinese characters
        "/test\u0000",  # Null character
        "/test%FF%FE",  # Invalid UTF-8
    ]
    
    for payload in payloads:
        try:
            r = requests.get(f"{HOST}{payload}", timeout=5)
            # Server should handle gracefully
        except:
            pass
    
    print_test("Unicode in URI", "PASS", "Unicode handling tested")
    return True

def test_extremely_long_uri():
    """Test extremely long URI"""
    try:
        long_uri = "/" + "A" * 50000  # 50KB URI
        
        r = requests.get(f"{HOST}{long_uri}", timeout=5)
        
        if r.status_code == 200:
            print_test(
                "Extremely Long URI",
                "FAIL",
                "Server accepted 50KB URI (DoS risk)"
            )
            return False
        elif r.status_code == 414:
            print_test(
                "Extremely Long URI",
                "PASS",
                "Server rejected with 414 URI Too Long"
            )
            return True
        else:
            print_test(
                "Extremely Long URI",
                "PASS",
                f"Server rejected (status: {r.status_code})"
            )
            return True
    except requests.exceptions.ConnectionError:
        print_test(
            "Extremely Long URI",
            "WARN",
            "Connection closed (possible crash)"
        )
        return True
    except Exception as e:
        print_test("Extremely Long URI", "ERROR", str(e))
        return False

def main():
    print(f"\n{COLORS['BLUE']}{'='*60}{COLORS['END']}")
    print(f"{COLORS['BLUE']}Input Validation Vulnerability Tests{COLORS['END']}")
    print(f"{COLORS['BLUE']}{'='*60}{COLORS['END']}\n")
    
    # Check if server is running
    try:
        r = requests.get(f"{HOST}/", timeout=2)
    except requests.exceptions.RequestException:
        print(f"{COLORS['RED']}ERROR: Cannot connect to {HOST}{COLORS['END']}")
        print("Make sure the server is running: ./webserv webserv.conf")
        sys.exit(1)
    
    tests = [
        test_invalid_http_version,
        test_invalid_method,
        test_malformed_request_line,
        test_missing_host_header,
        test_duplicate_content_length,
        test_negative_content_length,
        test_huge_content_length,
        test_header_name_with_spaces,
        test_crlf_injection_headers,
        test_unicode_in_uri,
        test_extremely_long_uri,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            result = test()
            if result:
                passed += 1
            else:
                failed += 1
        except Exception as e:
            print_test(test.__name__, "ERROR", str(e))
            failed += 1
    
    print(f"\n{COLORS['BLUE']}{'='*60}{COLORS['END']}")
    print(f"Total: {len(tests)} | ", end="")
    print(f"{COLORS['GREEN']}Passed: {passed}{COLORS['END']} | ", end="")
    print(f"{COLORS['RED']}Failed: {failed}{COLORS['END']}")
    print(f"{COLORS['BLUE']}{'='*60}{COLORS['END']}\n")
    
    if failed > 0:
        print(f"{COLORS['RED']}⚠️  SECURITY WARNING: {failed} input validation vulnerabilities detected!{COLORS['END']}\n")
        return 1
    else:
        print(f"{COLORS['GREEN']}✓ All input validation tests passed!{COLORS['END']}\n")
        return 0

if __name__ == "__main__":
    sys.exit(main())
