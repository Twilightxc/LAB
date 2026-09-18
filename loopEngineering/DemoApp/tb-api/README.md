# ThingsBoard Demo API — Bruno Collection

Bruno collection for exercising the `demo.thingsboard.io` REST API to prototype a web dashboard.

## Setup

1. Open this folder in Bruno (File → Open Collection).
2. Select the **Demo** environment.
3. Set your own `username` / `password` as runtime/secret env values in Bruno
   (Environment settings → these are marked `vars:secret` so they are never
   saved in plain text in the `.bru` files). For customer-role testing use a
   **CUSTOMER_USER** login (e.g. the public demo `customer@thingsboard.org` /
   `customer`, or a customer user created under your own tenant).

## Flow — run top to bottom (folders and requests are numbered with this exact `seq`)

Every response is requested as JSON (`Accept: application/json` on all REST requests) so Bruno's response pane renders it in the JSON viewer.

| # | Folder | Request | What it does |
|---|---|---|---|
| 1 | Auth | Login | Logs in, stores `accessToken` / `refreshToken`. |
| 2 | Auth | Refresh Token | Renews `accessToken` once it expires (~2.5h TTL). Re-run anytime you hit `401`. |
| 3 | Auth | Get Current User | Captures `userId`, and (only for `CUSTOMER_USER`) `customerId`. |
| 4 | Devices | List Tenant Devices | *TENANT_ADMIN / SYS_ADMIN only — 403 for customer role.* Lists all tenant devices. |
| 5 | Devices | List Customer Devices | *Use this for customer role.* `GET /api/customer/{{customerId}}/devices`, auto-captures `deviceId`. |
| 6 | Devices | Get Device By Id | Fetches metadata for `{{deviceId}}` (works for both roles). |
| 7 | Devices | Get Device Credentials | *TENANT_ADMIN / SYS_ADMIN only — 403 for customer role.* Captures `deviceCredentialsId` / `deviceAccessToken`. |
| 8 | Telemetry | Get Latest Timeseries | Latest values for all telemetry keys of `{{deviceId}}`. |
| 9 | Telemetry | Get Timeseries History | Historical values; set `telemetryKeys`, `startTs`, `endTs` env vars first. |
| 10 | Attributes | Get Client Attributes | Client-scope attribute values. |
| 11 | Attributes | Get All Attributes | Client + server + shared scope attribute values. |
| 12 | RPC | Send Oneway RPC | Fire-and-forget command to the device (both roles OK if device belongs to customer). |
| 13 | RPC | Send Twoway RPC | Same, but waits for the device's reply or `timeout`. |
| 14 | Device API | Post Telemetry (as device) | Simulates the device pushing telemetry, using `deviceAccessToken`. |
| 15 | Device API | Post Attributes (as device) | Simulates the device pushing attributes. |
| 16 | Device API | Poll RPC Requests (as device) | Long-polls for an incoming RPC and captures `rpcRequestId`. |
| 17 | Device API | Reply to RPC (as device) | Answers the RPC captured above — pairs with step 13. |
| 18 | WebSocket | Subscribe Latest Telemetry | Real-time telemetry push over WS; connect, send message 1 to subscribe, message 2 to unsubscribe. |
| 19 | WebSocket | Subscribe Attributes | Real-time attribute push over WS. |

Steps 4/7 are tenant-admin only and will `403` under a customer login — see **Testing as a customer user** below for the workaround.

## Environment variables stored

| Var | Set by | Purpose |
|---|---|---|
| `username` / `password` | you (secret) | your ThingsBoard login |
| `accessToken` / `refreshToken` | Login / Refresh Token | user JWT session |
| `userId` | Get Current User | logged-in user's id |
| `customerId` | Get Current User (customer role only) | scopes device listing to `/api/customer/{customerId}/devices` |
| `deviceId` | List Tenant Devices *or* List Customer Devices | target device for telemetry/attributes/RPC |
| `deviceCredentialsId` / `deviceAccessToken` | Get Device Credentials (tenant admin only) | the device's own auth token, used by the Device API requests |
| `rpcRequestId` | Poll RPC Requests (as device) | id of an incoming RPC to reply to |

## Notes for the web dashboard

