#!/usr/bin/env python3
"""
Master Security Test Runner
Runs all security vulnerability tests and generates a comprehensive report
"""

import subprocess
import sys
import os
import time
from datetime import datetime

COLORS = {
    'RED': '\033[91m',
    'GREEN': '\033[92m',
    'YELLOW': '\033[93m',
    'BLUE': '\033[94m',
    'MAGENTA': '\033[95m',
    'CYAN': '\033[96m',
    'END': '\033[0m'
}

def print_banner():
    banner = f"""
{COLORS['CYAN']}╔═══════════════════════════════════════════════════════════════╗
║                                                               ║
║        🛡️  WebServ Security Vulnerability Test Suite        ║
║                                                               ║
║           Comprehensive Security Assessment Tool             ║
║                                                               ║
╚═══════════════════════════════════════════════════════════════╝{COLORS['END']}
"""
    print(banner)

def run_test(test_script):
    """Run a single test script and return result"""
    script_path = os.path.join(os.path.dirname(__file__), test_script)
    
    if not os.path.exists(script_path):
        return {'status': 'SKIP', 'reason': 'Script not found', 'output': ''}
    
    try:
        print(f"\n{COLORS['BLUE']}▶ Running: {test_script}{COLORS['END']}")
        print(f"{COLORS['BLUE']}{'─' * 60}{COLORS['END']}")
        
        result = subprocess.run(
            ['python3', script_path],
            capture_output=True,
            text=True,
            timeout=180  # 3 minute timeout per test
        )
        
        # Print output in real-time effect
        print(result.stdout)
        if result.stderr:
            print(f"{COLORS['YELLOW']}{result.stderr}{COLORS['END']}")
        
        status = 'PASS' if result.returncode == 0 else 'FAIL'
        
        return {
            'status': status,
            'returncode': result.returncode,
            'output': result.stdout,
            'error': result.stderr
        }
    except subprocess.TimeoutExpired:
        return {
            'status': 'TIMEOUT',
            'reason': 'Test exceeded 3 minute timeout',
            'output': ''
        }
    except Exception as e:
        return {
            'status': 'ERROR',
            'reason': str(e),
            'output': ''
        }

def generate_report(results, start_time, end_time):
    """Generate comprehensive security report"""
    report_dir = os.path.join(os.path.dirname(__file__), 'results')
    os.makedirs(report_dir, exist_ok=True)
    
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    report_file = os.path.join(report_dir, f'security_report_{timestamp}.txt')
    
    with open(report_file, 'w') as f:
        f.write("=" * 80 + "\n")
        f.write("WebServ Security Vulnerability Assessment Report\n")
        f.write("=" * 80 + "\n\n")
        
        f.write(f"Test Date: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n")
        f.write(f"Duration: {end_time - start_time:.2f} seconds\n")
        f.write(f"Total Tests: {len(results)}\n\n")
        
        # Summary
        passed = sum(1 for r in results.values() if r['status'] == 'PASS')
        failed = sum(1 for r in results.values() if r['status'] == 'FAIL')
        errors = sum(1 for r in results.values() if r['status'] in ['ERROR', 'TIMEOUT'])
        skipped = sum(1 for r in results.values() if r['status'] == 'SKIP')
        
        f.write("SUMMARY\n")
        f.write("-" * 80 + "\n")
        f.write(f"Passed:  {passed}\n")
        f.write(f"Failed:  {failed}\n")
        f.write(f"Errors:  {errors}\n")
        f.write(f"Skipped: {skipped}\n\n")
        
        # Detailed Results
        f.write("DETAILED RESULTS\n")
        f.write("-" * 80 + "\n\n")
        
        for test_name, result in results.items():
            f.write(f"Test: {test_name}\n")
            f.write(f"Status: {result['status']}\n")
            
            if 'returncode' in result:
                f.write(f"Return Code: {result['returncode']}\n")
            
            if result.get('output'):
                f.write("\nOutput:\n")
                f.write(result['output'])
                f.write("\n")
            
            if result.get('error'):
                f.write("\nErrors:\n")
                f.write(result['error'])
                f.write("\n")
            
            if result.get('reason'):
                f.write(f"Reason: {result['reason']}\n")
            
            f.write("\n" + "-" * 80 + "\n\n")
        
        # Risk Assessment
        f.write("RISK ASSESSMENT\n")
        f.write("-" * 80 + "\n")
        if failed == 0:
            f.write("✓ LOW RISK: All security tests passed!\n")
            f.write("The server appears to have adequate security controls.\n")
        elif failed <= 2:
            f.write("⚠ MEDIUM RISK: Some vulnerabilities detected.\n")
            f.write("Review and address the failed tests.\n")
        else:
            f.write("🚨 HIGH RISK: Multiple critical vulnerabilities detected!\n")
            f.write("IMMEDIATE ACTION REQUIRED: The server has serious security issues.\n")
        
        f.write("\n" + "=" * 80 + "\n")
    
    return report_file

