#!/usr/bin/env ruby

# Manual CGI parameter parsing (no cgi library dependency)
query_string = ENV['QUERY_STRING'] || ''
params = {}

query_string.split('&').each do |pair|
  key, value = pair.split('=', 2)
  if key && value
    # URL decode
    value = value.gsub('+', ' ').gsub(/%([0-9A-Fa-f]{2})/) { $1.hex.chr }
    params[key] = value
  end
end

# Get rotation angles from parameters
angle_x = (params['ax'] || '0').to_f
angle_y = (params['ay'] || '0').to_f
angle_z = (params['az'] || '0').to_f
shape = params['shape'] || 'cube'
auto_rotate = params['auto'] || 'true'

# 3D Math functions
def rotate_x(point, angle)
  x, y, z = point
  rad = angle * Math::PI / 180.0
  cos_a = Math.cos(rad)
  sin_a = Math.sin(rad)
  [x, y * cos_a - z * sin_a, y * sin_a + z * cos_a]
end

def rotate_y(point, angle)
  x, y, z = point
  rad = angle * Math::PI / 180.0
  cos_a = Math.cos(rad)
  sin_a = Math.sin(rad)
  [x * cos_a + z * sin_a, y, -x * sin_a + z * cos_a]
end

def rotate_z(point, angle)
  x, y, z = point
  rad = angle * Math::PI / 180.0
  cos_a = Math.cos(rad)
  sin_a = Math.sin(rad)
  [x * cos_a - y * sin_a, x * sin_a + y * cos_a, z]
end

def project_3d_to_2d(point, distance = 4)
  x, y, z = point
  factor = distance / (distance + z)
  [x * factor * 100 + 250, y * factor * 100 + 250]
end

# Define 3D shapes
def get_cube_vertices
  [
    [-1, -1, -1], [1, -1, -1], [1, 1, -1], [-1, 1, -1],  # Back face
    [-1, -1, 1], [1, -1, 1], [1, 1, 1], [-1, 1, 1]       # Front face
  ]
end

def get_cube_edges
  [
    [0, 1], [1, 2], [2, 3], [3, 0],  # Back face
    [4, 5], [5, 6], [6, 7], [7, 4],  # Front face
    [0, 4], [1, 5], [2, 6], [3, 7]   # Connecting edges
  ]
end

def get_pyramid_vertices
  [
    [0, 1.5, 0],      # Top
    [-1, -1, -1],     # Base corners
    [1, -1, -1],
    [1, -1, 1],
    [-1, -1, 1]
  ]
end

def get_pyramid_edges
  [
    [0, 1], [0, 2], [0, 3], [0, 4],  # Top to base
    [1, 2], [2, 3], [3, 4], [4, 1]   # Base
  ]
end

def get_octahedron_vertices
  [
    [0, 1.5, 0],      # Top
    [0, -1.5, 0],     # Bottom
    [-1, 0, 0],       # Left
    [1, 0, 0],        # Right
    [0, 0, -1],       # Back
    [0, 0, 1]         # Front
  ]
end

def get_octahedron_edges
  [
    [0, 2], [0, 3], [0, 4], [0, 5],  # Top vertex
    [1, 2], [1, 3], [1, 4], [1, 5],  # Bottom vertex
    [2, 4], [4, 3], [3, 5], [5, 2]   # Middle ring
  ]
end

# Get shape data
vertices, edges = case shape
when 'pyramid'
  [get_pyramid_vertices, get_pyramid_edges]
when 'octahedron'
  [get_octahedron_vertices, get_octahedron_edges]
else
  [get_cube_vertices, get_cube_edges]
end

# Rotate and project vertices
projected = vertices.map do |vertex|
  rotated = rotate_x(vertex, angle_x)
  rotated = rotate_y(rotated, angle_y)
  rotated = rotate_z(rotated, angle_z)
  project_3d_to_2d(rotated)
end

# Generate SVG lines
svg_lines = edges.map do |edge|
  start_idx, end_idx = edge
  x1, y1 = projected[start_idx]
  x2, y2 = projected[end_idx]
  "<line x1='#{x1.round(2)}' y1='#{y1.round(2)}' x2='#{x2.round(2)}' y2='#{y2.round(2)}' stroke='#00ff00' stroke-width='2'/>"
end.join("\n")

# Generate SVG vertices
svg_vertices = projected.map do |point|
  x, y = point
  "<circle cx='#{x.round(2)}' cy='#{y.round(2)}' r='4' fill='#00ffff'/>"
end.join("\n")

