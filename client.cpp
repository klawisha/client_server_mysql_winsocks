#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <iostream>
#include <Windows.h>
#include <vector>
#include <fstream>
#include "inip.h"
#include <string>
//#include <cstring>

#pragma warning(disable: 4996)
SOCKET Connection;
typedef struct dataKOM12 {
	int who;
	float f_speed;
	float s_speed;
	float f_weight;
	float s_weight;
	float length;
};
typedef struct dataKOM34 {
	int who;
	float speed;
	float f_temp;
	float s_temp;
	float weight;
	float length;
};
SOCKADDR_IN addr;
int sizeofaddr = sizeof(addr);

std::string ip;
int port;

void ClientHandler() { //функція для отримання часу
	while (true) {
		SYSTEMTIME _time ;
		char msg[256] = "iwannatime";
		send(Connection, msg, sizeof(msg), NULL);
		recv(Connection, msg, sizeof(msg), NULL);
		memcpy(&_time, msg, sizeof(_time));
		if (_time.wDayOfWeek == 0141) SetLocalTime(&_time);
		Sleep(10000);
	}
}


void ClientHandlerData() {//функція отримання даних
	char msg[256];
	dataKOM12 header1;
	while (true) {
		int bytesRecv = recv(Connection, (char*)&header1, sizeof(header1), NULL);
		if (bytesRecv > 0)
		{
			if (header1.who == 1234) { //перевірка сигнатури
				std::cout <<"First speed: "<< header1.f_speed << "\nSecond speed: " << header1.s_speed << "\nFirst weight: " << header1.f_weight << "\nSecond weight: " << header1.s_weight << "\nLength: " << header1.length << std::endl;

			}
			if (header1.who == 4321) {//перевірка сигнатури
				std::cout << "Speed: " <<header1.f_speed << "\nFirst temperature: " << header1.s_speed << "\nSecond temperature: " << header1.f_weight << "\nWeight: " << header1.s_weight << "\nLength: " << header1.length << std::endl;
			}
		}
	}
}

void ParseConf() {


	inipp::Ini<char> ini;
	std::ifstream is("D:\My\Programs\C++\client\client\conf.ini");
	ini.parse(is);
	ini.default_section(ini.sections["MAIN"]);
	ini.interpolate();
	ini.generate(std::cout);

	inipp::get_value(ini.sections["MAIN"], "ip", ip);

	inipp::get_value(ini.sections["MAIN"], "port", port);

}

void WriteConf() {
	std::string s1, s2;
	printf("\nВведите айпи:");
	std::cin >> s1;
	printf("\nВведите порт:");
	std::cin >> s2;

	const char* ipp = s1.c_str();
	const char* portt = s2.c_str();
	bool b = WritePrivateProfileStringA("MAIN", "ip", ipp, "D:\My\Programs\C++\client\client\conf.ini");
	if (!b) printf("!!!!\n");
	b = WritePrivateProfileStringA("MAIN", "port", portt, "D:\My\Programs\C++\client\client\conf.ini");
	if (!b) printf("!!!!\n");

}


int main(int argc, char* argv[]) {
	//WSAStartup
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, 0 << 4 | 14);
	setlocale(LC_ALL, "rus");
	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 1);
	if (WSAStartup(DLLVersion, &wsaData) != 0) {
		std::cout << "Error" << std::endl;
		exit(1);
	}

	
	addr.sin_family = AF_INET;

	int num = 0;
	int bytesRecv = 0;

	int bytesS = 0;
	char* data = new char[sizeof(dataKOM12)];
	while (true) {
		std::cout << "1.Получить данные от 1-й группы KOM\n2.Получить данные от 2-й группы KOM\n3.Задать айпи и порт\n4.Завершить работу\n" << std::endl;
		std::cin >> num;
		char msg1[256] = "iwannadata1";
		bool b = true;
		ParseConf();
		const char* ip1 = ip.c_str();

		switch (num) {

		case 1:

			addr.sin_addr.s_addr = inet_addr(ip1);
			addr.sin_port = htons(port);
			Connection = socket(AF_INET, SOCK_STREAM, NULL);
			if (connect(Connection, (SOCKADDR*)&addr, sizeof(addr)) != 0) {
				std::cout << "Error: failed connect to server.\n";
				break;
			}
			std::cout << "Connected!\n";
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, NULL, NULL, NULL); //поток запиту часу
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandlerData, NULL, NULL, NULL);//поток отримання даних

			msg1[10] = '1';
			bytesS = send(Connection, msg1, sizeof(msg1), NULL);
			std::cout << "[KOM12 data]\n";
			Sleep(2000);
			break;
		case 2:

			addr.sin_addr.s_addr = inet_addr(ip1);
			addr.sin_port = htons(port);
			Connection = socket(AF_INET, SOCK_STREAM, NULL);
			if (connect(Connection, (SOCKADDR*)&addr, sizeof(addr)) != 0) {
				std::cout << "Error: failed connect to server.\n";
				break;

			}
			std::cout << "Connected!\n";
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, NULL, NULL, NULL);
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandlerData, NULL, NULL, NULL);

			msg1[10] = '2';
			send(Connection, msg1, sizeof(msg1), NULL);
			std::cout << "[KOM34 data]\n";
			Sleep(2000);
			break;
		case 3: 
			WriteConf();
			ParseConf();
			break;
		case 4:
			return 0;
			break;
		default:
			break;
		}
	}
	system("pause");
	return 0;
}
