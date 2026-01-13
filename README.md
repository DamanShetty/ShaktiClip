ShaktiClip is a concept-level IoT safety system designed to provide instant SOS alerts and real-time location tracking, aimed primarily at enhancing women’s safety in remote and low-network areas. The system is built as a software-based prototype and simulation, demonstrating a complete end-to-end emergency response pipeline.

The project includes a simulated wearable device workflow, a backend alert-processing server, and a real-time guardian dashboard. When an SOS is triggered (manually or via simulated fall detection), the system captures location and sensor data, sends it to the backend, logs the alert, and displays it live on a monitoring dashboard with map integration.

This project was developed as part of a National-Level Hackathon (Innovex, NMAMIT) and was shortlisted among the Top 20 teams, based on concept strength, system architecture, and working simulation—without a physical hardware prototype.

✨ Key Features

One-tap SOS / automatic fall detection (simulated)

Real-time GPS location handling

Flask-based backend for alert processing

Live guardian dashboard built with Streamlit

Modular design for future integration with LoRa, GSM, and SMS services

Scalable architecture suitable for real-world deployment

🛠️ Tech Stack

Frontend: Streamlit (Guardian Dashboard)

Backend: Python Flask (REST API)

Simulation: Python scripts

Data Storage: JSON-based alert logs

Future Enhancements: LoRa communication, GSM/SMS alerts, mobile app frontend

🚀 Future Scope

Hardware implementation with ESP32, GPS, and LoRa

Automatic SMS alerts to nearest police station

Mobile application for guardians

Cloud-based backend and database

Geo-fencing and advanced anomaly detection
