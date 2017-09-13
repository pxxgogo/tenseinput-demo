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

#include "serialInterface.h"

/***********************************************************
 * function: Serial_Interface() 
 * Input: 
 * Output:  
 **********************************************************/
Serial_Interface::
Serial_Interface():received_all(false), header00_flag(false),
					header01_flag(false), data_flag(false), base_info_done(false),
					imu_info_done(false), RC_Flag(0), rx_wr_index(0), checksum(0)
{
	tx_wr_index = 0;
	memset(rx_buffer, BUFFER_SIZE, 0);
	memset(tx_buffer, BUFFER_SIZE, 0);
	running = false;

	m_serial_port = new Serial_Port();
	// Start mutex
	int result = pthread_mutex_init(&m_serialRxLock, NULL);
	if ( result != 0 ) {
		printf("\n mutex init failed\n");
		throw 1;
	}
	result = pthread_mutex_init(&m_serialTxLock, NULL);
	if ( result != 0 ) {
		printf("\n mutex init failed\n");
		throw 1;
	}

}

/***********************************************************
 * function: ~Serial_Interface() 
 * Input: 
 * Output:  
 **********************************************************/
Serial_Interface::
~Serial_Interface() {
	stop();

	// destroy mutex
	pthread_mutex_destroy(&m_serialRxLock);
	pthread_mutex_destroy(&m_serialTxLock);
}

/***********************************************************
 * function: decode_messages(message) 
 * Input: message - the byte reading from serial
 * Output: 0 - Non-complete package, 
 *			1 - IMU package done, 
 *			2 - Base package done,
 *			3 - Base and IMU package all done
 **********************************************************/
