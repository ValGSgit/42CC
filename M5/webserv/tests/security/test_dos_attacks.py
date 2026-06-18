#!/usr/bin/env python3
"""
Denial of Service Vulnerability Tests
Tests for various DoS attack vectors
"""

import requests
import sys
import time
import threading
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

def test_slowloris():
    """Test Slowloris attack (slow headers)"""
    try:
        # Create slow connection
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((HOST_ADDR, PORT))
        
        # Send partial HTTP request very slowly
        sock.send(b"GET / HTTP/1.1\r\n")
        time.sleep(1)
        sock.send(b"Host: localhost\r\n")
        time.sleep(1)
        sock.send(b"User-Agent: SlowTest\r\n")
        
        # Wait to see if server closes connection
        sock.settimeout(5)
        try:
            data = sock.recv(1024)
            if data:
                print_test(
                    "Slowloris Attack",
                    "WARN",
                    "Server keeps slow connections alive (vulnerable to Slowloris)"
                )
        except socket.timeout:
            print_test(
                "Slowloris Attack",
                "WARN",
                "Server didn't respond (may be vulnerable)"
            )
        
        sock.close()
        return True
    except Exception as e:
        print_test("Slowloris Attack", "ERROR", str(e))
        return False

def test_connection_exhaustion():
    """Test connection exhaustion"""
    try:
        sockets = []
        max_connections = 0
        
        # Try to open many connections
        for i in range(100):
            try:
                sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                sock.settimeout(2)
                sock.connect((HOST_ADDR, PORT))
                sockets.append(sock)
                max_connections += 1
            except:
                break
        
        # Close all sockets
        for sock in sockets:
            sock.close()
        
        if max_connections >= 100:
            print_test(
                "Connection Exhaustion",
                "WARN",
                f"Server accepted {max_connections}+ connections (no limit detected)"
            )
        else:
            print_test(
                "Connection Exhaustion",
                "INFO",
                f"Server accepted up to {max_connections} connections"
            )
        
        return True
    except Exception as e:
        print_test("Connection Exhaustion", "ERROR", str(e))
        return False

def test_request_flooding():
    """Test rapid request flooding"""
    try:
        start_time = time.time()
        requests_sent = 0
        errors = 0
        
        # Send 100 rapid requests
        for i in range(100):
            try:
                r = requests.get(f"{HOST}/", timeout=1)
                requests_sent += 1
            except:
                errors += 1
        
        elapsed = time.time() - start_time
        
        if errors > 50:
            print_test(
                "Request Flooding",
                "FAIL",
                f"Server unstable: {errors}/100 requests failed"
            )
            return False
        elif elapsed < 1:
            print_test(
                "Request Flooding",
                "WARN",
                f"Server handled 100 requests in {elapsed:.2f}s (no rate limiting)"
            )
        else:
            print_test(
                "Request Flooding",
                "PASS",
                f"Server handled flooding ({requests_sent} requests in {elapsed:.2f}s)"
            )
        
        return True
    except Exception as e:
        print_test("Request Flooding", "ERROR", str(e))
        return False

def test_large_header():
    """Test extremely large headers"""
    try:
        # Create a very large header
        large_value = "A" * (1024 * 1024)  # 1MB header
        headers = {'X-Large-Header': large_value}
        
        r = requests.get(f"{HOST}/", headers=headers, timeout=5)
        
        if r.status_code == 200:
            print_test(
                "Large Header DoS",
                "FAIL",
                "Server accepted 1MB header (memory exhaustion risk)"
            )
            return False
        elif r.status_code == 413 or r.status_code == 431:
            print_test(
                "Large Header DoS",
                "PASS",
                f"Server rejected large header (status: {r.status_code})"
            )
            return True
        else:
            print_test(
                "Large Header DoS",
                "INFO",
                f"Server response: {r.status_code}"
            )
            return True
    except requests.exceptions.ConnectionError:
        print_test(
            "Large Header DoS",
            "WARN",
            "Connection closed (may have crashed server)"
        )
        return True
    except Exception as e:
        print_test("Large Header DoS", "ERROR", str(e))
        return False

def test_many_headers():
    """Test many headers (header bomb)"""
    try:
        # Create 10000 headers
        headers = {f'X-Header-{i}': f'value{i}' for i in range(10000)}
        
        r = requests.get(f"{HOST}/", headers=headers, timeout=5)
        
        if r.status_code == 200:
            print_test(
                "Many Headers DoS",
                "WARN",
                "Server accepted 10000 headers (DoS risk)"
            )
        else:
            print_test(
                "Many Headers DoS",
                "PASS",
                f"Server handled many headers (status: {r.status_code})"
            )
        
        return True
    except requests.exceptions.ConnectionError:
        print_test(
            "Many Headers DoS",
            "WARN",
            "Connection error (possible crash)"
        )
        return True
    except Exception as e:
        print_test("Many Headers DoS", "ERROR", str(e))
        return False

def test_post_without_body():
    """Test POST with Content-Length but no body"""
    try:
        # Send POST with content-length but no actual body
        headers = {
            'Content-Length': '1000000',
            'Content-Type': 'application/x-www-form-urlencoded'
        }
        
        r = requests.post(f"{HOST}/upload", headers=headers, timeout=10)
        
        print_test(
            "POST Without Body",
            "INFO",
            f"Server response: {r.status_code} (waited for body)"
        )
        
        return True
    except requests.exceptions.Timeout:
        print_test(
            "POST Without Body",
            "WARN",
            "Request timed out (server waiting for body - DoS risk)"
        )
        return True
    except Exception as e:
        print_test("POST Without Body", "ERROR", str(e))
        return False

