# Colorful-UI Pre-Release Audit — Task List

Date generated: 2026-05-08
Branch audited: `feature/cleanup`
Mod root: `Colorful-UI/` (single-mod repo, multi-PBO: `GUI.pbo` + `Scripts.pbo`)

The mod **builds** today. Audit focuses on **correctness** — defects that will crash the script VM, render wrong, fail silently on Linux servers, or look unprofessional to end users.

Recommended fix order: BLOCKING → HIGH → MEDIUM → LOW. Build + smoke test after BLOCKING+HIGH, then again after MEDIUM.

---

## Quick checklist

- [x] **BLOCKING** (6 items — all crash-causers; will fail on first real use)
  - [x] B-1 — `Configure.c` + `Exit.c` layout paths point at nonexistent `new_ui/dialogs/` directory
  - [x] B-2 — `ItemDropWarning.c` layout path doesn't exist
  - [x] B-3 — `Invite.c` layout doesn't exist + null-deref on line 15
  - [x] B-4 — `Respawn.c:34-35` uses `m_GameOverScreen` before assigning it
  - [x] B-5 — `Hints.c:3` `data/hints.json` vs actual `Data/hints.json` (case)
  - [x] B-6 — `InGameMenu.c:40-42` unguarded `m_TopShader` / `m_BottomShader` / `m_MenuDivider` dereferences
- [x] **HIGH** (9 items — config leftovers + casing inconsistency)
  - [x] H-1 — `CfgMods.ColorfulUI.name` is `"DayZ-Mod-Template"` (shows in launcher!)
  - [x] H-2 — `engineScriptModule` declares nonexistent `1_Core`
  - [x] H-3 — `Scripts/config.cpp` `requiredAddons[]` missing `ColorfulUI_GUI`
  - [x] H-4 — `PreloadAddons` references phantom `COLORFUL_HUD`
  - [x] H-5 — `LogoutMenu.c:80` `player` not null-checked before use
  - [x] H-6 — `Hints.c:4` `m_RootPath` uses `Gui` (wrong casing)
  - [x] H-7 — Systemic `gui` vs `GUI` casing across ~15 scripts + ~35 layout lines (36 files normalized)
  - [x] H-8 — 4 inventory layouts reference missing `SquareFont*` fonts (replaced with `GB-Regular`)
  - [x] H-10 — `TabberUI.c:57` loads vanilla `tab.layout` (now points at CUI tab.layout)
- [x] **MEDIUM** (7 items — name collisions, shared state, cosmetic correctness)
  - [x] M-1 — `DeathScreens.c` — `ShowDeadScreen` bool/method name collision (renamed bool to `ShowGameOverOverlay` across Settings.c, DeathScreens.c, InGameMenu.c, Respawn.c)
  - [x] M-2 — `utils.c:1` — `allInvalid` global bool bleeds between menus (added `allInvalid = true;` reset before social/url checks in MainMenu, InGameMenu, LogoutMenu, Respawn)
  - [x] M-3 — `Music.c:5` — `RegisterTracksMenu` wipes vanilla; intent comment added
  - [x] M-4 — `Scripts/config.cpp:69-75` — sound paths normalized to `Colorful-UI/GUI/sounds/MainMenu/...`
  - [x] M-5 — `Music_Menu_SoundSet` → `CUI_Music_Menu_SoundSet` (config.cpp + Music.c)
  - [x] M-7 — `UIColor.Twitter()` now returns legacy Twitter blue `ARGB(255, 29, 161, 242)`
  - [x] M-10 — `GB-Italic.fnt` + `.edds` + `.edds.meta` deleted
- [x] **LOW / Hygiene** (7 items)
  - [x] L-1 — `Start.bat` deleted (dead launcher referencing removed `Utils/Shared/` — already gone from working tree at audit time)
  - [x] L-2 — `.gitignore` already trimmed to current paths (matches HEAD; no stale `Servers/` or `Missions/` entries)
  - [x] L-4 — `GUI/layouts/proto/testing.layout` directory deleted
  - [x] L-5 — `Scripts/stringtable.csv` deferred — header-only file is acceptable for English-only ship; full localization is post-1.0 work
  - [x] L-7 — `Workbench/server.cfg` reviewed: contains only stock debug-server defaults (empty passwords, `BattlEye = 0`, `instanceId = 1`, dayzOffline.chernarusplus mission) — no sensitive data; safe in repo
  - [x] L-8 — AddonBuilder source roots are `P:\Colorful-UI\GUI` and `P:\Colorful-UI\Scripts` (not the parent), so `Colorful-UI/Workbench/` is never traversed during packing — naturally excluded
  - [x] post-fix — Rebuild PBOs and smoke test in DayZDiag (Mission module compiles, server binds, client connects, no script errors in RPT)
- [x] **LEAKS / PERF** (not in original audit — found in deeper review)
  - [x] LK-1 — `cuiElmnt.s_Handlers` per-owner cleanup: added `Class m_Owner` to `CUIButtonHandler`, added `cuiElmnt.CleanupForOwner(Class)`, threaded `this` as first arg to all `proBtnXX` call sites (8 menu files), added `~MenuClass()` destructors to 12 menu/dialog classes calling `cuiElmnt.CleanupForOwner(this)`
  - [x] LK-2 — `~InviteMenu()` now calls `Remove(UpdateTime)` to cancel the 1-sec repeating timer
  - [x] LK-3 — `LoginQueueBase.Show()` only allocates `m_HintPanel` when null
  - [x] LK-4 — `TutorialsMenu` calls `Remove(DrawConnectingLines)` before `Insert()` so reopens don't duplicate
  - [x] LK-5 — `DayZPlayerImplement.ShowDeadScreen` calls `Remove(StopDeathDarkeningEffect)` before scheduling the next one
  - [x] PF-1 — `CuiLogger` keeps a single `FileHandle` open; close+reopen every 64 writes acts as a bounded flush. Added explicit `Close()` for shutdown.
