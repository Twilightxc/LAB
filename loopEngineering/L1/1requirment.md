# Requirement and Structure

> **Goal:** Build a minimal, modern, responsive demo dashboard with HTML, CSS & backend API (using ThingsBoard REST API & WebSocket).

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
- Moves in **real-time** with a **rolling window** of the last **5–10 seconds**
- Visualizes signal noise/fluctuations when the sensor moves
- Each axis uses a **distinct color** (e.g., Red = X, Green = Y, Blue = Z)
- Axis labels with units ($\text{m/s}^2$)

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
- Maps `acc_x` and `acc_y` values to dot position
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
- Style: minimal, modern, mobile-friendly, and responsive
- Split CSS file per HTML page (`login.css`, `dashboard.css`)
- Each page CSS file uses `@import common.css`
- `common.css` contains global CSS variables and base styles

---

## JavaScript

> Module-based — uses `fetch` API (not Axios)

| Method | Endpoint               | Auth                         |
| ------ | ---------------------- | ---------------------------- |
| POST   | `/login`               | `Base64(username:password)`  |
| POST   | `/oauth2/token`        | `refreshToken`               |
| GET    | `/customer/credentials`| `accessToken`                |
| GET    | `/device/credentials`  | `accessToken`                |
| WS     | WebSocket              | Real-time telemetry data     |

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

1. Use **WebSocket** to subscribe to telemetry data from ThingsBoard
2. Use **REST API** to post device RPC commands to ThingsBoard
3. On logout → navigate back to Login