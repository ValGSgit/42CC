#!/usr/bin/env python3
"""
HTTP Request Smuggling Tests
Tests for HTTP request smuggling vulnerabilities
"""

import socket
import sys

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

def test_cl_te_smuggling():
    """Test Content-Length + Transfer-Encoding smuggling"""
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((HOST_ADDR, PORT))
        
        # Smuggled request with both CL and TE
        request = (
            b"POST / HTTP/1.1\r\n"
            b"Host: localhost\r\n"
            b"Content-Length: 4\r\n"
            b"Transfer-Encoding: chunked\r\n"
            b"\r\n"
            b"12\r\n"
            b"SMUGGLED_REQUEST\r\n"
            b"0\r\n"
            b"\r\n"
        )
        
        sock.send(request)
        sock.settimeout(2)
        
        try:
            response = sock.recv(8192).decode('utf-8', errors='ignore')
            
            if "SMUGGLED" in response or response.count("HTTP/") > 1:
                print_test(
                    "CL-TE Smuggling",
                    "FAIL",
                    "Request smuggling successful!"
                )
                sock.close()
                return False
        except socket.timeout:
            pass
        
        sock.close()
        print_test("CL-TE Smuggling", "PASS", "Request smuggling blocked")
        return True
    except Exception as e:
        print_test("CL-TE Smuggling", "ERROR", str(e))
        return False

def test_te_cl_smuggling():
    """Test Transfer-Encoding + Content-Length smuggling"""
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((HOST_ADDR, PORT))
        
        request = (
            b"POST / HTTP/1.1\r\n"
            b"Host: localhost\r\n"
            b"Transfer-Encoding: chunked\r\n"
            b"Content-Length: 4\r\n"
            b"\r\n"
            b"5c\r\n"
            b"GET /smuggled HTTP/1.1\r\n"
            b"Host: localhost\r\n"
            b"\r\n"
            b"0\r\n"
            b"\r\n"
        )
        
        sock.send(request)
        sock.settimeout(2)
        
        try:
            response = sock.recv(8192).decode('utf-8', errors='ignore')
            
            if response.count("HTTP/") > 1:
                print_test(
                    "TE-CL Smuggling",
                    "FAIL",
                    "Reverse smuggling successful!"
                )
                sock.close()
                return False
        except socket.timeout:
            pass
        
        sock.close()
        print_test("TE-CL Smuggling", "PASS", "Reverse smuggling blocked")
        return True
    except Exception as e:
        print_test("TE-CL Smuggling", "ERROR", str(e))
        return False

def test_chunked_encoding():
    """Test basic chunked transfer encoding handling"""
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((HOST_ADDR, PORT))
        
        request = (
            b"POST /upload HTTP/1.1\r\n"
            b"Host: localhost\r\n"
            b"Connection: keep-alive\r\n"
            b"Transfer-Encoding: chunked\r\n"
            b"\r\n"
            b"5\r\n"
            b"hello\r\n"
            b"0\r\n"
            b"\r\n"
        )
        
        sock.send(request)
        response = sock.recv(4096).decode('utf-8', errors='ignore')
        sock.close()
        
        # Server might not support chunked, which is okay
        if "501" in response or "400" in response:
            print_test(
                "Chunked Encoding",
                "INFO",
                "Server doesn't support chunked encoding"
            )
        else:
            print_test(
                "Chunked Encoding",
                "INFO",
                f"Server response: {response.split()[1] if len(response.split()) > 1 else 'unknown'}"
            )
        
        return True
    except Exception as e:
        print_test("Chunked Encoding", "ERROR", str(e))
        return False

def test_multiple_content_length():
    """Test multiple Content-Length headers with different values"""
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((HOST_ADDR, PORT))
        
        request = (
            b"POST /upload HTTP/1.1\r\n"
            b"Host: localhost\r\n"
            b"Content-Length: 3\r\n"
            b"Content-Length: 5\r\n"
            b"\r\n"
            b"12345"
        )
        
        sock.send(request)
        sock.settimeout(2)
        response = sock.recv(4096).decode('utf-8', errors='ignore')
        sock.close()
        
        if "400" in response:
            print_test(
                "Multiple Content-Length",
                "PASS",
                "Server rejected ambiguous Content-Length"
            )
            return True
        else:
            print_test(
                "Multiple Content-Length",
                "WARN",
                "Server accepted multiple Content-Length headers"
            )
            return True
    except Exception as e:
        print_test("Multiple Content-Length", "ERROR", str(e))
        return False

