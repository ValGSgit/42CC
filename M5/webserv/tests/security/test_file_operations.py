#!/usr/bin/env python3
"""
File Operations Vulnerability Tests
Tests for file upload, deletion, and file operation vulnerabilities
"""

import requests
import sys
import os
import time

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

def test_delete_path_traversal():
    """Test DELETE method with path traversal"""
    payloads = [
        "/../../../tmp/test_delete_file",
        "/../webserv.conf",
        "/../../etc/test_file",
        "/browse/../../../tmp/delete_me",
    ]
    
    vulnerable = False
    for payload in payloads:
        try:
            r = requests.delete(f"{HOST}{payload}", timeout=5)
            # If we get 200 or 204, deletion might have succeeded
            if r.status_code in [200, 204]:
                print_test(
                    f"DELETE Path Traversal: {payload}",
                    "FAIL",
                    f"Server accepted DELETE with path traversal! Status: {r.status_code}"
                )
                vulnerable = True
        except Exception as e:
            pass
    
    if not vulnerable:
        print_test("DELETE Path Traversal", "PASS", "Path traversal in DELETE blocked")
    
    return not vulnerable

def test_delete_without_validation():
    """Test if DELETE method validates file existence properly"""
    payload = "/../../../nonexistent_file_" + str(int(time.time()))
    
    try:
        r = requests.delete(f"{HOST}{payload}", timeout=5)
        if r.status_code in [200, 204]:
            print_test(
                "DELETE Without Validation",
                "FAIL",
                "Server returns success for nonexistent file deletion"
            )
            return False
        else:
            print_test(
                "DELETE Without Validation",
                "PASS",
                f"Server properly handles nonexistent files (status: {r.status_code})"
            )
            return True
    except Exception as e:
        print_test("DELETE Without Validation", "ERROR", str(e))
        return False

def test_upload_path_traversal():
    """Test file upload with path traversal in filename"""
    payloads = [
        "../../backdoor.php",
        "../../../etc/evil_file",
        "../../www/index.html",
        "../../../cgi-bin/shell.py",
    ]
    
    vulnerable = False
    for payload in payloads:
        try:
            files = {'file': (payload, 'malicious content', 'text/plain')}
            r = requests.post(f"{HOST}/upload", files=files, timeout=5)
            
            if r.status_code in [200, 201]:
                print_test(
                    f"Upload Path Traversal: {payload}",
                    "FAIL",
                    f"Server accepted filename with path traversal! Status: {r.status_code}"
                )
                vulnerable = True
        except Exception as e:
            pass
    
    if not vulnerable:
        print_test("Upload Path Traversal", "PASS", "Path traversal in filenames blocked")
    
    return not vulnerable

def test_upload_malicious_extensions():
    """Test upload of files with potentially dangerous extensions"""
    extensions = [
        '.php', '.py', '.sh', '.exe', '.bat', '.cgi', 
        '.pl', '.rb', '.jsp', '.asp', '.aspx'
    ]
    
    vulnerable = False
    for ext in extensions:
        try:
            filename = f"malicious{ext}"
            content = "<?php system($_GET['cmd']); ?>" if ext == '.php' else "#!/bin/bash\nrm -rf /"
            files = {'file': (filename, content, 'text/plain')}
            r = requests.post(f"{HOST}/upload", files=files, timeout=5)
            
            if r.status_code in [200, 201]:
                print_test(
                    f"Upload Malicious Extension: {ext}",
                    "WARN",
                    f"Server accepted dangerous file type! Status: {r.status_code}"
                )
                vulnerable = True
        except Exception as e:
            pass
    
    if not vulnerable:
        print_test("Upload Malicious Extensions", "PASS", "Dangerous file types blocked")
    else:
        print_test("Upload Malicious Extensions", "INFO", "Server accepts all file types (may be intended)")
    
    return True  # Not necessarily a vulnerability