int 
Serial_Interface::
decode_messages(uint8_t message)
{
	if (message == SERIAL_HEADER00) {
		header00_flag = true;
		RC_Flag |= b_uart_head;
		rx_buffer[rx_wr_index++] = message;
	} else if (message == SERIAL_HEADER01) {
		if (RC_Flag & b_uart_head) {
			header01_flag = true;
			rx_wr_index = 0;
			RC_Flag &= ~b_rx_over;
		} else {
			rx_buffer[rx_wr_index++] = message;
		}
		RC_Flag &= ~b_uart_head;
	} else {
		rx_buffer[rx_wr_index++] = message;
		RC_Flag &= ~b_uart_head;
		if (rx_wr_index == (rx_buffer[0] + 2)) {
			RC_Flag ^= b_rx_over;	
			if (Sum_check()) {	
				data_flag = true;		
				int i = 1;
				int num = 0;
                if (rx_buffer[i] == 0x10) {
                    i++;
					// decode battery_voltage (1 + 2*n) bytes
					num = rx_buffer[i++];
					feedbackPackets.battery_voltage.num = num;
					feedbackPackets.battery_voltage.value.resize(num);			
					while (num--) {
						feedbackPackets.battery_voltage.value[feedbackPackets.battery_voltage.num - num - 1] 
								= ((float)((rx_buffer[i]>>4) * 10 + (rx_buffer[i]&0x0F)))/100.0 +
									 ((rx_buffer[i+1]>>4) * 10 + (rx_buffer[i+1]&0x0F));
						i += 2; 					
					}
					// decode IR (1 + 2*n) bytes
					num = rx_buffer[i++];
					feedbackPackets.infrared.num = num;
					feedbackPackets.infrared.value.resize(num);			
					while (num--) {
						feedbackPackets.infrared.value[feedbackPackets.infrared.num - num - 1] 
								= (((uint16_t)rx_buffer[i+1] << 8) | rx_buffer[i]);
						i += 2; 					
					}
					// decode motor current (1 + 2*n) bytes
					num = rx_buffer[i++];
					feedbackPackets.current.num = num;
					feedbackPackets.current.value.resize(num);			
					while (num--) {
						feedbackPackets.current.value[feedbackPackets.current.num - num - 1] 
								= ((float)((rx_buffer[i]>>4) * 10 + (rx_buffer[i]&0x0F)))/100.0 +
									 ((rx_buffer[i+1]>>4) * 10 + (rx_buffer[i+1]&0x0F));
						i += 2; 					
					}
					// decode ultra (1 + 2*n) bytes
					num = rx_buffer[i++];
					feedbackPackets.ultrasound.num = num;			
					feedbackPackets.ultrasound.value.resize(num);
					while (num--) {
						feedbackPackets.ultrasound.value[feedbackPackets.ultrasound.num - num - 1] 
								= ((float)((rx_buffer[i]>>4) * 10 + (rx_buffer[i]&0x0F)))/100.0 +
									 ((rx_buffer[i+1]>>4) * 10 + (rx_buffer[i+1]&0x0F));
						i += 2; 					
					}
					// decode encoder (1 + 2*n) bytes
					num = rx_buffer[i++];
					feedbackPackets.encoder.num = num;					
					feedbackPackets.encoder.value.resize(num);
					while (num--) {
						feedbackPackets.encoder.value[feedbackPackets.encoder.num - num - 1] 
								= (unsigned int)rx_buffer[i+1] << 8 | rx_buffer[i];
						i += 2; 					
					}
/*ofstream ff;
ff.open("d.txt", ios::app);
ff << "one: " << feedbackPackets.encoder.value[0] <<  "two: " << feedbackPackets.encoder.value[1] <<endl;
cout << "one: " << feedbackPackets.encoder.value[0] <<  "two: " << feedbackPackets.encoder.value[1] <<endl;*/
					base_info_done = true;
				}
                if (rx_buffer[i] == 0x11) {
                    i++;
					// decode IMU raw data
					feedbackPackets.imuRawData.num = 1;		
					feedbackPackets.imuRawData.acce.resize(1);
					feedbackPackets.imuRawData.gyro.resize(1);
					feedbackPackets.imuRawData.mag.resize(1);	
                    feedbackPackets.imuRawData.acce[0][0] = (uint16_t)(rx_buffer[i+1] << 8 | rx_buffer[i]); i+=2;
                    feedbackPackets.imuRawData.acce[0][1] = (uint16_t)(rx_buffer[i+1] << 8 | rx_buffer[i]); i+=2;
                    feedbackPackets.imuRawData.acce[0][2] = (uint16_t)(rx_buffer[i+1] << 8 | rx_buffer[i]); i+=2;
                    feedbackPackets.imuRawData.gyro[0][0] = (uint16_t)(rx_buffer[i+1] << 8 | rx_buffer[i]); i+=2;
                    feedbackPackets.imuRawData.gyro[0][1] = (uint16_t)(rx_buffer[i+1] << 8 | rx_buffer[i]); i+=2;
                    feedbackPackets.imuRawData.gyro[0][2] = (uint16_t)(rx_buffer[i+1] << 8 | rx_buffer[i]); i+=2;
					feedbackPackets.imuRawData.mag[0][0] = (uint16_t)(rx_buffer[i+1] << 8 | rx_buffer[i]); i+=2;
					feedbackPackets.imuRawData.mag[0][1] = (uint16_t)(rx_buffer[i+1] << 8 | rx_buffer[i]); i+=2;
					feedbackPackets.imuRawData.mag[0][2] = (uint16_t)(rx_buffer[i+1] << 8 | rx_buffer[i]); i+=2;
				
					_FLOAT_t transF;
					transF._BYTE_t.byte[0] = rx_buffer[i];
					transF._BYTE_t.byte[1] = rx_buffer[i+1];
					transF._BYTE_t.byte[2] = rx_buffer[i+2];
					transF._BYTE_t.byte[3] = rx_buffer[i+3];			i+=4;
					feedbackPackets.imuRawData.barometer.resize(1);
					feedbackPackets.imuRawData.barometer[0] = transF.value;

					// decode IMU data
					feedbackPackets.ypr.num = 1;			
					feedbackPackets.ypr.yaw.resize(1);
					feedbackPackets.ypr.pitch.resize(1);
					feedbackPackets.ypr.roll.resize(1);	
                    feedbackPackets.ypr.yaw[0] = (int16_t)(rx_buffer[i+1] << 8 | rx_buffer[i]) * 1.0 / 10.0; i+=2;
                    feedbackPackets.ypr.pitch[0] = (int16_t)(rx_buffer[i+1] << 8 | rx_buffer[i]) * 1.0 / 100.0; i+=2;
                    feedbackPackets.ypr.roll[0] = (int16_t)(rx_buffer[i+1] << 8 | rx_buffer[i]) * 1.0 / 100.0; i+=2;
					
					_INT32_t transI;
					transI._BYTE_t.byte[0] = rx_buffer[i];
					transI._BYTE_t.byte[1] = rx_buffer[i+1];
					transI._BYTE_t.byte[2] = rx_buffer[i+2];
					transI._BYTE_t.byte[3] = rx_buffer[i+3];
					feedbackPackets.imuRawData.timestamp = transI.value;			i+=4;
					feedbackPackets.ypr.timestamp = feedbackPackets.imuRawData.timestamp;

					imu_info_done = true; 					
				}			
			}
		}
	}

	received_all = 
		header00_flag	&&
		header01_flag	&&
		data_flag;

	int return_value = 0;
	if (received_all) {
		if (base_info_done) {
			return_value += 1;
			base_info_done = false;
		}
		if (imu_info_done) {
			return_value += 2;
			imu_info_done = false;
		}
		received_all	= false; 
		header00_flag	= false;
		header01_flag	= false;
		data_flag		= false;		
	}
	return return_value;
}
/***********************************************************
* function: init() 
 * Input: 
* Output:
**********************************************************/
void Serial_Interface::
init(){
	
}