- [x] **DISCOVERED-AT-RUNTIME** (not in original audit — surfaced when actually loading the mod into Diag)
  - [x] D-1 — `Mission` script module failed to compile: 19 methods missing `override` keyword. Added across `OptionSelectorBase.c` (13), `MainMenu.c` (`NextCharacter`, `PreviousCharacter`), `Respawn.c` (`Update`), `OptionsTabs.c` (4× `GetLayoutName`).
  - [x] D-2 — Type errors after D-1 fix: ~50 `Cannot convert 'Widget' to 'ButtonWidget'` / `Unsafe down-casting` errors across 7 menu files. Cause: `m_X = layoutRoot.FindAnyWidget(...)` assigning untyped `Widget` to typed `ButtonWidget`/`ImageWidget` fields, plus vanilla field-name shadowing where the modded class's stricter type was overridden by the base class's `Widget` declaration. Fix: PowerShell pass that wrapped every assignment with `<Type>.Cast(...)` and every `cuiElmnt.proBtnXX(m_X, ...)` call site with `ButtonWidget.Cast(m_X)`. Files modified: `MainMenu.c`, `InGameMenu.c`, `LogoutMenu.c`, `Respawn.c`, `CharacterCreation.c`, `Options.c`, `Keybindings.c`.

---

# LEAKS & PERF (deeper review)

## LK-1 — `cuiElmnt.s_Handlers` grows unbounded (CRITICAL)

**File:** `Colorful-UI/Scripts/5_Mission/GUI/Components/buttons.c:232`

`s_Handlers` is a static `array<ref CUIButtonHandler>`. Every call to `proBtnCB`/`proBtnDC`/`proBtnURL`/`proSolidBtn` does `s_Handlers.Insert(h)`. A `Cleanup()` method exists at line 234, but **nothing in the codebase calls it**.

Per menu Init: 5–15 handlers added. Each handler holds a ButtonWidget ref, TextWidget ref, and Class targetClass ref. Closing the menu does not release them.

After ~50 menu opens (a normal session — open Esc, close, reopen), s_Handlers has hundreds of dead handlers each pinning widgets that should have been GC'd.

**Fix path:**
- Naive: call `cuiElmnt.Cleanup()` at the start of each menu's Init(). Side-effect: if a dialog opens on top of an active menu, its Cleanup() wipes the parent's handlers and breaks the parent's buttons. Not safe.
- Per-menu: change `proBtnXX(...)` to take an `owner` Class param. Track handlers as `map<Class, array<ref CUIButtonHandler>>`. Add `cuiElmnt.CleanupForOwner(this)` and call it from each menu's destructor (`~MainMenu()`, etc.). Or each menu maintains its own `protected ref array<ref CUIButtonHandler>` and disposes it in its destructor.
- Recommended: per-menu local handler array. Refactor in `cuiElmnt` + add destructors to MainMenu, InGameMenu, LogoutMenu, RespawnDialogue, CharacterCreationMenu, TutorialsMenu, OptionsMenu*, LogoutMenu*.

## LK-2 — `InviteMenu.UpdateTime` zombie timer

**File:** `Colorful-UI/Scripts/5_Mission/GUI/CUI/dialogs/Invite.c:34`

```c
GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(UpdateTime, 1000, true);
```

Repeating callback (`true`) with no matching `Remove()` anywhere. There is no `~InviteMenu()` destructor in this file. The vanilla destructor may or may not cancel it.

After the dialog closes, `UpdateTime` keeps firing every 1 second forever. If the player opens/closes Invite multiple times, each open spawns a new zombie timer.

**Fix:**
```c
void ~InviteMenu()
{
    GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(UpdateTime);
}
```

## LK-3 — `LoginQueueBase.Show()` re-allocates `m_HintPanel`

**File:** `Colorful-UI/Scripts/3_Game/Systems/Loading.c:190`

```c
override void Show()
{
    ...
    if (!NoHints)
    {
        layoutRoot.Show(true);
        m_HintPanel = new UiHintPanelLoading(layoutRoot.FindAnyWidget("hint_frame0"));
    }
}
```

`Show()` is called every time the queue screen is shown (potentially repeatedly during reconnects). Each call creates a new `UiHintPanelLoading` (which loads a video, allocates widgets). The previous instance's destructor will run when ref-count drops, but there's a window where two are live, and any code that holds an external ref to the old `m_HintPanel` keeps it alive forever.

`m_HintPanel` is also set in `Init()` at line 149, so the first `Show()` after Init double-creates.

**Fix:** Only create on first show, or null-check before creating. Better: move creation entirely into Init() and let Show() just toggle visibility.

## LK-4 — `TutorialsMenu.Init` duplicate-inserts `DrawConnectingLines`

**File:** `Colorful-UI/Scripts/5_Mission/GUI/CUI/options/Tutorials.c:18`

```c
m_TabScript.m_OnTabSwitch.Insert(DrawConnectingLines);
```

If TutorialsMenu is opened, closed, opened again, this inserts `DrawConnectingLines` into the event handler array a second time. On the next tab switch the function fires twice. After N opens, fires N times.

**Fix:** Either guard with a "already inserted" flag, or call `m_TabScript.m_OnTabSwitch.RemoveItem(DrawConnectingLines)` before inserting (idempotent).

