Import("env")
import os

# Load .env file
try:
    with open(".env", "r") as f:
        for line in f:
            line = line.strip()
            if line and not line.startswith("#"):
                key, value = line.split("=", 1)
                os.environ[key.strip()] = value.strip()
    print("✓ Loaded credentials from .env file")
except FileNotFoundError:
    print("⚠ Warning: .env file not found. Please create one from .env.example")
except Exception as e:
    print(f"⚠ Error reading .env file: {e}")