def main():
    print_banner()
    
    # Check if server is accessible
    import requests
    try:
        r = requests.get("http://localhost:8080/", timeout=2)
        print(f"{COLORS['GREEN']}✓ Server is running and accessible{COLORS['END']}\n")
    except:
        print(f"{COLORS['RED']}✗ ERROR: Cannot connect to server at http://localhost:8080{COLORS['END']}")
        print("Please start the server first: ./webserv webserv.conf\n")
        sys.exit(1)
    
    # List of test scripts to run
    tests = [
        'test_path_traversal.py',
        'test_file_operations.py',
        'test_cgi_injection.py',
        'test_dos_attacks.py',
        'test_input_validation.py',
        'test_http_smuggling.py',
        'test_information_disclosure.py',
    ]
    
    print(f"{COLORS['CYAN']}Running {len(tests)} security test suites...{COLORS['END']}")
    print(f"{COLORS['YELLOW']}This may take several minutes. Please wait...{COLORS['END']}\n")
    
    start_time = time.time()
    results = {}
    
    for test in tests:
        result = run_test(test)
        results[test] = result
    
    end_time = time.time()
    
    # Generate report
    report_file = generate_report(results, start_time, end_time)
    
    # Print summary
    print(f"\n{COLORS['CYAN']}{'=' * 60}{COLORS['END']}")
    print(f"{COLORS['CYAN']}FINAL SUMMARY{COLORS['END']}")
    print(f"{COLORS['CYAN']}{'=' * 60}{COLORS['END']}\n")
    
    passed = sum(1 for r in results.values() if r['status'] == 'PASS')
    failed = sum(1 for r in results.values() if r['status'] == 'FAIL')
    errors = sum(1 for r in results.values() if r['status'] in ['ERROR', 'TIMEOUT'])
    skipped = sum(1 for r in results.values() if r['status'] == 'SKIP')
    
    print(f"Total Tests Run:    {len(results)}")
    print(f"{COLORS['GREEN']}Passed:             {passed}{COLORS['END']}")
    print(f"{COLORS['RED']}Failed:             {failed}{COLORS['END']}")
    print(f"{COLORS['YELLOW']}Errors:             {errors}{COLORS['END']}")
    print(f"{COLORS['BLUE']}Skipped:            {skipped}{COLORS['END']}")
    print(f"\nDuration:           {end_time - start_time:.2f} seconds")
    print(f"\nReport saved to:    {report_file}")
    
    print(f"\n{COLORS['CYAN']}{'=' * 60}{COLORS['END']}\n")
    
    # Final verdict
    if failed == 0 and errors == 0:
        print(f"{COLORS['GREEN']}🎉 CONGRATULATIONS! All security tests passed!{COLORS['END']}")
        print(f"{COLORS['GREEN']}Your server demonstrates good security practices.{COLORS['END']}\n")
        return 0
    elif failed <= 2:
        print(f"{COLORS['YELLOW']}⚠️  WARNING: Some security issues detected.{COLORS['END']}")
        print(f"{COLORS['YELLOW']}Review the failed tests and implement fixes.{COLORS['END']}\n")
        return 1
    else:
        print(f"{COLORS['RED']}🚨 CRITICAL: Multiple security vulnerabilities detected!{COLORS['END']}")
        print(f"{COLORS['RED']}IMMEDIATE ACTION REQUIRED: Address these vulnerabilities before deployment.{COLORS['END']}\n")
        return 2

if __name__ == "__main__":
    try:
        sys.exit(main())
    except KeyboardInterrupt:
        print(f"\n\n{COLORS['YELLOW']}Tests interrupted by user{COLORS['END']}\n")
        sys.exit(130)
    except Exception as e:
        print(f"\n{COLORS['RED']}Fatal error: {e}{COLORS['END']}\n")
        sys.exit(1)
