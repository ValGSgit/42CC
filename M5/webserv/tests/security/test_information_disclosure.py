#!/usr/bin/env python3
"""
Information Disclosure Tests
Tests for information leakage vulnerabilities
"""

import requests
import sys

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

def test_server_header_disclosure():
    """Test if server reveals version information"""
    try:
        r = requests.get(f"{HOST}/", timeout=5)
        
        server_header = r.headers.get('Server', '')
        
        if server_header:
            if any(version in server_header.lower() for version in ['1.0', '2.0', '0.1', 'beta', 'alpha']):
                print_test(
                    "Server Header Disclosure",
                    "WARN",
                    f"Server header reveals version: {server_header}"
                )
            else:
                print_test(
                    "Server Header Disclosure",
                    "INFO",
                    f"Server header present: {server_header}"
                )
        else:
            print_test(
                "Server Header Disclosure",
                "PASS",
                "No Server header disclosed"
            )
        
        return True
    except Exception as e:
        print_test("Server Header Disclosure", "ERROR", str(e))
        return False

def test_error_page_information():
    """Test if error pages leak system information"""
    try:
        # Trigger 404
        r = requests.get(f"{HOST}/nonexistent_file_12345", timeout=5)
        
        content_lower = r.text.lower()
        
        leaks = []
        if '/home/' in content_lower or '/usr/' in content_lower or 'c:\\' in content_lower:
            leaks.append("file paths")
        if 'apache' in content_lower or 'nginx' in content_lower or 'webserv' in content_lower:
            leaks.append("server software")
        if 'python' in content_lower or 'php' in content_lower or 'ruby' in content_lower:
            leaks.append("runtime environment")
        
        if leaks:
            print_test(
                "Error Page Information",
                "WARN",
                f"Error pages leak: {', '.join(leaks)}"
            )
        else:
            print_test(
                "Error Page Information",
                "PASS",
                "Error pages don't leak sensitive information"
            )
        
        return True
    except Exception as e:
        print_test("Error Page Information", "ERROR", str(e))
        return False

def test_directory_listing():
    """Test if directory listing is enabled"""
    paths = [
        '/browse/',
        '/uploads/',
        '/cgi-bin/',
        '/docs/',
    ]
    
    listings_found = []
    
    for path in paths:
        try:
            r = requests.get(f"{HOST}{path}", timeout=5)
            
            if r.status_code == 200:
                # Check for directory listing indicators
                if ('<ul>' in r.text or '<table>' in r.text or 'Index of' in r.text or 
                    'Directory listing' in r.text):
                    listings_found.append(path)
        except:
            pass
    
    if listings_found:
        print_test(
            "Directory Listing",
            "WARN",
            f"Directory listing enabled on: {', '.join(listings_found)}"
        )
    else:
        print_test(
            "Directory Listing",
            "PASS",
            "No directory listings found"
        )
    
    return True

def test_http_methods_disclosure():
    """Test if server discloses allowed methods"""
    try:
        r = requests.options(f"{HOST}/", timeout=5)
        
        allow_header = r.headers.get('Allow', '')
        
        if allow_header:
            print_test(
                "HTTP Methods Disclosure",
                "INFO",
                f"Allowed methods: {allow_header}"
            )
        else:
            print_test(
                "HTTP Methods Disclosure",
                "PASS",
                "Methods not explicitly disclosed"
            )
        
        return True
    except Exception as e:
        print_test("HTTP Methods Disclosure", "ERROR", str(e))
        return False

def test_stack_trace_disclosure():
    """Test if server exposes stack traces"""
    # Try to trigger an error that might show stack trace
    try:
        r = requests.get(f"{HOST}/cgi-bin/nonexistent.py", timeout=5)
        
        content = r.text
        
        stack_indicators = ['traceback', 'stack trace', 'at line', 'error on line', 
                          'exception', 'file "/', 'line ']
        
        if any(indicator in content.lower() for indicator in stack_indicators):
            print_test(
                "Stack Trace Disclosure",
                "FAIL",
                "Server exposes stack traces in errors!"
            )
            return False
        else:
            print_test(
                "Stack Trace Disclosure",
                "PASS",
                "No stack traces in error responses"
            )
            return True
    except Exception as e:
        print_test("Stack Trace Disclosure", "ERROR", str(e))
        return False

