#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <iostream>
#include <mysql.h>
#include <string>
#include <sstream>
#include <Windows.h>
#include <stdlib.h>
#include <vector>
#include <fstream>

#include "inip.h"
#pragma warning(disable: 4996)
SYSTEMTIME _time;

SOCKET ConnectionsKOM[100];
int ConnectionsKOMCnt = 0;
std::string toGetTime = "iwannatime";
std::string toGetData1 = "iwannadata1";
std::string toGetData2 = "iwannadata2";
int id_cnt = 1;
HANDLE h = NULL;
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

int qstate;
bool on = false;
int port;
SOCKADDR_IN addr;
int sizeofaddr = sizeof(addr);

void connectionToMySQL(int kom_number, dataKOM12 *d1, dataKOM34 *d2) {
	MYSQL* connection;
	MYSQL_ROW row;
	MYSQL_RES* res;
	float resalts[5];
	int qstate;
	dataKOM12 data1 = { 0 };
	dataKOM34 data2 = { 0 };

	connection = mysql_init(0);
	connection = mysql_real_connect(connection, "localhost", "root", "0141", "serverDB", 3306, NULL, 0);
		if (connection) {
			puts("Successful connection to database!\n");
			
			if (kom_number == 1 || kom_number == 2) {
				std::string query = "INSERT INTO tableKOM12 (id, who, f_speed, s_speed, f_weight, s_weight, length) VALUES ("; //команда включення у БД - початок
				query += id_cnt;
				query += ", ";
				query += std::to_string(d1->who);
				query += ", ";
				query += std::to_string(d1->f_speed);
				query += ", ";
				query += std::to_string(d1->s_speed);
				query += ", ";
				query += std::to_string(d1->f_weight);
				query += ", ";
				query += std::to_string(d1->s_weight);
				query += ", ";
				query += std::to_string(d1->length);
				query += ");";//команда включення у БД - кінець

				std::cout << query << std::endl;
				id_cnt++;


			   const char* q = query.c_str();
			   qstate = mysql_query(connection, q);
			}
			else {
				std::string query = "INSERT INTO tableKOM34 (id, who, speed, f_temperature, s_temperature, weight, length) VALUES (";
				query += id_cnt;
				query += ", ";
				query += std::to_string(d2->who);
				query += ", ";
				query += std::to_string(d2->speed);
				query += ", ";
				query += std::to_string(d2->f_temp);
				query += ", ";
				query += std::to_string(d2->s_temp);
				query += ", ";
				query += std::to_string(d2->weight);
				query += ", ";
				query += std::to_string(d2->length);
				query += ");";

				std::cout << query << std::endl;
				id_cnt++;
				const char* q = query.c_str();
				qstate = mysql_query(connection, q);
			};
		}
		else {
			puts("Connection to database has failed!");
		}
}


