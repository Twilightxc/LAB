# ThingsBoard API Contract

> **Version:** 1.0
> **Last Updated:** 2026-09-16
> **Status:** Template — fill in `<PLACEHOLDER>` values before first build

---

## Base Configuration

| Parameter       | Value                                                                 |
| --------------- | --------------------------------------------------------------------- |
| **Base URL**    | `<THINGSBOARD_HOST>` (e.g. `http://localhost:8080` or `https://demo.thingsboard.io`) |
| **API Prefix**  | `/api`                                                                |
| **Auth Header** | `X-Authorization: Bearer {token}`                                     |

---

## 1. Authentication — Login

### `POST /api/auth/login`

> Maps to `POST /login` in `1requirment.md`

**Request Headers:**

```
Content-Type: application/json
```

**Request Body:**

```json
{
  "username": "customer@example.com",
  "password": "customerPassword"
}
```

> **⚠ Note:** `1requirment.md` specifies `Base64(username:password)`. Standard ThingsBoard uses a JSON body, **not** HTTP Basic Auth. Verify which method your ThingsBoard instance requires and update this contract accordingly.

**Response (200 OK):**

```json
{
  "token": "eyJhbGciOiJIUzUxMiJ9...",
  "refreshToken": "eyJhbGciOiJIUzUxMiJ9..."
}
```

**Error Responses:**

| Status | Meaning           | UI Action                              |
| ------ | ----------------- | -------------------------------------- |
| 401    | Invalid credentials | Show "Invalid username or password"  |
| 400    | Malformed request | Show "Login request failed — check input" |
| 5xx    | Server error      | Show "Server unavailable — try again later" |

---

## 2. Token Refresh

### `POST /api/auth/token`

> Maps to `POST /oauth2/token` in `1requirment.md`

**Request Headers:**

```
Content-Type: application/json
```

**Request Body:**

```json
{
  "refreshToken": "eyJhbGciOiJIUzUxMiJ9..."
}
```

**Response (200 OK):**

```json
{
  "token": "eyJhbGciOiJIUzUxMiJ9...",
  "refreshToken": "eyJhbGciOiJIUzUxMiJ9..."
}
```

**Error Responses:**

| Status | Meaning                       | UI Action               |
| ------ | ----------------------------- | ----------------------- |
| 401    | Refresh token expired/invalid | Redirect to login page  |

---

## 3. Get Current User (→ Customer ID)

### `GET /api/auth/user`

> Maps to `GET /customer/credentials` in `1requirment.md`

**Request Headers:**

```
X-Authorization: Bearer {token}
```

**Response (200 OK):**

```json
{
  "id": {
    "id": "<USER_ID>",
    "entityType": "USER"
  },
  "customerId": {
    "id": "<CUSTOMER_ID>",
    "entityType": "CUSTOMER"
  },
  "email": "customer@example.com",
  "authority": "CUSTOMER_USER"
}
```

**Key field:** `response.customerId.id` → use as `{customerId}` in subsequent calls.

---

## 4. Get Customer Devices (→ Device ID)

### `GET /api/customer/{customerId}/devices?pageSize=100&page=0`

> Implicit in `1requirment.md` flow step 6 — needed to obtain Device ID

**Request Headers:**

```
X-Authorization: Bearer {token}
```

**Response (200 OK):**

```json
{
  "data": [
    {
      "id": {
        "id": "<DEVICE_ID>",
        "entityType": "DEVICE"
      },
      "name": "ESP32-MPU6050",
      "type": "default"
    }
  ],
  "totalElements": 1,
  "totalPages": 1
}
```

**Key field:** `response.data[0].id.id` → use as `{deviceId}`.

> **Decision required:** If multiple devices exist, select the **first device** by default. If the lab setup requires a specific device, filter by `name`.

---

## 5. Get Device Credentials (→ Access Token)

### `GET /api/device/{deviceId}/credentials`

> Maps to `GET /device/credentials` in `1requirment.md`

**Request Headers:**

```
X-Authorization: Bearer {token}
```

**Response (200 OK):**

```json
{
  "id": {
    "id": "<CREDENTIAL_ID>",
    "entityType": "DEVICE_CREDENTIALS"
  },
  "deviceId": {
    "id": "<DEVICE_ID>",
    "entityType": "DEVICE"
  },
  "credentialsType": "ACCESS_TOKEN",
  "credentialsId": "<DEVICE_ACCESS_TOKEN>"
}
```

**Key field:** `response.credentialsId` → use as the device access token for WebSocket subscription.

---

## 6. WebSocket — Telemetry Subscription

### Connection

```
wss://<THINGSBOARD_HOST>/api/ws/plugins/telemetry?token={jwtToken}
```

