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


#ifndef SERIAL_INTERFACE_H_
#define SERIAL_INTERFACE_H_

#include "message_type.h"

#include <math.h>
#include <string.h>
#include <unistd.h>  // UNIX standard function definitions
#include <fcntl.h>   // File control definitions
#include <pthread.h> // This uses POSIX Threads
#include <stdlib.h>
#include "serial_port.h"
#include <fstream>
#include <iostream>
using namespace std;

#define DISABLE_T_ANGLE

#ifndef M_PI
#define M_PI	(3.1415926535898)
#endif

#ifndef ABS
#define ABS(x)	((x) > 0 ? (x) : (-x))
#endif

#define SERIAL_HEADER00	(0xAA)
#define SERIAL_HEADER01	(0x55)

#define MAX_LINEAR_SPEED	(1.8)
#define MAX_ANGULAR_SPEED	(0.8)
#define MAX_PERCENTAGE          (70)
#define MAX_P_ANGLE		(180)
#define MAX_T_ANGLE		(170)

#define b_uart_head		(0x80)
#define b_rx_over		(0x40)
#define BUFFER_SIZE 	(1024)

#define RADIANS2DEGREES(x)	((x) / M_PI * 180.0)
#define DEGREES2RADIANS(x)	((x) / 180.0 * M_PI)

void* startReadThreadForInterface(void *args);
void* startWriteThreadForInterface(void *args);

/***********************************************************
 * class: Serial_Interface
 **********************************************************/
class Serial_Interface
{
public:
	Serial_Interface();
	~Serial_Interface();
	
			
	/***********************************************************
	 * function: setMotorPower(int state) 
	 * Input: state  
	 *			0 - off 
	 *			1 - on
	 * Output:
	 **********************************************************/
	void setMotorPower(int state);
	/***********************************************************
	 * function: init() 
	 * Input: 
	 * Output:
	 **********************************************************/
	void init();
	/***********************************************************
	 * function: setMotorDistance(float linear, float angular) 
	 * Input: linear speed and angular distance  
	 * Output: 
	 **********************************************************/
	void setMotorDistance(float linear, float angular);
	/***********************************************************
	 * function: setMotorSpeed(float linear, float angular) 
	 * Input: linear speed and angular speed  
	 * Output: 
	 **********************************************************/
	void setMotorSpeed(float linear, float angular);
	
	/***********************************************************
	 * function: setMecanumMotorSpeed(float x, float y, float z) 
	 * Input: linear x speed, linear y speed and angular speed  
	 * Output: 
	 **********************************************************/
	void setMecanumMotorSpeed(float x, float y, float z); 

	/***********************************************************
	 * function: setLifterPercentage(int percentage) 
	 * Input: Percentage of the height relative to the 
	 *			bottom (from 0 to 100) 
	 * Output: 
	 **********************************************************/
	void setLifterPercentage(int percentage);
	
	/***********************************************************
	* function: stopLifter(void) 
	* Input:   
	* Output: 
	**********************************************************/
	void stopLifter(void);
	
	/***********************************************************
	 * function: setPTZ(int angle) 
	 * Input: PTZ rotation angle	(from 0 to 180) 
	 * Output: 
	 **********************************************************/
	void setPTZ(int P_angle, int T_angle);
	
	/***********************************************************
	 * function: getBatteryVoltage()
	 * Input:  
	 * Output:	Battery Voltage data	(type: std::vector<int>)
	 *			("vector<int>".size) battery voltage data
	 *			unit: v
	 **********************************************************/
	std::vector<int> getBatteryVoltage() const {
		int i = 0;
		std::vector<int> voltage;
		voltage.resize(feedbackPackets.battery_voltage.num);
		for(i = 0; i < feedbackPackets.battery_voltage.num; i++) {
			voltage[i] = feedbackPackets.battery_voltage.value[i];
		}
		return voltage;
	};
	/***********************************************************
	 * function: getInfrared()
	 * Input:  
	 * Output:	infrared data	(type: std::vector<int>)
	 *			("vector<int>".size) infrared data
	 *			unit: 
	 **********************************************************/
	std::vector<int> getInfrared() const {
		int i = 0;
		std::vector<int> infrared;
		infrared.resize(feedbackPackets.infrared.num);
		for(i = 0; i < feedbackPackets.infrared.num; i++) {
			infrared[i] = feedbackPackets.infrared.value[i];
		}
		return infrared;
	};
	/***********************************************************
	 * function: getUltrasound()
	 * Input:  
	 * Output:	ultrasound data	(type: std::vector<floar>)
	 *			("vector<float>".size) ultrasound data
	 *			unit: m
	 **********************************************************/	
	std::vector<float> getUltrasound() const {
		int i = 0;
		std::vector<float> ultrasound;
		ultrasound.resize(feedbackPackets.ultrasound.num);
		for(i = 0; i < feedbackPackets.ultrasound.num; i++) {
			ultrasound[i] = feedbackPackets.ultrasound.value[i];
		}
		return ultrasound;
	};

