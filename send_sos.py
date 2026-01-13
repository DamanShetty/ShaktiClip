import requests
import time

url = "http://127.0.0.1:5000/alert"



payload = {
    "device_id": "PC-01",
    "latitude": 12.9716,
    "longitude": 77.5946,
    "alert_type": "SOS_BUTTON"
}

print("\n🚨 Sending SOS alert...")
time.sleep(1)

response = requests.post(url, json=payload)
print("Status:", response.status_code)
print("Response:", response.text)
