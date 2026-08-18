@echo off
setlocal
set "CUI_DIR=%~dp0"
set "CUI_ACTION=%~1"
if "%CUI_ACTION%"=="" set "CUI_ACTION=run"
powershell -NoProfile -ExecutionPolicy Bypass -Command "iex ([string]::Join([char]10,((Get-Content -LiteralPath '%~f0') | Select-Object -Skip 9)))"
echo.
pause
exit /b
# ===================== Colorful UI — server engine =====================
# This file is BOTH a batch launcher (top) and its own PowerShell engine
# (below). Double-click it to run the server. It finds DayZ, sets up the
# work drive + mission on first run, and launches the server + client.
$ErrorActionPreference = 'Stop'
$Mod      = 'Colorful-UI'  # the mod THIS repo builds — leave as-is
# The mod source lives at <repo>\Colorful-UI (not under workspace\ like our other repos).
$ModSrc   = 'Colorful-UI'
# ==== The mod list lives in a plain text file: tools\mods.txt =============
# Open tools\mods.txt, put one mod per line, save, run the game. That's it.
# Colorful-UI always loads and is added automatically.
$ModsFile  = Join-Path $env:CUI_DIR 'mods.txt'
$ExtraMods = @()
if (Test-Path $ModsFile) {
    $ExtraMods = @(Get-Content -LiteralPath $ModsFile |
        ForEach-Object { ($_ -replace '#.*$', '').Trim() } |
        Where-Object { $_ })
}
$Instance = 'chernarus'
$Template = 'dayzOffline.chernarusplus'
$Port     = 2702   # game port. DayZ Deluxe uses 2302, The Lab 2402, The Vale 2502, CartelZ 2602
                   # on this machine, so Colorful UI gets its own and all can run side by side.
$QueryPort = 27022 # Steam query port (27016 = Deluxe, 27017 = The Lab, 27018 = The Vale, 27020 = CartelZ).
                   # Two servers sharing one query port collide even when their game ports differ.
$ToolsDir = $env:CUI_DIR
$RepoRoot = (Resolve-Path (Join-Path $ToolsDir '..')).Path
$Action   = if ($env:CUI_ACTION) { $env:CUI_ACTION } else { 'run' }

function Say ($m, $c = 'Gray') { Write-Host $m -ForegroundColor $c }
function Ok  ($m) { Say "  [OK]   $m" 'Green' }
function Info($m) { Say "  [..]   $m" 'Gray' }
function Warn($m) { Say "  [WARN] $m" 'Yellow' }
function Die ($m) { throw $m }

