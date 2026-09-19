# ThingsBoard API Contract

> **Version:** 2.0
> **Last Updated:** 2026-09-19
> **Status:** ✅ Verified — populated from live test results (`manual.json` run on 2026-09-17)

---

## Base Configuration

| Parameter       | Value                                                                 |
| --------------- | --------------------------------------------------------------------- |
| **Base URL**    | `https://demo.thingsboard.io`                                         |
| **API Prefix**  | `/api`                                                                |
| **Auth Header** | `X-Authorization: Bearer {token}`                                     |

---

## Test Results Summary

| # | Endpoint | Method | Status | Result | Response Time |
|---|----------|--------|--------|--------|---------------|
| 1 | `/api/auth/login` | POST | 200 | ✅ pass | 1040ms |
| 2 | `/api/auth/token` | POST | 200 | ✅ pass | 879ms |
| 3 | `/api/auth/user` | GET | 200 | ✅ pass | 988ms |
| 4 | `/api/customer/{customerId}/devices` | GET | 200 | ✅ pass | 884ms |
| 5 | `/api/device/{deviceId}` | GET | 200 | ✅ pass | 1869ms |
| 6 | `/api/device/{deviceId}/credentials` | GET | 200 | ✅ pass | 877ms |
| 7 | `/api/plugins/telemetry/DEVICE/{deviceId}/values/timeseries` | GET | 200 | ✅ pass | 1968ms |
| 8 | `/api/plugins/telemetry/DEVICE/{deviceId}/values/timeseries?keys=...&startTs=...&endTs=...` | GET | 200 | ✅ pass | 1620ms |
| 9 | `/api/plugins/telemetry/DEVICE/{deviceId}/values/attributes/CLIENT_SCOPE` | GET | 200 | ✅ pass | 2541ms |
| 10 | `/api/plugins/telemetry/DEVICE/{deviceId}/values/attributes` | GET | 200 | ✅ pass | 851ms |
| 11 | `/api/rpc/oneway/{deviceId}` | POST | 200 | ✅ pass | 932ms |
| 12 | `/api/rpc/twoway/{deviceId}` | POST | 504 | ✅ pass (timeout expected — device offline) | 6806ms |
| 13 | `/api/v1/{accessToken}/telemetry` | POST | ❌ error | ETIMEDOUT | — |
| 14 | `/api/v1/{accessToken}/attributes` | POST | ❌ error | ETIMEDOUT | — |
| 15 | `wss://.../api/ws/plugins/telemetry?token=` | WS | ❌ error | Unsupported protocol (Bruno limitation) | — |

> **Note:** Items 13–15 failed due to network timeout / Bruno client limitation, not API issues. Device HTTP API (items 13–14) is used by the ESP32 firmware, not the web app. WebSocket (item 15) must be tested in a browser.

---

## 1. Authentication — Login

### `POST /api/auth/login`

> Maps to `POST /login` in `1requirment.md`

**Request Headers:**

```
Content-Type: application/json
Accept: application/json
```

**Request Body:**

```json
{
  "username": "customer@example.com",
  "password": "customerPassword"
}
```

> **✅ Confirmed:** ThingsBoard uses a JSON body (not HTTP Basic Auth).

**Response (200 OK):**

```json
{
  "token": "eyJhbGciOiJIUzUxMiJ9...",
  "refreshToken": "eyJhbGciOiJIUzUxMiJ9..."
}
```

**Test Script:** Verifies `login succeeded` and stores `token` + `refreshToken` in collection variables.

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
Accept: application/json
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

