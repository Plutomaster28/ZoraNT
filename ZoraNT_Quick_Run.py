import os
import subprocess
import time
import pyautogui  # To simulate typing

# Define MSYS2 UCRT64 shell path
msys2_path = r"C:\msys64\ucrt64.exe"
script_name = "quick_run.sh"  # The script to execute inside MSYS2
script_dir = os.path.abspath(os.path.dirname(__file__))  # Get the directory of this script

# Fancy startup message
print("=" * 40)
print("Launching ZoraNT... Please wait.")
print("=" * 40)
time.sleep(1)

# Check if MSYS2 is installed
if not os.path.exists(msys2_path):
    print("\nMSYS2 is not installed. Please install MSYS2 first.")
    input("Press Enter to exit...")
    exit(1)

# Notify user that everything is good to go
print("\nMSYS2 detected! Preparing to start ZoraNT...")
time.sleep(1)

# Inform user before launching MSYS2
print("\nOpening MSYS2 UCRT64 shell...")
time.sleep(1)

# Launch the UCRT64 shell (non-blocking)
proc = subprocess.Popen(msys2_path)

# Wait a bit for the terminal to open
time.sleep(3)  # Adjust if needed based on system speed

# Type the necessary commands inside UCRT64
pyautogui.write(f'cd "{script_dir}"\n', interval=0.05)
time.sleep(0.5)
pyautogui.write(f'bash "{script_name}"\n', interval=0.05)

# Notify user that ZoraNT has exited
print("\nZoraNT has exited.")
input("Press Enter to close this window...")
