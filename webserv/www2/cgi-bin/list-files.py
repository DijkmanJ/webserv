#!/usr/bin/env python3
import os
import sys
import json

# Pad naar de upload-map
uploads_path = 'www2/uploads'

# Zorg ervoor dat het pad bestaat
if not os.path.exists(uploads_path):
    print("Content-Type: text/html")
    print("\r\n")
    print("<h1>Uploads directory not found</h1>")
    exit()

# Lijst de bestanden in de uploads directory
files = os.listdir(uploads_path)

# Filter eventueel alleen bepaalde bestandstypen als je wilt
files = [f for f in files if os.path.isfile(os.path.join(uploads_path, f))]

# Print JSON response
sys.stdout.flush()
print(json.dumps(files))
print("\r\n")
print("\r\n")
print("\r\n")
