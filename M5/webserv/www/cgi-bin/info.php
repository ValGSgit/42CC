#!/usr/bin/env php
<?php
header("Content-Type: text/html");
?>
<!DOCTYPE html>
<html>
<head>
    <title>PHP CGI Information</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 40px; background: #f5f5f5; }
        .container { background: white; padding: 30px; border-radius: 10px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }
        h1 { color: #8892bf; }
        .section { background: #f8f9fa; padding: 15px; margin: 15px 0; border-radius: 5px; }
        table { width: 100%; border-collapse: collapse; }
        th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }
        th { background-color: #8892bf; color: white; }
        .env-var { font-family: monospace; background: #f1f1f1; padding: 2px 4px; }
    </style>
</head>
<body>
    <div class="container">
        <h1>🐘 PHP CGI Test Script</h1>
        
        <div class="section">
            <h2>Server Information</h2>
            <table>
                <tr><th>Property</th><th>Value</th></tr>
                <tr><td>PHP Version</td><td><?php echo phpversion(); ?></td></tr>
                <tr><td>Server Software</td><td><?php echo $_SERVER['SERVER_SOFTWARE'] ?? 'Unknown'; ?></td></tr>
                <tr><td>Request Method</td><td><?php echo $_SERVER['REQUEST_METHOD'] ?? 'Unknown'; ?></td></tr>
                <tr><td>Request URI</td><td><?php echo $_SERVER['REQUEST_URI'] ?? 'Unknown'; ?></td></tr>
                <tr><td>Query String</td><td><?php echo $_SERVER['QUERY_STRING'] ?? '(empty)'; ?></td></tr>
                <tr><td>Server Time</td><td><?php echo date('Y-m-d H:i:s T'); ?></td></tr>
            </table>
        </div>
        
        <div class="section">
            <h2>CGI Environment Variables</h2>
            <table>
                <tr><th>Variable</th><th>Value</th></tr>
                <?php
                $cgi_vars = ['GATEWAY_INTERFACE', 'SERVER_NAME', 'SERVER_PORT', 'SERVER_PROTOCOL', 
                           'DOCUMENT_ROOT', 'SCRIPT_NAME', 'SCRIPT_FILENAME', 'CONTENT_TYPE', 'CONTENT_LENGTH'];
                
                foreach ($cgi_vars as $var) {
                    $value = $_SERVER[$var] ?? 'Not set';
                    echo "<tr><td class='env-var'>$var</td><td>$value</td></tr>";
                }
                ?>
            </table>
        </div>
        
        <div class="section">
            <h2>Request Headers</h2>
            <table>
                <tr><th>Header</th><th>Value</th></tr>
                <?php
                foreach ($_SERVER as $key => $value) {
                    if (strpos($key, 'HTTP_') === 0) {
                        $header = str_replace('HTTP_', '', $key);
                        $header = str_replace('_', '-', $header);
                        echo "<tr><td class='env-var'>$header</td><td>$value</td></tr>";
                    }
                }
                ?>
            </table>
        </div>
        
        <?php if ($_SERVER['REQUEST_METHOD'] === 'POST'): ?>
        <div class="section">
            <h2>POST Data</h2>
            <pre><?php echo htmlspecialchars(file_get_contents('php://input')); ?></pre>
        </div>
        <?php endif; ?>
        
        <div class="section">
            <h2>Form Test</h2>
            <form method="POST" action="<?php echo $_SERVER['SCRIPT_NAME']; ?>">
                <label for="name">Name:</label>
                <input type="text" id="name" name="name" value="<?php echo $_POST['name'] ?? ''; ?>">
                <label for="message">Message:</label>
                <textarea id="message" name="message"><?php echo $_POST['message'] ?? ''; ?></textarea>
                <button type="submit">Submit Test</button>
            </form>
            
            <?php if (!empty($_POST)): ?>
            <h3>Submitted Data:</h3>
            <pre><?php print_r($_POST); ?></pre>
            <?php endif; ?>
        </div>
    </div>
</body>
</html>
