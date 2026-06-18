#!/usr/bin/env python3

import os
import sys
from datetime import datetime

# Collect all CGI environment variables
env_vars = {
    'SERVER_SOFTWARE': os.environ.get('SERVER_SOFTWARE', 'N/A'),
    'SERVER_NAME': os.environ.get('SERVER_NAME', 'N/A'),
    'GATEWAY_INTERFACE': os.environ.get('GATEWAY_INTERFACE', 'N/A'),
    'SERVER_PROTOCOL': os.environ.get('SERVER_PROTOCOL', 'N/A'),
    'SERVER_PORT': os.environ.get('SERVER_PORT', 'N/A'),
    'REQUEST_METHOD': os.environ.get('REQUEST_METHOD', 'N/A'),
    'PATH_INFO': os.environ.get('PATH_INFO', 'N/A'),
    'PATH_TRANSLATED': os.environ.get('PATH_TRANSLATED', 'N/A'),
    'SCRIPT_NAME': os.environ.get('SCRIPT_NAME', 'N/A'),
    'QUERY_STRING': os.environ.get('QUERY_STRING', 'N/A'),
    'REMOTE_HOST': os.environ.get('REMOTE_HOST', 'N/A'),
    'REMOTE_ADDR': os.environ.get('REMOTE_ADDR', 'N/A'),
    'AUTH_TYPE': os.environ.get('AUTH_TYPE', 'N/A'),
    'REMOTE_USER': os.environ.get('REMOTE_USER', 'N/A'),
    'CONTENT_TYPE': os.environ.get('CONTENT_TYPE', 'N/A'),
    'CONTENT_LENGTH': os.environ.get('CONTENT_LENGTH', 'N/A'),
    'HTTP_USER_AGENT': os.environ.get('HTTP_USER_AGENT', 'N/A'),
    'HTTP_ACCEPT': os.environ.get('HTTP_ACCEPT', 'N/A'),
    'HTTP_HOST': os.environ.get('HTTP_HOST', 'N/A'),
}

