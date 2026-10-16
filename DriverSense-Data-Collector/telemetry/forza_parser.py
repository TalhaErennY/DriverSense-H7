
import struct
import math

PACKET_SIZE = 324

# Python struct data types:
# i = signed int32
# I = unsigned int32
# f = float32
# H = unsigned int16
# B = unsigned int8
# b = signed int8

FIELDS = [
    ("IsRaceOn", "i"),
    ("TimestampMS", "I"),

    ("EngineMaxRpm", "f"),
    ("EngineIdleRpm", "f"),
    ("CurrentEngineRpm", "f"),

    ("AccelerationX", "f"),
    ("AccelerationY", "f"),
    ("AccelerationZ", "f"),

    ("VelocityX", "f"),
    ("VelocityY", "f"),
    ("VelocityZ", "f"),

    ("AngularVelocityX", "f"),
    ("AngularVelocityY", "f"),
    ("AngularVelocityZ", "f"),

    ("Yaw", "f"),
    ("Pitch", "f"),
    ("Roll", "f"),
]

WHEELS = [
    "FrontLeft",
    "FrontRight",
    "RearLeft",
    "RearRight"
]

WHEEL_GROUPS = [
    "NormalizedSuspensionTravel",
    "TireSlipRatio",
    "WheelRotationSpeed",
    "WheelOnRumbleStrip",
    "WheelInPuddle",
    "SurfaceRumble",
    "TireSlipAngle",
    "TireCombinedSlip",
    "SuspensionTravelMeters"
]

# Each group contains four consecutive float32 values.
for group in WHEEL_GROUPS:
    for wheel in WHEELS:
        dtype = "i" if group in ("WheelOnRumbleStrip", "WheelInPuddle") else "f"
        FIELDS.append((group + wheel, dtype))

# Vehicle identification
FIELDS.extend([
    ("CarOrdinal", "i"),
    ("CarClass", "i"),
    ("CarPerformanceIndex", "i"),
    ("DrivetrainType", "i"),
    ("NumCylinders", "i"),

    # FH6-specific block (12 bytes)
    ("CarGroup", "I"),
    ("SmashableVelDiff", "f"),
    ("SmashableMass", "f"),

    ("PositionX", "f"),
    ("PositionY", "f"),
    ("PositionZ", "f"),

    ("Speed", "f"),
    ("Power", "f"),
    ("Torque", "f"),
])

for wheel in WHEELS:
    FIELDS.append(("TireTemp" + wheel, "f"))

FIELDS.extend([
    ("Boost", "f"),
    ("Fuel", "f"),
    ("DistanceTraveled", "f"),

    ("BestLap", "f"),
    ("LastLap", "f"),
    ("CurrentLap", "f"),
    ("CurrentRaceTime", "f"),

    ("LapNumber", "H"),

    ("RacePosition", "B"),
    ("Accel", "B"),
    ("Brake", "B"),
    ("Clutch", "B"),
    ("HandBrake", "B"),
    ("Gear", "B"),

    ("Steer", "b"),
    ("NormalizedDrivingLine", "b"),
    ("NormalizedAIBrakeDifference", "b"),

    # The documented field list totals 323 bytes; reserve the final byte
    # without interpreting it as telemetry.
    ("ReservedTrailingByte", "B"),
])

# Build one binary decoder for the entire packet.
PACKET_STRUCT = struct.Struct(
    "<" + "".join(data_type for _, data_type in FIELDS)
)

FIELD_NAMES = [name for name, _ in FIELDS]

assert PACKET_STRUCT.size == PACKET_SIZE, (
    f"Invalid packet layout: {PACKET_STRUCT.size} bytes"
)


def parse_forza_packet(data: bytes) -> dict[str, int | float]:
    """
    Decode one 324-byte Forza Horizon Dash packet.
    Returns a dictionary of named telemetry fields.
    """

    if len(data) != PACKET_SIZE:
        raise ValueError(
            f"Expected {PACKET_SIZE} bytes, got {len(data)}"
        )

    values = PACKET_STRUCT.unpack(data)

    packet = dict(zip(FIELD_NAMES, values))

    # Reject non-finite values in essential fields.
    for field in ("Speed", "CurrentEngineRpm", "AngularVelocityY"):
        if not math.isfinite(packet[field]):
            raise ValueError(f"Invalid numeric field: {field}")

    return packet
