#!/usr/bin/env php
<?php

// Manual query string parsing for CGI
$query_string = getenv('QUERY_STRING');
parse_str($query_string, $_GET);

// Parse query parameters
$data_str = isset($_GET['data']) ? urldecode($_GET['data']) : '23,45,67,34,89,56';
$labels_str = isset($_GET['labels']) ? urldecode($_GET['labels']) : 'Jan,Feb,Mar,Apr,May,Jun';
$type = isset($_GET['type']) ? $_GET['type'] : 'bar';

$data = array_map('floatval', explode(',', $data_str));
$labels = explode(',', $labels_str);

// Ensure arrays have the same length
$count = min(count($data), count($labels));
$data = array_slice($data, 0, $count);
$labels = array_slice($labels, 0, $count);

// Calculate statistics
$max_value = max($data);
$min_value = min($data);
$avg_value = array_sum($data) / count($data);
$total = array_sum($data);

// Output headers for CGI
echo "Content-Type: text/html\r\n\r\n";
?>
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <style>
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: linear-gradient(135deg, #fa709a 0%, #fee140 100%);
            padding: 20px;
            margin: 0;
        }
        .container {
            background: white;
            border-radius: 20px;
            padding: 30px;
            max-width: 800px;
            margin: 0 auto;
            box-shadow: 0 15px 50px rgba(0,0,0,0.2);
        }
        .header {
            color: #2c3e50;
            font-size: 1.8em;
            text-align: center;
            margin-bottom: 30px;
            border-bottom: 3px solid #fa709a;
            padding-bottom: 15px;
        }
        .chart {
            margin: 30px 0;
        }
        .bar-container {
            margin: 15px 0;
            display: flex;
            align-items: center;
        }
        .bar-label {
            width: 80px;
            font-weight: bold;
            color: #555;
        }
        .bar-wrapper {
            flex: 1;
            background: #f0f0f0;
            border-radius: 10px;
            overflow: hidden;
            margin: 0 10px;
            position: relative;
            height: 35px;
        }
        .bar {
            height: 100%;
            background: linear-gradient(90deg, #667eea 0%, #764ba2 100%);
            border-radius: 10px;
            display: flex;
            align-items: center;
            justify-content: flex-end;
            padding-right: 10px;
            color: white;
            font-weight: bold;
            transition: width 0.5s ease;
        }
        .bar-value {
            width: 60px;
            text-align: right;
            font-weight: bold;
            color: #333;
        }
        .stats-grid {
            display: grid;
            grid-template-columns: repeat(4, 1fr);
            gap: 15px;
            margin: 30px 0;
        }
        .stat-box {
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            padding: 20px;
            border-radius: 15px;
            text-align: center;
            box-shadow: 0 5px 15px rgba(0,0,0,0.1);
        }
        .stat-value {
            font-size: 2em;
            font-weight: bold;
            margin-bottom: 5px;
        }
        .stat-label {
            font-size: 0.9em;
            opacity: 0.9;
        }
        .progress-item {
            margin: 20px 0;
        }
        .progress-header {
            display: flex;
            justify-content: space-between;
            margin-bottom: 8px;
            font-size: 0.95em;
        }
        .progress-bar-bg {
            background: #e9ecef;
            border-radius: 10px;
            overflow: hidden;
            height: 25px;
        }
        .progress-bar-fill {
            height: 100%;
            background: linear-gradient(90deg, #fa709a 0%, #fee140 100%);
            border-radius: 10px;
            transition: width 0.5s ease;
        }
        .info {
            background: #d1f2eb;
            border-left: 4px solid #1abc9c;
            padding: 15px;
            border-radius: 5px;
            margin-top: 20px;
            font-size: 0.9em;
            color: #0e6655;
        }
        .badge {
            display: inline-block;
            background: #fa709a;
            color: white;
            padding: 5px 15px;
            border-radius: 20px;
            font-size: 0.85em;
            margin-top: 15px;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">🐘 Data Visualization Dashboard</div>
        
        <?php if ($type == 'bar' || $type == 'progress'): ?>
            <div class="chart">
                <?php foreach ($data as $index => $value): ?>
                    <?php $percentage = ($max_value > 0) ? ($value / $max_value * 100) : 0; ?>
                    
                    <?php if ($type == 'bar'): ?>
                        <div class="bar-container">
                            <div class="bar-label"><?php echo htmlspecialchars($labels[$index]); ?></div>
                            <div class="bar-wrapper">
                                <div class="bar" style="width: <?php echo $percentage; ?>%">
                                    <?php if ($percentage > 15): ?>
                                        <?php echo number_format($value, 1); ?>
                                    <?php endif; ?>
                                </div>
                            </div>
                            <div class="bar-value"><?php echo number_format($value, 1); ?></div>
                        </div>
                    <?php else: ?>
                        <div class="progress-item">
                            <div class="progress-header">
                                <strong><?php echo htmlspecialchars($labels[$index]); ?></strong>
                                <span><?php echo number_format($value, 1); ?> (<?php echo number_format($percentage, 1); ?>%)</span>
                            </div>
                            <div class="progress-bar-bg">
                                <div class="progress-bar-fill" style="width: <?php echo $percentage; ?>%"></div>
                            </div>
                        </div>
                    <?php endif; ?>
                <?php endforeach; ?>
            </div>
        <?php endif; ?>
        
        <div class="stats-grid">
            <div class="stat-box">
                <div class="stat-value"><?php echo number_format($max_value, 1); ?></div>
                <div class="stat-label">Maximum</div>
            </div>
            <div class="stat-box">
                <div class="stat-value"><?php echo number_format($min_value, 1); ?></div>
                <div class="stat-label">Minimum</div>
            </div>
            <div class="stat-box">
                <div class="stat-value"><?php echo number_format($avg_value, 1); ?></div>
                <div class="stat-label">Average</div>
            </div>
            <div class="stat-box">
                <div class="stat-value"><?php echo number_format($total, 1); ?></div>
                <div class="stat-label">Total</div>
            </div>
        </div>
        
        <div class="info">
            <strong>🐘 PHP Data Visualizer</strong><br>
            Processing <?php echo count($data); ?> data points with real-time calculation<br>
            Chart Type: <?php echo ucfirst($type); ?>
        </div>
        
        <div style="text-align: center;">
            <span class="badge">✓ PHP <?php echo phpversion(); ?></span>
        </div>
    </div>
</body>
</html>
