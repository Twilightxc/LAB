# Loop Engineering Process

> **Cycle:** Goal → Context → Action → Check → Fix → Repeat → Review

---

## Main Loop

```
main {
    think       ← Read context, understand requirements, plan tasks
    build       ← Execute one task at a time
    verify      ← Check results, fix errors, report status
}
```

---

## Startup Sequence — When Triggered to Start

Execute these steps **in order** every time you are triggered:

### Phase 1: Read

1. Read `Role.md` — understand your **role, skills, and constraints**
2. Read `L1/1requirment.md` — understand **what** to build

### Phase 2: Think

3. Read `DECISION.md` — check prior design decisions
4. Read `TASK.md` — check current task status
5. Read `SESSION.md` — check what was done previously
6. Summarize your understanding back to the human

### Phase 2.5: Clarification Gate

> **Do NOT proceed to planning or building until every ambiguity is resolved.**

7. Review your understanding — identify anything that is **unclear, ambiguous, or assumed** about:
   - Architecture (page structure, component layout, data flow)
   - Tech stack (libraries, APIs, protocols, versions)
   - Scope (which features are in/out, edge cases)
   - Design (UI style, colors, responsive behavior)
   - Any other aspect of the project
8. **Ask the developer** about every unclear point — list your questions explicitly
9. **Wait for answers** — do not guess, do not assume, do not proceed
10. Repeat steps 7–9 until you are **100% confident** you understand what to build and how
11. State: "All questions resolved — proceeding to planning" before moving on

### Phase 3: Plan

12. Break the remaining work into small, ordered tasks
13. Update `TASK.md` with the task list (mark pending items as `[ ]`)
14. Update `DECISION.md` with any new design decisions

### Phase 4: Build

15. Execute **one task at a time** from `TASK.md`
16. After completing each task, mark it `[x]` in `TASK.md`
17. After each task, state how to verify (e.g. open in browser, check console)

### Phase 5: Verify

18. Check: Does the output match `1requirment.md`?
19. Check: Does the code run without errors?
20. Check: Are there any security risks?
21. If any check fails → use the error as feedback → fix → repeat

### Phase 6: Log

22. Update `SESSION.md` — log what was done, what files changed, and why

---

## Input Context (Read)

| File                 | Purpose                |
| -------------------- | ---------------------- |
| `Role.md`            | Role & skills          |
| `L1/1requirment.md`  | Requirements & scope   |
| `DECISION.md`        | Prior design decisions |
| `TASK.md`            | Current task list      |
| `SESSION.md`         | Session history        |

---

## Output (Write / Update)

| File                 | What to update                        |
| -------------------- | ------------------------------------- |
| `DECISION.md`        | New design decisions made this loop   |
| `TASK.md`            | Mark tasks done, add new tasks found  |
| `SESSION.md`         | Log what was done, changed, and why   |

---

## Loop Evaluation — Check & Fix

After each action cycle, evaluate:

- ✅ Does the output match the requirement in `1requirment.md`?
- ✅ Does the code run without errors?
- ✅ Are there any security risks?
- ❌ If any check fails → use the error message as feedback → fix → repeat

---

## Human Checkpoint

Pause and report to the human when:

- You are **unsure** about a design decision
- A task has **security implications**
- You have completed a **milestone** (e.g. login page done, dashboard done)
- You need to **deviate** from the current plan

---

## Stop Conditions

The loop ends when:

- All tasks in `TASK.md` are marked complete
- All requirements in `1requirment.md` are satisfied
- The human explicitly says to stop
- A blocking error cannot be resolved after 3 fix attempts

---

## Rules

- Do **one task** at a time — do not batch unrelated changes
- If unsure, **ask before acting**
- If there is an error, use the **error message as feedback** and fix in the next loop
- After finishing, **summarize** all files created or modified
- If there is a **security risk**, flag it immediately

---

## Example

**Trigger**: "Start building L1"

**Agent response**:

1. ✅ Read `Role.md` — Role: AI Developer, Stack: HTML/CSS/JS + ThingsBoard
2. ✅ Read `1requirment.md` — Goal: build login + dashboard pages
3. ✅ Read `DECISION.md` — Empty, no prior decisions
4. ✅ Read `TASK.md` — Empty, no tasks yet
5. ✅ Read `SESSION.md` — Empty, first session

**Summary**: Building a 2-page IoT dashboard. Login page authenticates via ThingsBoard REST API. Dashboard page shows real-time telemetry via WebSocket.

**TASK.md updated**:
```
- [ ] Create common.css with CSS variables and base styles
- [ ] Create login.html + login.css
- [ ] Create auth.js module (login, token refresh, get credentials)
- [ ] Create dashboard.html + dashboard.css
- [ ] Create ws.js module (WebSocket telemetry subscription)
- [ ] Create rpc.js module (device RPC commands)
- [ ] Integration test: login → dashboard → live data
```

**Starting task 1**: Create `common.css`...
