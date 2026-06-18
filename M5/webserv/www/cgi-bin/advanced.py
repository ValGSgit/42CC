#!/usr/bin/env python3
import os
import sys
import json
import datetime
import platform
import urllib.parse
import html

# Print CGI headers
print("Content-Type: text/html")
print()  # Empty line required after headers

# Get CGI environment variables
method = os.environ.get('REQUEST_METHOD', 'Unknown')
uri = os.environ.get('REQUEST_URI', 'Unknown')
query = os.environ.get('QUERY_STRING', '')
content_type = os.environ.get('CONTENT_TYPE', 'Unknown')
content_length = os.environ.get('CONTENT_LENGTH', '0')

# Read POST data if any
post_data = ""
if method == 'POST' and content_length != '0':
    try:
        post_data = sys.stdin.read(int(content_length))
    except:
        post_data = "Error reading POST data"

# Parse query parameters
params = {}
if query:
    try:
        params = dict(urllib.parse.parse_qsl(query))
    except:
        params = {'error': 'Failed to parse query string'}

# Generate HTML response
print(f"""<!DOCTYPE html>
<html>
<head>
    <title>Advanced Python CGI Test</title>
    <style>
        body {{ font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; margin: 0; padding: 20px; background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); }}
        .container {{ background: white; padding: 30px; border-radius: 15px; box-shadow: 0 10px 30px rgba(0,0,0,0.3); max-width: 1200px; margin: 0 auto; }}
        h1 {{ color: #2c3e50; text-align: center; margin-bottom: 30px; }}
        .grid {{ display: grid; grid-template-columns: repeat(auto-fit, minmax(500px, 1fr)); gap: 20px; }}
        .section {{ background: #f8f9fa; padding: 20px; border-radius: 10px; border-left: 5px solid #3498db; }}
        .section h2 {{ color: #2c3e50; margin-top: 0; }}
        table {{ width: 100%; border-collapse: collapse; margin-top: 10px; }}
        th, td {{ border: 1px solid #dee2e6; padding: 12px; text-align: left; }}
        th {{ background: linear-gradient(135deg, #3498db, #2980b9); color: white; font-weight: 600; }}
        .code {{ font-family: 'Courier New', monospace; background: #2c3e50; color: #ecf0f1; padding: 3px 6px; border-radius: 4px; font-size: 0.9em; }}
        .json-output {{ background: #2c3e50; color: #e74c3c; padding: 15px; border-radius: 8px; font-family: monospace; white-space: pre-wrap; overflow-x: auto; }}
        .status-good {{ color: #27ae60; font-weight: bold; }}
        .status-bad {{ color: #e74c3c; font-weight: bold; }}
        .highlight {{ background: #f39c12; color: white; padding: 2px 5px; border-radius: 3px; }}
        .feature-list {{ list-style: none; padding: 0; }}
        .feature-list li {{ background: #ecf0f1; margin: 5px 0; padding: 10px; border-radius: 5px; border-left: 3px solid #3498db; }}
        .info-badge {{ display: inline-block; background: #3498db; color: white; padding: 2px 8px; border-radius: 12px; font-size: 0.8em; margin-left: 5px; }}
    </style>
</head>
<body>
    <div class="container">
        <h1>🐍 Advanced Python CGI Test Script</h1>
        
        <div class="grid">
            <div class="section">
                <h2>🔧 Python Environment</h2>
                <table>
                    <tr><th>Property</th><th>Value</th></tr>
                    <tr><td>Python Version</td><td><span class="code">{platform.python_version()}</span></td></tr>
                    <tr><td>Python Implementation</td><td>{platform.python_implementation()}</td></tr>
                    <tr><td>Platform</td><td>{platform.platform()}</td></tr>
                    <tr><td>Architecture</td><td>{platform.architecture()[0]}</td></tr>
                    <tr><td>Processor</td><td>{platform.processor() or 'Unknown'}</td></tr>
                    <tr><td>Python Executable</td><td><span class="code">{sys.executable}</span></td></tr>
                    <tr><td>Python Path Entries</td><td>{len(sys.path)} <span class="info-badge">paths</span></td></tr>
                </table>
            </div>
            
            <div class="section">
                <h2>🌐 Request Information</h2>
                <table>
                    <tr><th>Property</th><th>Value</th></tr>
                    <tr><td>Request Method</td><td><span class="code">{method}</span></td></tr>
                    <tr><td>Request URI</td><td><span class="code">{html.escape(uri)}</span></td></tr>
                    <tr><td>Query String</td><td><span class="code">{html.escape(query) if query else '(empty)'}</span></td></tr>
                    <tr><td>Content Type</td><td>{html.escape(content_type)}</td></tr>
                    <tr><td>Content Length</td><td>{content_length} <span class="info-badge">bytes</span></td></tr>
                    <tr><td>Server Time</td><td>{datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S %Z')}</td></tr>
                    <tr><td>UTC Time</td><td>{datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S UTC')}</td></tr>
                </table>
            </div>
        </div>
        
        <div class="section">
            <h2>🔐 CGI Environment Variables</h2>
            <table>
                <tr><th>Variable</th><th>Value</th></tr>""")

