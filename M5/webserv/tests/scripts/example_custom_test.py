#!/usr/bin/env python3
"""
Example: How to add your own custom test to the test suite
"""

import socket
import sys

class CustomTest:
    def __init__(self, host='localhost', port=8080):
        self.host = host
        self.port = port
    
    def send_request(self, request):
        """Send HTTP request and get response"""
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.settimeout(5.0)
            sock.connect((self.host, self.port))
            sock.sendall(request.encode())
            
            response = b""
            while True:
                chunk = sock.recv(4096)
                if not chunk:
                    break
                response += chunk
                if b"\r\n\r\n" in response:
                    break
            
            sock.close()
            return response.decode('utf-8', errors='ignore')
        except Exception as e:
            print(f"Error: {e}")
            return None
    
    def test_custom_endpoint(self):
        """Example: Test a custom endpoint"""
        print("Testing custom endpoint...")
        
        request = "GET /my-custom-path HTTP/1.1\r\nHost: localhost\r\n\r\n"
        response = self.send_request(request)
        
        if response and "200 OK" in response:
            print("✓ PASS: Custom endpoint works!")
            return True
        else:
            print("✗ FAIL: Custom endpoint failed")
            return False
    
    def test_custom_header(self):
        """Example: Test custom header handling"""
        print("Testing custom header...")
        
        request = (
            "GET / HTTP/1.1\r\n"
            "Host: localhost\r\n"
            "X-Custom-Header: MyValue\r\n"
            "\r\n"
        )
        response = self.send_request(request)
        
        if response and "200 OK" in response:
            print("✓ PASS: Custom header accepted!")
            return True
        else:
            print("✗ FAIL: Custom header test failed")
            return False
    
    def test_custom_post(self):
        """Example: Test custom POST data"""
        print("Testing custom POST...")
        
        data = "custom_field=custom_value&another=test"
        request = (
            f"POST /cgi-bin/test.py HTTP/1.1\r\n"
            f"Host: localhost\r\n"
            f"Content-Type: application/x-www-form-urlencoded\r\n"
            f"Content-Length: {len(data)}\r\n"
            f"\r\n"
            f"{data}"
        )
        response = self.send_request(request)
        
        if response and "200 OK" in response:
            print("✓ PASS: Custom POST works!")
            return True
        else:
            print("✗ FAIL: Custom POST failed")
            return False
    
    def run_all(self):
        """Run all custom tests"""
        print("\n" + "="*50)
        print("Custom Test Suite")
        print("="*50 + "\n")
        
        results = []
        results.append(self.test_custom_endpoint())
        results.append(self.test_custom_header())
        results.append(self.test_custom_post())
        
        print("\n" + "="*50)
        passed = sum(results)
        total = len(results)
        print(f"Results: {passed}/{total} passed")
        print("="*50 + "\n")
        
        return all(results)

if __name__ == "__main__":
    # Check if server is running
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(2.0)
        result = sock.connect_ex(('localhost', 8080))
        sock.close()
        
        if result != 0:
            print("Error: Server not running on localhost:8080")
            sys.exit(1)
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)
    
    # Run custom tests
    tester = CustomTest()
    success = tester.run_all()
    
    sys.exit(0 if success else 1)