def test_upload_overwrite():
    """Test if uploads can overwrite existing files"""
    # First, upload a file
    filename = "test_overwrite.txt"
    content1 = "Original content"
    
    try:
        files = {'file': (filename, content1, 'text/plain')}
        r1 = requests.post(f"{HOST}/upload", files=files, timeout=5)
        
        if r1.status_code in [200, 201]:
            # Try to upload again with different content
            content2 = "OVERWRITTEN CONTENT"
            files = {'file': (filename, content2, 'text/plain')}
            r2 = requests.post(f"{HOST}/upload", files=files, timeout=5)
            
            if r2.status_code in [200, 201]:
                # Check if file was overwritten
                print_test(
                    "Upload Overwrite",
                    "WARN",
                    "Server may allow file overwrites (check implementation)"
                )
                return True
            else:
                print_test(
                    "Upload Overwrite",
                    "PASS",
                    "Server prevents file overwrites"
                )
                return True
        else:
            print_test("Upload Overwrite", "SKIP", "Initial upload failed")
            return True
    except Exception as e:
        print_test("Upload Overwrite", "ERROR", str(e))
        return False

def test_upload_large_file():
    """Test upload of files exceeding max body size"""
    try:
        # Create a large file (100MB)
        large_content = 'A' * (100 * 1024 * 1024)
        filename = "huge_file.txt"
        files = {'file': (filename, large_content, 'text/plain')}
        
        r = requests.post(f"{HOST}/upload", files=files, timeout=10)
        
        if r.status_code in [200, 201]:
            print_test(
                "Upload Large File",
                "FAIL",
                "Server accepted file larger than max_body_size!"
            )
            return False
        elif r.status_code == 413:
            print_test(
                "Upload Large File",
                "PASS",
                "Server properly rejects oversized files (413)"
            )
            return True
        else:
            print_test(
                "Upload Large File",
                "PASS",
                f"Server rejected large file (status: {r.status_code})"
            )
            return True
    except requests.exceptions.Timeout:
        print_test("Upload Large File", "WARN", "Request timed out (possible DoS vulnerability)")
        return True
    except Exception as e:
        print_test("Upload Large File", "ERROR", str(e))
        return False

def test_upload_no_filename():
    """Test upload without filename parameter"""
    try:
        files = {'file': ('', 'content without name', 'text/plain')}
        r = requests.post(f"{HOST}/upload", files=files, timeout=5)
        
        # Server should handle this gracefully
        if r.status_code == 500:
            print_test(
                "Upload No Filename",
                "FAIL",
                "Server crashed or returned 500 for missing filename"
            )
            return False
        else:
            print_test(
                "Upload No Filename",
                "PASS",
                f"Server handled missing filename gracefully (status: {r.status_code})"
            )
            return True
    except Exception as e:
        print_test("Upload No Filename", "ERROR", str(e))
        return False

def test_upload_directory_listing_access():
    """Test if uploaded files are accessible via directory listing"""
    filename = "test_listing_" + str(int(time.time())) + ".txt"
    
    try:
        # Upload a file
        files = {'file': (filename, 'test content', 'text/plain')}
        r1 = requests.post(f"{HOST}/upload", files=files, timeout=5)
        
        if r1.status_code in [200, 201]:
            # Try to access uploads directory
            r2 = requests.get(f"{HOST}/uploads/", timeout=5)
            
            if r2.status_code == 200 and filename in r2.text:
                print_test(
                    "Upload Directory Listing",
                    "WARN",
                    "Uploaded files visible in directory listing"
                )
                return True  # Not necessarily a vulnerability
            else:
                print_test(
                    "Upload Directory Listing",
                    "PASS",
                    "Upload directory not browsable"
                )
                return True
        else:
            print_test("Upload Directory Listing", "SKIP", "Upload failed")
            return True
    except Exception as e:
        print_test("Upload Directory Listing", "ERROR", str(e))
        return False