> Use the JWT `token` from login (not the device access token) for WebSocket authentication.

### Subscription Command (send after `onopen`)

```json
{
  "tsSubCmds": [
    {
      "entityType": "DEVICE",
      "entityId": "<DEVICE_ID>",
      "scope": "LATEST_TELEMETRY",
      "cmdId": 1
    }
  ],
  "historyCmds": [],
  "attrSubCmds": []
}
```

### Incoming Telemetry Message Format

```json
{
  "subscriptionId": 1,
  "data": {
    "mpu_temp": [[1694000000000, "23.97647"]],
    "acc_x":    [[1694000000000, "0.05"]],
    "acc_y":    [[1694000000000, "-0.02"]],
    "acc_z":    [[1694000000000, "0.98"]],
    "gyro_x":   [[1694000000000, "0.1"]],
    "gyro_y":   [[1694000000000, "-0.3"]],
    "gyro_z":   [[1694000000000, "0.0"]],
    "angle_x":  [[1694000000000, "1.2"]],
    "angle_y":  [[1694000000000, "-0.5"]],
    "angle_z":  [[1694000000000, "0.0"]]
  }
}
```

> **Important:** All telemetry values arrive as **strings** — parse with `parseFloat()` and validate with `isNaN()` before use.

> **Data structure:** Each key maps to an array of `[timestamp_ms, "value_string"]` pairs. For latest telemetry, typically one pair per key per update.

---

## 7. RPC — Device Command (LED Control)

### `POST /api/plugins/rpc/twoway/{deviceId}`

> Maps to REST API RPC in `1requirment.md` § LED On/Off Control Switch

**Request Headers:**

```
Content-Type: application/json
X-Authorization: Bearer {token}
```

**Request Body (LED On):**

```json
{
  "method": "setLed",
  "params": {
    "state": true
  }
}
```

**Request Body (LED Off):**

```json
{
  "method": "setLed",
  "params": {
    "state": false
  }
}
```

> **Decision required:** Verify the exact RPC `method` name and `params` schema with the device firmware. The above is a common convention but may differ per lab setup.

**Response (200 OK):**

```json
{
  "result": "ok"
}
```

**Error Responses:**

| Status | Meaning                  | UI Action                       |
| ------ | ------------------------ | ------------------------------- |
| 408    | Device offline / timeout | Show "Device not responding"    |
| 401    | Token expired            | Trigger reactive token refresh  |
| 5xx    | Server error             | Show "Command failed — retry"   |

---

## Sensor Value Ranges

> Referenced by `Role.md` out-of-range guard and `1requirment.md` tilt indicator mapping.

| Sensor Key | Valid Range           | Unit   | Notes                                               |
| ---------- | --------------------- | ------ | --------------------------------------------------- |
| `acc_x`    | −16.0 to +16.0        | g      | MPU6050 full-scale ±16g. Tilt indicator uses ±1g     |
| `acc_y`    | −16.0 to +16.0        | g      | Same as `acc_x`                                     |
| `acc_z`    | −16.0 to +16.0        | g      | At rest ≈ 1.0g (gravity)                            |
| `gyro_x`   | −2000.0 to +2000.0    | °/s    | MPU6050 full-scale ±2000°/s                         |
| `gyro_y`   | −2000.0 to +2000.0    | °/s    | Same as `gyro_x`                                    |
| `gyro_z`   | −2000.0 to +2000.0    | °/s    | Same as `gyro_x`                                    |
| `angle_x`  | −180.0 to +180.0      | °      | Computed from accelerometer                         |
| `angle_y`  | −180.0 to +180.0      | °      | Computed from accelerometer                         |
| `angle_z`  | −180.0 to +180.0      | °      | Computed from gyroscope integration                 |
| `mpu_temp` | −40.0 to +85.0        | °C     | MPU6050 internal temperature sensor                 |

---

## Auth Flow Summary

```
User submits credentials
        │
        ▼
POST /api/auth/login  ──► {token, refreshToken}
        │                   Store both in sessionStorage
        ▼
GET /api/auth/user  ──► {customerId}
        │                   Store customerId in sessionStorage
        ▼
GET /api/customer/{customerId}/devices  ──► {deviceId}
        │                                    Store deviceId in sessionStorage
        ▼
GET /api/device/{deviceId}/credentials  ──► {deviceAccessToken}
        │                                    Store deviceAccessToken in sessionStorage
        ▼
WS connect + send subscription command  ──► Real-time telemetry stream begins
```

> **sessionStorage keys:** `token`, `refreshToken`, `customerId`, `deviceId`, `deviceAccessToken`
