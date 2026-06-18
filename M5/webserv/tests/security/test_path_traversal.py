#!/usr/bin/env python3
"""
Path Traversal Vulnerability Tests
Tests for directory traversal attacks to access files outside web root
"""

import requests
import sys
import os

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

def test_basic_path_traversal():
    """Test basic ../ path traversal"""
    payloads = [
        "/../../../etc/passwd",
        "/browse/../../../etc/passwd",
        "/../etc/shadow",
        "/./../../etc/passwd",
        "/./../etc/passwd",
    ]
    
    vulnerable = False
    for payload in payloads:
        try:
            r = requests.get(f"{HOST}{payload}", timeout=5)
            # If we get content that looks like a password file, it's vulnerable
            if r.status_code == 200 and ("root:" in r.text or "daemon:" in r.text):
                print_test(
                    f"Basic Path Traversal: {payload}",
                    "FAIL",
                    f"Successfully accessed system file! Status: {r.status_code}"
                )
                vulnerable = True
            elif r.status_code == 200:
                print_test(
                    f"Basic Path Traversal: {payload}",
                    "WARN",
                    f"Got 200 OK, content may be system file (length: {len(r.text)})"
                )
                vulnerable = True
        except Exception as e:
            pass
    
    if not vulnerable:
        print_test("Basic Path Traversal", "PASS", "Path traversal blocked")
    
    return not vulnerable

def test_encoded_path_traversal():
    """Test URL-encoded path traversal"""
    payloads = [
        "/%2e%2e/%2e%2e/%2e%2e/etc/passwd",  # URL encoded ../
        "/%2e%2e%2f%2e%2e%2f%2e%2e%2fetc%2fpasswd",  # Fully encoded
        "/..%2f..%2f..%2fetc%2fpasswd",  # Mixed encoding
        "/%252e%252e%252f%252e%252e%252fetc%252fpasswd",  # Double encoded
    ]
    
    vulnerable = False
    for payload in payloads:
        try:
            r = requests.get(f"{HOST}{payload}", timeout=5)
            if r.status_code == 200 and ("root:" in r.text or "daemon:" in r.text):
                print_test(
                    f"Encoded Path Traversal: {payload}",
                    "FAIL",
                    f"Successfully bypassed with encoding! Status: {r.status_code}"
                )
                vulnerable = True
        except Exception as e:
            pass
    
    if not vulnerable:
        print_test("Encoded Path Traversal", "PASS", "Encoded traversal blocked")
    
    return not vulnerable

def test_absolute_path_access():
    """Test absolute path access"""
    payloads = [
        "/etc/passwd",
        "/etc/shadow",
        "/home",
        "/root",
        "/var/log/",
    ]
    
    vulnerable = False
    for payload in payloads:
        try:
            r = requests.get(f"{HOST}{payload}", timeout=5)
            # Should get 404, not 200 with system file content
            if r.status_code == 200:
                print_test(
                    f"Absolute Path Access: {payload}",
                    "WARN",
                    f"Got 200 OK for system path (may be vulnerable)"
                )
                if "root:" in r.text or "daemon:" in r.text:
                    print_test(
                        f"Absolute Path Access: {payload}",
                        "FAIL",
                        "Successfully accessed system file!"
                    )
                    vulnerable = True
        except Exception as e:
            pass
    
    if not vulnerable:
        print_test("Absolute Path Access", "PASS", "Absolute paths blocked")
    
    return not vulnerable

def test_null_byte_injection():
    """Test null byte injection in path"""
    payloads = [
        "/index.html%00",
        "/../../../etc/passwd%00.jpg",
        "/browse/../../../etc/shadow%00.txt",
    ]
    
    vulnerable = False
    for payload in payloads:
        try:
            r = requests.get(f"{HOST}{payload}", timeout=5)
            if r.status_code == 200 and ("root:" in r.text or "daemon:" in r.text):
                print_test(
                    f"Null Byte Injection: {payload}",
                    "FAIL",
                    "Successfully bypassed with null byte!"
                )
                vulnerable = True
        except Exception as e:
            pass
    
    if not vulnerable:
        print_test("Null Byte Injection", "PASS", "Null byte injection blocked")
    
    return not vulnerable

def test_backslash_traversal():
    """Test backslash path traversal (Windows-style)"""
    payloads = [
        "/..\\..\\..\\etc\\passwd",
        "/browse\\..\\..\\..\\etc\\passwd",
        "/..\\etc\\passwd",
    ]
    
    vulnerable = False
    for payload in payloads:
        try:
            r = requests.get(f"{HOST}{payload}", timeout=5)
            if r.status_code == 200 and ("root:" in r.text or "daemon:" in r.text):
                print_test(
                    f"Backslash Traversal: {payload}",
                    "FAIL",
                    "Successfully bypassed with backslashes!"
                )
                vulnerable = True
        except Exception as e:
            pass
    
    if not vulnerable:
        print_test("Backslash Traversal", "PASS", "Backslash traversal blocked")
    
    return not vulnerable