void KOMHandler(int index){ //функція отримання даних

	char* data = new char[sizeof(dataKOM12)];
	MYSQL* connection;
	MYSQL_ROW row;
	MYSQL_RES* res;
	float resalts[5];

	dataKOM12 data1 = { 0 };
	dataKOM34 data2 = { 0 };
	while(true) {
		int bytesRecv = recv(ConnectionsKOM[index], data, sizeof(dataKOM12), NULL);
		if (bytesRecv > 0)
		{
			std::string str = std::string(data);
			if (str == toGetData1) {  //якщо запит на дані від РС про 1 групу КОМ
				std::cout << "...Sending data...\n";
				connection = mysql_init(0);

				connection = mysql_real_connect(connection, "localhost", "root", "0141", "serverDB", 3306, NULL, 0);

				if (connection) {
					puts("Successful connection to database!\nScope: speed1(0.0 - 10.0) speed2(0.0 - 40.0) weight1(0.0 - 200.0) weight2(0.0 - 1000.0) length(0.0 - 50.0)\n");

					std::string query = "SELECT * FROM tableKOM12";
					const char* q = query.c_str();
					qstate = mysql_query(connection, q);
					if (!qstate)
					{
						res = mysql_store_result(connection);

						while (row = mysql_fetch_row(res))
						{
							//for (int i = 0; i < 6; ++i) {
								//resalts[i] = atof(row[i]);
							data1.who = 1234; //встановлення сигнатури
							data1.f_speed = atof(row[2]);
							data1.s_speed = atof(row[3]);
							data1.f_weight = atof(row[4]);
							data1.s_weight = atof(row[5]);
							data1.length = atof(row[6]);
							//}
							if ((data1.f_speed > 0.0 && data1.f_speed < 10.0) && (data1.s_speed > 0.0 && data1.s_speed < 40.0) && (data1.f_weight > 0.0 && data1.f_weight < 200.0) && (data1.s_weight > 0.0 && data1.s_weight < 1000.0) && (data1.length > 0.0 && data1.length < 50.0)) {
								printf("ID: %s, Speed#1: %f, Speed#2: %f, Weight#1: %f, Weight#2: %f, Length: %f\n", row[0], data1.f_speed, data1.s_speed, data1.f_weight, data1.s_weight, data1.length);
								int bytesSend = send(ConnectionsKOM[index], (char*)&data1, sizeof(data1), NULL);
							}
						}

					}
					else
					{
						std::cout << "Query failed: " << mysql_error(connection) << std::endl;
					}
		
				}
				else {
					puts("Connection to database has failed!");
				}
			}
			if (str == toGetData2) { запит про дані 2-ї групи КОМ
				std::cout << "...Sending data...\n";
				connection = mysql_init(0);

				connection = mysql_real_connect(connection, "localhost", "root", "0141", "serverDB", 3306, NULL, 0);

				if (connection) {
					std::string query = "SELECT * FROM tableKOM34";
					const char* q = query.c_str();
					qstate = mysql_query(connection, q);
					if (!qstate)
					{
						res = mysql_store_result(connection);

						while (row = mysql_fetch_row(res))
						{
							//for (int i = 0; i < 6; ++i) {
								//resalts[i] = atof(row[i]);
							data2.who = 4321; //встановлення сигнатури
							data2.speed = atof(row[2]);
							data2.f_temp = atof(row[3]);
							data2.s_temp = atof(row[4]);
							data2.weight = atof(row[5]);
							data2.length = atof(row[6]);
							//}
							if ((data2.speed > 0.0 && data2.speed < 20.0) && (data2.f_temp > 0.0 && data2.f_temp < 1000.0) && (data2.s_temp > 0.0 && data2.s_temp < 500.0) && (data2.weight > 0.0 && data2.weight < 1000.0) && (data2.length > 0.0 && data2.length < 100.0)) {
								printf("ID: %s, Speed: %f, Temperature#1: %f, Temperature#2: %f, Weight: %f, Length: %f\n", row[0], data2.speed, data2.f_temp, data2.s_temp, data2.weight, data2.length);
								int bytesSend = send(ConnectionsKOM[index], (char*)&data2, sizeof(data2), NULL);
								//send to serv
							}
						}
					}
					else
					{
						std::cout << "Query failed: " << mysql_error(connection) << std::endl;
					}
				
				}
				else {
					puts("Connection to database has failed!\n");
				}
			}
			if (str == toGetTime) {//запит на отримання часу 
				std::cout << "...Sending time...\n";
				GetLocalTime(&_time);//отримання часу
				_time.wDayOfWeek = 0141; //своя сигнатура
				int bytesSent = send(ConnectionsKOM[index], (char*)&_time, sizeof(_time), NULL); //відсилання часу
				if (bytesSent > 0)
				{
					std::cout << _time.wDay << "." << _time.wMonth << "." << _time.wYear << " | " << _time.wHour << " : " << _time.wMinute << " : " << _time.wSecond << " : " << _time.wMilliseconds << std::endl;

				}
			}
			dataKOM12 header1;
			dataKOM34 header2;
			switch (data[0])
			{
			case 1:
				memcpy(&header1, data, sizeof(dataKOM12));
				printf("From: KOM1\n");
				std::cout << header1.f_speed << ": " << header1.s_speed << ": " << header1.f_weight << ": " << header1.s_weight << ": " << header1.length << std::endl;
				connectionToMySQL(1,&header1,NULL);
				break;
			case 2:
				memcpy(&header1, data, sizeof(dataKOM12));
				printf("From: KOM2\n");
				std::cout << header1.f_speed << ": " << header1.s_speed << ": " << header1.f_weight << ": " << header1.s_weight << ": " << header1.length << std::endl;
				connectionToMySQL(2, &header1, NULL);
				break;
			case 3:
				memcpy(&header2, data, sizeof(dataKOM34));
				printf("From: KOM3\n");
				std::cout << header2.speed << ": " << header2.f_temp << ": " << header2.s_temp << ": " << header2.weight << ": " << header2.length << std::endl;
				connectionToMySQL(3, NULL, &header2);
				break;
			case 4:
				memcpy(&header2, data, sizeof(dataKOM34));
				printf("From: KOM4\n");
				std::cout << header2.speed << ": " << header2.f_temp << ": " << header2.s_temp << ": " << header2.weight << ": " << header2.length << std::endl;
				connectionToMySQL(4, NULL, &header2);
				break;

			default:
				break;
			}
		}
	}
}

