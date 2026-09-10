# Requirement and Structure

> **Goal:** Build a minimal, modern, responsive demo dashboard with HTML, CSS & backend API (using ThingsBoard REST API & WebSocket).

---

## HTML

### Login Page

- Input fields for **username** and **password**
- Button: `Login to Dashboard`

### Dashboard Page

- Single `Logout` button → navigates back to Login
- Telemetry graph displaying real-time data from ThingsBoard
- Value card showing live telemetry readings

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