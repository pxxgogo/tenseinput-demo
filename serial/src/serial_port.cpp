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
#include "serial_port.h"

Serial_Port::
Serial_Port(char *uart_name_ , int baudrate_)
{
	initialize_defaults();
	uart_name = uart_name_;
	baudrate  = baudrate_;
}

Serial_Port::
Serial_Port()
{
	initialize_defaults();
}

Serial_Port::
~Serial_Port()
{
	// destroy mutex
	pthread_mutex_destroy(&lock);
}

void
Serial_Port::
initialize_defaults()
{
	// Initialize attributes
	debug  = false;
	fd     = -1;
	status = SERIAL_PORT_CLOSED;

	uart_name = (char*)"/dev/ttyUSB0";
	baudrate  = 57600;

	// Start mutex
	int result = pthread_mutex_init(&lock, NULL);
	if ( result != 0 )
	{
		printf("\n mutex init failed\n");
		throw 1;
	}
}



//   Write to Serial
void
Serial_Port::
write_message(uint8_t *message, unsigned len)
{
	_write_port(message, len);
	return;
}

//   Open Serial Port
void
Serial_Port::
open_serial()
{
	// Judge the port read and write private
	printf("%s\n", uart_name);
	if ( (access(uart_name, R_OK) != 0) || (access(uart_name, W_OK) != 0) ) {
		if ( chmod(uart_name, S_IROTH | S_IWOTH) != 0 ) {
			printf("\033[33m\nneed 'sudo' or to chmod the port\033[0m\n\n");
			exit(0);
		}
	}

	printf("OPEN PORT\n");
	
	fd = _open_port(uart_name);


	// Check success and wait for linking
	while(fd == -1) {
		printf("failure, could not open port.\n----- Wait for linking  1000ms .....\n");
		sleep(1);
		fd = _open_port(uart_name);
	}

	bool success = _setup_port(baudrate, 8, 1, false, false);

	if (!success) {
		printf("failure, could not configure port.\n");
		throw EXIT_FAILURE;
	}
	if (fd <= 0) {
		printf("Connection attempt to port %s with %d baud, 8N1 failed, exiting.\n", uart_name, baudrate);
		throw EXIT_FAILURE;
	}

	printf("Connected to %s with %d baud, 8 data bits, no parity, 1 stop bit (8N1)\n", uart_name, baudrate);

	status = true;

	printf("\n");

	return;

}
void
Serial_Port::
close_serial()
{
	int result = close(fd);

	if ( result ) {
		fprintf(stderr,"WARNING: Error on port close (%i)\n", result );
	}

	status = false;
	printf("CLOSE PORT\n\n");	
}

void
Serial_Port::
start()
{
	open_serial();
}

void
Serial_Port::
stop()
{
	close_serial();
}


void
Serial_Port::
handle_quit( )
{
	try {
		stop();
	}
	catch (int error) {
		fprintf(stderr,"Warning, could not stop serial port\n");
	}
	return;
}


int
Serial_Port::
_open_port(const char* port)
{
	// Open serial port
	// O_RDWR - Read and write
	// O_NOCTTY - Ignore special chars like CTRL-C
	fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY);

	// Check for Errors
	if (fd == -1) {
		/* Could not open the port. */
		fprintf(stderr, "Could not open the port.");
		return(-1);
	}
	// Finalize
	else {
		fcntl(fd, F_SETFL, 0);
	}

	// Done!
	return fd;
}

