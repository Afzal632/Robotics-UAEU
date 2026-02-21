# Robotics Phase 1 — UAEU

Arduino-based differential-drive robot sketches covering motion control, encoder odometry, line following, and obstacle detection.

---

## Hardware

| Component | Details |
|-----------|---------|
| Microcontroller | Arduino Uno / Nano |
| Drive system | 2× DC motors with L298N H-bridge |
| Line sensor | 5-channel IR sensor array |
| Distance sensor | HC-SR04 ultrasonic (trig: pin 3, echo: pin 2) |
| Encoder | Optical wheel encoder (interrupt: pin 7) |

### Pin Map (shared across sketches)

| Signal | Pin |
|--------|-----|
| Right motor IN1 | 12 |
| Right motor IN2 | 11 |
| Right motor EN | 10 |
| Left motor IN1 | 9 |
| Left motor IN2 | 8 |
| Left motor EN | 13 |
| IR sensor 1 (leftmost) | A5 |
| IR sensor 2 | A4 |
| IR sensor 3 (center) | A3 |
| IR sensor 4 | A2 |
| IR sensor 5 (rightmost) | A1 |
| Ultrasonic TRIG | 3 |
| Ultrasonic ECHO | 2 |
| Encoder input | 7 |

---

## Sketches

### 1. `line_follow_pid/`
**PID line follower with sensor history**

The most advanced line-following implementation. Uses all 5 IR sensors with a rolling history buffer (depth 5) to smooth noisy readings and estimate the line's angular offset. A PID controller (Kp=5.0, Ki=0.05, Kd=8.0) computes differential motor corrections in real time. Handles sharp turns and full line-loss events using history-based recovery logic.

Key features:
- PID control loop running at 100 Hz (10 ms interval)
- Quadratic correction scaling for gentle curves
- Sensor history used for line-loss direction recovery
- Serial debug output: direction, angle, and correction values

---

### 2. `line_follow_obstacle/`
**Line follower with ultrasonic obstacle stop**

Combines basic line following with collision avoidance. The robot follows a line using 5 IR sensors with simple if/else sensor pattern matching. An ultrasonic sensor is polled every loop; if an object is detected within **10 cm** the motors are immediately cut.

Key features:
- 10-case sensor pattern table covering all common line positions
- Hard stop triggered by obstacle ≤ 10 cm
- Serial output when stopped

---

### 3. `encoder_move_50cm/`
**Encoder-based 50 cm straight drive**

Uses a hardware interrupt on the encoder wheel to count encoder pulses. Distance is calculated as:

```
distance = (pulses / HOLES_PER_REV) × π × WHEEL_DIAMETER
```

Where `HOLES_PER_REV = 20` and `WHEEL_DIAMETER = 6.3 cm`. The robot drives forward until 50 cm is reached, then stops and waits 5 seconds before resetting the counter.

Key features:
- Interrupt-driven pulse counting (pin 7, FALLING edge)
- Accurate distance tracking independent of motor speed variation
- Automatic reset after stop

---

### 4. `timed_rotate_360/`
**Timed 360° in-place rotation**

Spins the left and right motors in opposite directions for a fixed duration (1640 ms) to complete one full rotation, then stops and waits 5 seconds. Rotation duration is calibrated empirically for a specific robot and surface — adjust the delay if the heading drift is noticeable.

Key features:
- Simple open-loop rotation
- Both motors run at PWM 100
- Easily tuneable via the delay value

---

### 5. `ultrasonic_stop_10cm/`
**Ultrasonic obstacle stop**

Drives the robot straight forward while continuously polling the PING/HC-SR04 ultrasonic sensor. When the measured distance drops to **10 cm or less** the motors stop. Includes a circuit layout image and schematic for reference.

Additional files:
- `layout.png` — breadboard layout
- `schematic.png` — circuit schematic
- `Ping.txt` — sensor description note

Key features:
- Real-time distance measurement using `pulseIn`
- Outputs distance in both cm and inches over Serial when stopped
- Motors resume movement after the stop condition clears

---

## Getting Started

1. **Clone the repo**
   ```bash
   git clone https://github.com/Afzal632/Robotics-UAEU.git
   ```

2. **Open a sketch**
   Open any `.ino` file inside its matching folder with the [Arduino IDE](https://www.arduino.cc/en/software).

3. **Select board & port**
   `Tools → Board → Arduino Uno` and select the correct COM port.

4. **Upload**
   Click the Upload button or press `Ctrl+U`.

5. **Monitor**
   Open the Serial Monitor at **9600 baud** to view real-time debug output (relevant for `line_follow_pid` and `line_follow_obstacle`).

---

## Tuning Tips

| Sketch | What to tune |
|--------|-------------|
| `line_follow_pid` | `Kp`, `Ki`, `Kd` constants; `base_speed`; `MAX_SPEED` / `MIN_SPEED` |
| `encoder_move_50cm` | `HOLES_PER_REV`, `WHEEL_DIAMETER` to match your hardware |
| `timed_rotate_360` | The `1640` ms delay to match your surface friction and battery level |
| `ultrasonic_stop_10cm` | The `<= 10` threshold (cm) in the `if` condition |

---

## Repository Structure

```
phase 1/
├── line_follow_pid/
│   └── line_follow_pid.ino
├── line_follow_obstacle/
│   └── line_follow_obstacle.ino
├── encoder_move_50cm/
│   └── encoder_move_50cm.ino
├── timed_rotate_360/
│   └── timed_rotate_360.ino
├── ultrasonic_stop_10cm/
│   ├── ultrasonic_stop_10cm.ino
│   ├── layout.png
│   ├── schematic.png
│   └── Ping.txt
└── README.md
```

---

## License

Academic project — UAEU Spring 2025 Robotics Course.