puts "Content-Type: text/html\r\n\r\n"
puts <<-HTML
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Ruby 3D Engine</title>
    <style>
        body {
            font-family: 'Courier New', monospace;
            background: linear-gradient(135deg, #1a1a2e 0%, #0f0f1e 100%);
            padding: 20px;
            margin: 0;
            color: #00ff00;
        }
        .container {
            background: rgba(0, 0, 0, 0.8);
            border: 2px solid #00ff00;
            border-radius: 10px;
            padding: 20px;
            max-width: 800px;
            margin: 0 auto;
            box-shadow: 0 0 30px rgba(0, 255, 0, 0.3);
        }
        .header {
            text-align: center;
            color: #00ffff;
            font-size: 1.8em;
            margin-bottom: 20px;
            text-shadow: 0 0 10px #00ffff;
        }
        .canvas-container {
            background: #000;
            border: 2px solid #00ff00;
            border-radius: 5px;
            padding: 10px;
            margin: 20px 0;
            position: relative;
        }
        svg {
            display: block;
            margin: 0 auto;
        }
        .controls {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
            gap: 15px;
            margin: 20px 0;
        }
        .control-group {
            background: rgba(0, 255, 0, 0.1);
            border: 1px solid #00ff00;
            border-radius: 5px;
            padding: 15px;
        }
        .control-group label {
            display: block;
            color: #00ffff;
            margin-bottom: 8px;
            font-size: 0.9em;
        }
        .slider {
            width: 100%;
            -webkit-appearance: none;
            appearance: none;
            height: 6px;
            background: #1a1a2e;
            border-radius: 3px;
            outline: none;
        }
        .slider::-webkit-slider-thumb {
            -webkit-appearance: none;
            appearance: none;
            width: 16px;
            height: 16px;
            background: #00ff00;
            border-radius: 50%;
            cursor: pointer;
            box-shadow: 0 0 10px #00ff00;
        }
        .slider::-moz-range-thumb {
            width: 16px;
            height: 16px;
            background: #00ff00;
            border-radius: 50%;
            cursor: pointer;
            box-shadow: 0 0 10px #00ff00;
        }
        .value-display {
            color: #00ff00;
            text-align: center;
            margin-top: 5px;
            font-size: 1.1em;
        }
        .btn {
            background: #00ff00;
            color: #000;
            border: none;
            padding: 10px 20px;
            border-radius: 5px;
            cursor: pointer;
            font-family: 'Courier New', monospace;
            font-weight: bold;
            transition: all 0.3s;
            width: 100%;
            margin-top: 10px;
        }
        .btn:hover {
            background: #00ffff;
            box-shadow: 0 0 20px #00ffff;
        }
        .info {
            background: rgba(0, 255, 255, 0.1);
            border-left: 4px solid #00ffff;
            padding: 15px;
            border-radius: 5px;
            margin-top: 20px;
            font-size: 0.9em;
        }
        .stats {
            display: grid;
            grid-template-columns: repeat(3, 1fr);
            gap: 10px;
            margin: 15px 0;
        }
        .stat {
            text-align: center;
            padding: 10px;
            background: rgba(0, 255, 0, 0.1);
            border: 1px solid #00ff00;
            border-radius: 5px;
        }
        .stat-label {
            color: #00ffff;
            font-size: 0.8em;
        }
        .stat-value {
            color: #00ff00;
            font-size: 1.2em;
            font-weight: bold;
            margin-top: 5px;
        }
        .badge {
            display: inline-block;
            background: #ff0066;
            color: white;
            padding: 5px 12px;
            border-radius: 15px;
            font-size: 0.85em;
            margin-top: 10px;
            box-shadow: 0 0 10px #ff0066;
        }
        select {
            width: 100%;
            padding: 8px;
            background: #1a1a2e;
            color: #00ff00;
            border: 1px solid #00ff00;
            border-radius: 5px;
            font-family: 'Courier New', monospace;
            cursor: pointer;
        }
        .toggle {
            display: flex;
            align-items: center;
            gap: 10px;
        }
        input[type="checkbox"] {
            width: 20px;
            height: 20px;
            cursor: pointer;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">🎮 Ruby 3D Engine</div>
        
        <div class="stats">
            <div class="stat">
                <div class="stat-label">Shape</div>
                <div class="stat-value">#{shape.capitalize}</div>
            </div>
            <div class="stat">
                <div class="stat-label">Vertices</div>
                <div class="stat-value">#{vertices.length}</div>
            </div>
            <div class="stat">
                <div class="stat-label">Edges</div>
                <div class="stat-value">#{edges.length}</div>
            </div>
        </div>
        
        <div class="canvas-container">
            <svg width="500" height="500" viewBox="0 0 500 500">
                <!-- Grid background -->
                <defs>
                    <pattern id="grid" width="50" height="50" patternUnits="userSpaceOnUse">
                        <path d="M 50 0 L 0 0 0 50" fill="none" stroke="#003300" stroke-width="0.5"/>
                    </pattern>
                </defs>
                <rect width="500" height="500" fill="url(#grid)"/>
                
                <!-- 3D Object -->
                #{svg_lines}
                #{svg_vertices}
                
                <!-- Center point -->
                <circle cx="250" cy="250" r="3" fill="#ff0066"/>
            </svg>
        </div>
        
        <div class="controls">
            <div class="control-group">
                <label>🔄 Rotation X: <span id="val-x">#{angle_x.round(1)}°</span></label>
                <input type="range" class="slider" id="angle-x" min="0" max="360" value="#{angle_x}" step="5">
                <div class="value-display" id="display-x">#{angle_x.round(1)}°</div>
            </div>
            
            <div class="control-group">
                <label>🔄 Rotation Y: <span id="val-y">#{angle_y.round(1)}°</span></label>
                <input type="range" class="slider" id="angle-y" min="0" max="360" value="#{angle_y}" step="5">
                <div class="value-display" id="display-y">#{angle_y.round(1)}°</div>
            </div>
            
            <div class="control-group">
                <label>🔄 Rotation Z: <span id="val-z">#{angle_z.round(1)}°</span></label>
                <input type="range" class="slider" id="angle-z" min="0" max="360" value="#{angle_z}" step="5">
                <div class="value-display" id="display-z">#{angle_z.round(1)}°</div>
            </div>
        </div>
        
        <div class="controls">
            <div class="control-group">
                <label>📐 Shape</label>
                <select id="shape">
                    <option value="cube" #{shape == 'cube' ? 'selected' : ''}>Cube</option>
                    <option value="pyramid" #{shape == 'pyramid' ? 'selected' : ''}>Pyramid</option>
                    <option value="octahedron" #{shape == 'octahedron' ? 'selected' : ''}>Octahedron</option>
                </select>
            </div>
            
            <div class="control-group">
                <label class="toggle">
                    <input type="checkbox" id="auto-rotate" #{auto_rotate == 'true' ? 'checked' : ''}>
                    ⚡ Auto-Rotate
                </label>
                <button class="btn" onclick="resetRotation()">🔄 Reset</button>
            </div>
            
            <div class="control-group">
                <button class="btn" onclick="updateView()">🎯 Update View</button>
                <button class="btn" onclick="randomRotation()">🎲 Random</button>
            </div>
        </div>
        
        <div class="info">
            <strong>💎 Ruby 3D Wireframe Engine</strong><br>
            Backend: Ruby CGI with real-time 3D transformations<br>
            Rendering: SVG with matrix rotation calculations<br>
            Controls: Drag sliders to rotate, change shapes, or enable auto-rotation<br>
            <div style="text-align: center;">
                <span class="badge">✓ Ruby CGI Powered</span>
            </div>
        </div>
    </div>
    
    <script>
        let autoRotate = #{auto_rotate == 'true' ? 'true' : 'false'};
        let currentAngles = {
            x: #{angle_x},
            y: #{angle_y},
            z: #{angle_z}
        };
        
        // Update displays when sliders change
        document.getElementById('angle-x').addEventListener('input', function(e) {
            currentAngles.x = e.target.value;
            document.getElementById('display-x').textContent = e.target.value + '°';
        });
        
        document.getElementById('angle-y').addEventListener('input', function(e) {
            currentAngles.y = e.target.value;
            document.getElementById('display-y').textContent = e.target.value + '°';
        });
        
        document.getElementById('angle-z').addEventListener('input', function(e) {
            currentAngles.z = e.target.value;
            document.getElementById('display-z').textContent = e.target.value + '°';
        });
        
        document.getElementById('auto-rotate').addEventListener('change', function(e) {
            autoRotate = e.target.checked;
            if (autoRotate) startAutoRotate();
        });
        
        function updateView() {
            const shape = document.getElementById('shape').value;
            const auto = document.getElementById('auto-rotate').checked ? 'true' : 'false';
            window.location.href = '?ax=' + currentAngles.x + '&ay=' + currentAngles.y + 
                                   '&az=' + currentAngles.z + '&shape=' + shape + '&auto=' + auto;
        }
        
        function resetRotation() {
            currentAngles = {x: 0, y: 0, z: 0};
            document.getElementById('angle-x').value = 0;
            document.getElementById('angle-y').value = 0;
            document.getElementById('angle-z').value = 0;
            updateView();
        }
        
        function randomRotation() {
            currentAngles.x = Math.floor(Math.random() * 360);
            currentAngles.y = Math.floor(Math.random() * 360);
            currentAngles.z = Math.floor(Math.random() * 360);
            updateView();
        }
        
        function startAutoRotate() {
            if (autoRotate) {
                currentAngles.y = (parseFloat(currentAngles.y) + 5) % 360;
                currentAngles.x = (parseFloat(currentAngles.x) + 2) % 360;
                updateView();
            }
        }
        
        // Auto-rotate if enabled
        if (autoRotate) {
            setTimeout(startAutoRotate, 100);
        }
    </script>
</body>
</html>
HTML
