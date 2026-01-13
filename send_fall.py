import requests
import time

url = "http://127.0.0.1:5000/sos"

payload = {
    "device_id": "PC-01",
    "latitude": 12.9720,
    "longitude": 77.5950,
    "alert_type": "FALL_DETECTED"
}

print("\n🤕 Sending FALL alert...")
time.sleep(1)

response = requests.post(url, json=payload)
print("Status:", response.status_code)
print("Response:", response.text)
