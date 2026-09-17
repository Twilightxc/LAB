# Requirement and Structure

> **Goal:** Build a clean, responsive IoT demo dashboard (Login + Dashboard) with HTML, CSS & vanilla JavaScript, integrating ThingsBoard REST API & WebSocket for real-time telemetry.

---

## HTML

### Login Page

- Input fields for **username** and **password**
- Button: `Login to Dashboard`

### Dashboard Page

- Single `Logout` button → navigates back to Login

#### Lab 1: Raw Telemetry Streaming — "Raw Signal Diagnostics" Dashboard

> **Concept:** A diagnostics-style dashboard focusing on the speed of numerical updates and physical waveforms.

**MQTT Telemetry Payload (from device):**

```json
{
  "mpu_temp": 23.97647,
  "acc_x": 0, "acc_y": 0, "acc_z": 1,
  "gyro_x": 0, "gyro_y": 0, "gyro_z": 0,
  "angle_x": 0, "angle_y": 0, "angle_z": 0
}
```

**Serial monitor format:**
```
0:T=24,ACC=0.00,0.00,1.00,GYRO=0,0,0,ACC ANGLE=0,0,ANGLE=0,0,0
```

---

##### 1. Multi-axis Real-time Line Chart (Live Signal Waveform)

- Displays **overlaid line graphs** for 3-axis acceleration ($X$, $Y$, $Z$)
- Moves in **real-time** with a **rolling window** of the last **8 seconds** (target: ~160 data points at 20Hz)
- Visualizes signal noise/fluctuations when the sensor moves
- Each axis uses a **distinct HSL-tuned color**: X = `hsl(0, 75%, 60%)`, Y = `hsl(145, 65%, 50%)`, Z = `hsl(215, 80%, 60%)`
- Axis labels with units ($\text{m/s}^2$)
- Render using **HTML5 `<canvas>`** — no external charting libraries

##### 2. Raw Value Numeric Tiles (Live Numeric Display Boxes)

- A table/grid of tiles displaying **current numerical values**
- Separated by sensor type and axis:

| Sensor        | X     | Y     | Z     | Unit            |
| ------------- | ----- | ----- | ----- | --------------- |
| Acceleration  | acc_x | acc_y | acc_z | $\text{m/s}^2$  |
| Gyroscope     | gyro_x| gyro_y| gyro_z| $\text{rad/s}$  |
| Angle         | angle_x| angle_y| angle_z| $\text{°}$   |
| Temperature   | —     | —     | mpu_temp | $\text{°C}$  |

- Values update in **real-time** as telemetry arrives

##### 3. Sensor Tilt Indicator (2D Tilt Gauge)

- A small **square box** containing a **dot**
- The dot moves along the **X and Y axes** reflecting sensor tilt
- Maps `acc_x` and `acc_y` values to dot position using linear mapping: input range `[-1g, +1g]` → output range `[0%, 100%]` of box dimensions, clamped at boundaries
- Allows learners to **visualize the sensor's tilt** interactively
- Includes crosshair/grid lines for center reference

##### 4. LED On/Off Control Switch

- A toggle switch (On/Off) to **send RPC commands** to the device
- Toggles an **LED** on the connected hardware
- Sends command via ThingsBoard **REST API (RPC)**
- Visual feedback showing current LED state

---

## CSS

- Controlled by **CSS variables only**
- Style: dark-theme, mobile-first, responsive (breakpoints: 320px, 768px, 1280px)
- Split CSS file per HTML page (`login.css`, `dashboard.css`)
- Each page CSS file uses `@import common.css`
- `common.css` contains global CSS variables and base styles

---

## JavaScript

> Module-based — uses only native browser `fetch` API for all HTTP requests

| Method | Endpoint               | Auth                         |
| ------ | ---------------------- | ---------------------------- |
| POST   | `/login`               | `Base64(username:password)`  |
| POST   | `/oauth2/token`        | `refreshToken`               |
| GET    | `/customer/credentials`| `accessToken`                |
| GET    | `/device/credentials`  | `accessToken`                |
| WS     | WebSocket              | Real-time telemetry data     |

> **Full API contract** (base URL, request/response schemas, WS subscription format): see `L1/3api-contract.md`

---

## Flow

### Login Page

1. User submits credentials
2. **On success** → navigate to Dashboard
3. **On failure** → display error message
4. Use REST API to login → obtain `token` & `refreshToken` from Customer User credentials
5. Use REST API to get **Customer ID** from Customer User credentials
6. Use REST API to get **Device ID** and **Access Token** from Customer ID

### Dashboard Page

1. Use **WebSocket** to subscribe to telemetry data from ThingsBoard (see `L1/3api-contract.md` for subscription format)
2. Use **REST API** to post device RPC commands to ThingsBoard
3. On logout → navigate back to Login