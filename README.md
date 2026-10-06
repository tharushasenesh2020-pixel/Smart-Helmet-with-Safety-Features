# Safety Ride Helmet 🪖⚡

An IoT-enabled smart helmet designed to transition motorcycle gear from passive protection to intelligent rider assistance[cite: 2, 8]. The system incorporates real-time drowsiness detection, speed monitoring, automated crash detection, and voice-guided safety alerts[cite: 2].

---

## 📌 Project Overview

Many motorcycle accidents occur due to preventable human errors such as over-speeding, microsleeps, or delayed emergency rescues following solo crashes[cite: 3]. **Safety Ride Helmet** addresses these risks with an autonomous, battery-powered system that requires no modification to vehicle wiring[cite: 2, 8].

### Key Objectives
* **Drowsiness Detection**: Monitor eye closure patterns to prevent fatigue-related accidents[cite: 4].
* **Speed Monitoring**: Measure real-time speed using GPS and issue voice warnings when limits are exceeded[cite: 4].
* **Crash Alerts**: Detect impacts/tilts, lock GPS coordinates, and dispatch automated emergency SMS messages[cite: 4].
* **Voice Coaching**: Provide audio alerts for speed compliance and headlight reminders[cite: 7].

---

## ⚙️ System Architecture

### Hardware Components
* **Microcontroller**: Arduino / ESP32 (Core processing unit)[cite: 5]
* **GPS Module**: NEO-6M (Location tracking & speed calculation)[cite: 5]
* **GSM Module**: SIM800L (Automated SMS dispatch with Google Maps link)[cite: 5, 7]
* **Sensors**:
  * IR Sensor (Eye blink / closure duration monitoring)[cite: 5]
  * Accelerometer / Tilt Sensor (Impact and abnormal orientation detection)[cite: 5]
* **Audio Unit**: DFPlayer Mini + Speaker (Voice guidance and alerts)[cite: 5]
* **Power**: Internal rechargeable battery[cite: 8]

---

## 🔄 Software Logic Flow

1. **Continuous Monitoring Loop**: Analyzes IR sensor input for eye closure duration[cite: 6]. If prolonged closure is detected, immediate wake-up audio prompts are played[cite: 6, 7].
2. **Speed Evaluation**: Calculates real-time velocity via NEO-6M GPS[cite: 5, 6]. Triggers voice warnings if current speed exceeds programmed thresholds[cite: 4, 6].
3. **Emergency Routine**: If the tilt/accelerometer detects a crash, the system locks current GPS coordinates and sends an automated SMS with location details to predefined emergency contacts[cite: 4, 6, 7].

---

## 🚀 Future Enhancements

* **AI-based Drowsiness Detection**: Incorporate machine learning to adapt to individual rider blinking patterns[cite: 9].
* **Hardware Optimization**: Upgrade to more compact microcontrollers for lighter weight and extended battery life[cite: 9].
* **Mobile Application**: Bluetooth-connected companion app to customize speed limits and update emergency contacts dynamically[cite: 7, 9].
* **Cloud Analytics**: Secure ride history tracking to monitor personal riding habits over time[cite: 9].

---

## 👥 Team Members

* K.M.P.N. Karunaratne[cite: 2]
* R.M.P.B. Karunasekara[cite: 2]
* H.M.T.H. Lenadora[cite: 2]
* G.W.T. Senesh[cite: 2]

---

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
