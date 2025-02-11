#!/usr/bin/env python3

import os
import urllib.parse
import sys

# Get Content-Length from environment
content_length = os.environ.get('CONTENT_LENGTH')
if content_length:
    try:
        content_length = int(content_length)
    except ValueError:
        print("Content-Type: text/html\n")
        print("<h1>Error: Invalid Content-Length</h1>")
        sys.exit(1)

    # Read exactly `content_length` bytes from stdin
    raw_input = sys.stdin.read(content_length)
else:
    raw_input = ''

# Debugging logs (print to stderr)
print("Debugging CGI script", file=sys.stderr)
print(f"Raw input: {raw_input}", file=sys.stderr)

# Parse input manually
form_data = urllib.parse.parse_qs(raw_input)
name = form_data.get('name', [''])[0]
email = form_data.get('email', [''])[0]

# Save the data to a file
with open('www/uploads/form.txt', 'a') as f:
    f.write(f"Name: {name}\nEmail: {email}\n---\n\n")

# Return an HTML response
# print("Content-Type: text/html\n")  # Correct header
print(f"""<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link href="https://fonts.googleapis.com/css?family=Comic+Code+Ligatures&display=swap" rel="stylesheet" />
    <link rel="stylesheet" href="/www/css/style.css">
    <link rel="stylesheet" href="/www/css/index.css">
    <title>Form Submitted Successfully</title>
</head>
<body>
    <div class="container">
        <div class="welcome-text">
            <span class="main-title">Success!</span>
            <span class="sub-title">Your form has been submitted.</span>
        </div>
        <div class="success-icon">
            <i class="fa fa-check-circle" style="font-size: 100px; color: #4CAF50;"></i>
        </div>
        <div class="success-message">
            <p>Thank you, {name}, for submitting your data. We’ve received your form and will get back to you shortly.</p>
        </div>
        <div class="button-container">
            <a href="/www/index.html" class="button">
                <span class="about-text">Return to Home</span>
            </a>
        </div>
    </div>
</body>
</html>
""")

