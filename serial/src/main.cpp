#include <stdio.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <fstream>
#include "serialInterface.h"
using namespace std;
#define TT
ofstream ofile("a.txt", ios::out);
int lastPosL, newPosL, lastPosR, newPosR, delta;
std::vector<int > encoder;
Serial_Interface serialInterface;
vector<float> YPR;

void goStraight(int dis, float speed)
{
	delta = 0;
	encoder = serialInterface.getEncoder();
	newPosL = encoder[0];
	newPosR = encoder[1];
 	while(1)
	{
		if(delta >= dis)
			break;
		serialInterface.setMotorSpeed(speed, 0.0);
		lastPosL = newPosL;
		lastPosR = newPosR;
		encoder = serialInterface.getEncoder();
		newPosL = encoder[0];
		newPosR = encoder[1];
		ofile << newPosL << ", " << newPosR << endl;
		if(newPosL > lastPosL)
			delta += (newPosL - lastPosL);
		else if(lastPosL > newPosL + 30000)
			delta += (newPosL + 65536 - lastPosL);
	}
	cout << dis << "--" << delta << endl;
}

void turnD(int dis, float speed)
{
	delta = 0;
	encoder = serialInterface.getEncoder();
	newPosL = encoder[0];
	newPosR = encoder[1];
 	while(1)
	{
		if(delta >= dis)
			break;
		serialInterface.setMotorSpeed(0.0, speed);
		lastPosL = newPosL;
		lastPosR = newPosR;
		encoder = serialInterface.getEncoder();
		newPosL = encoder[0];
		newPosR = encoder[1];
		ofile << newPosL << ", " << newPosR << endl;
		if(speed > 0)
		{
			if(newPosL > lastPosL)
				delta += (newPosL - lastPosL);
			else if(lastPosL > newPosL + 30000)
				delta += (newPosL + 65536 - lastPosL);
		}
		else
		{
			if(newPosR > lastPosR)
				delta += (newPosR - lastPosR);
			else if(lastPosR > newPosR + 30000)
				delta += (newPosR + 65536 - lastPosR);
		}
		
	}
	cout << dis << "--" << delta << endl;
}      

int main()
{
	char *uart_name = (char*)"/dev/ttyUSB0";
	int baudrate = 115200;
	int dis = 4100, i;
	printf ("[main]: start\n");
	if( !serialInterface.start(uart_name, 115200) ) {
		cout << "[main]: Serial Error" << endl;
		exit(0);
	}
	cout << "================" << endl;

	serialInterface.setMotorDistance(0.2, 0.0);
	sleep(5);
	cout << "start turn" << endl;
	YPR = serialInterface.getYPR();
	for(i = 0; i < YPR.size(); i++)
		cout << YPR[i] << "  ";
	cout << endl;

	goStraight(14000*2, 0.3);
	YPR = serialInterface.getYPR();
	for(i = 0; i < YPR.size(); i++)
		cout << YPR[i] << "  ";
	cout << endl;

	sleep(3);
	turnD(dis, 0.3);
	YPR = serialInterface.getYPR();
	for(i = 0; i < YPR.size(); i++)
		cout << YPR[i] << "  ";
	cout << endl;
	sleep(3);
	cout << "fking turn finish1" << endl;
	goStraight(7000, 0.3);
	sleep(3);
	turnD(dis * 2, 0.3);

	sleep(3);
	cout << "fking turn finish2" << endl;
	goStraight(14000, 0.3);
	sleep(5);
	turnD(dis, -0.3);
sleep(3);
goStraight(12000, 0.3);
	_exit(0);
	return 0;
}
