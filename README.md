# Safety Ride Helmet 🪖⚡

An IoT-enabled smart helmet designed to transition motorcycle gear from passive protection to intelligent rider assistance. The system incorporates real-time drowsiness detection, speed monitoring, automated crash detection, and voice-guided safety alerts.

---

## 📌 Project Overview

Many motorcycle accidents occur due to preventable human errors such as over-speeding, microsleeps, or delayed emergency rescues following solo crashes. **Safety Ride Helmet** addresses these risks with an autonomous, battery-powered system that requires no modification to vehicle wiring.

### Key Objectives
* **Drowsiness Detection**: Monitor eye closure patterns to prevent fatigue-related accidents.
* **Speed Monitoring**: Measure real-time speed using GPS and issue voice warnings when limits are exceeded.
* **Crash Alerts**: Detect impacts/tilts, lock GPS coordinates, and dispatch automated emergency SMS messages.
* **Voice Coaching**: Provide audio alerts for speed compliance and headlight reminders.

---

## ⚙️ System Architecture

### Hardware Components
* **Microcontroller**: Arduino / ESP32 (Core processing unit)
* **GPS Module**: NEO-6M (Location tracking & speed calculation)
* **GSM Module**: SIM800L (Automated SMS dispatch with Google Maps link)
* **Sensors**:
  * IR Sensor (Eye blink / closure duration monitoring)
  * Accelerometer / Tilt Sensor (Impact and abnormal orientation detection)
* **Audio Unit**: DFPlayer Mini + Speaker (Voice guidance and alerts)
* **Power**: Internal rechargeable battery

---

## 🔄 Software Logic Flow

1. **Continuous Monitoring Loop**: Analyzes IR sensor input for eye closure duration. If prolonged closure is detected, immediate wake-up audio prompts are played.
2. **Speed Evaluation**: Calculates real-time velocity via NEO-6M GPS. Triggers voice warnings if current speed exceeds programmed thresholds.
3. **Emergency Routine**: If the tilt/accelerometer detects a crash, the system locks current GPS coordinates and sends an automated SMS with location details to predefined emergency contacts.

---

## 🚀 Future Enhancements

* **AI-based Drowsiness Detection**: Incorporate machine learning to adapt to individual rider blinking patterns.
* **Hardware Optimization**: Upgrade to more compact microcontrollers for lighter weight and extended battery life.
* **Mobile Application**: Bluetooth-connected companion app to customize speed limits and update emergency contacts dynamically.
* **Cloud Analytics**: Secure ride history tracking to monitor personal riding habits over time.

---

## 👥 Team Members

* K.M.P.N. Karunaratne
* R.M.P.B. Karunasekara
* H.M.T.H. Lenadora
* G.W.T. Senesh

---

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