/***********************************************************
 * function: setMotorPower(int state) 
 * Input: state  
 *			0 - off 
 *			1 - on
 * Output: 
 **********************************************************/
void 
Serial_Interface::
setMotorPower(int state) 
{
	pthread_mutex_lock(&m_serialTxLock);

	tx_buffer[tx_wr_index+0] = 0xAA;
	tx_buffer[tx_wr_index+1] = 0x55;
			
	tx_buffer[tx_wr_index+2] = 0x02;
	tx_buffer[tx_wr_index+3] = 0x01;
	if (state == 0) {		
		tx_buffer[tx_wr_index+4] = 0x00;
	} else {
		tx_buffer[tx_wr_index+4] = 0x01;
	}
	tx_buffer[tx_wr_index+5] = get_Sum_check(tx_wr_index, 4);
	tx_wr_index += 6;

	m_serial_port->write_message(tx_buffer, tx_wr_index);

	tx_wr_index = 0;

	pthread_mutex_unlock(&m_serialTxLock);
	usleep(2000);

}
/***********************************************************
 * function: setMotorDistance(float linear, float angular) 
 * Input: linear speed and angular distance  
 * Output: 

 **********************************************************/
void 
Serial_Interface::
setMotorDistance(float linear, float angular) 
{
	pthread_mutex_lock(&m_serialTxLock);

	

	tx_buffer[tx_wr_index+0] = 0xAA;
	tx_buffer[tx_wr_index+1] = 0x55;
			
	_FLOAT_t trans;
	tx_buffer[tx_wr_index+2] = 0x09;
	tx_buffer[tx_wr_index+3] = 0x06;
	trans.value = linear;
	tx_buffer[tx_wr_index+4] = trans._BYTE_t.byte[0];
	tx_buffer[tx_wr_index+5] = trans._BYTE_t.byte[1];
	tx_buffer[tx_wr_index+6] = trans._BYTE_t.byte[2];
	tx_buffer[tx_wr_index+7] = trans._BYTE_t.byte[3];
	trans.value = RADIANS2DEGREES(-angular);
	tx_buffer[tx_wr_index+8] = trans._BYTE_t.byte[0];
	tx_buffer[tx_wr_index+9] = trans._BYTE_t.byte[1];
	tx_buffer[tx_wr_index+10] = trans._BYTE_t.byte[2];
	tx_buffer[tx_wr_index+11] = trans._BYTE_t.byte[3];
	tx_buffer[tx_wr_index+12] = get_Sum_check(tx_wr_index, 11);
	tx_wr_index += 13;

	
	m_serial_port->write_message(tx_buffer, tx_wr_index);

	tx_wr_index = 0;


	pthread_mutex_unlock(&m_serialTxLock);
	usleep(2000);

}
/***********************************************************
 * function: setMotorSpeed(float linear, float angular) 
 * Input: linear speed and angular speed  
 * Output: 
 **********************************************************/
void 
Serial_Interface::
setMotorSpeed(float linear, float angular) 
{
	pthread_mutex_lock(&m_serialTxLock);

	if (ABS(linear) > MAX_LINEAR_SPEED) {
		linear = (linear > 0) ? (MAX_LINEAR_SPEED):(-MAX_LINEAR_SPEED);
	}
	if (ABS(angular) > MAX_ANGULAR_SPEED) {
		angular = (angular > 0) ? (MAX_ANGULAR_SPEED):(-MAX_ANGULAR_SPEED);
	}

	tx_buffer[tx_wr_index+0] = 0xAA;
	tx_buffer[tx_wr_index+1] = 0x55;
			
	_FLOAT_t trans;
	tx_buffer[tx_wr_index+2] = 0x09;
	tx_buffer[tx_wr_index+3] = 0x03;

	trans.value = linear;
	tx_buffer[tx_wr_index+4] = trans._BYTE_t.byte[0];
	tx_buffer[tx_wr_index+5] = trans._BYTE_t.byte[1];
	tx_buffer[tx_wr_index+6] = trans._BYTE_t.byte[2];
	tx_buffer[tx_wr_index+7] = trans._BYTE_t.byte[3];
	trans.value = RADIANS2DEGREES(-angular);
	tx_buffer[tx_wr_index+8] = trans._BYTE_t.byte[0];
	tx_buffer[tx_wr_index+9] = trans._BYTE_t.byte[1];
	tx_buffer[tx_wr_index+10] = trans._BYTE_t.byte[2];
	tx_buffer[tx_wr_index+11] = trans._BYTE_t.byte[3];
	tx_buffer[tx_wr_index+12] = get_Sum_check(tx_wr_index, 11);
	tx_wr_index += 13;

	
	m_serial_port->write_message(tx_buffer, tx_wr_index);

	tx_wr_index = 0;


	pthread_mutex_unlock(&m_serialTxLock);
	usleep(2000);

}
/***********************************************************
 * function: setMecanumMotorSpeed(float x, float y, float z) 
 * Input: linear x speed, linear y speed and angular speed
 * Output: 
 **********************************************************/
