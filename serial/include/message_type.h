/*
 *  QbotSI SDK
 *
 *  Copyright (c) 2016 - 2018 Beijing qfeeltech Co., Ltd.
 *  http://www.qfeeltech.com/
 *
 */
/*
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#ifndef MESSAGE_TYPE_H_
#define MESSAGE_TYPE_H_

#include <vector>
#include <stdint.h>

#define MaxNumOfSensor (10)

typedef union {
	struct {
		uint8_t byte[4];
	} _BYTE_t;
	float value;
} _FLOAT_t;

typedef union {
	struct {
		uint8_t byte[4];
	} _BYTE_t;
	int value;
} _INT32_t;

typedef struct __serial_battery_voltage {
	__serial_battery_voltage() : num(0), 	\
			value(MaxNumOfSensor, 0.0) {}
	int num;
	std::vector<float> value;	
} Battery_voltage_t;

typedef struct __serial_infrared {
	__serial_infrared() : num(0), 	\
			value(MaxNumOfSensor, 0) {} 
	int num;
	std::vector<int> value;	
} Infrared_t;

typedef struct __serial_motor_current {
	__serial_motor_current() : num(0),	\
			value(MaxNumOfSensor, 0.0) {} 
	int num;
	std::vector<float> value;
} Current_t;

typedef struct __serial_ultrasound {
	__serial_ultrasound() : num(0),	\
			value(MaxNumOfSensor, 0.0) {}
	int num;
	std::vector<float> value;
} Ultrasound_t;

typedef struct __serial_encoder {
	__serial_encoder() : num(0),	\
			value(MaxNumOfSensor, 0) {}

	int num;
	std::vector<int> value;
} Encoder_t;

typedef struct __serial_imu {
	__serial_imu() : num(0), timestamp(0),	\
			yaw(MaxNumOfSensor, 0.0),		\
			pitch(MaxNumOfSensor, 0.0),		\
			roll(MaxNumOfSensor, 0.0) {}
	int num;
	std::vector<float> yaw;
	std::vector<float> pitch;
	std::vector<float> roll;	
	int timestamp;
} YPR_t;

typedef struct __serial_imu_raw {
	__serial_imu_raw() : num(0), timestamp(0),	\
			acce(MaxNumOfSensor, std::vector<int16_t>(3, 0)),	\
			gyro(MaxNumOfSensor, std::vector<int16_t>(3, 0)),	\
			mag(MaxNumOfSensor, std::vector<int16_t>(3, 0)),	\
			barometer(MaxNumOfSensor, 0.0) {}
	int num;
    std::vector<std::vector<int16_t> > acce;
	std::vector<std::vector<int16_t> > gyro;
	std::vector<std::vector<int16_t> > mag;
    std::vector<float> barometer;
	int timestamp;
} ImuRawData_t;

/***********************************************************
 * user interface message struct
 **********************************************************/
typedef struct __FeedbackPackets_t {
	// Feedback packets
	Battery_voltage_t	battery_voltage;
	Infrared_t			infrared;
	Current_t			current;
	Ultrasound_t		ultrasound;
	Encoder_t			encoder;
	// Imu feedback packets
	YPR_t			ypr;
	ImuRawData_t	imuRawData;	
} FeedbackPackets_t; 

#endif