void Listen() {   //функція прослуховування сокету

	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);
	bind(sListen, (SOCKADDR*)&addr, sizeof(addr));
	WSAAsyncSelect(sListen, NULL, NULL, FD_ACCEPT | FD_CLOSE);
	listen(sListen, SOMAXCONN);


	SOCKET newConnection;

	for (int i = 0; i < 100; i++) {  //до 100 підключень
		newConnection = accept(sListen, (SOCKADDR*)&addr, &sizeofaddr);

		if (newConnection == 0) {
			std::cout << "Error #2\n";
		}
		else {
			std::cout << "\nClient Connected!\n";
			ConnectionsKOM[i] = newConnection;
			ConnectionsKOMCnt++;
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)KOMHandler, (LPVOID)(i), NULL, NULL);


		}

	}

	return;
}


void OnOffPort() {    //вмикання/вимикання порту
	on = !on;
	if (on) {
		std::cout << "Прослушивание включено.\n";
		h = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Listen, NULL, NULL, NULL);
		
		}
	else {
		std::cout << "Прослушивание выключено.\n";
		CloseHandle(h);
	}
		
}



void ParseConf() {


	inipp::Ini<char> ini;
	std::ifstream is("D:\My\Programs\C++\server\server\conf.ini");
	ini.parse(is);
	ini.default_section(ini.sections["MAIN"]);
	ini.interpolate();
	ini.generate(std::cout);

	inipp::get_value(ini.sections["MAIN"], "port", port);

}

void WriteConf() {
	std::string s;
	int i;
	printf("\nВведите порт:");
	std::cin >> s;
	const char* p1 = s.c_str();
	bool b = WritePrivateProfileStringA("MAIN", "port", p1, "D:\My\Programs\C++\server\server\conf.ini");
}


int main(int argc, char* argv[]) {
	
	
	//WSAStartup
	setlocale(LC_ALL, "rus");
	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 1);
	if (WSAStartup(DLLVersion, &wsaData) != 0) {
		std::cout << "Error" << std::endl;
		exit(1);
	}
	ParseConf();

	while (true) {
		addr.sin_addr.s_addr = inet_addr("127.0.0.1");
		addr.sin_port = htons(port);
		addr.sin_family = AF_INET;

		int ch;
		std::cout << "1.Задать порт\n2.Вкл/выкл прослушивание порта\n3.Завершить работу\n";
		std::cin >> ch;
		switch (ch) {
		case 1:
			WriteConf();
			ParseConf();
			break;
		case 2:
			OnOffPort();
			break;
		case 3:
			return 0;
			break;
		}
	}
	system("pause");
	return 0;
}
