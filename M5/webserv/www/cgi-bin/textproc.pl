#!/usr/bin/perl

use strict;
use warnings;

# Manual query string parsing for CGI
my $query_string = $ENV{'QUERY_STRING'} || '';
my %params;

# Parse query string
foreach my $pair (split(/&/, $query_string)) {
    if ($pair =~ /^([^=]+)=(.*)$/) {
        my ($key, $value) = ($1, $2);
        # Decode URL encoding
        $value =~ s/\+/ /g;
        $value =~ s/%([0-9A-Fa-f]{2})/chr(hex($1))/eg;
        $params{$key} = $value;
    }
}

my $text = $params{'text'} || 'The quick brown fox jumps over the lazy dog';
my $op = $params{'op'} || 'analyze';

my $output_text = $text;
my $operation_name = "Text Analysis";

if ($op eq 'reverse') {
    $output_text = reverse($text);
    $operation_name = "Reversed Text";
} elsif ($op eq 'uppercase') {
    $output_text = uc($text);
    $operation_name = "Uppercase Conversion";
} elsif ($op eq 'titlecase') {
    $output_text = join(' ', map { ucfirst(lc($_)) } split(/\s+/, $text));
    $operation_name = "Title Case Conversion";
}

# Calculate statistics
my $char_count = length($text);
my $word_count = scalar(split(/\s+/, $text));
my $line_count = scalar(split(/\n/, $text));
my %char_freq;
foreach my $char (split //, lc($text)) {
    $char_freq{$char}++ if $char =~ /[a-z]/;
}
my @sorted_chars = sort { $char_freq{$b} <=> $char_freq{$a} } keys %char_freq;
my $most_common = $sorted_chars[0] || 'N/A';

print "Content-Type: text/html\r\n\r\n";
print <<HTML;
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <style>
        body {
            font-family: 'Georgia', serif;
            background: linear-gradient(135deg, #f093fb 0%, #f5576c 100%);
            padding: 20px;
            margin: 0;
        }
        .container {
            background: white;
            border-radius: 15px;
            padding: 30px;
            max-width: 600px;
            margin: 0 auto;
            box-shadow: 0 10px 40px rgba(0,0,0,0.2);
        }
        .header {
            color: #2c3e50;
            font-size: 1.5em;
            margin-bottom: 20px;
            text-align: center;
            border-bottom: 3px solid #f5576c;
            padding-bottom: 10px;
        }
        .text-box {
            background: #f8f9fa;
            border: 2px solid #e9ecef;
            border-radius: 10px;
            padding: 20px;
            margin: 20px 0;
            font-size: 1.1em;
            line-height: 1.6;
            color: #495057;
            word-wrap: break-word;
        }
        .stats-grid {
            display: grid;
            grid-template-columns: repeat(2, 1fr);
            gap: 15px;
            margin: 20px 0;
        }
        .stat-card {
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            padding: 15px;
            border-radius: 10px;
            text-align: center;
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
        .badge {
            display: inline-block;
            background: #f093fb;
            color: white;
            padding: 5px 15px;
            border-radius: 20px;
            font-size: 0.85em;
            margin-top: 15px;
        }
        .info {
            background: #fff3cd;
            border-left: 4px solid #ffc107;
            padding: 15px;
            border-radius: 5px;
            margin-top: 20px;
            font-size: 0.9em;
            color: #856404;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">🐪 $operation_name</div>
        
        <div class="text-box">$output_text</div>
        
        <div class="stats-grid">
            <div class="stat-card">
                <div class="stat-value">$char_count</div>
                <div class="stat-label">Characters</div>
            </div>
            <div class="stat-card">
                <div class="stat-value">$word_count</div>
                <div class="stat-label">Words</div>
            </div>
            <div class="stat-card">
                <div class="stat-value">$line_count</div>
                <div class="stat-label">Lines</div>
            </div>
            <div class="stat-card">
                <div class="stat-value">$most_common</div>
                <div class="stat-label">Most Common</div>
            </div>
        </div>
        
        <div class="info">
            <strong>🐪 Perl Text Processor</strong><br>
            Powered by Perl CGI with advanced text manipulation capabilities
        </div>
        
        <div style="text-align: center;">
            <span class="badge">✓ Perl CGI</span>
        </div>
    </div>
</body>
</html>
HTML
