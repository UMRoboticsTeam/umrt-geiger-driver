import os
import pty
import time
import random

# Create a pseudo-terminal (pty)
master_fd, slave_fd = pty.openpty()
slave_name = os.ttyname(slave_fd)
print(f"Simulated UART device created at: {slave_name}")

# Start sending data
try:
    while True:
        number = random.randint(0, 999)
        ascii_data = f"{number}\n".encode('ascii')  # newline = 0x0A
        os.write(master_fd, ascii_data)
        time.sleep(0.1)  # ~10 samples per second
except KeyboardInterrupt:
    print("\nStopped.")