## LK-5 — `DeathScreens.ShowDeadScreen()` queues without canceling pending

**File:** `Colorful-UI/Scripts/4_World/DeathScreens.c:18`

```c
GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(StopDeathDarkeningEffect, duration * 1000, false);
```

If the player dies, respawns mid-fade, and dies again before the previous `duration*1000` ms elapses, two `StopDeathDarkeningEffect` calls stack up. Each will fire and stomp the fade state.

**Fix:** Call `Remove(StopDeathDarkeningEffect)` before scheduling.

## PF-1 — `CuiLogger.Log()` opens/closes file every call

**File:** `Colorful-UI/Scripts/3_Game/Systems/Logger.c:16`

```c
FileHandle handle = OpenFile(LOG_FILE, FileMode.APPEND);
if (handle != 0)
{
    FPrintln(handle, ...);
    CloseFile(handle);
}
```

Every `CuiLogger.Log()` call opens the file, appends one line, closes the file. With `CuiDebug = true`, every mouse-enter/leave on a button triggers a Log call. The FS is hammered. On HDD-backed servers this is meaningfully slow.

`CuiDebug = false` in release (defaults to false in `Settings.c`), so this is mostly a dev-time perf issue. But the pattern is wrong even for debug builds.

**Fix path:**
- Open file once at `InitCUILogger()` and keep the handle open until shutdown. Risk: the engine doesn't always call a clean shutdown; partial writes possible.
- Buffer log lines in memory and flush every N calls or every M ms.
- Cheapest: just leave the open/close pattern but add a guard so identical messages within 1ms are coalesced.

---

# BLOCKING

These six defects will crash the script VM, fail silently on case-sensitive servers, or break the highest-traffic UI flow in the mod. Fix all six before any release.

## B-1 — Two dialog scripts load layouts that do not exist

**Severity:** BLOCKING
**Specialist:** dayz-script-specialist
**Files:**
- `Colorful-UI/Scripts/5_Mission/GUI/CUI/dialogs/Configure.c:19`
- `Colorful-UI/Scripts/5_Mission/GUI/CUI/dialogs/Exit.c:15`

**Evidence (current, broken):**
```c
// Configure.c:19
"Colorful-UI/gui/layouts/new_ui/dialogs/cui.Configure.dialog.layout"

// Exit.c:15
"Colorful-UI/gui/layouts/new_ui/dialogs/cui.Exit.dialog.layout"
```

**Why it crashes:** The directory `GUI/layouts/new_ui/dialogs/` does not exist anywhere under `Colorful-UI/GUI/`. The actual files live at `GUI/layouts/dialogs/cui.configure.dialog.layout` and `GUI/layouts/dialogs/cui.Exit.dialog.layout`. `CreateWidgets()` returns null. The next dereference (`FindAnyWidget`, `Cast`, etc.) hard-crashes the script VM.

**Fix:**
```c
// Configure.c:19
"Colorful-UI/GUI/layouts/dialogs/cui.configure.dialog.layout"

// Exit.c:15
"Colorful-UI/GUI/layouts/dialogs/cui.Exit.dialog.layout"
```

Note: also fixes the casing (`gui` → `GUI`) per H-7.

---

## B-2 — `WarningMenuBase` (ItemDropWarning) loads a layout that does not exist

**Severity:** BLOCKING
**Specialist:** dayz-script-specialist
**File:** `Colorful-UI/Scripts/5_Mission/GUI/CUI/dialogs/ItemDropWarning.c:8`

**Evidence (current, broken):**
```c
"Colorful-UI/gui/layouts/cui.day_z_dropped_items.layout"
```

**Why it crashes:** No file matching `cui.day_z_dropped_items.layout` exists anywhere in the mod. The real layout is `GUI/layouts/dialogs/cui.droppedItems.dialog.layout`. Null root crashes on the next `FindAnyWidget` call.

**Fix:**
```c
"Colorful-UI/GUI/layouts/dialogs/cui.droppedItems.dialog.layout"
```

---

## B-3 — `InviteMenu` loads a layout that does not exist + null-deref on line 15

**Severity:** BLOCKING
**Specialist:** dayz-script-specialist
**File:** `Colorful-UI/Scripts/5_Mission/GUI/CUI/dialogs/Invite.c:11,15`

**Evidence (current, broken):**
```c
// Line 11
"colorful-ui/gui/layouts/cui.day_z_invite_dialog.layout"

// Line 15 — runs unconditionally even if widgets aren't found
m_LogoutTimetext.SetText(m_iTime.ToString())
```

**Why it crashes:** The layout file does not exist anywhere in the mod. The class also unconditionally dereferences `m_LogoutTimetext` without a null guard, so even fixing the layout path won't save it if the widget name doesn't match.

**Note:** The `COLORFUL_EXIT` and `COLORFUL_CONFIGURE` menu IDs that route to these dialogs are commented out in `missionbase.c`, so B-1/B-2/B-3 may not currently fire in this build. They remain ticking bombs the moment those handlers are uncommented or vanilla code calls `InviteMenu`.

**Fix options:**
1. Supply the missing layout file under `GUI/layouts/dialogs/cui.invite.dialog.layout` and update the path string (preferred).
2. Stub or delete the class if invite functionality is deferred for this release.
3. Add a `if (m_LogoutTimetext)` guard around line 15 either way.

---

## B-4 — `Respawn.c` uses `m_GameOverScreen` before it is assigned

**Severity:** BLOCKING
**Specialist:** dayz-script-specialist
**File:** `Colorful-UI/Scripts/5_Mission/GUI/CUI/menus/Respawn.c:34-35`