function SteamRoot {
    foreach ($k in @(
            @{ P = 'HKCU:\Software\Valve\Steam';             N = 'SteamPath' },
            @{ P = 'HKLM:\SOFTWARE\WOW6432Node\Valve\Steam'; N = 'InstallPath' },
            @{ P = 'HKLM:\SOFTWARE\Valve\Steam';             N = 'InstallPath' })) {
        $v = (Get-ItemProperty -Path $k.P -Name $k.N -ErrorAction SilentlyContinue).$($k.N)
        if ($v -and (Test-Path $v)) { return (Resolve-Path $v).Path }
    }
    return $null
}
function SteamLibs {
    $s = SteamRoot
    $libs = New-Object System.Collections.Generic.List[string]
    if ($s) { $libs.Add($s) }
    $vdf = Join-Path $s 'steamapps\libraryfolders.vdf'
    if ($s -and (Test-Path $vdf)) {
        foreach ($m in [regex]::Matches((Get-Content $vdf -Raw), '"path"\s+"([^"]+)"')) {
            $p = $m.Groups[1].Value -replace '\\\\', '\'
            if ((Test-Path $p) -and ($libs -notcontains $p)) { $libs.Add($p) }
        }
    }
    return $libs
}
function FindCommon ($folder, $marker, $envVar) {
    if ($envVar) {
        $ov = [Environment]::GetEnvironmentVariable($envVar)
        if ($ov -and (Test-Path (Join-Path $ov $marker))) { return (Resolve-Path $ov).Path }
    }
    foreach ($lib in (SteamLibs)) {
        $c = Join-Path $lib "steamapps\common\$folder"
        if (Test-Path (Join-Path $c $marker)) { return (Resolve-Path $c).Path }
    }
    return $null
}
function DayZGame   { FindCommon 'DayZ' 'DayZ_x64.exe' 'DAYZ_GAME_PATH' }
function DayZServer { FindCommon 'DayZServer' 'DayZServer_x64.exe' 'DAYZ_SERVER_PATH' }
function DayZDiag {
    $ov = [Environment]::GetEnvironmentVariable('DAYZ_DIAG_PATH')
    if ($ov -and (Test-Path $ov)) { return (Resolve-Path $ov).Path }
    $g = DayZGame
    if ($g) { $d = Join-Path $g 'DayZDiag_x64.exe'; if (Test-Path $d) { return $d } }
    return $null
}
function DayZTools {
    $t = FindCommon 'DayZ Tools' 'Bin\AddonBuilder\AddonBuilder.exe' 'DAYZ_TOOLS_PATH'
    if ($t) { return $t }
    foreach ($k in @('HKLM:\SOFTWARE\WOW6432Node\Bohemia Interactive\DayZ Tools',
                     'HKLM:\SOFTWARE\Bohemia Interactive\DayZ Tools',
                     'HKCU:\SOFTWARE\Bohemia Interactive\DayZ Tools')) {
        $p = (Get-ItemProperty -Path $k -Name 'Path' -ErrorAction SilentlyContinue).Path
        if ($p -and (Test-Path (Join-Path $p 'Bin\AddonBuilder\AddonBuilder.exe'))) { return (Resolve-Path $p).Path }
    }
    return $null
}
function WorkDrivePath {
    $ov = [Environment]::GetEnvironmentVariable('DAYZ_WORK_DRIVE')
    if ($ov -and (Test-Path $ov)) { return (Resolve-Path $ov).Path }
    $t = DayZTools
    if ($t) {
        $ini = Join-Path $t 'settings.ini'
        if (Test-Path $ini) {
            $sec = ''
            foreach ($raw in (Get-Content $ini)) {
                $x = $raw.Trim()
                if ($x -match '^\[(.+)\]$') { $sec = $Matches[1].ToLower(); continue }
                if ($sec -eq 'projectdrive' -and $x -match '^\s*path\s*=\s*(.+)$') {
                    $v = $Matches[1].Trim().Trim('"')
                    if ($v -and (Test-Path $v)) { return (Resolve-Path $v).Path }
                }
            }
        }
    }
    foreach ($k in @('HKCU:\Software\Bohemia Interactive\DayZ Tools',
                     'HKLM:\SOFTWARE\WOW6432Node\Bohemia Interactive\DayZ Tools',
                     'HKLM:\SOFTWARE\Bohemia Interactive\DayZ Tools')) {
        foreach ($n in @('WorkDrive', 'WorkDrivePath', 'P_Drive', 'PDrive')) {
            $v = (Get-ItemProperty -Path $k -Name $n -ErrorAction SilentlyContinue).$n
            if ($v -and (Test-Path $v)) { return (Resolve-Path $v).Path }
        }
    }
    if ($t) { $wd = Join-Path $t 'Bin\WorkDrive'; if (Test-Path $wd) { return $wd } }
    return $null
}
function MountP {
    if (Test-Path 'P:\') { Ok 'P:\ ready'; return }
    $work = WorkDrivePath
    if (-not $work) { Die 'Could not find your DayZ work drive. Open DayZ Tools once to set it up (it creates P:\), then try again.' }
    Info "Mounting P:\ -> $work"
    $t = DayZTools
    $wdExe = if ($t) { Join-Path $t 'Bin\WorkDrive\WorkDrive.exe' } else { $null }
    if ($wdExe -and (Test-Path $wdExe)) {
        $p = Start-Process $wdExe -ArgumentList '/Mount', "`"$work`"" -PassThru
        for ($i = 0; $i -lt 15 -and -not (Test-Path 'P:\'); $i++) { Start-Sleep 1 }
        if (-not $p.HasExited) { try { $p.Kill() } catch {} }
    }
    if (-not (Test-Path 'P:\')) { & subst 'P:' $work | Out-Null }
    if (-not (Test-Path 'P:\')) { Die "Failed to mount P:\ to $work" }
    Ok "P:\ mounted -> $work"
}
function EnsureJunction ($link, $target, $label) {
    if (-not (Test-Path $target)) { Die "$label source missing: $target" }
    if (Test-Path $link) {
        $it = Get-Item $link -Force
        if ($it.LinkType -eq 'Junction') {
            $cur = @($it.Target)[0]
            if ((Resolve-Path $cur).Path -ieq (Resolve-Path $target).Path) { Ok "$label ready"; return }
            Warn "$label points elsewhere; recreating"
            (Get-Item $link -Force).Delete()
        } else { Die "$label ($link) is a real folder, not a junction. Move it aside." }
    }
    New-Item -ItemType Junction -Path $link -Target $target | Out-Null
    Ok "$label linked"
}
function ServerCfg {
    $shard = ($Instance + 'shard').Substring(0, 6).ToLower()
    @"
hostname = "Local Test ($Instance)";
password = "";
passwordAdmin = "";
description = "Colorful-UI local diag test instance";
maxPlayers = 4;
verifySignatures = 0;
forceSameBuild = 0;
allowFilePatching = 1;
disable3rdPerson = 0;
disableCrosshair = 0;
serverTimeAcceleration = 12;
instanceId = 1;
storageAutoFix = 1;
persistent = 0;
shardId = "$shard";
steamQueryPort = $QueryPort;
class Missions
{
    class DayZ
    {
        template = "$Template";
    };
};
"@
}
function StageServer {
    $srvRoot = Join-Path $RepoRoot '.server'
    $instDir = Join-Path $srvRoot $Instance
    $profiles = Join-Path $instDir 'Profiles'
    $clientLogs = Join-Path $srvRoot '!ClientDiagLogs'
    New-Item -ItemType Directory -Force -Path $profiles, $clientLogs | Out-Null
    $cfg = Join-Path $instDir 'serverDZ.cfg'
    if (-not (Test-Path $cfg)) { ServerCfg | Set-Content -Path $cfg -Encoding utf8; Ok 'serverDZ.cfg created' }
    else {
        $changed = $false
        if (-not (Select-String -Path $cfg -Pattern 'allowFilePatching' -Quiet)) { Add-Content -Path $cfg -Value "`nallowFilePatching = 1;" -Encoding utf8; $changed = $true }
        if (-not (Select-String -Path $cfg -Pattern 'steamQueryPort' -Quiet)) { Add-Content -Path $cfg -Value "`nsteamQueryPort = $QueryPort;" -Encoding utf8; $changed = $true }
        if ($changed) { Ok 'serverDZ.cfg updated' } else { Ok 'serverDZ.cfg ready' }
    }
    # Mission: one real, independent copy of the map's mission folder, named
    # after the template. Copied from the DayZ Server install so a Steam update
    # to the vanilla files never overwrites our edits. No junction, no mission/.
    $missionDir = Join-Path $instDir $Template
    if (-not ((Test-Path $missionDir) -and (Get-ChildItem $missionDir -Force -ErrorAction SilentlyContinue))) {
        $srv = DayZServer
        if (-not $srv) { Die 'DayZ Server is not installed. Install it from Steam (free) so the Chernarus mission can be copied, then try again.' }
        $src = Join-Path $srv "mpmissions\$Template"
        if (-not (Test-Path $src)) { Die "Mission '$Template' not found at $src" }
        Info 'Copying the Chernarus mission (first run only)...'
        Copy-Item $src $missionDir -Recurse -Force
        Ok 'Mission copied'
    } else { Ok 'Mission ready' }
}
function Prep {
    MountP
    EnsureJunction 'P:\Mods' (Join-Path (DayZGame) '!Workshop') 'P:\Mods'
    EnsureJunction "P:\$Mod" (Join-Path $RepoRoot $ModSrc) "P:\$Mod"
}
function BuildMod ($Name) {
    $t = DayZTools
    if (-not $t) { Die 'DayZ Tools not found. Install it from Steam (free).' }
    $ab = Join-Path $t 'Bin\AddonBuilder\AddonBuilder.exe'
    $target = "P:\Mods\@$Name\Addons"
    $temp = "P:\temp\$Name"
    New-Item -ItemType Directory -Force -Path $target, $temp | Out-Null
    $inc = Join-Path $env:TEMP "cui_inc_$PID.lst"
    Set-Content -Path $inc -Value '*.c;*.h;*.hpp;*.cpp;*.bin;*.xml;*.json;*.layout;*.style;*.styles;*.p3d;*.paa;*.edds;*.rvmat;*.bisurf;*.anm;*.rtm;*.ogg;*.wss;*.wrp;*.txt;*.csv;*.fnt;*.imageset;*.tga;*.png;*.fsm;*.sqf;*.ext;*.material;*.fxy;*.ptc;*.emat;*.mov' -Encoding ascii
    Info "Building $Name ..."
    $p = Start-Process $ab -ArgumentList "P:\$Name", $target, "-prefix=$Name", "-temp=$temp", "-include=$inc" -Wait -NoNewWindow -PassThru
    Remove-Item $inc -Force -ErrorAction SilentlyContinue
    $pbo = Join-Path $target "$Name.pbo"
    if ($p.ExitCode -ne 0) { Die "AddonBuilder failed (code $($p.ExitCode))." }
    if (-not (Test-Path $pbo)) { Die 'Build produced no PBO. Check the output above.' }
    Ok ("Built $Name ({0:N0} bytes)" -f (Get-Item $pbo).Length)
    try { Remove-Item $temp -Recurse -Force } catch {}
}
function QuotedArg ($name, $value) {
    # Start-Process joins ArgumentList items into one native command line.
    # DayZ parses the raw command line itself, so quote the entire argument
    # ("-profiles=C:\Path With Spaces"), not just its value.
    return "`"$name=$value`""
}
function StartSession {
    $diag = DayZDiag
    if (-not $diag) { Die 'DayZDiag_x64.exe not found. Make sure DayZ is installed and updated.' }
    $dayzDir = Split-Path $diag -Parent
    $launchMods = @($ExtraMods) + $Mod
    foreach ($m in $launchMods) {
        if (-not (Test-Path "P:\Mods\@$m\Addons")) {
            if ($m -eq $Mod) { Die "$Mod is not built yet. Use 'Build and Run Server.bat', or run 'Build Mods.bat' first." }
            Die "Mod '@$m' is not in P:\Mods. Subscribe to it in Steam and let the DayZ Launcher download it, or remove it from `$ExtraMods at the top of this file."
        }
    }
    $modArg = ($launchMods | ForEach-Object { "P:\Mods\@$_" }) -join ';'
    $instDir = Join-Path $RepoRoot ".server\$Instance"
    $cfg = Join-Path $instDir 'serverDZ.cfg'
    $profiles = Join-Path $instDir 'Profiles'
    $mission = Join-Path $instDir $Template
    $clientLogs = Join-Path $RepoRoot '.server\!ClientDiagLogs'
    Info 'Starting server ...'
    $serverArgs = @(
        '-server'
        (QuotedArg '-config' $cfg)
        (QuotedArg '-profiles' $profiles)
        (QuotedArg '-mission' $mission)
        (QuotedArg '-mod' $modArg)
        '-filePatching'
        "-port=$Port"
    )
    $srv = Start-Process $diag -WorkingDirectory $dayzDir -ArgumentList $serverArgs -PassThru
    Ok "Server PID $($srv.Id)"
    Info 'Waiting for the server to come up (up to 180s) ...'
    $ready = $false
    $deadline = (Get-Date).AddSeconds(180)
    while ((Get-Date) -lt $deadline) {
        if ($srv.HasExited) { Die "Server closed unexpectedly (code $($srv.ExitCode)). Check the RPT in $profiles." }
        try { $u = [System.Net.Sockets.UdpClient]::new($Port); $u.Close() } catch { $ready = $true; break }
        Start-Sleep 2
    }
    if (-not $ready) { Die 'Server did not come up in time.' }
    Ok "Server is up (port $Port)"
    Info 'Starting client ...'
    $clientArgs = @(
        (QuotedArg '-profiles' $clientLogs)
        (QuotedArg '-mod' $modArg)
        '-connect=127.0.0.1'
        "-port=$Port"
        '-filePatching'
        '-window'
        '-x=1920'
        '-y=1080'
    )
    $cli = Start-Process $diag -WorkingDirectory $dayzDir -ArgumentList $clientArgs -PassThru
    Ok "Client PID $($cli.Id)"
    Say ''
    Ok "Colorful UI is running. Close the game windows or run 'Stop Server.bat' to stop."
}
# DayZ always writes its logs (RPT / .log / .ADM / .mdmp) straight into the
# -profiles folder; there is no launch flag to redirect them. So after every
# stop/kill we sweep them into a Logs\ subfolder to keep Profiles\ tidy.
# The log of a session that is still running stays put until the next run/stop.
function SweepLogs {
    $dirs = @((Join-Path $RepoRoot ".server\$Instance\Profiles"),
              (Join-Path $RepoRoot '.server\!ClientDiagLogs'))
    foreach ($d in $dirs) {
        if (-not (Test-Path $d)) { continue }
        $logs = Join-Path $d 'Logs'
        New-Item -ItemType Directory -Force -Path $logs | Out-Null
        $files = @(Get-ChildItem -LiteralPath $d -File | Where-Object { $_.Extension -in '.RPT','.log','.ADM','.mdmp' })
        $moved = 0
        foreach ($f in $files) {
            try { Move-Item -LiteralPath $f.FullName -Destination $logs -Force; $moved++ } catch { }
        }
        if ($moved) { Info "Moved $moved log file(s) into $logs" }
    }
}
# Only Colorful UI's own DayZDiag processes: both the server and the client are
# launched with -profiles pointing inside this repo's .server\ folder, so that
# path on the command line is the fingerprint. Other projects' local servers
# (DayZ Deluxe, The Vale, The Lab, ...) running alongside are left alone.
function OurProcs {
    $mark = (Join-Path $RepoRoot '.server').ToLower()
    Get-CimInstance Win32_Process -Filter "name='DayZDiag_x64.exe'" -ErrorAction SilentlyContinue |
        Where-Object { $_.CommandLine -and $_.CommandLine.ToLower().Contains($mark) }
}
function StopSession {
    $ps = @(OurProcs)
    if (-not $ps) { Ok 'No Colorful UI server is running.'; SweepLogs; return }
    $ps | ForEach-Object { Stop-Process -Id $_.ProcessId -Force -ErrorAction SilentlyContinue }
    Ok 'Stopped the Colorful UI server and client.'
    Start-Sleep 2   # let the game release its log file handles
    SweepLogs
}
# Kill any leftover server/client before we build or launch, so a stale process
# can't hold the game port (causing "server unreachable") or lock the PBO file.
function KillStale {
    $ps = @(OurProcs)
    if ($ps) {
        $ps | ForEach-Object { Stop-Process -Id $_.ProcessId -Force -ErrorAction SilentlyContinue }
        Info 'Closed a previous Colorful UI server/client session first.'
        Start-Sleep 2   # let the game port and PBO file handles release
    }
    SweepLogs
}

Say ''
Say "==================  COLORFUL UI - $Action  ==================" 'Cyan'
try {
    switch ($Action) {
        'stop'     { StopSession }
        'build'    { KillStale; Prep; BuildMod $Mod }
        'run'      { KillStale; Prep; StageServer; StartSession }
        'buildrun' { KillStale; Prep; BuildMod $Mod; StageServer; StartSession }
        default    { Die "Unknown action '$Action'" }
    }
} catch {
    Say ''
    Say "  [FAIL] $($_.Exception.Message)" 'Red'
    exit 1
}