void 
Serial_Interface::
setMecanumMotorSpeed(float x, float y, float z) 
{
	pthread_mutex_lock(&m_serialTxLock);

	if (ABS(x) > MAX_LINEAR_SPEED) {
		x = (x > 0) ? (MAX_LINEAR_SPEED):(-MAX_LINEAR_SPEED);
	}
	if (ABS(y) > MAX_LINEAR_SPEED) {
		y = (y > 0) ? (MAX_LINEAR_SPEED):(-MAX_LINEAR_SPEED);
	}
	if (ABS(z) > MAX_ANGULAR_SPEED) {
		z = (z > 0) ? (MAX_ANGULAR_SPEED):(-MAX_ANGULAR_SPEED);
	}

	tx_buffer[tx_wr_index+0] = 0xAA;
	tx_buffer[tx_wr_index+1] = 0x55;
			
	_FLOAT_t trans;
	tx_buffer[tx_wr_index+2] = 0x0D;
	tx_buffer[tx_wr_index+3] = 0x06;
	trans.value = x;
	tx_buffer[tx_wr_index+4] = trans._BYTE_t.byte[0];
	tx_buffer[tx_wr_index+5] = trans._BYTE_t.byte[1];
	tx_buffer[tx_wr_index+6] = trans._BYTE_t.byte[2];
	tx_buffer[tx_wr_index+7] = trans._BYTE_t.byte[3];

	trans.value = y;
	tx_buffer[tx_wr_index+8] = trans._BYTE_t.byte[0];
	tx_buffer[tx_wr_index+9] = trans._BYTE_t.byte[1];
	tx_buffer[tx_wr_index+10] = trans._BYTE_t.byte[2];
	tx_buffer[tx_wr_index+11] = trans._BYTE_t.byte[3];

	trans.value = RADIANS2DEGREES(z);
	tx_buffer[tx_wr_index+12] = trans._BYTE_t.byte[0];
	tx_buffer[tx_wr_index+13] = trans._BYTE_t.byte[1];
	tx_buffer[tx_wr_index+14] = trans._BYTE_t.byte[2];
	tx_buffer[tx_wr_index+15] = trans._BYTE_t.byte[3];
	tx_buffer[tx_wr_index+16] = get_Sum_check(tx_wr_index, 15);
	tx_wr_index += 17;

	
	m_serial_port->write_message(tx_buffer, tx_wr_index);

	tx_wr_index = 0;

	pthread_mutex_unlock(&m_serialTxLock);
	usleep(2000);

}

/***********************************************************
 * function: setLifterPercentage(int percentage) 
 * Input: Percentage of the height relative to the 
 *			bottom (from 0 to 100) 
 * Output: 
 **********************************************************/
void 
Serial_Interface::
setLifterPercentage(int percentage)
{
	pthread_mutex_lock(&m_serialTxLock);

	if(percentage < 0)percentage = 0;
	if(percentage > MAX_PERCENTAGE) percentage = MAX_PERCENTAGE;
	tx_buffer[tx_wr_index+0] = 0xAA;
	tx_buffer[tx_wr_index+1] = 0x55;
			
	tx_buffer[tx_wr_index+2] = 0x03;
	tx_buffer[tx_wr_index+3] = 0x04;
	tx_buffer[tx_wr_index+4] = 0x01;
	tx_buffer[tx_wr_index+5] = percentage;
	tx_buffer[tx_wr_index+6] = get_Sum_check(tx_wr_index,5);
	tx_wr_index += 7;

	
	m_serial_port->write_message(tx_buffer, tx_wr_index);

	tx_wr_index = 0;

	pthread_mutex_unlock(&m_serialTxLock);
	usleep(2000);

}

/***********************************************************
 * function: stopLifter(void) 
 * Input:   
 * Output: 
 **********************************************************/