**Evidence (current, broken):**
```c
m_GameOverScreenImage = ImageWidget.Cast(m_GameOverScreen.FindAnyWidget("GameOverScreenImage")); // line 34 — uses m_GameOverScreen
m_GameOverScreen      = Widget.Cast(layoutRoot.FindAnyWidget("GameOverScreen"));                 // line 35 — assigns m_GameOverScreen
```

**Why it crashes:** `m_GameOverScreen` is a member field with no initializer, so it is null on entry to `Init()`. Line 34 dereferences null. The fix is one swap — but it crashes the VM on every player respawn until fixed.

**Fix:**
```c
m_GameOverScreen      = Widget.Cast(layoutRoot.FindAnyWidget("GameOverScreen"));                 // assign FIRST
m_GameOverScreenImage = ImageWidget.Cast(m_GameOverScreen.FindAnyWidget("GameOverScreenImage")); // then use
```

Also recommended: add a null guard on the new line 35 in case `GameOverScreen` isn't in the layout for some loadouts.

---

## B-5 — `UiHintPanel.m_DataPath` uses wrong subdirectory casing

**Severity:** BLOCKING
**Specialist:** dayz-script-specialist
**File:** `Colorful-UI/Scripts/3_Game/Systems/Hints.c:3`

**Evidence (current):**
```c
protected const string m_DataPath = "Colorful-UI/Scripts/data/hints.json";
```

**Why it breaks:** The file on disk is `Scripts/Data/hints.json` (capital `D`). On Windows the PBO filesystem is case-insensitive at build time, so this masquerades as working in dev. **DayZ servers hosted on Linux use a case-sensitive VFS inside the PBO.** On those servers the hints panel will silently fail to load and emit "Could not create the hint panel." in the log. All loading-screen hint panels disable.

**Fix:**
```c
protected const string m_DataPath = "Colorful-UI/Scripts/Data/hints.json";
```

Note: also see H-7 for the full casing pass.

---

## B-6 — `InGameMenu.Init()` unconditionally dereferences three widgets

**Severity:** BLOCKING
**Specialist:** dayz-script-specialist
**File:** `Colorful-UI/Scripts/5_Mission/GUI/CUI/menus/InGameMenu.c:40-42`

**Evidence (current, unguarded):**
```c
m_TopShader.SetColor(colorScheme.TopShader());
m_BottomShader.SetColor(colorScheme.BottomShader());
m_MenuDivider.SetColor(colorScheme.Separator());
```

**Why it crashes:** All three are assigned via plain `FindAnyWidget` (not `Class.CastTo`) with no null check. If the layout loads but any widget name is missing or misnamed, this crashes. Every other menu in this mod (`MainMenu.c`, `LogoutMenu.c`, `CharacterCreation.c`) consistently guards every widget. The in-game menu is the highest-traffic screen in the mod (opens every time the player presses Escape).

**Fix:**
```c
if (m_TopShader)    m_TopShader.SetColor(colorScheme.TopShader());
if (m_BottomShader) m_BottomShader.SetColor(colorScheme.BottomShader());
if (m_MenuDivider)  m_MenuDivider.SetColor(colorScheme.Separator());
```

Or normalize the assignments above to use `Class.CastTo(m_TopShader, layoutRoot.FindAnyWidget("TopShader"))` and let the cast fail gracefully.

---

# HIGH

These ten won't necessarily crash, but they're release blockers in the sense that end users will see broken text, wrong fonts, the wrong mod name in their launcher, or silent breakage on Linux server hosting.

## H-1 — `CfgMods.ColorfulUI.name` is still the template default

**Severity:** HIGH
**Specialist:** user can fix directly (one-line config edit)
**File:** `Colorful-UI/Scripts/config.cpp:27`

**Evidence:**
```cpp
name = "DayZ-Mod-Template";
```

**Why it matters:** This is the placeholder from the original project template. **The value shows in the DayZ launcher mod list.** End users will see "DayZ-Mod-Template" instead of "Colorful-UI" when browsing their installed mods. Looks unprofessional and breaks recognition.

**Fix:**
```cpp
name = "Colorful-UI";
```

(Or the desired display name — could be "Colorful UI Pro" to match the README header.)

---

## H-2 — `engineScriptModule` declares `1_Core` which does not exist

**Severity:** HIGH
**Specialist:** dayz-config-specialist
**File:** `Colorful-UI/Scripts/config.cpp:40`

**Evidence:**
```cpp
class engineScriptModule {files[]= {"Colorful-UI/scripts/1_Core"};};
```

**Why it matters:** There is no `Scripts/1_Core/` directory in the mod. The engine module is the earliest-loading context. Behavior depends on engine version: older builds silently skip the missing path, newer builds emit warnings that obscure other errors in the RPT. At minimum this is dead config that misleads anyone reading the mod source. Note: also has wrong casing (`scripts` should be `Scripts`).

