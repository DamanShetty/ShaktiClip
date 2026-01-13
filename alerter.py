import os

TWILIO_SID = os.getenv("TWILIO_ACCOUNT_SID")
TWILIO_TOKEN = os.getenv("TWILIO_AUTH_TOKEN")
TWILIO_FROM = os.getenv("TWILIO_FROM")  # Example: "+1234567890"

def send_sms_if_configured(to, message):
    """
    Sends SMS using Twilio if environment variables are set.
    If not, prints SMS to console for simulation.
    """
    if not TWILIO_SID or not TWILIO_TOKEN or not TWILIO_FROM:
        print(f"[SIMULATED SMS] To {to}:\n{message}\n")
        return

    try:
        from twilio.rest import Client
        client = Client(TWILIO_SID, TWILIO_TOKEN)
        msg = client.messages.create(body=message, from_=TWILIO_FROM, to=to)
        print("Twilio SMS sent:", msg.sid)
    except Exception as e:
        print("SMS Error:", e)