void
Serial_Interface:: 
stopLifter(void)
{

	pthread_mutex_lock(&m_serialTxLock);

	tx_buffer[tx_wr_index+0] = 0xAA;
	tx_buffer[tx_wr_index+1] = 0x55;
			
	tx_buffer[tx_wr_index+2] = 0x03;
	tx_buffer[tx_wr_index+3] = 0x04;
	tx_buffer[tx_wr_index+4] = 0x00;
	tx_buffer[tx_wr_index+5] = 0x00;
	tx_buffer[tx_wr_index+6] = get_Sum_check(tx_wr_index, 5);
	tx_wr_index += 7;

	
	m_serial_port->write_message(tx_buffer, tx_wr_index);

	tx_wr_index = 0;

	pthread_mutex_unlock(&m_serialTxLock);
	usleep(2000);
}

/***********************************************************
 * function: setPTZ(int angle) 
 * Input: PTZ rotation angle	(from 0 to 180) 
 * Output: 
 **********************************************************/
void 
Serial_Interface::
setPTZ(int P_angle, int T_angle) 
{
	pthread_mutex_lock(&m_serialTxLock);

	if(P_angle < 0) P_angle =0;
	if(P_angle > MAX_P_ANGLE) P_angle = MAX_P_ANGLE;
	if(T_angle < 10) T_angle =10;
	if(T_angle > MAX_T_ANGLE) T_angle = MAX_T_ANGLE;

#ifdef DISABLE_T_ANGLE
	T_angle = 90;
#endif

	tx_buffer[tx_wr_index+0] = 0xAA;
	tx_buffer[tx_wr_index+1] = 0x55;
			
	tx_buffer[tx_wr_index+2] = 0x03;
	tx_buffer[tx_wr_index+3] = 0x05;
	tx_buffer[tx_wr_index+4] = P_angle;
	tx_buffer[tx_wr_index+5] = T_angle;
	tx_buffer[tx_wr_index+6] = get_Sum_check(tx_wr_index, 5);
	tx_wr_index += 7;

	
	m_serial_port->write_message(tx_buffer, tx_wr_index);

	tx_wr_index = 0;

	pthread_mutex_unlock(&m_serialTxLock);
	usleep(2000);

}

/***********************************************************
 * function: startReadThread() 
 * Input:  
 * Output: 
 **********************************************************/
int 
Serial_Interface::
startReadThread()
{
	int result;
	// check serial port
	if ( not m_serial_port->status == 1 ) {
		fprintf(stderr,"ERROR: serial port not open\n");
		throw 1;
	}
	// write thread
	result = pthread_create( &m_readTid, NULL, &startReadThreadForInterface, this );
	if ( result ) throw result;
	// now we're reading messages
	return 0;
}

/***********************************************************
 * function: startWriteThread() 
 * Input:  
 * Output: 
 **********************************************************/
int 
Serial_Interface::
startWriteThread()
{
	int result;
	// check serial port
	if ( not m_serial_port->status == 1 ) {
		fprintf(stderr,"ERROR: serial port not open\n");
		throw 1;
	}
	// read thread
	result = pthread_create( &m_writeTid, NULL, &startWriteThreadForInterface, this );
	if ( result ) throw result;
	// now we're reading messages
	return 0;
}

void* 
startReadThreadForInterface(void *args)
{
	// takes an autopilot object argument
	Serial_Interface *p_SerialInterface = (Serial_Interface*)args;

	uint8_t message[256];
	int i = 0, len;
	while(p_SerialInterface->running) {
		len = p_SerialInterface->m_serial_port->read_message(message);	
		if (len > 0) {
			for(i = 0; i < len; i++) {
				p_SerialInterface->decode_messages(message[i]);
			}	
		}
		usleep(30000);
	}

	// done!
	return NULL;
}
void* 
startWriteThreadForInterface(void *args)
{
	// takes an autopilot object argument
	Serial_Interface *p_SerialInterface = (Serial_Interface*)args;

	while(p_SerialInterface->running) {
		pthread_mutex_lock(&p_SerialInterface->m_serialTxLock);
		if (p_SerialInterface->tx_wr_index != 0) {
			p_SerialInterface->m_serial_port->write_message(p_SerialInterface->tx_buffer, 
				p_SerialInterface->tx_wr_index);
			
			p_SerialInterface->tx_wr_index = 0;
			memset(p_SerialInterface->tx_buffer, BUFFER_SIZE, 0);
		}
		pthread_mutex_unlock(&p_SerialInterface->m_serialTxLock);

		usleep(5000);
	}
	// done!
	return NULL;
}