**Fix options:**
1. Remove the line entirely (recommended — there's no engine-module code in this mod).
2. Create `Scripts/1_Core/` with a placeholder if engine-module code is planned.

---

## H-3 — `requiredAddons[]` missing `ColorfulUI_GUI` dependency

**Severity:** HIGH
**Specialist:** dayz-config-specialist
**File:** `Colorful-UI/Scripts/config.cpp:7`

**Evidence:**
```cpp
class ColorfulUI_Scripts
{
    requiredVersion = 0.1;
    requiredAddons[] = {"DZ_Data","DZ_Scripts","DZ_Sounds_Effects"};
};
```

**Why it matters:** Scripts reference imageset paths (`set:cui image:*`) and layout files that live in the GUI PBO. If `ColorfulUI_GUI` isn't a declared dependency of `ColorfulUI_Scripts`, load order isn't guaranteed. On multi-mod load orders where the engine resolves PBO order from `requiredAddons`, Scripts can initialize before GUI, causing imageset references to resolve to nothing.

**Fix:**
```cpp
requiredAddons[] = {"DZ_Data","DZ_Scripts","DZ_Sounds_Effects","ColorfulUI_GUI"};
```

---

## H-4 — `PreloadAddons` references phantom `COLORFUL_HUD`

**Severity:** HIGH
**Specialist:** dayz-config-specialist
**File:** `Colorful-UI/Scripts/config.cpp:18`

**Evidence:**
```cpp
list[] = {"DZ_Data","DZ_Scripts","DZ_UI","DZ_UIFonts","DZ_Sounds_Effects","COLORFUL_HUD"};
```

**Why it matters:** `COLORFUL_HUD` is not the name of either PBO in this mod (correct names: `ColorfulUI_GUI`, `ColorfulUI_Scripts`). Looks like a leftover from a previous project. The engine attempts to find `COLORFUL_HUD`, fails silently, and the preload intent is not fulfilled.

**Fix:**
```cpp
list[] = {"DZ_Data","DZ_Scripts","DZ_UI","DZ_UIFonts","DZ_Sounds_Effects","ColorfulUI_GUI","ColorfulUI_Scripts"};
```

---

## H-5 — `LogoutMenu.Init()` dereferences `player` without a null check

**Severity:** HIGH
**Specialist:** dayz-script-specialist
**File:** `Colorful-UI/Scripts/5_Mission/GUI/CUI/menus/LogoutMenu.c:80-84`

**Evidence:**
```c
PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
if (player.GetEmoteManager() && !player.IsRestrained() && !player.IsUnconscious())
```

**Why it crashes:** `player` is never null-checked before `player.GetEmoteManager()` is called. If the logout menu opens when there's no player entity (disconnecting state, certain mission phases), this crashes the VM. Compare with `Invite.c` which uses the right pattern: `if (player && player.GetEmoteManager() ...)`.

**Fix:**
```c
PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
if (player && player.GetEmoteManager() && !player.IsRestrained() && !player.IsUnconscious())
```

---

## H-6 — `Hints.c:4` `m_RootPath` uses wrong `Gui` casing

**Severity:** HIGH
**Specialist:** dayz-script-specialist
**File:** `Colorful-UI/Scripts/3_Game/Systems/Hints.c:4`

**Evidence:**
```c
protected string m_RootPath = "Colorful-UI/Gui/layouts/loading/hints/cui.ingamehints.layout";
```

**Why it matters:** The directory on disk is `GUI` (uppercase). This is the default for `UiHintPanel` used by `InGameMenu` (the hint panel shown during the death/respawn overlay). Fails on case-sensitive Linux server VFS — the in-game hint panel doesn't render. (`UiHintPanelLoading.Init()` overrides this with a different path that has the same problem — see H-7.)

**Fix:**
```c
protected string m_RootPath = "Colorful-UI/GUI/layouts/loading/hints/cui.ingamehints.layout";
```

---

## H-7 — Systemic `gui` vs `GUI` casing inconsistency

**Severity:** HIGH
**Specialist:** dayz-script-specialist (scripts) + dayz-ui-specialist (layouts)

**Files — scripts (~15):**
- `Colorful-UI/Scripts/3_Game/Systems/Hints.c:30`
- `Colorful-UI/Scripts/5_Mission/GUI/CUI/options/OptionsTabs.c:5,14,22,30`
- `Colorful-UI/Scripts/5_Mission/GUI/CUI/options/Tutorials.c:9`
- `Colorful-UI/Scripts/5_Mission/GUI/CUI/options/CharacterCreation.c:10`
- `Colorful-UI/Scripts/5_Mission/GUI/CUI/options/Options.c:16`
- `Colorful-UI/Scripts/5_Mission/GUI/CUI/menus/LogoutMenu.c:11`
- `Colorful-UI/Scripts/5_Mission/GUI/CUI/menus/Respawn.c:10`
- `Colorful-UI/Scripts/5_Mission/GUI/CUI/dialogs/Invite.c:11`
- `Colorful-UI/Scripts/5_Mission/GUI/CUI/dialogs/ItemDropWarning.c:8`
- `Colorful-UI/Scripts/5_Mission/GUI/CUI/dialogs/Exit.c:15`
- `Colorful-UI/Scripts/5_Mission/GUI/CUI/dialogs/Configure.c:19`

**Files — layouts (~35 lines across these files):**
- `Colorful-UI/GUI/layouts/dialogs/cui.reset.dialog.layout:147,180,273,293`
- `Colorful-UI/GUI/layouts/dialogs/cui.Exit.dialog.layout:146,179,211,243,272,292`
- `Colorful-UI/GUI/layouts/dialogs/cui.configure.dialog.layout:145,178,211,304,324`
- `Colorful-UI/GUI/layouts/loading/hints/cui.loadinghints.layout:222,270`
- `Colorful-UI/GUI/layouts/options/cui.characters.layout:654,947`
- `Colorful-UI/GUI/layouts/options/cui.tutorials.layout:5145`

**Why it matters:** The directory on disk is `GUI`. Scripts and layouts mix `Colorful-UI/GUI/...` and `Colorful-UI/gui/...` interchangeably. On the Windows dev machine the PBO filesystem is case-folded so both forms resolve. On Linux-hosted DayZ servers (and the Linux server binary in general), the PBO VFS is case-sensitive — paths with lowercase `gui` silently fail to resolve. Affects loading screen fonts, options tab layouts, hint panel.

**Fix:** Single-pass case-normalize. Recommend:
```bash
# In every script and layout file, replace:
Colorful-UI/gui/   →   Colorful-UI/GUI/
colorful-ui/gui/   →   Colorful-UI/GUI/
colorful-ui/GUI/   →   Colorful-UI/GUI/
```

Verify after the pass: `grep -ri 'colorful-ui/gui' Colorful-UI/` should return zero hits (case-insensitive grep).

---

## H-8 — Inventory layouts reference fonts that don't exist in this mod

**Severity:** HIGH
**Specialist:** dayz-ui-specialist (layouts) + dayz-asset-specialist (if shipping new fonts)
**Files:**
- `Colorful-UI/GUI/layouts/inventory/collapsible_header.layout:83`
- `Colorful-UI/GUI/layouts/inventory/closable_header.layout:154`
- `Colorful-UI/GUI/layouts/inventory/attachments_group_container.layout:65`
- `Colorful-UI/GUI/layouts/inventory/hands_header.layout:48,187`

**Evidence:**
```
font "Colorful-UI/gui/fonts/SquareFont28"
font "Colorful-UI/gui/fonts/SquareFont22"
font "Colorful-UI/gui/fonts/SquareFont20"
font "Colorful-UI/gui/fonts/SquareFont24"
```

**Why it matters:** `GUI/fonts/` contains only `GB-CleanItalic.fnt`, `GB-ClearnRegular.fnt`, `GB-Italic.fnt`, `GB-Regular.fnt`. No `SquareFont*` files exist. **Engine falls back to default font silently** — doesn't crash, but all inventory header text renders in vanilla fallback font instead of the intended custom face.

**Fix options:**
1. Supply the missing `.fnt` + `.edds` files for `SquareFont20/22/24/28` (preferred if those fonts were intended).
2. Replace all references with an existing font (e.g., `Colorful-UI/GUI/fonts/GB-Regular`).

Either way, also fix the casing per H-7.

---

## H-10 — `TabberUI.AddTab()` calls vanilla layout path directly

**Severity:** HIGH
**Specialist:** dayz-ui-specialist
**File:** `Colorful-UI/Scripts/5_Mission/GUI/Components/TabberUI.c:57`

**Evidence:**
```c
Widget tab = GetGame().GetWorkspace().CreateWidgets("gui/layouts/new_ui/tabber_prefab/tab.layout", m_Root);
```

**Why it matters:** No `Colorful-UI/` prefix — this loads vanilla DayZ's `tab.layout`, not a CUI tab. The next line correctly loads CUI's `cui.tab_control.layout`. Given that `SelectTabControl` and `ApplyTabColor` override the styling at runtime, the visual result may be acceptable. But if Bohemia ever changes vanilla `tab.layout` widget names (`PanelWidget0`, etc.), this breaks. And the styling is fighting vanilla's defaults rather than starting from a clean CUI base.

**Fix options:**
1. Confirm this is intentional and add a comment to `TabberUI.c:57` saying so.
2. Create `Colorful-UI/GUI/layouts/components/tabber_prefab/tab.layout` as a CUI-styled equivalent and update the path.

---

# MEDIUM

Cosmetic correctness, name collisions, and shared-state foot-guns.

## M-1 — `ShowDeadScreen` bool vs method name collision

**Severity:** MEDIUM
**Specialist:** dayz-script-specialist
**Files:**
- `Colorful-UI/Scripts/3_Game/Config/Settings.c:5` — declares `static bool ShowDeadScreen = false;`
- `Colorful-UI/Scripts/4_World/DeathScreens.c:19,24` — overrides `void ShowDeadScreen(bool show, float duration)` and assigns `ShowDeadScreen = true` inside the override

**Why it matters:** Inside the body of the overridden method, `ShowDeadScreen = true` assigns to the global static bool (which `InGameMenu.Update()` reads), **not** to the method itself. This works *now*, but it's a maintainability hazard: any future reader, linter, or refactor will trip on the name shadow. Will also break if Bohemia changes the method signature.

**Fix:** Rename the static bool to `ShowGameOverOverlay` (or similar) and update all references.

---

## M-2 — `allInvalid` global bool bleeds between menu instances

**Severity:** MEDIUM
**Specialist:** dayz-script-specialist
**File:** `Colorful-UI/Scripts/3_Game/Helpers/utils.c:1`

**Evidence:**
```c
bool allInvalid = true;
```

**Why it matters:** `allInvalid` is module-level (file-scope global) shared state. `CheckSocials()` mutates it as a side effect. `MainMenu`, `InGameMenu`, `LogoutMenu`, and `RespawnDialogue` all call `CheckSocials()` / `CheckURL()`. Each menu's check writes to the same global, bleeding state into the next menu opened in that session.

**Concrete bug:** If all socials are configured, `MainMenu.Init()` sets `allInvalid = false`. On player death, `InGameMenu.Init()` reads `allInvalid == false` and skips hiding the divider regardless of whether *its own* checks returned all-invalid. Manifests as a phantom MenuDivider showing/hiding incorrectly after the first menu transition.

**Fix:** Convert `allInvalid` to a local variable computed per-menu before the divider show/hide check.

---

## M-3 — `RegisterTracksMenu` wipes vanilla tracks without documenting intent

**Severity:** MEDIUM
**Specialist:** dayz-script-specialist
**File:** `Colorful-UI/Scripts/3_Game/Systems/Music.c:5`

**Evidence:**
```c
protected override void RegisterTracksMenu()
{
    m_TracksMenu = new array<ref DynamicMusicTrackData>();
    RegisterTrackMenu("Music_Menu_SoundSet", true);
}
```

**Why it matters:** Override creates a fresh array and registers only the CUI track — replaces all vanilla menu tracks with no `super` call. Architecturally sound (intentional full replacement), but a future maintainer will likely "fix" this by adding `super.RegisterTracksMenu()` and bring vanilla music back. Also: if another mod also overrides this, only the last-loaded mod's tracks register.

**Fix:** Add a comment documenting the intent:
```c
// CUI fully replaces the vanilla menu music array — do NOT add super.RegisterTracksMenu()
// here, that would re-add vanilla tracks alongside ours.
```

---

## M-4 — Sound paths use backslash + leading slash (legacy Arma style)

**Severity:** MEDIUM
**Specialist:** dayz-config-specialist
**File:** `Colorful-UI/Scripts/config.cpp:69-75`

**Evidence:**
```cpp
samples[] = {
    {"\Colorful-UI\GUI\sounds\MainMenu\Battle", 1},
    {"\Colorful-UI\GUI\sounds\MainMenu\Cello", 1},
    ...
};
```

**Why it matters:** Rest of the mod uses forward slashes (`Colorful-UI/GUI/...`). This array uses backslash with a leading backslash (legacy Arma absolute-path notation). Both resolve correctly in-engine on Windows. Inconsistency is the issue — and on case-sensitive servers, this is yet another place where casing matters.

**Fix:**
```cpp
samples[] = {
    {"Colorful-UI/GUI/sounds/MainMenu/Battle", 1},
    {"Colorful-UI/GUI/sounds/MainMenu/Cello", 1},
    ...
};
```

---

## M-5 — `Music_Menu_SoundSet` is un-namespaced (mod collision risk)

**Severity:** MEDIUM
**Specialist:** dayz-config-specialist
**Files:**
- `Colorful-UI/Scripts/config.cpp:50` — class definition
- `Colorful-UI/Scripts/3_Game/Systems/Music.c:7` — reference

**Evidence:**
```cpp
class Music_Menu_SoundSet
```

**Why it matters:** Class names in `CfgSoundSets` and `CfgSoundShaders` are global. Generic name `Music_Menu_SoundSet` will collide with any other mod that defines a same-named class. Last-loaded wins; behavior depends on alphabetical mod order.

**Fix:** Rename to `CUI_Music_Menu_SoundSet` (and the related `DZNC_Music_Menu_SoundShader` is already namespaced — keep that as-is, or align it as `CUI_Music_Menu_SoundShader`). Update the reference in `Music.c:7`.

---

## M-7 — `UIColor.Twitter()` returns Discord's color

**Severity:** MEDIUM (cosmetic but obvious)
**Specialist:** user can fix directly
**File:** `Colorful-UI/Scripts/3_Game/Config/Color.c:23-24`

**Evidence:**
```c
static int Discord() { return ARGB(255, 88, 101, 242); };
static int Twitter() { return ARGB(255, 88, 101, 242); };  // ← same value as Discord
```

**Why it matters:** Twitter button on every menu using `UIColor.Twitter()` renders in Discord purple. Looks like a copy-paste bug because it is.

**Fix:**
```c
static int Twitter() { return ARGB(255, 29, 161, 242); };  // legacy Twitter blue
// OR
static int Twitter() { return ARGB(255, 0, 0, 0); };       // current X brand (black)
```

---

## M-10 — `GB-Italic.fnt` is an orphan asset

**Severity:** MEDIUM
**Specialist:** dayz-asset-specialist
**Files:**
- `Colorful-UI/GUI/fonts/GB-Italic.fnt`
- `Colorful-UI/GUI/fonts/GB-Italic.edds`
- `Colorful-UI/GUI/fonts/GB-Italic.edds.meta`

**Why it matters:** No layout or script references `GB-Italic.fnt`. It packs into the GUI PBO and adds size for nothing.

**Fix options:**
1. Delete all three files if the italic variant isn't intended for use.
2. Wire it up in at least one layout if it's meant to be available.

---

# LOW / Hygiene

## L-1 — `Start.bat` is dead

**Severity:** LOW
**File:** `Start.bat:5-6,24`

**Evidence:**
```bat
SET "ASCIIARTPATH=.\Utils\Shared\Branding.txt"
...
if exist ".\Utils\Shared\Globals.cfg"
```

**Why it matters:** `Utils/Shared/` was removed in commit `bf8c895 Full removal of Servers and tools`. Anyone running `Start.bat` gets PowerShell errors reading `Branding.txt`, then "Globals.cfg not found. Exiting." Dead launcher in the repo root. Confusing for new contributors who try to "just run Start.bat".

**Fix options:**
1. Delete `Start.bat`.
2. Rewrite as a one-liner that points users at the new workflow (e.g., `/dayz-init` or whatever the new entry point is).

---

## L-2 — `.gitignore` has stale entries

**Severity:** LOW
**File:** `.gitignore:29-55`

**Why it matters:** Lines 29-55 reference `Servers/Cherno/`, `Servers/Frostline/`, `Servers/Livonia/`, and 12 `Missions/` subdirectories. None of those paths exist after `bf8c895`. Harmless but confusing — a new contributor reading `.gitignore` will think the repo has those directories.

**Fix:** Delete lines referring to `Servers/` and `Missions/`. Add the new `.server/`, `.agentic-z/`, and any other Agentic-Z artifacts if not already covered (current rules `.server`, `.claude`, `.agentic-z` are correctly bare-name and match at any depth — no change needed there).

---

## L-4 — `proto/testing.layout` ships in release PBO

**Severity:** LOW
**File:** `Colorful-UI/GUI/layouts/proto/testing.layout`

**Why it matters:** Prototype/test layout file gets packed into `GUI.pbo` and shipped to end users. Wastes a few KB of PBO space and exposes WIP to anyone who unpacks the mod.

**Fix options:**
1. Delete the file.
2. Move it outside the source tree (e.g., to a separate `Workbench/Prototypes/` dir that AddonBuilder excludes).
3. Add a build exclusion for `Colorful-UI/GUI/layouts/proto/`.

---

## L-5 — `stringtable.csv` is empty (no localization path)

**Severity:** LOW (deferred — not a release blocker)
**File:** `Colorful-UI/Scripts/stringtable.csv`

**Why it matters:** File contains only the column header. Zero data rows. Custom user-facing strings ("Settings", "Tutorial", "Credits", "Visit Website", etc.) are hardcoded English in `buttons.c` and the menu scripts. Mod can never be translated without a refactor.

**Fix:** Defer for now. When localization becomes a goal, extract hardcoded strings into `stringtable.csv` and replace them with `#cui_settings`-style key references.

---

## L-7 — `Workbench/server.cfg` tracked in git

**Severity:** LOW
**File:** `Colorful-UI/Workbench/server.cfg`

**Why it matters:** `Workbench/` is excluded from PBO builds but it's tracked in version control. If `server.cfg` contains credentials, passwords, or IP addresses, they're in the git history.

**Fix:** Open `Workbench/server.cfg` and inspect. If it has sensitive data, scrub it from history (`git filter-repo` or `bfg`) and add `Workbench/server.cfg` to `.gitignore`. If it's just a default cfg, leave it but consider whether dev configs belong in the public repo.

---

## L-8 — Confirm AddonBuilder excludes `Workbench/`

**Severity:** LOW
**File:** `Colorful-UI/Workbench/`

**Why it matters:** The `Workbench/` directory is documented as "not packed", but this relies on the AddonBuilder configuration being correct. There's no `.exclude` file or AddonBuilder exclusion list in the source tree.

**Fix:** Inspect the include/exclude lists used by `/dayz-build-pbo`. The current Agentic-Z `include.lst` is allowlist-based (specific extensions like `*.c`, `*.layout`, etc.) — `Workbench/` only contains `dayz.gproj`, `project.cfg`, `server.cfg`, and `.c` plugin files. The `.c` files in `Workbench/ToolAddons/Plugins/` (`DayZTools.c`, `KillDay.c`) **will currently be packed** because they match the `.c` extension allowlist. Verify by unpacking the most recent `Scripts.pbo` and checking for these paths.

**Verification command:**
```powershell
# Use DayZ Tools' PboUtils to list contents of the latest build
& 'C:\Program Files (x86)\Steam\steamapps\common\DayZ Tools\Bin\PboUtils\FileBank.exe' -dumpall 'P:\Mods\@Colorful-UI\Addons\GUI.pbo' 'P:\temp\check_gui'
& 'C:\Program Files (x86)\Steam\steamapps\common\DayZ Tools\Bin\PboUtils\FileBank.exe' -dumpall 'P:\Mods\@Colorful-UI\Addons\Scripts.pbo' 'P:\temp\check_scripts'
```

If `Workbench/` artifacts appear, the AddonBuilder source needs to point at `GUI/` and `Scripts/` directly (which it already does for GUI/Scripts builds — confirming this is fine for the GUI and Scripts PBOs since `Workbench/` lives at the `Colorful-UI/` parent level, not inside either subfolder). **Likely no actual issue here — flagging for explicit verification.**

---

# Post-fix verification

After completing each batch:

1. **Rebuild both PBOs:**
   ```powershell
   $ab = 'C:\Program Files (x86)\Steam\steamapps\common\DayZ Tools\Bin\AddonBuilder\AddonBuilder.exe'
   $inc = "$env:USERPROFILE\.claude\skills\dayz-build-pbo\include.lst"
   & $ab 'P:\Colorful-UI\GUI'     'P:\Mods\@Colorful-UI\Addons' '-prefix=Colorful-UI\GUI'     '-temp=P:\temp\Colorful-UI\GUI'     "-include=$inc" -clear
   & $ab 'P:\Colorful-UI\Scripts' 'P:\Mods\@Colorful-UI\Addons' '-prefix=Colorful-UI\Scripts' '-temp=P:\temp\Colorful-UI\Scripts' "-include=$inc"
   ```

2. **Smoke test in DayZDiag:**
   ```powershell
   python "$env:USERPROFILE\.claude\skills\dayz-launch-test\launch.py" 'Colorful-UI' --server chernarus
   ```

3. **Click through every menu** that the audit touched:
   - Main Menu → all buttons render, divider shows correctly
   - Options Menu → all four tabs (Game, Sound, Video, Controls) load
   - Character Creation
   - Tutorials menu
   - Spawn into world → press Esc → InGame Menu loads, divider correct
   - Logout dialog
   - Death/Respawn screen — most critical, this was B-4
   - Loading screen → hint panel renders
   - Drop an item → drop warning dialog (B-2)

4. **Tail RPT log** for any "Cannot resolve path" / "Cannot create widget" errors:
   ```
   .server\chernarus\client-profiles\!ClientDiagLogs\*.RPT
   .server\chernarus\server-profiles\*.RPT
   ```

5. **Optional but recommended:** test on a Linux server (or in a Linux container running DayZServer Linux) to catch any remaining case-sensitivity issues that Windows masks.
