/******************************************************************
  @file       ReefwingAHRS.h
  @brief      Attitude and Heading Reference System (AHRS) for the 
              Arduino Nano 33 BLE and XIAO Sense
  @author     David Such
  @copyright  Please see the accompanying LICENSE file.

  Code:        David Such
  Version:     2.2.0
  Date:        10/02/23

  1.0.0 Original Release.                         22/02/22
  1.1.0 Added NONE fusion option.                 25/05/22
  2.0.0 Changed Repo & Branding                   15/12/22
  2.0.1 Invert Gyro Values PR                     24/12/22
  2.1.0 Updated Fusion Library                    30/12/22
  2.2.0 Add support for Nano 33 BLE Sense Rev. 2  10/02/23

  Credits: - The C++ code for our quaternion position update 
             using the Madgwick Filter is based on the paper, 
             "An efficient orientation filter for inertial and 
             inertial/magnetic sensor arrays" written by Sebastian 
             O.H. Madgwick in April 30, 2010.
           - Fusion Library is based on the revised AHRS algorithm 
             presented in chapter 7 of Madgwick's PhD thesis.
             ref: https://github.com/xioTechnologies/Fusion
         
******************************************************************/

#ifndef ReefwingAHRS_h
#define ReefwingAHRS_h

#include <Arduino.h>
#include <Reefwing_imuTypes.h>

#include "Fusion.h"

/******************************************************************
 *
 * Definitions - 
 * 
 ******************************************************************/

#define DEG_TO_RAD 0.01745329251994   //  PI/180 * degrees
#define RAD_TO_DEG 57.2957795130823   //  180/PI * radians

/******************************************************************
 *
 * ENUM Class & Struct Definitions - 
 * 
 ******************************************************************/

enum class BoardType {
  NANO33BLE = 0,
  NANO33BLE_SENSE_R1,
  NANO33BLE_SENSE_R2,
  XIAO_SENSE,
  NOT_DEFINED
};

enum class ImuType {
  LSM9DS1 = 0,
  LSM6DS3,
  BMI270_BMM150,
  UNKNOWN
};

enum class SensorFusion { // Sensor fusion algorithm options
  MADGWICK = 0,
  MAHONY,
  COMPLEMENTARY,
  FUSION,
  CLASSIC,
  NONE
};

/******************************************************************
 *
 * ReefwingAHRS Class Definition - 
 * 
 ******************************************************************/

class ReefwingAHRS {
  public:
    ReefwingAHRS();

    void begin();
    void update();

    void setFusionAlgorithm(SensorFusion algo);
    void setAlpha(float a);
    void setBeta(float b);
    void setGyroMeasError(float gme);
    void setKp(float p);
    void setKi(float i);
    void setFusionGain(float g);
    void setDeclination(float dec);
    void setData(SensorData d);
    void setImuType(ImuType i);
    void setBoardType(BoardType b);

    void updateEulerAngles(float deltaT);
    void classicUpdate();
    void tiltCompensatedYaw();
    void madgwickUpdate(SensorData d, float deltaT); 
    void mahoneyUpdate(SensorData d, float deltaT);
    void complementaryUpdate(SensorData d, float deltaT);
    
    SensorData filterFormat();
    BoardType getBoardType();
    const char* getBoardTypeString();

    void formatAnglesForConfigurator();
    Quaternion getQuaternion();
    EulerAngles angles, configAngles;

  private:
    long _lastUpdate;
    float _declination;
    float _gyroMeasError, _alpha, _beta, _Kp, _Ki;   //  Sensor Fusion free parameters
    float _eInt[3] = {0.0f, 0.0f, 0.0f};             //  Vector to hold integral error for Mahony filter
    float _att[4] = {1.0f, 0.0f, 0.0f, 0.0f};        //  Attitude quaternion for complementary filter

    ImuType _imuType;
    BoardType _boardType;
    SensorFusion _fusion;
    
    SensorData _data;   //  Sensor Data: gyro - DPS, accel - g's, mag - gauss
    Quaternion _q;      //  Quaternion used for AHRS update

    const char* _boardTypeStr[5];

};

#endif