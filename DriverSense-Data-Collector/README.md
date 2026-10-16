# DriverSense Data Collector v1.5 — FH6 automatic vehicle metadata

## What changed
- Manual vehicle make, model, Car Type, Drive Type and Performance Class fields have been removed.
- CarOrdinal, CarClass, CarPerformanceIndex, DrivetrainType, CarGroup are read from FH6 UDP packet and saved with all raw/full samples and metadata.
- **Car Type as the game UI category (e.g. Hypercars) is NOT included in the official Data Out schema.** CarGroup is a numeric identifier; it cannot be reliably translated to the UI category without an external mapping. We do not fabricate that mapping.
- CarOrdinal=0 / IsRaceOn=0 / missing UDP data pauses recording, without consuming the 180-second active-driving target.
- On return, CarOrdinal, drivetrain, CarGroup AND world position within 25 meters of last valid recorded position must match. If not, the session is closed separately to avoid mixing vehicles or distant locations. FH6 TimestampMS is not required to continue.
- Active 25-Hz grid excludes pause time; pause events and their durations are saved in metadata.json. Invalid/no-data samples within active periods retain Valid=0; never train on these rows.

## Run
```powershell
py -3.11 -m venv .venv
.\.venv\Scripts\python.exe -m pip install -r requirements.txt
.\.venv\Scripts\python.exe main.py
```
Forza Horizon 6 > Settings > HUD and Gameplay > Data Out ON, IP 127.0.0.1, port 5005.

**Important**: GPS/world position is a safety heuristic, not absolute proof of session continuity. If menu/car selection changes position more than 25 m, recording is ended, not silently joined. If you deliberately start another drive in the same position with same car, this heuristic cannot distinguish it. Review pause events and raw timestamps before model training. CarGroup and vehicle IDs are reserved for grouping/verification per feature_policy.py.
