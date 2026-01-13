from flask import Flask, request, jsonify
import os, json
from datetime import datetime
from alerter import send_sms_if_configured

# Paths
APP_ROOT = os.path.dirname(os.path.abspath(__file__))
ALERT_DIR = os.path.join(APP_ROOT, "alerts")
MAPPING_FILE = os.path.join(APP_ROOT, "mapping.json")

# Make alerts folder if missing
os.makedirs(ALERT_DIR, exist_ok=True)

# Load guardian/police mapping
with open(MAPPING_FILE, "r") as f:
    mapping = json.load(f)

app = Flask(__name__)

def log_alert(device, payload):
    ts = datetime.utcnow().strftime("%Y%m%d_%H%M%S")
    filename = f"alert_{device}_{ts}.json"
    path = os.path.join(ALERT_DIR, filename)
    with open(path, "w") as f:
        json.dump(payload, f, indent=2)
    return path

@app.post("/alert")
def alert():
    data = request.get_json()
    if not data:
        return jsonify({"error": "Invalid JSON"}), 400

    device = data.get("deviceID", "unknown")
    data["server_ts"] = datetime.utcnow().isoformat()

    # Log to alerts folder
    path = log_alert(device, data)
    print(f"\n[SOS RECEIVED] from {device}")
    print(data)

    # Notify guardian and police
    if device in mapping:
        guardian = mapping[device]["guardian"]
        police = mapping[device]["police"]

        msg = (
            f"SHAKTICLIP ALERT\n"
            f"Device: {device}\n"
            f"Reason: {data.get('reason')}\n"
            f"Location: {data.get('lat')}, {data.get('lon')}\n"
            f"Time: {data['server_ts']}"
        )

        print("\nSending SMS to Guardian...")
        send_sms_if_configured(guardian, msg)

        print("Sending SMS to Police...")
        send_sms_if_configured(police, msg)

    return jsonify({"status": "logged", "file": path})


@app.get("/alerts")
def get_alerts():
    files = sorted(os.listdir(ALERT_DIR))
    alerts = []
    for file in files[-20:]:
        with open(os.path.join(ALERT_DIR, file)) as f:
            alerts.append(json.load(f))
    return jsonify(alerts)


if __name__ == "__main__":
    app.run(port=5000, host="0.0.0.0")
