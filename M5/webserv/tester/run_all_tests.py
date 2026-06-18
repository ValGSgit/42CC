#!/usr/bin/env python3
"""
MASTER TEST RUNNER
Runs all comprehensive test suites for WebServ
"""

import subprocess
import sys
import os
import time
from pathlib import Path

class Colors:
    GREEN = '\033[92m'
    RED = '\033[91m'
    YELLOW = '\033[93m'
    BLUE = '\033[94m'
    MAGENTA = '\033[95m'
    CYAN = '\033[96m'
    RESET = '\033[0m'
    BOLD = '\033[1m'
    DIM = '\033[2m'

def run_tester(script_name, description, args=None):
    """Run a single tester script"""
    print(f"\n{Colors.BOLD}{Colors.CYAN}{'='*100}{Colors.RESET}")
    print(f"{Colors.BOLD}{Colors.CYAN}Running: {description}{Colors.RESET}")
    print(f"{Colors.BOLD}{Colors.CYAN}{'='*100}{Colors.RESET}\n")
    
    script_path = Path(__file__).parent / script_name
    
    if not script_path.exists():
        print(f"{Colors.RED}ERROR: {script_name} not found{Colors.RESET}")
        return 1
    
    cmd = [sys.executable, str(script_path)]
    if args:
        cmd.extend(args)
    
    start_time = time.time()
    
    try:
        result = subprocess.run(cmd, cwd=Path(__file__).parent)
        duration = time.time() - start_time
        
        if result.returncode == 0:
            print(f"\n{Colors.GREEN}✓ {description} completed successfully in {duration:.1f}s{Colors.RESET}")
            return 0
        else:
            print(f"\n{Colors.YELLOW}⚠ {description} completed with failures in {duration:.1f}s{Colors.RESET}")
            return 1
    except KeyboardInterrupt:
        print(f"\n{Colors.YELLOW}Test interrupted by user{Colors.RESET}")
        return 1
    except Exception as e:
        print(f"\n{Colors.RED}ERROR running {script_name}: {e}{Colors.RESET}")
        return 1

def main():
    import argparse
    
    parser = argparse.ArgumentParser(
        description='Master Test Runner - Run all WebServ test suites',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Test Suites:
  1. Comprehensive Tester    - 1000+ test cases covering all HTTP status codes
  2. RFC Compliance Tester   - HTTP/1.1 RFC compliance and edge cases  
  3. Performance Tester      - Load testing and performance metrics
  4. Original Status Tester  - Original 300+ test suite

Examples:
  %(prog)s                          # Run all test suites
  %(prog)s --quick                  # Run only comprehensive and RFC tests
  %(prog)s --comprehensive          # Run only comprehensive tester
  %(prog)s --port 8081              # Test on different port
        """
    )
    
    parser.add_argument('--host', default='localhost', help='Server host (default: localhost)')
    parser.add_argument('--port', type=int, default=8080, help='Server port (default: 8080)')
    parser.add_argument('--quick', action='store_true', help='Run only comprehensive and RFC tests')
    parser.add_argument('--comprehensive', action='store_true', help='Run only comprehensive tester')
    parser.add_argument('--rfc', action='store_true', help='Run only RFC compliance tester')
    parser.add_argument('--performance', action='store_true', help='Run only performance tester')
    parser.add_argument('--original', action='store_true', help='Run only original status tester')
    
    args = parser.parse_args()
    
    # Build common arguments
    common_args = ['--host', args.host, '--port', str(args.port)]
    
    print(f"\n{Colors.BOLD}{Colors.CYAN}╔{'═'*98}╗{Colors.RESET}")
    print(f"{Colors.BOLD}{Colors.CYAN}║{' '*30}WEBSERV MASTER TEST RUNNER{' '*32}║{Colors.RESET}")
    print(f"{Colors.BOLD}{Colors.CYAN}╚{'═'*98}╝{Colors.RESET}\n")
    
    print(f"{Colors.YELLOW}Testing server at {args.host}:{args.port}{Colors.RESET}")
    print(f"{Colors.DIM}Press Ctrl+C to interrupt any test{Colors.RESET}\n")
    
    # Check if server is running
    import socket
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(2)
        sock.connect((args.host, args.port))
        sock.close()
        print(f"{Colors.GREEN}✓ Server is running{Colors.RESET}\n")
    except:
        print(f"{Colors.RED}ERROR: Cannot connect to server at {args.host}:{args.port}{Colors.RESET}")
        print(f"{Colors.YELLOW}Please start your server first!{Colors.RESET}\n")
        return 1
    
    results = []
    start_time = time.time()
    
    try:
        # Determine which tests to run
        if args.comprehensive:
            tests = [('comprehensive_tester.py', 'Comprehensive HTTP Tester (1000+ tests)')]
        elif args.rfc:
            tests = [('rfc_compliance_tester.py', 'RFC Compliance & Edge Cases')]
        elif args.performance:
            tests = [('performance_tester.py', 'Performance & Load Testing')]
        elif args.original:
            tests = [('status_code_tester.py', 'Original Status Code Tester')]
        elif args.quick:
            tests = [
                ('comprehensive_tester.py', 'Comprehensive HTTP Tester (1000+ tests)'),
                ('rfc_compliance_tester.py', 'RFC Compliance & Edge Cases'),
            ]
        else:
            # Run all tests
            tests = [
                ('comprehensive_tester.py', 'Comprehensive HTTP Tester (1000+ tests)'),
                ('rfc_compliance_tester.py', 'RFC Compliance & Edge Cases'),
                ('performance_tester.py', 'Performance & Load Testing'),
                ('status_code_tester.py', 'Original Status Code Tester'),
            ]
        
        # Run each test
        for script, description in tests:
            result = run_tester(script, description, common_args)
            results.append((description, result))
        
        # Print final summary
        total_duration = time.time() - start_time
        
        print(f"\n{Colors.BOLD}{Colors.CYAN}{'='*100}{Colors.RESET}")
        print(f"{Colors.BOLD}{Colors.CYAN}MASTER TEST RUNNER - FINAL SUMMARY{Colors.RESET}")
        print(f"{Colors.BOLD}{Colors.CYAN}{'='*100}{Colors.RESET}\n")
        
        passed = sum(1 for _, r in results if r == 0)
        failed = sum(1 for _, r in results if r != 0)
        
        for description, result in results:
            status = f"{Colors.GREEN}✓ PASSED{Colors.RESET}" if result == 0 else f"{Colors.RED}✗ FAILED{Colors.RESET}"
            print(f"{status} - {description}")
        
        print(f"\n{Colors.BOLD}Test Suites: {len(results)}{Colors.RESET}")
        print(f"{Colors.GREEN}Passed: {passed}{Colors.RESET}")
        print(f"{Colors.RED}Failed: {failed}{Colors.RESET}")
        print(f"{Colors.BOLD}Total Duration: {total_duration/60:.1f} minutes{Colors.RESET}")
        
        if failed == 0:
            print(f"\n{Colors.GREEN}{Colors.BOLD}🎉 ALL TEST SUITES PASSED! 🎉{Colors.RESET}\n")
            return 0
        else:
            print(f"\n{Colors.YELLOW}Some test suites had failures. Review the output above for details.{Colors.RESET}\n")
            return 1
        
    except KeyboardInterrupt:
        print(f"\n{Colors.YELLOW}Testing interrupted by user{Colors.RESET}\n")
        return 1
    except Exception as e:
        print(f"\n{Colors.RED}ERROR: {e}{Colors.RESET}\n")
        import traceback
        traceback.print_exc()
        return 1

if __name__ == '__main__':
    sys.exit(main())
