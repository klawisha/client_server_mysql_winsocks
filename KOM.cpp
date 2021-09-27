#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <iostream>
#include <vector>
#include <mysql.h>
#include <stdlib.h>
#include <fstream>
#include <conio.h>
#include "inip.h"
#pragma warning(disable: 4996)
using namespace std;
int qstate;
int period;
typedef struct dataKOM12 {				//структура для першої групи КОМ
	int who;
	float f_speed;
	float s_speed;
	float f_weight;
	float s_weight;
	float length;
	};
typedef struct dataKOM34 {				//структура для другої групи КОМ
	int who;
	float speed;
	float f_temp;
	float s_temp;
	float weight;
	float length;
};
std::string ip;
int port;
bool on1 = false;
bool on2 = false;
bool on3 = false;
bool on4 = false;
SOCKET Connection;
SOCKADDR_IN addr;


void ParseConf();			//функція для зчитування даних з файлу ініціалізації


void TimeHandler() {					//функція отримання часу від серверу
	while (true) {
		SYSTEMTIME _time;
		char msg[256] = "iwannatime";
		send(Connection, msg, sizeof(msg), NULL);	//посилання
		recv(Connection, msg, sizeof(msg), NULL);	//приймання
		memcpy(&_time, msg, sizeof(_time));
		if (_time.wDayOfWeek == 0141) SetLocalTime(&_time);//перевірка правильності пакету
		Sleep(10000);
	}
}

void connectionToServ(dataKOM12* d, dataKOM34 *d1) {
	
	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 1);
	if (WSAStartup(DLLVersion, &wsaData) != 0) {
		std::cout << "Error" << std::endl;
		exit(1);
	}


	int sizeofaddr = sizeof(addr);
	const char* p = ip.c_str();
	addr.sin_addr.s_addr = inet_addr(p);	//прив’язування сокету айпи
	addr.sin_port = htons(port);		//прив’язування порту
	addr.sin_family = AF_INET;
	Connection = socket(AF_INET, SOCK_STREAM, NULL);
	if (connect(Connection, (SOCKADDR*)&addr, sizeof(addr)) != 0) {//підключення
		std::cout << "Error: failed connect to server.\n";
		return;
	}
	std::cout << "Connected!\n";
	if(d != NULL) std::cout << "data: " <<d->f_speed << " : " << d->s_speed << " : " << d->f_weight << std::endl;
	else std::cout << "data: " << d1->speed << " : " << d1->f_temp << " : " << d1->s_temp << std::endl;
	char msg[sizeof(dataKOM12)];
	//memcpy(msg, &d, sizeof(dataKOM12));
	//msg[0] = ((uint32_t)d->f_speed >> 8) & 0xFF;
	//msg[1] = ((uint32_t)d->s_speed >> 8) & 0xFF;
	//msg[2] = ((uint32_t)d->f_weight >> 8) & 0xFF;
	//msg[3] = ((uint32_t)d->s_weight >> 8) & 0xFF;
	//msg[4] = ((uint32_t)d->length >> 8) & 0xFF;
	//

	int bytesSent;
	if(d != NULL) bytesSent = send(Connection, (char*)d, sizeof(dataKOM12), NULL);
	//відсилання даних першої групи КОМ
	else bytesSent = send(Connection, (char*)d1, sizeof(dataKOM34), NULL);
	//відсилання даних другої групи КОМ
	if (bytesSent > 0)
	{
		cout << "Sent:" << bytesSent << endl;
	}
	//recv(Connection, msg, sizeof(msg), NULL);
	//std::cout << msg << std::endl;

}