def test_unicode_encoding():
    """Test Unicode/UTF-8 encoded path traversal"""
    payloads = [
        "/%c0%ae%c0%ae/%c0%ae%c0%ae/etc/passwd",  # Unicode ..
        "/%e0%80%ae%e0%80%ae/etc/passwd",  # Overlong UTF-8
    ]
    
    vulnerable = False
    for payload in payloads:
        try:
            r = requests.get(f"{HOST}{payload}", timeout=5)
            if r.status_code == 200 and ("root:" in r.text or "daemon:" in r.text):
                print_test(
                    f"Unicode Encoding: {payload}",
                    "FAIL",
                    "Successfully bypassed with Unicode encoding!"
                )
                vulnerable = True
        except Exception as e:
            pass
    
    if not vulnerable:
        print_test("Unicode Encoding", "PASS", "Unicode encoding blocked")
    
    return not vulnerable

def test_symlink_traversal():
    """Test if server follows symlinks outside web root"""
    # This test requires setup - create a symlink in web root
    print_test("Symlink Traversal", "SKIP", "Requires manual setup (create symlink in web root)")
    return True

def test_cgi_path_traversal():
    """Test path traversal through CGI scripts"""
    payloads = [
        "/cgi-bin/../../../etc/passwd",
        "/cgi-bin/../../webserv.conf",
    ]
    
    vulnerable = False
    for payload in payloads:
        try:
            r = requests.get(f"{HOST}{payload}", timeout=5)
            if r.status_code == 200:
                content_lower = r.text.lower()
                if "root:" in content_lower or "server {" in content_lower:
                    print_test(
                        f"CGI Path Traversal: {payload}",
                        "FAIL",
                        "Successfully accessed file through CGI path!"
                    )
                    vulnerable = True
        except Exception as e:
            pass
    
    if not vulnerable:
        print_test("CGI Path Traversal", "PASS", "CGI path traversal blocked")
    
    return not vulnerable

def test_path_normalization():
    """Test if path normalization is properly implemented"""
    payloads = [
        "/./././etc/passwd",
        "/browse/./../../etc/passwd",
        "///etc///passwd",
        "/browse//..//../../etc/passwd",
    ]
    
    vulnerable = False
    for payload in payloads:
        try:
            r = requests.get(f"{HOST}{payload}", timeout=5)
            if r.status_code == 200 and ("root:" in r.text or "daemon:" in r.text):
                print_test(
                    f"Path Normalization: {payload}",
                    "FAIL",
                    "Path normalization bypass!"
                )
                vulnerable = True
        except Exception as e:
            pass
    
    if not vulnerable:
        print_test("Path Normalization", "PASS", "Path normalization working")
    
    return not vulnerable

def test_config_file_access():
    """Test access to server configuration files"""
    payloads = [
        "/../webserv.conf",
        "/../config/webserv.conf",
        "/browse/../../webserv.conf",
        "/../Makefile",
    ]
    
    vulnerable = False
    for payload in payloads:
        try:
            r = requests.get(f"{HOST}{payload}", timeout=5)
            if r.status_code == 200:
                content_lower = r.text.lower()
                if "server {" in content_lower or "listen" in content_lower or "location" in content_lower:
                    print_test(
                        f"Config File Access: {payload}",
                        "FAIL",
                        "Successfully accessed configuration file!"
                    )
                    vulnerable = True
                    print(f"      First 100 chars: {r.text[:100]}")
        except Exception as e:
            pass
    
    if not vulnerable:
        print_test("Config File Access", "PASS", "Config files protected")
    
    return not vulnerable

def test_source_code_access():
    """Test access to server source code"""
    payloads = [
        "/../src/main.cpp",
        "/../includes/webserv.hpp",
        "/browse/../../src/http/HttpHandler.cpp",
    ]
    
    vulnerable = False
    for payload in payloads:
        try:
            r = requests.get(f"{HOST}{payload}", timeout=5)
            if r.status_code == 200:
                content = r.text.lower()
                if "#include" in content or "void " in content or "class " in content:
                    print_test(
                        f"Source Code Access: {payload}",
                        "FAIL",
                        "Successfully accessed source code!"
                    )
                    vulnerable = True
        except Exception as e:
            pass
    
    if not vulnerable:
        print_test("Source Code Access", "PASS", "Source code protected")
    
    return not vulnerable

def main():
    print(f"\n{COLORS['BLUE']}{'='*60}{COLORS['END']}")
    print(f"{COLORS['BLUE']}Path Traversal Vulnerability Tests{COLORS['END']}")
    print(f"{COLORS['BLUE']}{'='*60}{COLORS['END']}\n")
    
    # Check if server is running
    try:
        r = requests.get(f"{HOST}/", timeout=2)
    except requests.exceptions.RequestException:
        print(f"{COLORS['RED']}ERROR: Cannot connect to {HOST}{COLORS['END']}")
        print("Make sure the server is running: ./webserv webserv.conf")
        sys.exit(1)
    
    tests = [
        test_basic_path_traversal,
        test_encoded_path_traversal,
        test_absolute_path_access,
        test_null_byte_injection,
        test_backslash_traversal,
        test_unicode_encoding,
        test_symlink_traversal,
        test_cgi_path_traversal,
        test_path_normalization,
        test_config_file_access,
        test_source_code_access,
    ]
    
    passed = 0
    failed = 0
    skipped = 0
    
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
        print(f"{COLORS['RED']}⚠️  SECURITY WARNING: {failed} path traversal vulnerabilities detected!{COLORS['END']}")
        print(f"{COLORS['RED']}   Server is vulnerable to directory traversal attacks.{COLORS['END']}\n")
        return 1
    else:
        print(f"{COLORS['GREEN']}✓ All path traversal tests passed!{COLORS['END']}\n")
        return 0

if __name__ == "__main__":
    sys.exit(main())