bool
Serial_Port::
_setup_port(int baud, int data_bits, int stop_bits, bool parity, bool hardware_control)
{
	// Check file descriptor
	if(!isatty(fd)) {
		fprintf(stderr, "\nERROR: file descriptor %d is NOT a serial port\n", fd);
		return false;
	}

	// Read file descritor configuration
	struct termios  config;
	if(tcgetattr(fd, &config) < 0) {
		fprintf(stderr, "\nERROR: could not read configuration of fd %d\n", fd);
		return false;
	}

	// Input flags - Turn off input processing
	// convert break to null byte, no CR to NL translation,
	// no NL to CR translation, don't mark parity errors or breaks
	// no input parity check, don't strip high bit off,
	// no XON/XOFF software flow control
	config.c_iflag &= ~(IGNBRK | BRKINT | ICRNL |
						INLCR | PARMRK | INPCK | ISTRIP | IXON);

	// Output flags - Turn off output processing
	// no CR to NL translation, no NL to CR-NL translation,
	// no NL to CR translation, no column 0 CR suppression,
	// no Ctrl-D suppression, no fill characters, no case mapping,
	// no local output processing
	config.c_oflag &= ~(OCRNL | ONLCR | ONLRET |
						 ONOCR | OFILL | OPOST);

	#ifdef OLCUC
		config.c_oflag &= ~OLCUC;
	#endif

	#ifdef ONOEOT
		config.c_oflag &= ~ONOEOT;
	#endif

	// No line processing:
	// echo off, echo newline off, canonical mode off,
	// extended input processing off, signal chars off
	config.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);

	// Turn off character processing
	// clear current char size mask, no parity checking,
	// no output processing, force 8 bit input
	config.c_cflag &= ~(CSIZE | PARENB);
	config.c_cflag |= CS8;

	// One input byte is enough to return from read()
	// Inter-character timer off
	config.c_cc[VMIN]  = 1;
	config.c_cc[VTIME] = 10; // was 0

	// Get the current options for the port
	////struct termios options;
	////tcgetattr(fd, &options);

	// Apply baudrate
	switch (baud)
	{
		case 1200:
			if (cfsetispeed(&config, B1200) < 0 || cfsetospeed(&config, B1200) < 0) {
				fprintf(stderr, "\nERROR: Could not set desired baud rate of %d Baud\n", baud);
				return false;
			}
			break;
		case 1800:
			cfsetispeed(&config, B1800);
			cfsetospeed(&config, B1800);
			break;
		case 9600:
			cfsetispeed(&config, B9600);
			cfsetospeed(&config, B9600);
			break;
		case 19200:
			cfsetispeed(&config, B19200);
			cfsetospeed(&config, B19200);
			break;
		case 38400:
			if (cfsetispeed(&config, B38400) < 0 || cfsetospeed(&config, B38400) < 0) {
				fprintf(stderr, "\nERROR: Could not set desired baud rate of %d Baud\n", baud);
				return false;
			}
			break;
		case 57600:
			if (cfsetispeed(&config, B57600) < 0 || cfsetospeed(&config, B57600) < 0) {
				fprintf(stderr, "\nERROR: Could not set desired baud rate of %d Baud\n", baud);
				return false;
			}
			break;
		case 115200:
			if (cfsetispeed(&config, B115200) < 0 || cfsetospeed(&config, B115200) < 0) {
				fprintf(stderr, "\nERROR: Could not set desired baud rate of %d Baud\n", baud);
				return false;
			}
			break;

		// These two non-standard (by the 70'ties ) rates are fully supported on
		// current Debian and Mac OS versions (tested since 2010).
		case 460800:
			if (cfsetispeed(&config, B460800) < 0 || cfsetospeed(&config, B460800) < 0) { 
				fprintf(stderr, "\nERROR: Could not set desired baud rate of %d Baud\n", baud);
				return false;
			}
			break;
		case 921600:
			if (cfsetispeed(&config, B921600) < 0 || cfsetospeed(&config, B921600) < 0) {
				fprintf(stderr, "\nERROR: Could not set desired baud rate of %d Baud\n", baud);
				return false;
			}
			break;
		default:
			fprintf(stderr, "ERROR: Desired baud rate %d could not be set, aborting.\n", baud);
			return false;
			break;
	}

	// Finally, apply the configuration
	if(tcsetattr(fd, TCSAFLUSH, &config) < 0) {
		fprintf(stderr, "\nERROR: could not set configuration of fd %d\n", fd);
		return false;
	}

	// Done!
	return true;
}

//   Read from Serial
int
Serial_Port::
read_message(uint8_t *message)
{
	uint8_t          cp[256];
	int timeout = 0;
	// this function locks the port during read
	int result = _read_port(cp);

	if (result > 0) {
		memcpy(message, cp, sizeof(uint8_t)*result);
	}
	// Couldn't read from port
	else {
		fprintf(stderr, "ERROR: Could not read from fd %d\n", fd);
		perror("read");
	}
	// Done!
	return result;
}

//   Read Port with Lock
int
Serial_Port::
_read_port(uint8_t *cp)
{
	// Lock
	pthread_mutex_lock(&lock);
	int result = read(fd, cp, 256);

	// Unlock
	pthread_mutex_unlock(&lock);
	return result;
}

//   Write Port with Lock
void
Serial_Port::
_write_port(uint8_t *buf, unsigned len)
{
	// Lock
	pthread_mutex_lock(&lock);

	// Write packet via serial link
	write(fd, buf, len);
	// Wait until all data has been written
	tcdrain(fd);

	// Unlock
	pthread_mutex_unlock(&lock);
}

