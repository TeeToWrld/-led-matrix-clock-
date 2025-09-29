# 8x8 LED Matrix Digital Clock with Alarm ⏰

A digital clock with alarm functionality built on an **8x8 WS2812 LED matrix** and **DS3231 RTC**, fully simulated on 
[Wokwi](https://wokwi.com/).

---

## 🚀 Features
- Displays real-time clock (HH:MM) using custom digit patterns.
- Alarm function with buzzer + LED visual feedback.
- Button controls:
  - **Power** → Toggle device on/off.
  - **Mode** → Switch between Clock and Alarm setting modes.
  - **Inc** → Adjust alarm hours/minutes.
- Works **entirely on simulation (Wokwi)** – no hardware needed.

---

## 🛠️ Hardware (Virtual in Wokwi)
- **8x8 WS2812 LED Matrix**
- **DS3231 RTC Module**
- **Buzzer**
- **3 Push Buttons (Power, Mode, Increment)**

---

## 📂 Project Structure
   ```
led-matrix-clock
├── src/
│   └── sketch.ino       # Main Arduino code
├── docs/
│   └── screenshot.png   # Simulation screenshot (optional)
└── README.md            # Documentation
```
---

## 🛠️ Tools & Technologies
- **Arduino (C++)**
- **Wokwi Simulator** – for virtual prototyping  
- **LED Matrix** with shift register driver  
- Push buttons for **Mode**, **Increment**, and **Alarm Stop**  

---

## 📖 How It Works
1. **Normal Mode**  
   - The LED matrix shows the current time.
   
2. **Alarm Setting Mode**  
   - Using buttons, the user can set `alarmHours` and `alarmMin`.

3. **Alarm Trigger**  
   - When `currentTime == alarmTime`, the alarm is triggered on the LED matrix.

4. **Stop Alarm**  
   - Alarm can be stopped with the **Stop button** (can reuse the power button or dedicate a new one).

---

## 📸 Demo
![LED Matrix Clock Screenshot](./docs/screenshot.png)  
*(Screenshot from Wokwi simulation)*

Try the simulation yourself here: [Wokwi Project Link](https://wokwi.com/projects/443331002149893121))

---

## ⚡ Getting Started
1. Clone the repository:
   ```bash
   git clone https://github.com/your-username/ed-matrix-clock.git
2.	Open src/sketch.ino in Arduino IDE or PlatformIO.
3.	Run the simulation on Wokwi or upload to real hardware.

📌 Next Steps
	•	PCB layout design for real-world deployment.
	•	Hardware testing with physical LED matrix + RTC module.
	•	Add features like snooze, brightness control, or multiple alarms.

⸻

## 📝 License

This project is licensed under the MIT License. Feel free to use and modify.

⸻

## 👩‍💻 Author
	  Gbemisola Ogunkanmbi
    Electrical & Electronics Engineering Student, University of Lagos
    Aspiring Embedded Systems & Security Engineer
    [LinkedIn] ()