# Display environment variables
env_vars = ['GATEWAY_INTERFACE', 'SERVER_NAME', 'SERVER_PORT', 'SERVER_PROTOCOL', 
           'SERVER_SOFTWARE', 'DOCUMENT_ROOT', 'SCRIPT_NAME', 'SCRIPT_FILENAME']

for var in env_vars:
    value = os.environ.get(var, 'Not set')
    print(f"<tr><td><span class='code'>{var}</span></td><td>{html.escape(value)}</td></tr>")

print("""            </table>
        </div>
        
        <div class="section">
            <h2>📡 HTTP Headers</h2>
            <table>
                <tr><th>Header</th><th>Value</th></tr>""")

# Display HTTP headers
http_headers = {k.replace('HTTP_', '').replace('_', '-'): v 
                for k, v in os.environ.items() if k.startswith('HTTP_')}

for header, value in sorted(http_headers.items()):
    print(f"<tr><td><span class='code'>{header}</span></td><td>{html.escape(value)}</td></tr>")

print("</table></div>")

# Show POST data if available
if method == 'POST' and post_data:
    print(f"""
        <div class="section">
            <h2>📝 POST Data</h2>
            <div class="json-output">{html.escape(post_data)}</div>
        </div>""")

# Show parsed parameters
if params:
    print("""
        <div class="section">
            <h2>🔍 Parsed URL Parameters</h2>
            <table>
                <tr><th>Parameter</th><th>Value</th></tr>""")
    
    for key, value in params.items():
        print(f"<tr><td><span class='code'>{html.escape(key)}</span></td><td>{html.escape(value)}</td></tr>")
    
    print("</table></div>")

# Python modules test
print("""
        <div class="section">
            <h2>📦 Python Modules Test</h2>
            <table>
                <tr><th>Module</th><th>Status</th><th>Version/Info</th></tr>""")

modules_to_test = [
    ('json', 'JSON processing'),
    ('urllib', 'URL handling'),
    ('datetime', 'Date/time operations'),
    ('os', 'Operating system interface'),
    ('sys', 'System parameters'),
    ('platform', 'Platform identification'),
    ('html', 'HTML utilities'),
    ('hashlib', 'Cryptographic hashing'),
    ('base64', 'Base64 encoding'),
    ('sqlite3', 'SQLite database'),
    ('requests', 'HTTP library'),
    ('numpy', 'Scientific computing'),
    ('flask', 'Web framework')
]

for module_name, description in modules_to_test:
    try:
        module = __import__(module_name)
        version = getattr(module, '__version__', 'Available')
        status = f'<span class="status-good">✓ Available</span>'
        info = f'{version} - {description}'
    except ImportError:
        status = f'<span class="status-bad">✗ Not Available</span>'
        info = description
    
    print(f"<tr><td><span class='code'>{module_name}</span></td><td>{status}</td><td>{info}</td></tr>")

print("""</table></div>
        
        <div class="section">
            <h2>🧮 Python Code Demonstration</h2>
            <p>Here are some Python calculations and demonstrations:</p>
            <div class="json-output">""")

# Demonstrate Python capabilities
calculations = {
    "fibonacci_sequence": [0, 1, 1, 2, 3, 5, 8, 13, 21, 34],
    "prime_numbers_under_50": [n for n in range(2, 50) if all(n % i != 0 for i in range(2, int(n**0.5) + 1))],
    "current_timestamp": datetime.datetime.now().timestamp(),
    "system_info": {
        "python_version": sys.version,
        "platform": platform.uname()._asdict(),
        "environment_variables_count": len(os.environ)
    },
    "request_summary": {
        "method": method,
        "has_query_params": bool(query),
        "has_post_data": bool(post_data),
        "content_length": content_length
    }
}

print(json.dumps(calculations, indent=2, default=str))

print("""</div>
        </div>
        
        <div class="section">
            <h2>🎯 Interactive Features</h2>
            <ul class="feature-list">
                <li><strong>Environment Inspection:</strong> Complete CGI environment analysis</li>
                <li><strong>Request Parsing:</strong> GET and POST data handling</li>
                <li><strong>Module Testing:</strong> Python library availability check</li>
                <li><strong>JSON Output:</strong> Structured data demonstration</li>
                <li><strong>Security:</strong> HTML escaping for safe output</li>
                <li><strong>Responsive Design:</strong> Modern CSS Grid layout</li>
            </ul>
            
            <p>Try these URLs to test different features:</p>
            <ul>
                <li><span class="code">?name=John&amp;age=25</span> - Test URL parameters</li>
                <li><span class="code">?test=json&amp;format=pretty</span> - Different parameter combinations</li>
                <li>Use POST method to test request body parsing</li>
            </ul>
        </div>
    </div>
</body>
</html>""")
