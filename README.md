# CSPB476 Line Following Robot — UAEU Spring 2025

A full-semester robotics project built for the CSPB476 Spring 2025 competition at UAEU. The robot follows a line track, handles obstacles, and adapts between white-line and black-line surface sections. The team placed **3rd overall**, completing the track in **1:19**.

**Video demonstration:** https://www.youtube.com/watch?v=0ud6cWR-w9w

---

## Team

| Name | Student ID |
|------|-----------|
| Afzal Nujum Navaz | 700037478 |
| Jesty Jacob | 700038919 |
| Dhiaeddine Guettouche | 700037569 |
| Salem Almarar | 202230389 |
| Husam AlSabbah | 700040107 |

---

## Hardware

| Component | Details |
|-----------|---------|
| Microcontroller | Arduino Leonardo |
| Drive system | 2× DC motors — L298N H-bridge |
| Line sensor | 5-channel IR sensor array |
| Distance sensor | HC-SR04 ultrasonic (trig: 3, echo: 2) |
| Encoder | Optical wheel encoder (interrupt: pin 7) |
| Buzzer | Milestone notification |

### Shared Pin Map

| Signal | Pin |
|--------|-----|
| Right motor IN1 / IN2 | 12 / 11 |
| Right motor EN | 10 |
| Left motor IN1 / IN2 | 9 / 8 |
| Left motor EN | 13 |
| IR sensor 1–5 (left→right) | A5, A4, A3, A2, A1 |
| Ultrasonic TRIG / ECHO | 3 / 2 |
| Encoder input | 7 |

---

## Project Structure

```
├── phase_1/                        ← Individual subsystem experiments
│   ├── encoder_move_50cm/          ← Drive exactly 50 cm via wheel encoder
│   ├── line_follow_obstacle/       ← Basic 5-sensor line follow + ultrasonic stop
│   ├── line_follow_pid/            ← PID line follower with sensor history (advanced)
│   ├── timed_rotate_360/           ← 360° in-place rotation via timed drive
│   └── ultrasonic_stop_10cm/       ← Stop at 10 cm obstacle (with schematic)
│
└── phase_2/                        ← Iterative development → competition code
    ├── rule_based_line_follow/     ← Trial 1: 10-case rule table
    ├── rule_based_encoder_switch/  ← Trial 2: rule-based + encoder distance switch
    ├── encoder_stop_straight_resume/ ← State machine: line follow → straight → resume
    ├── pid_basic/                  ← Clean PD controller (Kp=25, Kd=15)
    ├── pid_sensor_history/         ← Full PID with rolling history + recovery
    ├── competition_white_line/     ← Competition: white line on dark (Kp=175, Kd=80)
    ├── competition_black_line/     ← Competition: black line on white (Kp=120, Kd=35)
    └── assets/                     ← Flowchart and parts photo
```

---

## Phase 1 — Subsystem Experiments

Individual sketches built to validate each hardware component before integration.

| Folder | What it does |
|--------|-------------|
| `encoder_move_50cm` | Uses a hardware interrupt to count encoder pulses; moves forward until `(pulses/20) × π × 6.3 cm` reaches 50 cm, then stops |
| `line_follow_obstacle` | Follows a line with 10 sensor-pattern cases; cuts motors when ultrasonic sensor reads ≤ 10 cm |
| `line_follow_pid` | Full PID (Kp=5, Ki=0.05, Kd=8) over a 5-sensor history buffer; quadratic correction scaling; line-loss recovery using history direction |
| `timed_rotate_360` | Spins both motors in opposite directions for 1640 ms — calibrated empirically for one full turn |
| `ultrasonic_stop_10cm` | Drives forward and stops when PING sensor detects obstacle ≤ 10 cm; includes `layout.png` and `schematic.png` |

---

## Phase 2 — Development Iterations → Competition

Iterative development moving from rule-based logic toward the PD controller used in the competition.

### Control Algorithm (competition)

**Weighted average line position:**
```
sensor_line = (s1×0 + s2×1 + s3×2 + s4×3 + s5×4) / (s1+s2+s3+s4+s5)
```

**PD control output:**
```
error  = setpoint(2) − sensor_line
output = Kp × error + Kd × (error − last_error) / Δt
right_speed = base_speed + output
left_speed  = base_speed − output
```

If all 5 sensors read the same value (all-white or all-black), the last computed output is held to avoid division-by-zero and maintain trajectory.

### Sketch Progression

| Sketch | Controller | Key parameters | Notes |
|--------|-----------|----------------|-------|
| `rule_based_line_follow` | Rule table | Speed 100 | 10 hand-coded sensor patterns, no PID |
| `rule_based_encoder_switch` | Rule table + encoder | Speed 100, target 583 cm | Switches to straight drive after 583 cm — encoder-triggered |
| `encoder_stop_straight_resume` | Rule table + state machine | Speed 80, 50 cm segments | Line-follow → straight 50 cm → line-follow (one cycle only) |
| `pid_basic` | PD (Ki=0) | Kp=25, Kd=15, base 70 | Clean PD template; speeds 0–255 range |
| `pid_sensor_history` | Full PID + history | Kp=5, Ki=0.05, Kd=8 | History buffer depth 5; angle-based correction; recovery logic |
| `competition_white_line` | PD | Kp=175, Kd=80, base 100 | White line on dark surface; normal `digitalRead` |
| `competition_black_line` | PD | Kp=120, Kd=35, base 60 | Black line on white surface; inverted `!digitalRead`; propagation rule: if s2 active → force s1 active |

### Competition Features

- **Adaptive sensing:** white-line and black-line versions with tuned separate gains
- **Obstacle detection:** ultrasonic stops motors when object ≤ 10 cm; resumes when cleared
- **All-sensor holdover:** last PD output held when all sensors agree (prevents erratic stop)
- **Serial debug:** error, output, left speed, right speed streamed at 9600 baud
- **Assets:** see `phase_2/assets/` for hardware photo and control flowchart

---

## Getting Started

1. **Clone**
   ```bash
   git clone https://github.com/Afzal632/Robotics-UAEU.git
   ```

2. **Open a sketch** — open any `.ino` file in the [Arduino IDE](https://www.arduino.cc/en/software). The folder name must match the `.ino` file name.

3. **Install library** — the competition sketches require the `Encoder` library:
   `Sketch → Include Library → Manage Libraries → search "Encoder" by Paul Stoffregen`

4. **Select board & port**
   `Tools → Board → Arduino Leonardo` then select your COM port.

5. **Upload** — `Ctrl+U`

6. **Monitor** — open Serial Monitor at **9600 baud** for real-time PD debug output.

---

## Tuning Reference

| Parameter | White line | Black line | Notes |
|-----------|-----------|-----------|-------|
| `Kp` | 175 | 120 | Higher = more aggressive turn response |
| `Kd` | 80 | 35 | Higher = more damping, less oscillation |
| `base_speed` | 100 | 60 | Reduce if oscillating at high speed |
| Obstacle threshold | 10 cm | 10 cm | Adjust `<= 10` in ultrasonic block |
| Setpoint | 2 | 2 | Corresponds to center sensor (index 2 of 0–4) |

---

## License

Academic project — UAEU CSPB476 Robotics, Spring 2025.