def test_git_folder_exposure():
    """Test if .git folder is exposed"""
    try:
        r = requests.get(f"{HOST}/.git/config", timeout=5)
        
        if r.status_code == 200 and 'repositoryformatversion' in r.text:
            print_test(
                "Git Folder Exposure",
                "FAIL",
                "/.git/ folder is publicly accessible!"
            )
            return False
        else:
            print_test(
                "Git Folder Exposure",
                "PASS",
                ".git folder not accessible"
            )
            return True
    except Exception as e:
        print_test("Git Folder Exposure", "ERROR", str(e))
        return False

def test_backup_file_exposure():
    """Test if backup files are exposed"""
    backup_patterns = [
        '/index.html~',
        '/index.html.bak',
        '/index.html.backup',
        '/config.php.old',
        '/.htaccess.bak',
    ]
    
    exposed = []
    
    for pattern in backup_patterns:
        try:
            r = requests.get(f"{HOST}{pattern}", timeout=5)
            if r.status_code == 200:
                exposed.append(pattern)
        except:
            pass
    
    if exposed:
        print_test(
            "Backup File Exposure",
            "WARN",
            f"Backup files accessible: {', '.join(exposed)}"
        )
    else:
        print_test(
            "Backup File Exposure",
            "PASS",
            "No backup files found"
        )
    
    return True

def test_phpinfo_disclosure():
    """Test if phpinfo() output is accessible"""
    try:
        r = requests.get(f"{HOST}/cgi-bin/info.php", timeout=5)
        
        if r.status_code == 200 and 'phpinfo()' in r.text.lower():
            print_test(
                "PHPInfo Disclosure",
                "FAIL",
                "phpinfo() page is publicly accessible!"
            )
            return False
        else:
            print_test(
                "PHPInfo Disclosure",
                "PASS",
                "No phpinfo() exposure detected"
            )
            return True
    except Exception as e:
        print_test("PHPInfo Disclosure", "ERROR", str(e))
        return False

def test_cors_header_disclosure():
    """Test CORS header configuration"""
    try:
        r = requests.get(
            f"{HOST}/",
            headers={'Origin': 'http://evil.com'},
            timeout=5
        )
        
        acao = r.headers.get('Access-Control-Allow-Origin', '')
        
        if acao == '*':
            print_test(
                "CORS Configuration",
                "WARN",
                "CORS allows all origins (*)"
            )
        elif acao:
            print_test(
                "CORS Configuration",
                "INFO",
                f"CORS configured: {acao}"
            )
        else:
            print_test(
                "CORS Configuration",
                "PASS",
                "CORS not configured"
            )
        
        return True
    except Exception as e:
        print_test("CORS Configuration", "ERROR", str(e))
        return False

def test_sensitive_files():
    """Test access to common sensitive files"""
    sensitive_files = [
        '/.env',
        '/config.php',
        '/database.yml',
        '/wp-config.php',
        '/.htpasswd',
        '/robots.txt',
    ]
    
    accessible = []
    
    for file in sensitive_files:
        try:
            r = requests.get(f"{HOST}{file}", timeout=5)
            if r.status_code == 200 and len(r.text) > 0:
                accessible.append(file)
        except:
            pass
    
    if accessible:
        print_test(
            "Sensitive Files",
            "WARN",
            f"Sensitive files accessible: {', '.join(accessible)}"
        )
    else:
        print_test(
            "Sensitive Files",
            "PASS",
            "No sensitive files found"
        )
    
    return True

def main():
    print(f"\n{COLORS['BLUE']}{'='*60}{COLORS['END']}")
    print(f"{COLORS['BLUE']}Information Disclosure Tests{COLORS['END']}")
    print(f"{COLORS['BLUE']}{'='*60}{COLORS['END']}\n")
    
    # Check if server is running
    try:
        r = requests.get(f"{HOST}/", timeout=2)
    except requests.exceptions.RequestException:
        print(f"{COLORS['RED']}ERROR: Cannot connect to {HOST}{COLORS['END']}")
        print("Make sure the server is running: ./webserv webserv.conf")
        sys.exit(1)
    
    tests = [
        test_server_header_disclosure,
        test_error_page_information,
        test_directory_listing,
        test_http_methods_disclosure,
        test_stack_trace_disclosure,
        test_git_folder_exposure,
        test_backup_file_exposure,
        test_phpinfo_disclosure,
        test_cors_header_disclosure,
        test_sensitive_files,
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
        print(f"{COLORS['RED']}⚠️  SECURITY WARNING: {failed} information disclosure issues detected!{COLORS['END']}\n")
        return 1
    else:
        print(f"{COLORS['GREEN']}✓ All information disclosure tests passed!{COLORS['END']}\n")
        return 0

if __name__ == "__main__":
    sys.exit(main())