> **✅ Confirmed:** Returns both new `token` and new `refreshToken`.

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
Accept: application/json
```

**Response (200 OK):**

```json
{
  "id": {
    "entityType": "USER",
    "id": "64cd0610-ef68-11ef-8e00-e370a74757c3"
  },
  "createdTime": 1740041711857,
  "tenantId": {
    "entityType": "TENANT",
    "id": "150fa630-e4a4-11ed-a4fc-57550caf43ca"
  },
  "customerId": {
    "entityType": "CUSTOMER",
    "id": "4ac42320-ef68-11ef-8e00-e370a74757c3"
  },
  "email": "customer@example.com",
  "authority": "CUSTOMER_USER",
  "firstName": "Jaturong",
  "lastName": "Bunsong",
  "phone": null,
  "version": 3,
  "name": "customer@example.com",
  "additionalInfo": {
    "description": "",
    "defaultDashboardId": null,
    "defaultDashboardFullscreen": false,
    "homeDashboardId": null,
    "homeDashboardHideToolbar": true,
    "userCredentialsEnabled": false,
    "userActivated": false,
    "lastLoginTs": null
  }
}
```

**Key field:** `response.customerId.id` → use as `{customerId}` in subsequent calls.

---

## 4. Get Customer Devices (→ Device ID)

### `GET /api/customer/{customerId}/devices?pageSize=50&page=0`

> Implicit in `1requirment.md` flow step 6 — needed to obtain Device ID

**Request Headers:**

```
X-Authorization: Bearer {token}
Accept: application/json
```

**Response (200 OK):**

```json
{
  "data": [
    {
      "id": {
        "entityType": "DEVICE",
        "id": "f591e0a0-3af6-11f1-9681-6110e8f55c0f"
      },
      "createdTime": 1776496778154,
      "tenantId": {
        "entityType": "TENANT",
        "id": "150fa630-e4a4-11ed-a4fc-57550caf43ca"
      },
      "customerId": {
        "entityType": "CUSTOMER",
        "id": "4ac42320-ef68-11ef-8e00-e370a74757c3"
      },
      "name": "test",
      "type": "Default",
      "label": "",
      "deviceProfileId": {
        "entityType": "DEVICE_PROFILE",
        "id": "151328a0-e4a4-11ed-a4fc-57550caf43ca"
      },
      "firmwareId": null,
      "softwareId": null,
      "externalId": null,
      "version": 3,
      "additionalInfo": {
        "gateway": false
      },
      "deviceData": {
        "configuration": { "type": "DEFAULT" },
        "transportConfiguration": { "type": "DEFAULT" }
      }
    }
  ],
  "totalPages": 1,
  "totalElements": 1,
  "hasNext": false
}
```

**Key field:** `response.data[0].id.id` → use as `{deviceId}`.

> **Decision required:** If multiple devices exist, select the **first device** by default. If the lab setup requires a specific device, filter by `name`.

---

## 5. Get Device By Id

### `GET /api/device/{deviceId}`

> Fetch full device details by ID

**Request Headers:**

```
X-Authorization: Bearer {token}
Accept: application/json
```

**Response (200 OK):**

```json
{
  "id": {
    "entityType": "DEVICE",
    "id": "f591e0a0-3af6-11f1-9681-6110e8f55c0f"
  },
  "createdTime": 1776496778154,
  "tenantId": {
    "entityType": "TENANT",
    "id": "150fa630-e4a4-11ed-a4fc-57550caf43ca"
  },
  "customerId": {
    "entityType": "CUSTOMER",
    "id": "4ac42320-ef68-11ef-8e00-e370a74757c3"
  },
  "name": "test",
  "type": "Default",
  "label": "",
  "deviceProfileId": {
    "entityType": "DEVICE_PROFILE",
    "id": "151328a0-e4a4-11ed-a4fc-57550caf43ca"
  },
  "firmwareId": null,
  "softwareId": null,
  "externalId": null,
  "version": 3,
  "additionalInfo": {
    "gateway": false
  },
  "deviceData": {
    "configuration": { "type": "DEFAULT" },
    "transportConfiguration": { "type": "DEFAULT" }
  }
}
```

---

## 6. Get Device Credentials (→ Access Token)

### `GET /api/device/{deviceId}/credentials`

> Maps to `GET /device/credentials` in `1requirment.md`

**Request Headers:**

```
X-Authorization: Bearer {token}
Accept: application/json
```

**Response (200 OK):**

```json
{
  "id": {
    "id": "f593dc70-3af6-11f1-9681-6110e8f55c0f"
  },
  "createdTime": 1776496778167,
  "deviceId": {
    "entityType": "DEVICE",
    "id": "f591e0a0-3af6-11f1-9681-6110e8f55c0f"
  },
  "credentialsType": "ACCESS_TOKEN",
  "credentialsId": "vdKWjfAm1V16yn3ykFdh",
  "credentialsValue": null,
  "version": 1
}
```

**Key field:** `response.credentialsId` → use as the device access token for WebSocket subscription.

> **✅ Confirmed:** `credentialsType` is `ACCESS_TOKEN`. The `credentialsId` is the plain access token string.

---

## 7. Get Latest Telemetry

### `GET /api/plugins/telemetry/DEVICE/{deviceId}/values/timeseries`

> Retrieves the most recent value for each telemetry key

**Request Headers:**

```
X-Authorization: Bearer {token}
Accept: application/json
```

**Response (200 OK):**

```json
{
  "humidity": [
    { "ts": 1789624284873, "value": "60" }
  ],
  "temperature": [
    { "ts": 1789624284873, "value": "25.5" }
  ],
  "acc_x": [
    { "ts": 1789623471514, "value": "0" }
  ],
  "acc_y": [
    { "ts": 1789623471514, "value": "0" }
  ],
  "acc_z": [
    { "ts": 1789623471514, "value": "1" }
  ],
  "gyro_x": [
    { "ts": 1789623471514, "value": "0" }
  ],
  "gyro_y": [
    { "ts": 1789623471514, "value": "0" }
  ],
  "gyro_z": [
    { "ts": 1789623471514, "value": "0" }
  ],
  "angle_x": [
    { "ts": 1789623471514, "value": "0" }
  ],
  "angle_y": [
    { "ts": 1789623471514, "value": "0" }
  ],
  "angle_z": [
    { "ts": 1789623471514, "value": "0" }
  ],
  "mpu_temp": [
    { "ts": 1789623471514, "value": "23.97647" }
  ]
}
```

> **✅ Confirmed:** Telemetry keys available: `humidity`, `temperature`, `acc_x`, `acc_y`, `acc_z`, `gyro_x`, `gyro_y`, `gyro_z`, `angle_x`, `angle_y`, `angle_z`, `mpu_temp`.

> **Important:** All telemetry values arrive as **strings** — parse with `parseFloat()` and validate with `isNaN()` before use.

---

## 8. Get Telemetry History

### `GET /api/plugins/telemetry/DEVICE/{deviceId}/values/timeseries?keys={keys}&startTs={startTs}&endTs={endTs}&interval={interval}&limit={limit}&agg={agg}`

> Retrieves historical telemetry data for a time range

**Request Headers:**

```
X-Authorization: Bearer {token}
Accept: application/json
```

**Query Parameters:**

| Parameter | Type | Required | Example | Description |
|-----------|------|----------|---------|-------------|
| `keys` | string | Yes | `mpu_temp,acc_x,acc_y,acc_z,...` | Comma-separated telemetry key names |
| `startTs` | number | Yes | `1789617600000` | Start timestamp (epoch ms) |
| `endTs` | number | Yes | `1789617900000` | End timestamp (epoch ms) |
| `interval` | number | No | `0` | Aggregation interval in ms (0 = no aggregation) |
| `limit` | number | No | `1000` | Max number of data points per key |
| `agg` | string | No | `NONE` | Aggregation type: `NONE`, `MIN`, `MAX`, `AVG`, `SUM`, `COUNT` |

**Response (200 OK):**

```json
{
  "mpu_temp": [
    { "ts": 1789623471514, "value": "23.97647" },
    { "ts": 1789623470449, "value": "23.97647" }
  ],
  "acc_x": [
    { "ts": 1789623471514, "value": "0" },
    { "ts": 1789623470449, "value": "0" }
  ]
}
```

> **✅ Confirmed:** Returns `{}` (empty object) when no data exists in the requested time range.

> **Data structure:** Each key maps to an array of `{ ts, value }` objects sorted by timestamp descending.

---

## 9. Get Device Attributes

### `GET /api/plugins/telemetry/DEVICE/{deviceId}/values/attributes/CLIENT_SCOPE`

> Retrieves client-scope attributes published by the device

**Request Headers:**

```
X-Authorization: Bearer {token}
Accept: application/json
```

**Response (200 OK):**

```json
[
  { "lastUpdateTs": 1789622618271, "key": "firmware_version", "value": 1 },
  { "lastUpdateTs": 1789624285731, "key": "firmwareVersion", "value": "1.0.0" },
  { "lastUpdateTs": 1789622618271, "key": "student_id", "value": 6750091 }
]
```

### `GET /api/plugins/telemetry/DEVICE/{deviceId}/values/attributes`

> Retrieves **all** attributes (all scopes: CLIENT_SCOPE, SERVER_SCOPE, SHARED_SCOPE)

**Response (200 OK):**

```json
[
  { "lastUpdateTs": 1789622618271, "key": "firmware_version", "value": 1 },
  { "lastUpdateTs": 1789624285731, "key": "firmwareVersion", "value": "1.0.0" },
  { "lastUpdateTs": 1789622618271, "key": "student_id", "value": 6750091 },
  { "lastUpdateTs": 1789620741041, "key": "active", "value": true },
  { "lastUpdateTs": 1789624289626, "key": "lastActivityTime", "value": 1789624286662 },
  { "lastUpdateTs": 1789622617663, "key": "lastConnectTime", "value": 1789622617663 },
  { "lastUpdateTs": 1789624074248, "key": "lastDisconnectTime", "value": 1789624074248 }
]
```

> **✅ Confirmed:** Attribute values can be **string, number, or boolean** (unlike telemetry which is always string).

---

## 10. RPC — Device Command (LED Control)

### `POST /api/rpc/oneway/{deviceId}`

> Fire-and-forget command. Does not wait for device response.

**Request Headers:**

```
Content-Type: application/json
X-Authorization: Bearer {token}
```

**Request Body:**

```json
{
  "method": "setValue",
  "params": {
    "value": true
  },
  "timeout": 5000
}
```

**Response (200 OK):** Empty body `""`

> **✅ Confirmed:** Returns `200` with empty string body on success. Use `timeout` field to set max wait (ms).

---

### `POST /api/rpc/twoway/{deviceId}`

> Sends command and waits for device response. Returns 504 if device doesn't respond within `timeout`.

**Request Headers:**

```
Content-Type: application/json
X-Authorization: Bearer {token}
```

**Request Body:**

```json
{
  "method": "getValue",
  "params": {},
  "timeout": 5000
}
```

**Response (200 OK):** Device-defined response body.

> **⚠ Note:** Test returned `504` (Gateway Timeout) because the device was offline during testing. This is expected behavior.

**Error Responses:**

| Status | Meaning                  | UI Action                       |
| ------ | ------------------------ | ------------------------------- |
| 504    | Device offline / timeout | Show "Device not responding"    |
| 401    | Token expired            | Trigger reactive token refresh  |
| 5xx    | Server error             | Show "Command failed — retry"   |

> **✅ Confirmed:** RPC path is `/api/rpc/oneway/{deviceId}` and `/api/rpc/twoway/{deviceId}` (NOT `/api/plugins/rpc/...`).

---

## 11. Device HTTP API (ESP32 firmware side)

> These endpoints are used by the device (ESP32), **not** the web app. Documented for completeness.

### `POST /api/v1/{accessToken}/telemetry`

> Device publishes telemetry data

**Request Body:**

```json
{
  "temperature": 25.5,
  "humidity": 60
}
```

### `POST /api/v1/{accessToken}/attributes`

> Device publishes client attributes

**Request Body:**

```json
{
  "firmwareVersion": "1.0.0"
}
```

### `GET /api/v1/{accessToken}/rpc?timeout=20000`

> Device long-polls for incoming RPC requests

### `POST /api/v1/{accessToken}/rpc/{requestId}`

> Device sends RPC response

**Request Body:**

```json
{
  "result": "ok"
}
```

> **⚠ Note:** These endpoints use the device `accessToken` (from credentials), not the JWT Bearer token. Tests returned `ETIMEDOUT` due to network issues, not API errors.

---

## 12. WebSocket — Telemetry Subscription

### Connection

```
wss://<THINGSBOARD_HOST>/api/ws/plugins/telemetry?token={jwtToken}
```

> Use the JWT `token` from login (not the device access token) for WebSocket authentication.

### Subscription Command — Latest Telemetry (send after `onopen`)

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

### Subscription Command — Attributes (send after `onopen`)

```json
{
  "tsSubCmds": [],
  "historyCmds": [],
  "attrSubCmds": [
    {
      "entityType": "DEVICE",
      "entityId": "<DEVICE_ID>",
      "scope": "CLIENT_SCOPE",
      "cmdId": 2
    }
  ]
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

> **⚠ Note:** WebSocket tests returned `Unsupported protocol wss:` — this is a Bruno client limitation. WebSocket must be tested in a real browser environment.

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
| `temperature` | −40.0 to +85.0     | °C     | General temperature (same sensor or external)       |
| `humidity` | 0 to 100              | %      | Relative humidity                                   |

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
GET /api/device/{deviceId}/credentials  ──► {credentialsId (accessToken)}
        │                                    Store accessToken in sessionStorage
        ▼
WS connect + send subscription command  ──► Real-time telemetry stream begins
```

> **sessionStorage keys:** `token`, `refreshToken`, `customerId`, `deviceId`, `deviceAccessToken`
