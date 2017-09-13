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

	YPR = serialInterface.getYPR();
	for(i = 0; i < YPR.size(); i++)
		ofile << YPR[i] << "  ";
	ofile << endl;
	return 0;
}
