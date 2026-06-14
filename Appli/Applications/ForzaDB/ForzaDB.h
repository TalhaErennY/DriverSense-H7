/*
 * ForzaDB.h
 *
 *  Created on: 11 Haz 2026
 *      Author: talha
 */

#ifndef FORZADB_FORZADB_H_
#define FORZADB_FORZADB_H_

#include <stdint.h>

/* -------------------------------------------------------------------------- */
/* Telemetry Data Out Configuration                                           */
/* -------------------------------------------------------------------------- */
/*
 * UDP Port
 *
 * Forza Data Out Port should be set to this port.
 */
#define FORZA_UDP_DST_PORT				5005U

/* -------------------------------------------------------------------------- */
/* Oled Dashboard															  */
/* -------------------------------------------------------------------------- */


/* -------------------------------------------------------------------------- */
/* Telemetry Format															  */
/* -------------------------------------------------------------------------- */
/*
 * Forza UDP Packet Size
 */
#define FORZA_PACKET_SIZE  324U

/*
 * Forza UDP Packet Selection
 */
typedef struct __attribute__((packed)){
    int32_t  IsRaceOn;
    uint32_t TimestampMS;

    float EngineMaxRpm;
    float EngineIdleRpm;
    float CurrentEngineRpm;

    float AccelerationX;
    float AccelerationY;
    float AccelerationZ;

    float VelocityX;
    float VelocityY;
    float VelocityZ;

    float AngularVelocityX;
    float AngularVelocityY;
    float AngularVelocityZ;

    float Yaw;
    float Pitch;
    float Roll;

    float NormalizedSuspensionTravelFrontLeft;
    float NormalizedSuspensionTravelFrontRight;
    float NormalizedSuspensionTravelRearLeft;
    float NormalizedSuspensionTravelRearRight;

    float TireSlipRatioFrontLeft;
    float TireSlipRatioFrontRight;
    float TireSlipRatioRearLeft;
    float TireSlipRatioRearRight;

    float WheelRotationSpeedFrontLeft;
    float WheelRotationSpeedFrontRight;
    float WheelRotationSpeedRearLeft;
    float WheelRotationSpeedRearRight;

    int32_t WheelOnRumbleStripFrontLeft;
    int32_t WheelOnRumbleStripFrontRight;
    int32_t WheelOnRumbleStripRearLeft;
    int32_t WheelOnRumbleStripRearRight;

    int32_t WheelInPuddleFrontLeft;
    int32_t WheelInPuddleFrontRight;
    int32_t WheelInPuddleRearLeft;
    int32_t WheelInPuddleRearRight;

    float SurfaceRumbleFrontLeft;
    float SurfaceRumbleFrontRight;
    float SurfaceRumbleRearLeft;
    float SurfaceRumbleRearRight;

    float TireSlipAngleFrontLeft;
    float TireSlipAngleFrontRight;
    float TireSlipAngleRearLeft;
    float TireSlipAngleRearRight;

    float TireCombinedSlipFrontLeft;
    float TireCombinedSlipFrontRight;
    float TireCombinedSlipRearLeft;
    float TireCombinedSlipRearRight;

    float SuspensionTravelMetersFrontLeft;
    float SuspensionTravelMetersFrontRight;
    float SuspensionTravelMetersRearLeft;
    float SuspensionTravelMetersRearRight;

    int32_t CarOrdinal;
    int32_t CarClass;
    int32_t CarPerformanceIndex;
    int32_t DrivetrainType;
    int32_t NumCylinders;

    uint32_t CarGroup;

    float SmashableVelDiff;
    float SmashableMass;

    float PositionX;
    float PositionY;
    float PositionZ;

    float Speed;
    float Power;
    float Torque;

    float TireTempFrontLeft;
    float TireTempFrontRight;
    float TireTempRearLeft;
    float TireTempRearRight;

    float Boost;
    float Fuel;
    float DistanceTraveled;

    float BestLap;
    float LastLap;
    float CurrentLap;
    float CurrentRaceTime;

    uint16_t LapNumber;

    uint8_t RacePosition;

    uint8_t Accel;
    uint8_t Brake;
    uint8_t Clutch;
    uint8_t HandBrake;

    int8_t Gear;

    int8_t Steer;
    int8_t NormalizedDrivingLine;
    int8_t NormalizedAIBrakeDifference;

    uint8_t Reserved;   /* Packet is 324 bytes. Listed fields are 323 bytes. */

} ForzaHorizonPacket_t;

#endif /* FORZADB_FORZADB_H_ */
