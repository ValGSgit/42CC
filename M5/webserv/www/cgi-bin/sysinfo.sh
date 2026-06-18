#!/bin/bash

# Parse query string
IFS='&' read -ra PARAMS <<< "$QUERY_STRING"
TYPE="full"

for param in "${PARAMS[@]}"; do
    IFS='=' read -r key value <<< "$param"
    if [ "$key" = "type" ]; then
        TYPE="$value"
    fi
done

# Get system information
HOSTNAME=$(hostname)
DATE=$(date '+%A, %B %d, %Y')
TIME=$(date '+%H:%M:%S %Z')
UPTIME=$(uptime -p 2>/dev/null || echo "N/A")
USER=$(whoami)
SHELL_TYPE=$(basename "$SHELL")
PWD_DIR=$(pwd)

echo "Content-Type: text/html"
echo ""
cat << EOF
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <style>
        body {
            font-family: 'Courier New', monospace;
            background: linear-gradient(135deg, #232526 0%, #414345 100%);
            padding: 20px;
            margin: 0;
            color: #f0f0f0;
        }
        .terminal {
            background: #1e1e1e;
            border-radius: 10px;
            padding: 20px;
            box-shadow: 0 10px 40px rgba(0,0,0,0.5);
            border: 1px solid #333;
        }
        .header {
            color: #4EC9B0;
            font-size: 1.3em;
            margin-bottom: 15px;
            border-bottom: 2px solid #4EC9B0;
            padding-bottom: 10px;
        }
        .info-grid {
            display: grid;
            gap: 15px;
            margin: 20px 0;
        }
        .info-row {
            background: #252526;
            padding: 15px;
            border-radius: 5px;
            border-left: 3px solid #569cd6;
        }
        .label {
            color: #9cdcfe;
            font-weight: bold;
            margin-bottom: 5px;
        }
        .value {
            color: #ce9178;
            font-size: 1.1em;
        }
        .badge {
            display: inline-block;
            background: #0e639c;
            color: white;
            padding: 5px 12px;
            border-radius: 3px;
            font-size: 0.85em;
            margin: 5px 5px 5px 0;
        }
        .success { background: #107c10; }
        .warning { background: #ca5010; }
        .timestamp {
            color: #608b4e;
            text-align: center;
            margin-top: 20px;
            font-size: 0.9em;
        }
    </style>
</head>
<body>
    <div class="terminal">
        <div class="header">🐚 System Information Dashboard</div>
EOF

if [ "$TYPE" = "full" ] || [ "$TYPE" = "time" ]; then
    cat << EOF
        <div class="info-grid">
            <div class="info-row">
                <div class="label">📅 Date</div>
                <div class="value">$DATE</div>
            </div>
            <div class="info-row">
                <div class="label">🕐 Time</div>
                <div class="value">$TIME</div>
            </div>
        </div>
EOF
fi

if [ "$TYPE" = "full" ] || [ "$TYPE" = "system" ]; then
    cat << EOF
        <div class="info-grid">
            <div class="info-row">
                <div class="label">💻 Hostname</div>
                <div class="value">$HOSTNAME</div>
            </div>
            <div class="info-row">
                <div class="label">⏱️ Uptime</div>
                <div class="value">$UPTIME</div>
            </div>
        </div>
EOF
fi

if [ "$TYPE" = "full" ] || [ "$TYPE" = "user" ]; then
    cat << EOF
        <div class="info-grid">
            <div class="info-row">
                <div class="label">👤 User</div>
                <div class="value">$USER</div>
            </div>
            <div class="info-row">
                <div class="label">🐚 Shell</div>
                <div class="value">$SHELL_TYPE</div>
            </div>
            <div class="info-row">
                <div class="label">📁 Working Directory</div>
                <div class="value">$PWD_DIR</div>
            </div>
        </div>
EOF
fi

cat << EOF
        <div style="margin-top: 20px; text-align: center;">
            <span class="badge success">✓ Bash Script</span>
            <span class="badge">CGI Enabled</span>
            <span class="badge warning">Real-time Data</span>
        </div>
        
        <div class="timestamp">
            Generated at $(date '+%Y-%m-%d %H:%M:%S') by WebServ CGI
        </div>
    </div>
</body>
</html>
EOF
