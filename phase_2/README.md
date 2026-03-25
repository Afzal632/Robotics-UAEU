# Phase 2 — Iterative Development & Competition Code

This folder contains all development iterations leading up to the final competition sketches for the CSPB476 Spring 2025 line-following robot.

**Result: 3rd place — track completed in 1:19**

---

## Sketch Overview

### `rule_based_line_follow/`
**First working trial — 10-case rule table**

No PID. Each of the 5 IR sensors is read and matched against a fixed lookup table of sensor patterns. Motor directions and speeds are set statically per case.

| Sensor pattern | Action |
|----------------|--------|
| Only center (s3=0) | Forward 100/100 |
| Left (s2=0) | Gentle right: 90/70 |
| Leftmost (s1=0) | Sharp right: right fwd, left reverse |
| Right (s4=0) | Gentle left: 70/90 |
| Rightmost (s5=0) | Sharp left: right reverse, left fwd |
| Center+right (s3,s4=0) | Left bias: right stop, left fwd |
| Center+left (s2,s3=0) | Right bias: right fwd, left stop |
| Left 3 (s1,s2,s3=0) | Strong right: right fwd, left stop |
| Right 3 (s3,s4,s5=0) | Strong left: right stop, left fwd |
| All (all=0) | Full stop |

---

### `rule_based_encoder_switch/`
**Rule table + encoder-triggered mode switch**

Same 10-case rule table as above, but adds a wheel encoder (pin 7, interrupt on FALLING edge). Tracks distance using:
```
distance = (pulses / 20.0) × π × 6.3 cm
```
After **583 cm** of travel, switches out of line-following mode.
> Note: the "straight drive" block after the switch sets motor enable to 0 — the robot stops rather than going straight. This was an identified bug left in for reference.

---

### `encoder_stop_straight_resume/`
**State machine: line-follow → straight → line-follow**

Two-state machine (`LINE_FOLLOWING` / `GO_STRAIGHT`) using the encoder for distance gating:

1. Line-follow at speed 80 until **50 cm** traveled
2. Drive straight at speed 80 for **50 cm**
3. Return to line-following permanently (`hasGoneStraight` flag prevents re-triggering)

Soft turn speeds reduced to 80/40 (vs 90/70 in trial 1) for smoother transitions.

---

### `pid_basic/`
**Clean PD controller — development template**

Weighted average line position with PD output. Ki is declared but set to 0, making this effectively a PD controller. Speeds constrained to 0–255.

```
sensor_line = (s1×0 + s2×1 + s3×2 + s4×3 + s5×4) / sum
error       = 2 − sensor_line
correction  = −(Kp×error + Kd×derivative)
```

| Parameter | Value |
|-----------|-------|
| Kp | 25 |
| Kd | 15 |
| base_speed | 70 |

If no sensor detects a line (`sum == 0`), motors stop immediately.

---

### `pid_sensor_history/`
**Full PID with rolling sensor history — most advanced iteration**

Maintains a depth-5 rolling history for all 5 IR sensors (updated every 10 ms). Computes an angular position of the line using a weighted sum over sensor positions `{−2, −1, 0, 1, 2}` with time-decay weighting for historical frames.

PID with integral windup clamp:
```cpp
Kp=5.0, Ki=0.05, Kd=8.0
integral = constrain(integral, −50, 50)
```

Quadratic correction scaling for gentler small-angle turns:
```cpp
adjusted_correction = correction² / 50
```

Handles edge cases:
- **Sharp left / right:** detected by 3+ consecutive outer sensors; one motor briefly reverses
- **Full line loss:** history buffer scanned for last-seen direction; robot sweeps toward that side

---

### `competition_white_line/`
**Competition submission — white line on dark surface**

PD controller tuned for white-line sections. `loop()` calls `white()` twice per iteration for higher effective sample rate.

| Parameter | Value |
|-----------|-------|
| Kp | 175 |
| Kd | 80 |
| base_speed | 100 |
| Sensor read | `digitalRead` (normal polarity) |
| Obstacle stop | ≤ 10 cm |
| No-line stop | `sum_sensor == 0` → immediate stop |

---

### `competition_black_line/`
**Competition submission — black line on white surface**

Same PD structure as white version but with inverted sensor polarity and retuned gains.

| Parameter | Value |
|-----------|-------|
| Kp | 120 |
| Kd | 35 |
| base_speed | 60 |
| Sensor read | `!digitalRead` (inverted polarity) |
| Obstacle stop | ≤ 10 cm |
| Propagation rule | If s2 active → force s1 active; if s4 active → force s5 active |

The propagation rule widens the effective detection zone on sharp turns for black-surface conditions.

---

### `assets/`

| File | Contents |
|------|---------|
| `flowchart.jpeg` | Control flow diagram of the robot decision logic |
| `parts.jpg` | Photo of the hardware components used |

---

## Dependencies

The competition sketches (`competition_white_line`, `competition_black_line`) include `<Encoder.h>`. Install via:
`Arduino IDE → Sketch → Include Library → Manage Libraries → "Encoder" by Paul Stoffregen`

> Note: the `Encoder` objects are declared in both competition files but encoder values are not used in the final competition logic. They were retained from an earlier iteration.
