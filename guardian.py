import streamlit as st
import requests
import time
from datetime import datetime
import pytz

BACKEND_URL = "http://127.0.0.1:5000/alerts"

st.set_page_config(page_title="ShaktiClip Guardian Dashboard", layout="wide")

st.title("🛡️ ShaktiClip – Guardian Dashboard")
st.write("Real-time monitoring of SOS alerts from ShaktiClip devices.")

placeholder = st.empty()

# --- TIMEZONE CONFIG ---
IST = pytz.timezone("Asia/Kolkata")

def convert_to_ist(utc_timestamp):
    """
    Convert ISO UTC timestamp → IST formatted time.
    Example output: 21 Nov 2025 – 11:26 PM IST
    """
    try:
        dt = datetime.fromisoformat(utc_timestamp)
        dt_ist = dt.astimezone(IST)
        return dt_ist.strftime("%d %b %Y – %I:%M %p IST")
    except:
        return "N/A"


def fetch_alerts():
    try:
        response = requests.get(BACKEND_URL, timeout=3)
        if response.status_code == 200:
            return response.json()
    except:
        return []
    return []


while True:
    alerts = fetch_alerts()

    with placeholder.container():
        st.subheader("🔔 Latest Alerts")

        if len(alerts) == 0:
            st.info("No alerts yet. Waiting for device...")
        else:
            for alert in reversed(alerts[-10:]):  # Show last 10
                with st.container():
                    st.markdown("---")
                    cols = st.columns(2)

                    utc_time = alert.get("server_ts", "N/A")
                    ist_time = convert_to_ist(utc_time) if utc_time != "N/A" else "N/A"

                    with cols[0]:
                        st.write(f"### 📟 Device: **{alert.get('deviceID', 'Unknown')}**")
                        st.write(f"**Reason:** {alert.get('reason', 'N/A')}")
                        st.write(f"**Battery:** {alert.get('battery', 'N/A')}%")
                        st.write(f"**Heart Rate:** {alert.get('hr', 'N/A')} bpm")
                        st.write(f"**Accel:** {alert.get('accel', 'N/A')}")
                        st.write(f"**Time:** {ist_time}")

                    with cols[1]:
                        lat = alert.get("lat")
                        lon = alert.get("lon")
                        if lat and lon:
                            map_link = f"https://maps.google.com/?q={lat},{lon}"
                            st.markdown(f"🌍 **Location:** [{lat}, {lon}]({map_link})")
                        else:
                            st.write("Location: No GPS Data")

    time.sleep(2)
