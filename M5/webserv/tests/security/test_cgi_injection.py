#!/usr/bin/env python3
"""
CGI Injection Vulnerability Tests
Tests for command injection and environment variable manipulation in CGI scripts
"""

import requests
import sys
import urllib.parse

HOST = "http://localhost:8080"
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

def test_cgi_query_injection():
    """Test command injection through query string"""
    payloads = [
        "?cmd=$(whoami)",
        "?param=value; cat /etc/passwd",
        "?input=`id`",
        "?data=test|whoami",
        "?file=test;ls -la /",
        "?x=$(cat /etc/passwd)",
    ]
    
    vulnerable = False
    for payload in payloads:
        try:
            r = requests.get(f"{HOST}/cgi-bin/test.py{payload}", timeout=5)
            
            # Check if command output appears in response
            if r.status_code == 200:
                content_lower = r.text.lower()
                # Look for command execution indicators
                if any(indicator in content_lower for indicator in ['uid=', 'root:', 'daemon:', 'www-data']):
                    print_test(
                        f"CGI Query Injection: {payload}",
                        "FAIL",
                        "Command injection succeeded! Output in response"
                    )
                    vulnerable = True
                    print(f"      Response snippet: {r.text[:200]}")
        except Exception as e:
            pass
    
    if not vulnerable:
        print_test("CGI Query Injection", "PASS", "Query string injection blocked")
    
    return not vulnerable

def test_cgi_header_injection():
    """Test command injection through HTTP headers"""
    headers_to_test = [
        ('User-Agent', '$(whoami)'),
        ('User-Agent', '; cat /etc/passwd #'),
        ('Referer', '`id`'),
        ('X-Custom', '|ls -la'),
        ('Accept-Language', '$(uname -a)'),
        ('Cookie', 'session=abc; rm -rf / #'),
    ]
    
    vulnerable = False
    for header_name, header_value in headers_to_test:
        try:
            headers = {header_name: header_value}
            r = requests.get(f"{HOST}/cgi-bin/test.py", headers=headers, timeout=5)
            
            if r.status_code == 200:
                content_lower = r.text.lower()
                if any(indicator in content_lower for indicator in ['uid=', 'root:', 'linux', 'darwin']):
                    print_test(
                        f"CGI Header Injection: {header_name}={header_value}",
                        "FAIL",
                        "Header injection succeeded!"
                    )
                    vulnerable = True
        except Exception as e:
            pass
    
    if not vulnerable:
        print_test("CGI Header Injection", "PASS", "Header injection blocked")
    
    return not vulnerable

def test_cgi_post_injection():
    """Test command injection through POST data"""
    payloads = [
        "cmd=$(whoami)",
        "input=value; cat /etc/passwd",
        "data=`id`",
        "param=test|ls",
    ]
    
    vulnerable = False
    for payload in payloads:
        try:
            r = requests.post(
                f"{HOST}/cgi-bin/test.py",
                data=payload,
                headers={'Content-Type': 'application/x-www-form-urlencoded'},
                timeout=5
            )
            
            if r.status_code == 200:
                content_lower = r.text.lower()
                if any(indicator in content_lower for indicator in ['uid=', 'root:', 'daemon:']):
                    print_test(
                        f"CGI POST Injection: {payload}",
                        "FAIL",
                        "POST injection succeeded!"
                    )
                    vulnerable = True
        except Exception as e:
            pass
    
    if not vulnerable:
        print_test("CGI POST Injection", "PASS", "POST injection blocked")
    
    return not vulnerable

def test_cgi_newline_injection():
    """Test CRLF injection in CGI"""
    payloads = [
        "test\r\nSet-Cookie: admin=true",
        "value\nX-Injected: true",
        "data\r\n\r\n<script>alert(1)</script>",
    ]
    
    vulnerable = False
    for payload in payloads:
        try:
            r = requests.get(
                f"{HOST}/cgi-bin/test.py",
                params={'input': payload},
                timeout=5
            )
            
            # Check if injected headers appear
            if 'Set-Cookie' in r.headers or 'X-Injected' in r.headers:
                print_test(
                    f"CGI CRLF Injection",
                    "FAIL",
                    "CRLF injection succeeded! Headers manipulated"
                )
                vulnerable = True
        except Exception as e:
            pass
    
    if not vulnerable:
        print_test("CGI CRLF Injection", "PASS", "CRLF injection blocked")
    
    return not vulnerable

