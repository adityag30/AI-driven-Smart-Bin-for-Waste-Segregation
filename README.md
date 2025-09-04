# ♻️ AI-driven Smart Bin for Waste Segregation

An **IoT + AI-powered Smart Bin** that automatically classifies waste into **Organic** and **Recyclable** categories using an ESP32, Arduino Uno, Servo motor, and a Flask-based Machine Learning server.

---

## 🚀 Features
- Connects ESP32 to WiFi for real-time image processing.
- Captures waste images from an IP camera or ESP32-CAM (extendable).
- Sends images to a **Flask AI server** (via ngrok/public endpoint).
- Predicts waste category (**organic/recyclable**) using trained ML model.
- Controls **servo motor flaps** to drop waste into the correct bin.
- Can be scaled to **smart city waste management systems**.

---

## 🛠️ Tech Stack
- **Hardware:** ESP32, Arduino Uno, Ultrasonic Sensor, Servo Motor  
- **Software:** Arduino IDE, Flask, Python, Ngrok  
- **Libraries:** `WiFi.h`, `HTTPClient.h`, `Servo.h`  
- **Cloud Support:** Can be deployed on AWS/GCP/Azure  

---


## ⚙️ Setup Instructions 

###-- Hardware Setup
- Connect ESP32 with Servo motor (GPIO5 as signal pin).  
- Attach Ultrasonic sensor for bin fill-level detection (optional).  
- Power via USB or external supply.

## 📊 Workflow

- Waste is dropped in front of the bin.
- Camera captures an image.
- ESP32 sends the image to the Flask AI server.
- ML model predicts organic/recyclable.
- ESP32 controls the servo to open the correct flap.

## 🎯 Future Improvements

- Add more categories (metal, plastic, paper, etc.).
- Integrate a mobile dashboard for monitoring.
- Use ESP32-CAM directly instead of an external IP camera.
- Store usage data on a cloud database for analytics.

## 👨‍💻 Authors

Aditya Garg and Team
(B.Tech CSE, AI & Data Science, 5th Semester, Graphic Era University)

