import requests
import time

url = "http://127.0.0.1:5000/sos"

payload = {
    "device_id": "PC-01",
    "latitude": 12.9730,
    "longitude": 77.5960,
    "alert_type": "TAMPER_REMOVAL"
}

print("\n🔓 Sending TAMPER alert...")
time.sleep(1)

response = requests.post(url, json=payload)
print("Status:", response.status_code)
print("Response:", response.text)