- REST auth uses `X-Authorization: Bearer <token>` header (ThingsBoard's convention, not the standard `Authorization` header).
- `accessToken` expires — call **Refresh Token** or re-run **Login** if you get `401`.
- **WebSocket** requests need Bruno v4+ (WS support). Bruno passes the JWT as a `token` query param on the WS URL (`wss://.../api/ws/plugins/telemetry?token=...`), matching ThingsBoard's WS auth scheme. Connect, send message 1 (subscribe), watch incoming pushes, then send message 2 (unsubscribe) before disconnecting.
- **RPC**: `Send Oneway RPC` doesn't wait for the device to respond (202 Accepted); `Send Twoway RPC` blocks until the device answers or `timeout` elapses — the device only "answers" if something is actively polling `Device API/Poll RPC Requests (as device)` and replying, since `demo.thingsboard.io` devices aren't really online.
- **Device API** calls (`/api/v1/{{deviceAccessToken}}/...`) intentionally have `auth: none` and no `X-Authorization` header — device-to-platform auth is via the access token embedded in the URL, not the user JWT.

## Testing as a customer user

ThingsBoard scopes REST access by role. When logged in as a `CUSTOMER_USER`:

- Use **List Customer Devices**, not **List Tenant Devices** (the latter returns `403 Forbidden`).
- **Get Device Credentials** also returns `403` — customer users aren't allowed to read a device's access token via REST. If you still need `deviceAccessToken` to exercise the **Device API** / RPC-reply requests while testing as a customer:
  1. Temporarily switch the `Demo` environment's `username`/`password` to a tenant admin login.
  2. Run **Get Device Credentials** once for the shared device to populate `deviceAccessToken`.
  3. Switch `username`/`password` back to the customer login — `deviceAccessToken` stays set as a runtime var and keeps working for the Device API calls.
- Everything else (telemetry, attributes, RPC, WebSocket subscriptions) works the same for a customer, provided the device is assigned to that customer's account.

## Running from the CLI and saving results as JSON

Every response can be captured to a machine-readable JSON file via [Bruno CLI](https://docs.usebruno.com/bru-cli/overview), which is ideal for handing results to an AI/automation for analysis.

```powershell
npm install                # installs @usebruno/cli (devDependency, see package.json)
npm test                   # runs the whole collection, saving a NEW timestamped report each time
```

`npm test` runs [scripts/run-tests.ps1](scripts/run-tests.ps1), which writes a fresh, uniquely named report per run — `results/<yyyyMMdd-HHmmss>.json` / `.html` — instead of silently overwriting the previous one, then also refreshes `results/latest.json` / `.html` as a "most recent" pointer. This way every run's true result is preserved and you can diff runs instead of losing history.

`username` / `password` are secrets and are **not** stored on disk, so the CLI can't read them from the GUI's keychain — pass them explicitly if you invoke `bru run` directly instead of `npm test`:

```powershell
npx bru run --env Demo --env-var username=customer@thingsboard.org --env-var password=customer --reporter-json results/manual-run.json --reporter-html results/manual-run.html
```

Each JSON report is an array of per-request objects, each with `request` (method/url/headers/body), `response` (status/headers/body/responseTime), and `testResults` — everything an AI needs to diagnose a failing step without re-running the collection. Run `npm run test:junit` instead if you also want a JUnit XML for CI tooling.

Notes:
- **Login without CLI secrets still fails (401) and that's fine** — the **Login**/**Refresh Token** scripts only overwrite `accessToken`/`refreshToken` on a `200` response, so a failed CLI login (no `--env-var username/password` passed) no longer clobbers the still-valid token already saved in the environment from your last GUI login. That's why the rest of the requests in a run can legitimately show `200` even though `Login` itself shows `401` — it's not a false result, it's Login being skipped in favor of the still-valid session.
- The `WebSocket/*` requests currently aren't executable via `bru run` (CLI has no WS transport yet) — they'll show as failed/unsupported in the report; test those from the Bruno app instead.
- `Device API/Poll RPC Requests (as device)` long-polls for up to 20s and will show `408` in the report if no RPC arrives during the run — that's expected unless something is actively sending an RPC at the same time. `Reply to RPC (as device)` will then show `404` since there's no `rpcRequestId` to reply to.