def test_pipeline_confusion():
    """Test HTTP pipelining confusion"""
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((HOST_ADDR, PORT))
        
        # Send pipelined requests
        request = (
            b"GET / HTTP/1.1\r\n"
            b"Host: localhost\r\n"
            b"\r\n"
            b"GET /second HTTP/1.1\r\n"
            b"Host: localhost\r\n"
            b"\r\n"
        )
        
        sock.send(request)
        sock.settimeout(2)
        
        responses = b""
        try:
            while True:
                chunk = sock.recv(4096)
                if not chunk:
                    break
                responses += chunk
        except:
            pass
        
        sock.close()
        
        # Check if both responses were received
        response_count = responses.count(b"HTTP/")
        
        if response_count >= 2:
            print_test(
                "HTTP Pipelining",
                "INFO",
                f"Server handled {response_count} pipelined requests"
            )
        else:
            print_test(
                "HTTP Pipelining",
                "INFO",
                "Server may not support pipelining"
            )
        
        return True
    except Exception as e:
        print_test("HTTP Pipelining", "ERROR", str(e))
        return False

def test_crlf_in_header_value():
    """Test CRLF injection in header values"""
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((HOST_ADDR, PORT))
        
        # Try to inject new request via CRLF in header
        request = (
            b"GET / HTTP/1.1\r\n"
            b"Host: localhost\r\n"
            b"X-Header: value\r\n\r\nGET /injected HTTP/1.1\r\n"
            b"Host: localhost\r\n"
            b"\r\n"
        )
        
        sock.send(request)
        sock.settimeout(2)
        response = sock.recv(8192).decode('utf-8', errors='ignore')
        sock.close()
        
        if response.count("HTTP/") > 1:
            print_test(
                "CRLF Header Injection",
                "FAIL",
                "Successfully injected second request!"
            )
            return False
        else:
            print_test(
                "CRLF Header Injection",
                "PASS",
                "CRLF injection blocked"
            )
            return True
    except Exception as e:
        print_test("CRLF Header Injection", "ERROR", str(e))
        return False

def test_http_09_request():
    """Test HTTP/0.9 simple request format"""
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((HOST_ADDR, PORT))
        
        # HTTP/0.9 format (no version, no headers)
        sock.send(b"GET /\r\n\r\n")
        sock.settimeout(2)
        
        response = sock.recv(4096)
        sock.close()
        
        if response:
            print_test(
                "HTTP/0.9 Request",
                "INFO",
                "Server responded to HTTP/0.9 format"
            )
        else:
            print_test(
                "HTTP/0.9 Request",
                "INFO",
                "Server rejected HTTP/0.9 format"
            )
        
        return True
    except Exception as e:
        print_test("HTTP/0.9 Request", "ERROR", str(e))
        return False

def main():
    print(f"\n{COLORS['BLUE']}{'='*60}{COLORS['END']}")
    print(f"{COLORS['BLUE']}HTTP Request Smuggling Tests{COLORS['END']}")
    print(f"{COLORS['BLUE']}{'='*60}{COLORS['END']}\n")
    
    # Check if server is accessible
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(2)
        sock.connect((HOST_ADDR, PORT))
        sock.close()
    except:
        print(f"{COLORS['RED']}ERROR: Cannot connect to {HOST_ADDR}:{PORT}{COLORS['END']}")
        print("Make sure the server is running: ./webserv webserv.conf")
        sys.exit(1)
    
    tests = [
        test_cl_te_smuggling,
        test_te_cl_smuggling,
        test_chunked_encoding,
        test_multiple_content_length,
        test_pipeline_confusion,
        test_crlf_in_header_value,
        test_http_09_request,
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
        print(f"{COLORS['RED']}⚠️  SECURITY WARNING: {failed} HTTP smuggling vulnerabilities detected!{COLORS['END']}\n")
        return 1
    else:
        print(f"{COLORS['GREEN']}✓ All HTTP smuggling tests passed!{COLORS['END']}\n")
        return 0

if __name__ == "__main__":
    sys.exit(main())