def test_cgi_environment_pollution():
    """Test environment variable pollution"""
    try:
        # Try to set dangerous environment variables
        headers = {
            'HTTP_PROXY': 'http://attacker.com:8080',
            'LD_PRELOAD': '/tmp/malicious.so',
            'PATH': '/tmp:/usr/bin',
        }
        
        r = requests.get(f"{HOST}/cgi-bin/test.py", headers=headers, timeout=5)
        
        # This is hard to detect automatically, but we check if server crashes
        if r.status_code == 500:
            print_test(
                "CGI Environment Pollution",
                "WARN",
                "Server returned 500 (possible environment issue)"
            )
        else:
            print_test(
                "CGI Environment Pollution",
                "INFO",
                f"Server handled custom headers (status: {r.status_code})"
            )
        
        return True
    except Exception as e:
        print_test("CGI Environment Pollution", "ERROR", str(e))
        return False

def test_cgi_null_byte():
    """Test null byte injection in CGI parameters"""
    payloads = [
        "test.py\x00.jpg",
        "value\x00; cat /etc/passwd",
        "param=test\x00|id",
    ]
    
    vulnerable = False
    for payload in payloads:
        try:
            # URL encode the null byte
            encoded = urllib.parse.quote(payload)
            r = requests.get(f"{HOST}/cgi-bin/test.py?input={encoded}", timeout=5)
            
            if r.status_code == 200:
                content_lower = r.text.lower()
                if 'uid=' in content_lower or 'root:' in content_lower:
                    print_test(
                        f"CGI Null Byte Injection",
                        "FAIL",
                        "Null byte injection succeeded!"
                    )
                    vulnerable = True
        except Exception as e:
            pass
    
    if not vulnerable:
        print_test("CGI Null Byte Injection", "PASS", "Null byte injection blocked")
    
    return not vulnerable

def test_cgi_script_timeout():
    """Test if CGI timeout is enforced"""
    try:
        # Try to run a slow script
        r = requests.get(
            f"{HOST}/cgi-bin/test.py?sleep=60",
            timeout=35  # Slightly longer than CGI_TIMEOUT
        )
        
        print_test(
            "CGI Script Timeout",
            "WARN",
            "Script completed (timeout may not be enforced)"
        )
        return True
    except requests.exceptions.Timeout:
        print_test(
            "CGI Script Timeout",
            "PASS",
            "Request timed out (timeout enforced)"
        )
        return True
    except Exception as e:
        print_test("CGI Script Timeout", "ERROR", str(e))
        return False

def test_cgi_script_execution_limit():
    """Test if multiple CGI processes can exhaust resources"""
    import threading
    import time
    
    try:
        results = []
        start_time = time.time()
        
        def request_cgi():
            try:
                r = requests.get(f"{HOST}/cgi-bin/test.py", timeout=5)
                results.append(r.status_code)
            except:
                results.append(0)
        
        # Start 50 simultaneous CGI requests
        threads = []
        for _ in range(50):
            t = threading.Thread(target=request_cgi)
            threads.append(t)
            t.start()
        
        for t in threads:
            t.join()
        
        elapsed = time.time() - start_time
        successful = sum(1 for s in results if s == 200)
        
        if successful == 50 and elapsed < 10:
            print_test(
                "CGI Execution Limit",
                "WARN",
                f"All 50 CGI processes completed in {elapsed:.1f}s (no limit enforced)"
            )
        else:
            print_test(
                "CGI Execution Limit",
                "INFO",
                f"{successful}/50 successful in {elapsed:.1f}s"
            )
        
        return True
    except Exception as e:
        print_test("CGI Execution Limit", "ERROR", str(e))
        return False

