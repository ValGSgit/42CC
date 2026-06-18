#!/usr/bin/env python3

import os
import sys
import re

# Parse query string
query = os.environ.get('QUERY_STRING', '')
matches = re.findall(r'(num1|num2|op)=([^=]+?)(?=num1=|num2=|op=|$)', query)
params = dict(matches)

# Safely get parameters with error handling
try:
    num1 = int(params['num1'])#float(params.get('num1', ['0'])[0])
except (ValueError, IndexError):
    num1 = 0.0

try:
    num2 = int(params['num2'])#float(params.get('num2', ['0'])[0])
except (ValueError, IndexError):
    num2 = 0.0

op = params['op'] if 'op' in params else 'add'

# Perform calculation
result = 0
operation_symbol = '+'
operation_name = 'Addition'

if op == 'add':
    result = num1 + num2
    operation_symbol = '+'
    operation_name = 'Addition'
elif op == 'subtract':
    result = num1 - num2
    operation_symbol = '-'
    operation_name = 'Subtraction'
elif op == 'multiply':
    result = num1 * num2
    operation_symbol = '×'
    operation_name = 'Multiplication'
elif op == 'divide':
    if num2 != 0:
        result = num1 / num2
        operation_symbol = '÷'
        operation_name = 'Division'
    else:
        result = 'Error: Division by zero'
        operation_symbol = '÷'
        operation_name = 'Division'
elif op == 'power':
    result = num1 ** num2
    operation_symbol = '^'
    operation_name = 'Power'

# Generate HTML
print("Content-Type: text/html\r")
print("\r")
print(f"""<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <style>
        body {{
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            padding: 20px;
            margin: 0;
            display: flex;
            justify-content: center;
            align-items: center;
            min-height: 100vh;
        }}
        .calculator {{
            background: white;
            border-radius: 20px;
            padding: 30px;
            box-shadow: 0 10px 40px rgba(0,0,0,0.2);
            max-width: 400px;
            width: 100%;
        }}
        .display {{
            background: #f8f9fa;
            border-radius: 10px;
            padding: 20px;
            margin-bottom: 20px;
            text-align: center;
        }}
        .operation {{
            color: #6c757d;
            font-size: 0.9em;
            margin-bottom: 10px;
        }}
        .expression {{
            font-size: 1.5em;
            color: #495057;
            margin: 15px 0;
            font-weight: 300;
        }}
        .result {{
            font-size: 2.5em;
            color: #667eea;
            font-weight: bold;
            margin-top: 15px;
        }}
        .error {{
            color: #dc3545;
        }}
        .info {{
            background: #e7f3ff;
            border-left: 4px solid #2196F3;
            padding: 15px;
            border-radius: 5px;
            margin-top: 20px;
            font-size: 0.9em;
        }}
        .info-title {{
            font-weight: bold;
            color: #1976D2;
            margin-bottom: 8px;
        }}
        .info-item {{
            color: #455a64;
            margin: 5px 0;
        }}
        .badge {{
            display: inline-block;
            background: #667eea;
            color: white;
            padding: 5px 12px;
            border-radius: 15px;
            font-size: 0.85em;
            margin-top: 10px;
        }}
    </style>
</head>
<body>
    <div class="calculator">
        <div class="display">
            <div class="operation">{operation_name}</div>
            <div class="expression">
                {num1} {operation_symbol} {num2}
            </div>
            <div class="result {'error' if isinstance(result, str) else ''}">
                = {result if isinstance(result, str) else f'{result:.4f}'.rstrip('0').rstrip('.')}
            </div>
        </div>
        
        <div class="info">
            <div class="info-title">🐍 Python CGI Calculator</div>
            <div class="info-item">✓ Query string parsing</div>
            <div class="info-item">✓ Dynamic HTML generation</div>
            <div class="info-item">✓ Mathematical operations</div>
            <div class="info-item">✓ Error handling</div>
            <div class="badge">Python {sys.version.split()[0]}</div>
        </div>
    </div>
</body>
</html>""")