def test_upload_race_condition():
    """Test for race conditions in file upload"""
    filename = "race_test_" + str(int(time.time())) + ".txt"
    
    try:
        import threading
        results = []
        
        def upload_file():
            try:
                files = {'file': (filename, f'content_{time.time()}', 'text/plain')}
                r = requests.post(f"{HOST}/upload", files=files, timeout=5)
                results.append(r.status_code)
            except:
                pass
        
        # Start 5 simultaneous uploads with same filename
        threads = []
        for _ in range(5):
            t = threading.Thread(target=upload_file)
            threads.append(t)
            t.start()
        
        for t in threads:
            t.join()
        
        successful = [s for s in results if s in [200, 201]]
        
        if len(successful) > 1:
            print_test(
                "Upload Race Condition",
                "WARN",
                f"{len(successful)} simultaneous uploads succeeded (potential race condition)"
            )
        else:
            print_test(
                "Upload Race Condition",
                "PASS",
                "Race condition handling appears adequate"
            )
        
        return True
    except Exception as e:
        print_test("Upload Race Condition", "ERROR", str(e))
        return False

def test_multipart_boundary_injection():
    """Test multipart boundary injection"""
    try:
        # Craft malicious multipart data with fake boundaries
        boundary = "----WebKitFormBoundary" + str(int(time.time()))
        
        body = f'------{boundary}\r\n'
        body += 'Content-Disposition: form-data; name="file"; filename="test.txt"\r\n'
        body += 'Content-Type: text/plain\r\n\r\n'
        body += 'Fake content\r\n'
        body += f'------{boundary}--\r\n'  # Fake end
        body += 'MALICIOUS DATA AFTER BOUNDARY\r\n'
        body += f'------{boundary}--\r\n'  # Real end
        
        headers = {'Content-Type': f'multipart/form-data; boundary=----{boundary}'}
        r = requests.post(f"{HOST}/upload", data=body, headers=headers, timeout=5)
        
        # Server should handle this without crashes
        if r.status_code == 500:
            print_test(
                "Multipart Boundary Injection",
                "FAIL",
                "Server returned 500 for boundary injection"
            )
            return False
        else:
            print_test(
                "Multipart Boundary Injection",
                "PASS",
                f"Server handled boundary injection (status: {r.status_code})"
            )
            return True
    except Exception as e:
        print_test("Multipart Boundary Injection", "ERROR", str(e))
        return False

def test_delete_timing_attack():
    """Test if DELETE leaks information through timing"""
    # This is a timing-based test
    print_test("DELETE Timing Attack", "INFO", "Timing analysis not implemented in this test")
    return True

def main():
    print(f"\n{COLORS['BLUE']}{'='*60}{COLORS['END']}")
    print(f"{COLORS['BLUE']}File Operations Vulnerability Tests{COLORS['END']}")
    print(f"{COLORS['BLUE']}{'='*60}{COLORS['END']}\n")
    
    # Check if server is running
    try:
        r = requests.get(f"{HOST}/", timeout=2)
    except requests.exceptions.RequestException:
        print(f"{COLORS['RED']}ERROR: Cannot connect to {HOST}{COLORS['END']}")
        print("Make sure the server is running: ./webserv webserv.conf")
        sys.exit(1)
    
    tests = [
        test_delete_path_traversal,
        test_delete_without_validation,
        test_upload_path_traversal,
        test_upload_malicious_extensions,
        test_upload_overwrite,
        test_upload_large_file,
        test_upload_no_filename,
        test_upload_directory_listing_access,
        test_upload_race_condition,
        test_multipart_boundary_injection,
        test_delete_timing_attack,
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
        print(f"{COLORS['RED']}⚠️  SECURITY WARNING: {failed} file operation vulnerabilities detected!{COLORS['END']}\n")
        return 1
    else:
        print(f"{COLORS['GREEN']}✓ All file operation tests passed!{COLORS['END']}\n")
        return 0

if __name__ == "__main__":
    sys.exit(main())