def test_cgi_path_info():
    """Test PATH_INFO manipulation"""
    payloads = [
        "/cgi-bin/test.py/../../etc/passwd",
        "/cgi-bin/test.py/../../../etc/shadow",
        "/cgi-bin/test.py/./././../../../etc/passwd",
    ]
    
    vulnerable = False
    for payload in payloads:
        try:
            r = requests.get(f"{HOST}{payload}", timeout=5)
            
            if r.status_code == 200:
                content_lower = r.text.lower()
                if 'root:' in content_lower or 'daemon:' in content_lower:
                    print_test(
                        f"CGI PATH_INFO: {payload}",
                        "FAIL",
                        "PATH_INFO manipulation succeeded!"
                    )
                    vulnerable = True
        except Exception as e:
            pass
    
    if not vulnerable:
        print_test("CGI PATH_INFO", "PASS", "PATH_INFO manipulation blocked")
    
    return not vulnerable

def test_cgi_interpreter_injection():
    """Test if CGI interpreter path can be manipulated"""
    # This tests if uploading a malicious script can execute
    try:
        # Try to access a script that shouldn't exist
        r = requests.get(f"{HOST}/cgi-bin/../../tmp/malicious.py", timeout=5)
        
        if r.status_code == 200:
            print_test(
                "CGI Interpreter Injection",
                "FAIL",
                "CGI executed script outside cgi-bin!"
            )
            return False
        else:
            print_test(
                "CGI Interpreter Injection",
                "PASS",
                "CGI path validation working"
            )
            return True
    except Exception as e:
        print_test("CGI Interpreter Injection", "ERROR", str(e))
        return False

def test_cgi_shell_metacharacters():
    """Test shell metacharacters in CGI parameters"""
    metacharacters = ['&', '|', ';', '\n', '$()', '``', '>', '<', '*', '?', '[', ']', '{', '}']
    
    vulnerable = False
    for meta in metacharacters:
        try:
            r = requests.get(
                f"{HOST}/cgi-bin/test.sh",
                params={'input': f"test{meta}whoami"},
                timeout=5
            )
            
            if r.status_code == 200:
                content_lower = r.text.lower()
                if 'uid=' in content_lower or any(u in content_lower for u in ['root', 'www-data', 'nobody']):
                    print_test(
                        f"CGI Shell Metachar: '{meta}'",
                        "FAIL",
                        "Shell metacharacter injection succeeded!"
                    )
                    vulnerable = True
        except Exception as e:
            pass
    
    if not vulnerable:
        print_test("CGI Shell Metacharacters", "PASS", "Shell metacharacters handled safely")
    
    return not vulnerable

def main():
    print(f"\n{COLORS['BLUE']}{'='*60}{COLORS['END']}")
    print(f"{COLORS['BLUE']}CGI Injection Vulnerability Tests{COLORS['END']}")
    print(f"{COLORS['BLUE']}{'='*60}{COLORS['END']}\n")
    
    # Check if server is running
    try:
        r = requests.get(f"{HOST}/", timeout=2)
    except requests.exceptions.RequestException:
        print(f"{COLORS['RED']}ERROR: Cannot connect to {HOST}{COLORS['END']}")
        print("Make sure the server is running: ./webserv webserv.conf")
        sys.exit(1)
    
    # Check if CGI is available
    try:
        r = requests.get(f"{HOST}/cgi-bin/test.py", timeout=5)
        if r.status_code == 404:
            print(f"{COLORS['YELLOW']}WARNING: CGI script not found. Some tests may be skipped.{COLORS['END']}\n")
    except:
        pass
    
    tests = [
        test_cgi_query_injection,
        test_cgi_header_injection,
        test_cgi_post_injection,
        test_cgi_newline_injection,
        test_cgi_environment_pollution,
        test_cgi_null_byte,
        test_cgi_script_timeout,
        test_cgi_script_execution_limit,
        test_cgi_path_info,
        test_cgi_interpreter_injection,
        test_cgi_shell_metacharacters,
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
        print(f"{COLORS['RED']}⚠️  SECURITY WARNING: {failed} CGI injection vulnerabilities detected!{COLORS['END']}\n")
        return 1
    else:
        print(f"{COLORS['GREEN']}✓ All CGI injection tests passed!{COLORS['END']}\n")
        return 0

if __name__ == "__main__":
    sys.exit(main())