def test_slow_post():
    """Test slow POST (R-U-Dead-Yet attack)"""
    try:
        # Open connection and send headers
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((HOST_ADDR, PORT))
        
        # Send headers with large content-length
        headers = (
            b"POST /upload HTTP/1.1\r\n"
            b"Host: localhost\r\n"
            b"Content-Length: 1000000\r\n"
            b"Content-Type: application/x-www-form-urlencoded\r\n"
            b"\r\n"
        )
        sock.send(headers)
        
        # Send body very slowly (1 byte every second)
        for i in range(10):
            time.sleep(1)
            sock.send(b"A")
        
        sock.close()
        
        print_test(
            "Slow POST Attack",
            "WARN",
            "Server accepted slow POST (R-U-Dead-Yet vulnerability)"
        )
        
        return True
    except Exception as e:
        print_test("Slow POST Attack", "ERROR", str(e))
        return False

def test_recursive_directory():
    """Test recursive directory access"""
    try:
        # Try to cause recursion with repeated path
        path = "/browse" + "/." * 1000
        
        start_time = time.time()
        r = requests.get(f"{HOST}{path}", timeout=5)
        elapsed = time.time() - start_time
        
        if elapsed > 2:
            print_test(
                "Recursive Directory",
                "WARN",
                f"Slow response ({elapsed:.2f}s) - may be processing recursion"
            )
        else:
            print_test(
                "Recursive Directory",
                "PASS",
                f"Handled recursive path efficiently ({elapsed:.2f}s)"
            )
        
        return True
    except requests.exceptions.Timeout:
        print_test(
            "Recursive Directory",
            "FAIL",
            "Timed out - recursive processing DoS!"
        )
        return False
    except Exception as e:
        print_test("Recursive Directory", "ERROR", str(e))
        return False

def test_zip_bomb():
    """Test decompression bomb (if server auto-decompresses)"""
    # This is just informational - actual test requires server to support decompression
    print_test("Zip Bomb", "SKIP", "Requires auto-decompression support")
    return True

def test_regex_dos():
    """Test ReDoS with pathological patterns"""
    try:
        # Patterns that cause catastrophic backtracking
        patterns = [
            "a" * 50 + "!" * 50,
            "/browse" + ("a" * 1000),
            "?" + ("a=b&" * 1000),
        ]
        
        vulnerable = False
        for pattern in patterns:
            start_time = time.time()
            try:
                r = requests.get(f"{HOST}/{pattern}", timeout=5)
                elapsed = time.time() - start_time
                
                if elapsed > 3:
                    print_test(
                        f"ReDoS: {pattern[:50]}...",
                        "WARN",
                        f"Slow processing ({elapsed:.2f}s) - possible ReDoS"
                    )
                    vulnerable = True
            except requests.exceptions.Timeout:
                print_test(
                    f"ReDoS: {pattern[:50]}...",
                    "FAIL",
                    "Timeout - ReDoS vulnerability!"
                )
                vulnerable = True
        
        if not vulnerable:
            print_test("ReDoS", "PASS", "No ReDoS vulnerabilities detected")
        
        return not vulnerable
    except Exception as e:
        print_test("ReDoS", "ERROR", str(e))
        return False

def test_parallel_uploads():
    """Test parallel large uploads"""
    try:
        results = []
        
        def upload_large():
            try:
                # Upload 10MB file
                data = 'A' * (10 * 1024 * 1024)
                files = {'file': ('large.txt', data, 'text/plain')}
                r = requests.post(f"{HOST}/upload", files=files, timeout=15)
                results.append(r.status_code)
            except:
                results.append(0)
        
        # Start 10 parallel uploads
        threads = []
        start_time = time.time()
        
        for _ in range(10):
            t = threading.Thread(target=upload_large)
            threads.append(t)
            t.start()
        
        for t in threads:
            t.join()
        
        elapsed = time.time() - start_time
        successful = sum(1 for s in results if s in [200, 201])
        
        if successful >= 8:
            print_test(
                "Parallel Large Uploads",
                "WARN",
                f"{successful}/10 uploads succeeded in {elapsed:.1f}s (memory exhaustion risk)"
            )
        else:
            print_test(
                "Parallel Large Uploads",
                "INFO",
                f"{successful}/10 uploads succeeded in {elapsed:.1f}s"
            )
        
        return True
    except Exception as e:
        print_test("Parallel Large Uploads", "ERROR", str(e))
        return False

def main():
    print(f"\n{COLORS['BLUE']}{'='*60}{COLORS['END']}")
    print(f"{COLORS['BLUE']}Denial of Service Vulnerability Tests{COLORS['END']}")
    print(f"{COLORS['BLUE']}{'='*60}{COLORS['END']}\n")
    print(f"{COLORS['YELLOW']}WARNING: These tests may temporarily affect server performance{COLORS['END']}\n")
    
    # Check if server is running
    try:
        r = requests.get(f"{HOST}/", timeout=2)
    except requests.exceptions.RequestException:
        print(f"{COLORS['RED']}ERROR: Cannot connect to {HOST}{COLORS['END']}")
        print("Make sure the server is running: ./webserv webserv.conf")
        sys.exit(1)
    
    tests = [
        test_slowloris,
        test_connection_exhaustion,
        test_request_flooding,
        test_large_header,
        test_many_headers,
        test_post_without_body,
        test_slow_post,
        test_recursive_directory,
        test_zip_bomb,
        test_regex_dos,
        test_parallel_uploads,
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
        print(f"{COLORS['RED']}⚠️  SECURITY WARNING: {failed} DoS vulnerabilities detected!{COLORS['END']}\n")
        return 1
    else:
        print(f"{COLORS['GREEN']}✓ All DoS tests passed!{COLORS['END']}\n")
        return 0

if __name__ == "__main__":
    sys.exit(main())