void connectionToMySQL(int kom_number) {//функція підключення до БД
	MYSQL* connection;
	MYSQL_ROW row;
	MYSQL_RES* res;
	float resalts[5];

	dataKOM12 data1 = { 0 };
	dataKOM34 data2 = { 0 };
	while (true) {
		if (_kbhit() != 0) {
			if (_getch() == 'q') {
				return;
			}

		}
				connection = mysql_init(0);
				//while (true){
				if (kom_number == 1 || kom_number == 2) {//1 група КОМ
					connection = mysql_real_connect(connection, "localhost", "root", "0141", "firstKOMgroup", 3306, NULL, 0);//підключення до БД

					if (connection) {
						puts("Successful connection to database!\nScope: speed1(0.0 - 10.0) speed2(0.0 - 40.0) weight1(0.0 - 200.0) weight2(0.0 - 1000.0) length(0.0 - 50.0)\n");

						string query = "SELECT * FROM table1";//команда
						const char* q = query.c_str();
						qstate = mysql_query(connection, q);//виконання
						if (!qstate)
						{
							res = mysql_store_result(connection);

							while (row = mysql_fetch_row(res
							{//построково
								
								data1.who = kom_number;
								data1.f_speed = atof(row[1]);
								data1.s_speed = atof(row[2]);
								data1.f_weight = atof(row[3]);
								data1.s_weight = atof(row[4]);
								data1.length = atof(row[5]);
								
								if ((data1.f_speed > 0.0 && data1.f_speed < 10.0) && (data1.s_speed > 0.0 && data1.s_speed < 40.0) && (data1.f_weight > 0.0 && data1.f_weight < 200.0) && (data1.s_weight > 0.0 && data1.s_weight < 1000.0) && (data1.length > 0.0 && data1.length < 50.0)) { //перевірка даних
									printf("ID: %s, Speed#1: %f, Speed#2: %f, Weight#1: %f, Weight#2: %f, Length: %f\n", row[0], data1.f_speed, data1.s_speed, data1.f_weight, data1.s_weight, data1.length);
									connectionToServ(&data1, NULL); //відсилання
									Sleep(period);//затримка
								}
							}

						}
						else
						{
							cout << "Query failed: " << mysql_error(connection) << endl;
						}
					}
					else {
						puts("Connection to database has failed!");
					}
				}
				else {
					connection = mysql_real_connect(connection, "localhost", "root", "0141", "secondKOMgroup", 3306, NULL, 0);

					if (connection) {
						puts("Successful connection to database!\nScope: speed(0.0 - 20.0) temperature1(0.0 - 1000.0) temperature2(0.0 - 500.0) weight(0.0 - 1000.0) length(0.0 - 100.0)\n");

						string query = "SELECT * FROM table2";
						const char* q = query.c_str();
						qstate = mysql_query(connection, q);
						if (!qstate)
						{
							res = mysql_store_result(connection);

							while (row = mysql_fetch_row(res))
							{
								data2.who = kom_number;
								data2.speed = atof(row[1]);
								data2.f_temp = atof(row[2]);
								data2.s_temp = atof(row[3]);
								data2.weight = atof(row[4]);
								data2.length = atof(row[5]);

								if ((data2.speed > 0.0 && data2.speed < 20.0) && (data2.f_temp > 0.0 && data2.f_temp < 1000.0) && (data2.s_temp > 0.0 && data2.s_temp < 500.0) && (data2.weight > 0.0 && data2.weight < 1000.0) && (data2.length > 0.0 && data2.length < 100.0)) {
									printf("ID: %s, Speed: %f, Temperature#1: %f, Temperature#2: %f, Weight: %f, Length: %f\n", row[0], data2.speed, data2.f_temp, data2.s_temp, data2.weight, data2.length);

									connectionToServ(NULL, &data2);
									//Sleep(period);

									//send to serv

								}
							}


						}
						else
						{
							cout << "Query failed: " << mysql_error(connection) << endl;
						}
					}
					else {
						puts("Connection to database has failed!");
					}

				}
				Sleep(period);
			
		
	}


}

void chooseKOM() { //меню вибору КОМ
	int choice;
	printf("Выберите КОМ:\n\t1.KOM1\n\t2.KOM2\n\t3.KOM3\n\t4.KOM4\n(Чтобы остановить передачу нажмите 'q')\n");
	scanf("%d", &choice);
	HANDLE h1 = nullptr;
	HANDLE h2 = nullptr;  //для кожної КОМ окремий HANDLE та буде окремий потік
	HANDLE h3 = nullptr;
	HANDLE h4 = nullptr;
	switch (choice) {
	case 1:
		on1 = !on1;
		if (on1) h1 = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)connectionToMySQL, (LPVOID)(1), NULL, NULL); //створення потоку
		else CloseHandle(h1);					//закриття потоку
		//connectionToMySQL(1);
		break;
	case 2:
		on2 = !on2;
		if (on2) h2 = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)connectionToMySQL, (LPVOID)(2), NULL, NULL);
		else CloseHandle(h2);
		//connectionToMySQL(2);
		break;
	case 3:
		on3 = !on3;
		if (on3) h3 = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)connectionToMySQL, (LPVOID)(3), NULL, NULL);
		else CloseHandle(h3);
		//connectionToMySQL(3);
		break;
	case 4:
		on4 = !on4;
		if (on4) h4 = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)connectionToMySQL, (LPVOID)(4), NULL, NULL);
		else CloseHandle(h4);
		//connectionToMySQL(4);
		break;

	default:
		break;

	}

}
void SetPeriod() {
	printf("\nВведите период задержки в секундах:");
	scanf("%d", &period);
	period = period * 1000;		//встановлення затримки
	return;
}


void ParseConf() { 

	
	inipp::Ini<char> ini;
	std::ifstream is("D:\My\Programs\C++\KOM\KOM\conf.ini");
	ini.parse(is);
	ini.default_section(ini.sections["MAIN"]);
	ini.interpolate();
	ini.generate(std::cout);

	inipp::get_value(ini.sections["MAIN"], "ip", ip); //отримання айпі

	inipp::get_value(ini.sections["MAIN"], "port", port); //отримання порту
}

void WriteConf() {   //функція записування у файл ініціалізації
	std::string s1,s2;
	printf("\nВведите айпи:");
	scanf("%s", &s1);
	printf("\nВведите порт:");
	scanf("%s", &s2);
	const char* ip1 = s1.c_str();
	const char* port1 = s2.c_str();
	bool b = WritePrivateProfileStringA("MAIN", "ip", ip1, "D:\My\Programs\C++\KOM\KOM\conf.ini");   //записування айпі
	 b = WritePrivateProfileStringA("MAIN", "port", port1, "D:\My\Programs\C++\KOM\KOM\conf.ini");  //записування порту
}

int main()
{

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, 0 << 4 | 11);
	setlocale(LC_ALL, "russian");
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)TimeHandler, NULL, NULL, NULL);
	ParseConf();
	for (;;) {
		int choice;
		printf("\n1. Подключиться к серверу и передать данные.\n2.Задать период для передачи(default=1sec)\n3.Задать айпи и порт для передачи(default = 127.0.0.1:1111)\n4.Завершить программу.\n");
		scanf("%d", &choice);

		switch (choice)
		{
		case 1:
			chooseKOM();
			break;
		case 2:
			SetPeriod();
			break;
		case 3:
			WriteConf();
			ParseConf();
			break;
		case 4:

			return 0;

		default:
			break;
		}
	
	
	}
	return 0;
}