# Generate HTML
print("Content-Type: text/html\r")
print("\r")
print(f"""<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <style>
        body {{
            font-family: 'Consolas', 'Monaco', monospace;
            background: linear-gradient(135deg, #0f2027 0%, #203a43 50%, #2c5364 100%);
            padding: 20px;
            margin: 0;
            color: #ecf0f1;
        }}
        .container {{
            max-width: 1000px;
            margin: 0 auto;
            background: #1a1a2e;
            border-radius: 15px;
            padding: 30px;
            box-shadow: 0 20px 60px rgba(0,0,0,0.5);
        }}
        .header {{
            text-align: center;
            margin-bottom: 30px;
            padding-bottom: 20px;
            border-bottom: 3px solid #16a085;
        }}
        .header h1 {{
            color: #16a085;
            font-size: 2em;
            margin-bottom: 10px;
        }}
        .timestamp {{
            color: #95a5a6;
            font-size: 0.9em;
        }}
        .env-grid {{
            display: grid;
            gap: 15px;
            margin: 20px 0;
        }}
        .env-item {{
            background: #16213e;
            border-left: 4px solid #16a085;
            border-radius: 8px;
            padding: 15px;
            transition: all 0.3s;
        }}
        .env-item:hover {{
            background: #1f2d47;
            transform: translateX(5px);
        }}
        .env-key {{
            color: #3498db;
            font-weight: bold;
            margin-bottom: 5px;
            font-size: 0.95em;
        }}
        .env-value {{
            color: #2ecc71;
            word-wrap: break-word;
            font-size: 1em;
            padding-left: 10px;
        }}
        .section {{
            margin: 30px 0;
        }}
        .section-title {{
            color: #e74c3c;
            font-size: 1.3em;
            margin-bottom: 15px;
            padding-bottom: 10px;
            border-bottom: 2px solid #34495e;
        }}
        .stats-grid {{
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
            gap: 15px;
            margin: 20px 0;
        }}
        .stat-card {{
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            padding: 20px;
            border-radius: 10px;
            text-align: center;
        }}
        .stat-value {{
            font-size: 1.8em;
            font-weight: bold;
            margin-bottom: 5px;
        }}
        .stat-label {{
            font-size: 0.9em;
            opacity: 0.9;
        }}
        .badge {{
            display: inline-block;
            background: #16a085;
            color: white;
            padding: 5px 15px;
            border-radius: 20px;
            font-size: 0.85em;
            margin: 5px;
        }}
        .code-block {{
            background: #0d1117;
            border: 1px solid #30363d;
            border-radius: 8px;
            padding: 15px;
            color: #c9d1d9;
            overflow-x: auto;
            margin: 15px 0;
        }}
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>🔧 CGI Environment Inspector</h1>
            <div class="timestamp">Generated at {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}</div>
        </div>
        
        <div class="stats-grid">
            <div class="stat-card">
                <div class="stat-value">{len(env_vars)}</div>
                <div class="stat-label">Environment Variables</div>
            </div>
            <div class="stat-card">
                <div class="stat-value">{env_vars['REQUEST_METHOD']}</div>
                <div class="stat-label">Request Method</div>
            </div>
            <div class="stat-card">
                <div class="stat-value">{env_vars['SERVER_PROTOCOL']}</div>
                <div class="stat-label">Protocol</div>
            </div>
            <div class="stat-card">
                <div class="stat-value">{env_vars['SERVER_PORT']}</div>
                <div class="stat-label">Server Port</div>
            </div>
        </div>
        
        <div class="section">
            <div class="section-title">📡 Server Information</div>
            <div class="env-grid">
                <div class="env-item">
                    <div class="env-key">SERVER_SOFTWARE</div>
                    <div class="env-value">{env_vars['SERVER_SOFTWARE']}</div>
                </div>
                <div class="env-item">
                    <div class="env-key">SERVER_NAME</div>
                    <div class="env-value">{env_vars['SERVER_NAME']}</div>
                </div>
                <div class="env-item">
                    <div class="env-key">GATEWAY_INTERFACE</div>
                    <div class="env-value">{env_vars['GATEWAY_INTERFACE']}</div>
                </div>
                <div class="env-item">
                    <div class="env-key">SERVER_PROTOCOL</div>
                    <div class="env-value">{env_vars['SERVER_PROTOCOL']}</div>
                </div>
            </div>
        </div>
        
        <div class="section">
            <div class="section-title">🌐 Request Information</div>
            <div class="env-grid">
                <div class="env-item">
                    <div class="env-key">REQUEST_METHOD</div>
                    <div class="env-value">{env_vars['REQUEST_METHOD']}</div>
                </div>
                <div class="env-item">
                    <div class="env-key">QUERY_STRING</div>
                    <div class="env-value">{env_vars['QUERY_STRING'] if env_vars['QUERY_STRING'] != 'N/A' else '(empty)'}</div>
                </div>
                <div class="env-item">
                    <div class="env-key">SCRIPT_NAME</div>
                    <div class="env-value">{env_vars['SCRIPT_NAME']}</div>
                </div>
                <div class="env-item">
                    <div class="env-key">PATH_INFO</div>
                    <div class="env-value">{env_vars['PATH_INFO']}</div>
                </div>
            </div>
        </div>
        
        <div class="section">
            <div class="section-title">🖥️ Client Information</div>
            <div class="env-grid">
                <div class="env-item">
                    <div class="env-key">REMOTE_ADDR</div>
                    <div class="env-value">{env_vars['REMOTE_ADDR']}</div>
                </div>
                <div class="env-item">
                    <div class="env-key">HTTP_HOST</div>
                    <div class="env-value">{env_vars['HTTP_HOST']}</div>
                </div>
                <div class="env-item">
                    <div class="env-key">HTTP_USER_AGENT</div>
                    <div class="env-value">{env_vars['HTTP_USER_AGENT']}</div>
                </div>
            </div>
        </div>
        
        <div class="section">
            <div class="section-title">🐍 Python Runtime Information</div>
            <div class="code-block">
                Python Version: {sys.version}<br>
                Executable: {sys.executable}<br>
                Platform: {sys.platform}
            </div>
        </div>
        
        <div style="text-align: center; margin-top: 30px;">
            <span class="badge">✓ CGI/1.1</span>
            <span class="badge">Python {sys.version.split()[0]}</span>
            <span class="badge">WebServ</span>
        </div>
    </div>
</body>
</html>""")