	/***********************************************************
	 * function: getEncoder()
	 * Input:  
	 * Output:	encoder data	(type: std::vector<int>)
	 *			("vector<int>".size) encoder data
	 **********************************************************/
	std::vector<int> getEncoder() const {
		//ofstream ff;
		//ff.open("c.txt", ios::app);
		int i = 0;
		std::vector<int> encoder;
		encoder.resize(feedbackPackets.encoder.num);
int a, b;
		for(i = 0; i < feedbackPackets.encoder.num; i++) {
			encoder[i] = feedbackPackets.encoder.value[i];
			if(i == 0) a = encoder[0];
			if(i == 1) b = encoder[1];
		}
//ff << a << ", " << b << endl;
		return encoder;
	};

	/***********************************************************
	 * function: getYPR()
	 * Input:  
	 * Output:	yaw, pitch and roll data	(type: std::vector<float>)
	 *			0 	=> yaw
	 *			1 	=> pitch
	 *			2 	=> roll
	 *			3  	=> timestamp
	 **********************************************************/
	std::vector<float> getYPR() const {
		std::vector<float> YPR;
		YPR.resize(4);
		YPR[0] = feedbackPackets.ypr.yaw[0];
		YPR[1] = feedbackPackets.ypr.pitch[0];
		YPR[2] = feedbackPackets.ypr.roll[0];
		YPR[3] = feedbackPackets.ypr.timestamp;
		return YPR;
	};

	/***********************************************************
	 * function: getImuRawData()
	 * Input:  
	 * Output:	imu raw data	(type: std::vector<float>)
	 *			0 ~ 2 	=> accex, accey, accez
	 *			3 ~ 5 	=> gyrox, gyroy, gyroz
	 *			6 ~ 8 	=> magx, magy, magz
	 *			9  		=> timestamp
	 **********************************************************/
	std::vector<float> getImuRawData() const {
		int i = 0;
		std::vector<float> ImuRawData;
		ImuRawData.resize(10);
		for ( i = 0; i < 3; i++) {
			ImuRawData[i] 		= feedbackPackets.imuRawData.acce[0][i];
			ImuRawData[i + 3] 	= feedbackPackets.imuRawData.gyro[0][i];
			ImuRawData[i + 6] 	= feedbackPackets.imuRawData.mag[0][i];
		}
		ImuRawData[9] = feedbackPackets.imuRawData.timestamp;
		return ImuRawData;
	};

	/***********************************************************
	 * function: start(char*& serialName_, int blot_)
	 * description: start serial port, read thread and write thread
	 * Input:
	 *			int serialName  => ex: "ttyUSB0" -> serialName = "ttyUSB0"
	 *			int blot		=> serial blot -> 9600, 115200 ....
	 * Output:	the start statue	=> true or false
	 ***********************************************************/
	bool start(char*& serialName_, int blot_) {

		running = true;
		tx_wr_index = 0;
		m_serial_port->setSerialName(serialName_);
		m_serial_port->setSerialBlot(blot_);
		m_serial_port->open_serial();
		if ( !m_serial_port->status) {
			printf("[Start Serial]: Failed\n");
			return false;
		}
		printf("[Start Serial]: OK\n");
		if (startReadThread() < 0 ) {
			printf("[Start Read Thread]: Error\n");
			return false;
		}
		printf("[Start Read Thread]: OK\n");
		/*
			if (startWriteThread() < 0 ) {
			printf("[Start Write Thread]: Error\n");
			return false;
		}
		
		printf("[Start Write Thread]: OK\n");
		*/		
		return true;
	};
	
	/***********************************************************
	 * function: stop()
	 * description: stop serial port, read thread and write thread
	 * Input:
	 * Output:
	 ***********************************************************/
	void stop() {
		running = false;
		sleep(4000);
		m_serial_port->handle_quit();
		return;
	};

	friend void* startReadThreadForInterface(void *args);
	friend void* startWriteThreadForInterface(void *args);

private:
	bool received_all;
	bool header00_flag;
	bool header01_flag;
	bool data_flag;
	bool base_info_done;
	bool imu_info_done;

	bool running;

	uint8_t RC_Flag;
	uint8_t checksum;
	uint8_t rx_wr_index;
	uint8_t tx_wr_index;
	uint8_t rx_buffer[BUFFER_SIZE];
	uint8_t tx_buffer[BUFFER_SIZE];

	FeedbackPackets_t	feedbackPackets;

	Serial_Port			*m_serial_port;
	pthread_t 			m_readTid;
	pthread_t 			m_writeTid;
	pthread_mutex_t 	m_serialRxLock;
	pthread_mutex_t 	m_serialTxLock;

	int decode_messages(uint8_t message);
	int startReadThread();
	int startWriteThread();

	uint8_t Sum_check(void){
		uint8_t i;
		checksum = 0;
		for (i = 0; i < rx_buffer[0] + 2; i++)
			checksum ^= rx_buffer[i];
        return !checksum;
	}
	
	uint8_t get_Sum_check(int index_start, int offset){
		uint8_t i;
		checksum = 0;
		for (i = 2 + index_start; i <= index_start+offset; i++)
			checksum ^= tx_buffer[i];
		return checksum;
	}
};


#endif /* SERIAL_INTERFACE_H_ */
