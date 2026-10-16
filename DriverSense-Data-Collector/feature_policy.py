"""Explicit non-leaky feature policy for future model training.

The full raw and 25 Hz CSVs are ALWAYS retained. The curated train CSV omits
fields reserved for later validation of terrain / impact hypotheses.
"""
from telemetry.forza_parser import FIELD_NAMES

VALIDATION_EXACT = {
    'SmashableVelDiff', 'SmashableMass', 'CarGroup',
    'CarOrdinal', 'CarClass', 'CarPerformanceIndex', 'DrivetrainType',
    'NumCylinders', 'PositionX', 'PositionY', 'PositionZ',
    'IsRaceOn', 'TimestampMS', 'ReservedTrailingByte',
    'NormalizedDrivingLine', 'NormalizedAIBrakeDifference',
    'RacePosition', 'LapNumber', 'CurrentLap', 'BestLap', 'LastLap',
    'CurrentRaceTime', 'DistanceTraveled',
}
VALIDATION_PREFIXES = ('WheelInPuddle', 'WheelOnRumbleStrip', 'SurfaceRumble')

VERIFICATION_ONLY = [name for name in FIELD_NAMES if name in VALIDATION_EXACT or name.startswith(VALIDATION_PREFIXES)]
TRAINING_FEATURES = [name for name in FIELD_NAMES if name not in VERIFICATION_ONLY]
TRAINING_COLUMNS = ['ElapsedSec', 'Valid', 'PacketAgeMs', *TRAINING_FEATURES]

assert len(set(VERIFICATION_ONLY) & set(TRAINING_FEATURES)) == 0
assert set(VERIFICATION_ONLY) | set(TRAINING_FEATURES) == set(FIELD_NAMES)

POLICY = {
    'version': '1.3',
    'target': 'metadata.json:label',
    'training_file': 'train_25hz.csv',
    'training_features': TRAINING_FEATURES,
    'verification_only_features': VERIFICATION_ONLY,
    'full_telemetry_files': ['raw_packets.csv', 'samples_25hz.csv'],
    'vehicle_profile_role': 'verification/grouped evaluation only; not part of training CSV',
    'rule': 'Never train on verification-only fields, including via feature derivation, preprocessing, automated feature selection, or model selection. Do not automatically label terrain/crashes from validation features.',
    'notes': 'Labels are user-supplied scenarios, not ground truth for anomalous events. Slip and dynamics remain potential predictors; puddle, rumble and smashable contact are held out as corroborating evidence, not guaranteed ground-truth labels.'
}
