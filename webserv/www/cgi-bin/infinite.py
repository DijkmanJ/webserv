#!/usr/bin/env python3

import time
import sys

# Print the necessary HTTP headers
print("Content-Type: text/plain\n")

# Start an infinite loop
while True:
    # Output a line of text
    print("This is an infinite CGI loop. The server is working!")
    sys.stdout.flush()  # Ensure the output is sent immediately

    # Sleep for a short time to avoid overwhelming the client
    time.sleep(0.5)  # Sleep for 0.5 seconds
