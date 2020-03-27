#include <stdio.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <string.h>
#include "cJSON.h"
#include "data.h"

#define LEN  400
#define TEST 1

unsigned char RxBuffer[LEN];
data_t sys;

//---------------------------------------------
int Find(const char *str)
{
	if (strstr(RxBuffer, str) != NULL)
		return 1;
	else
		return 0;
}

//----------------------------------------------------------
void clear(char* dst, int len)
{
	for (int i = 0; i < len; i++) dst[i] = 0;
}
//---------------------------------------------------------
void getRec(int fd)
{
	int num = 0;
	while (num == 0) {
		delay(100);
		num = serialDataAvail(fd);
	}
	for (int i = 0; i < LEN; i++) {
		RxBuffer[i] = 0;
	}
	for (int i = 0; i < num; i++) {
		RxBuffer[i] = serialGetchar(fd);
	}
	if (TEST) printf(RxBuffer);
}
//-----------------------------------------------------------------
void readId(char b[], char* src, int ofset)
{
	char* result = src + 2 + ofset;
	char* p = strstr(result, "\r\n");
	strncpy(b, result, strlen(result) - strlen(p));
	if (TEST) printf("Id = %s\r\n", b);
}
//-----------------------------------------------------
void send(int fd, char* at)
{
	delay(200);
	serialPuts(fd, at);
	if (TEST) printf(at);
	getRec(fd);
}
//-------------------------------------------------------------------
int isOk(int num, int n)
{
	if (num == 0)
	{
		clear(sys.msg, MSG_LEN);
		readId(sys.msg, RxBuffer, 0);
		return 0;
	}
	return n;
}
//-----------------------------------------------
int commitAT(int fd, char* at, const char* result, int num)
{
	while (num > 0)
	{
		send(fd, at);
		if (Find(result)) break;
		else num--;
	}
	return isOk(num, 1);
}
//------------------------------------------------------------------
int commitReg(int fd, char* at, const char* result1, const char* result2, int num)
{
	int n = 0;
	while (num > 0)
	{
		send(fd, at);
		if (Find(result1)) n = 1;
		else if (Find(result2)) n = 2;

		if (n > 0) break;
		else num--;
	}
	return isOk(num, n);
}
//-----------------------------------------------------------------------------------------------------------
int commitGet(int fd, char* at, char* b, int ofset, int num)
{
	while (num > 0)
	{
		send(fd, at);
		readId(b, RxBuffer, ofset);
		if (Find("OK")) break;
		else   num--;
	}
	return isOk(num, 1);
}
//-----------------------------------------------------------------------------------------------------------
int commitCsq(int fd, int num)
{
	char buf[20];
	char* token;
	clear(buf, 20);
	while (num > 0)
	{
		send(fd, "AT+CSQ\r\n");
		readId(buf, RxBuffer, 6);
		token = strtok(buf, ",");
		clear(sys.csq, CSQ_LEN);
		strcpy(sys.csq, token);
		if (TEST) printf("csq = %s\r\n", sys.csq);

		if (Find("OK")) break;
		else   num--;
	}
	return isOk(num, 1);
}
//-----------------------------------------------------------------------------------------------------------
int commitLbs(int fd, int num)
{
	char buf[50];
	char* token;
	clear(buf, 50);
	while (num > 0)
	{
		send(fd, "AT+CLBS=1,1\r\n");
		if (!Find("+CLBS: 0")) { num = 0; break; }
		readId(buf, RxBuffer, 9);

		token = strtok(buf, ",");
		clear(sys.longitude, LOG_LEN);
		strcpy(sys.longitude, token);
		if (TEST) printf("lon = %s\r\n", sys.longitude);

		token = strtok(NULL, ",");
		clear(sys.latitude, LAT_LEN);
		strcpy(sys.latitude, token);
		if (TEST) printf("lat = %s\r\n", sys.latitude);

		if (Find("OK")) break;
		else   num--;
	}
	return isOk(num, 1);
}
//----------------------------------------------------------------------------------
void writeGsm()
{
	FILE *fp = NULL;
	char* json;
	cJSON* root = cJSON_CreateObject();
	//cJSON_AddItemToObject(root, "lon", cJSON_CreateString(sys.longitude));
	//cJSON_AddItemToObject(root, "lat", cJSON_CreateString(sys.latitude));

	strcat(sys.latitude, ",");
	strcat(sys.latitude, sys.longitude);
	cJSON_AddItemToObject(root, "location", cJSON_CreateString(sys.latitude));
	cJSON_AddItemToObject(root, "signal", cJSON_CreateString(sys.csq));
	int csq = atoi(sys.csq);
	char* result;
	if (csq > 26) result = "H";
	else if (csq > 12 ) result = "M";
	else result = "L";
	cJSON_AddItemToObject(root, "status", cJSON_CreateString(result));

	json = cJSON_PrintUnformatted(root);
	cJSON_Delete(root);

	fp = fopen("../data/gsm.json", "w");
	fputs(json, fp);
	cJSON_free(json);
	fclose(fp);
}
//----------------------------------------------------------------------------------
void writeStatus()
{
	FILE *fp = NULL;
	char* json;
	cJSON* root = cJSON_CreateObject();
	int tim = 240;
	char* work;
	if (tim < 240) work = "1"; else work = "0";

	cJSON_AddItemToObject(root, "moldClosed", cJSON_CreateString("1"));
	cJSON_AddItemToObject(root, "moldWorking", cJSON_CreateString(work));
	json = cJSON_PrintUnformatted(root);
	cJSON_Delete(root);
	
	fp = fopen("../data/status.json", "w");
	fputs(json, fp);
	fclose(fp);
}
//----------------------------------------------------------------------------------
void writeAlerts()
{
	FILE *fp = NULL;
	char* json;
	cJSON* root = cJSON_CreateObject();

	cJSON_AddItemToObject(root, "alertCode", cJSON_CreateString("E01"));
	json = cJSON_PrintUnformatted(root);
	cJSON_Delete(root);

	fp = fopen("../data/alerts.json", "w");
	fputs(json, fp);
	fclose(fp);
}
//----------------------------------------------------------------------------------
void writeICCID()
{
	FILE *fp = NULL;
	char* json;
	cJSON* root = cJSON_CreateObject();
	cJSON_AddItemToObject(root, "iccid", cJSON_CreateString(sys.imei));
	json = cJSON_PrintUnformatted(root);
	cJSON_Delete(root);

	fp = fopen("../data/ICCID.json", "w");
	fputs(json, fp);
	fclose(fp);
}

//------------------------------------------------------------------
void reg(int fd)
{
	static int step = 0;
	int n = 0;
	char* token;
	sys.isReg = 0;
	switch (step)
	{
	case 0:  if (commitAT(fd, "AT\r\n", "OK", 10)) step++; break;
	case 1:  if (commitAT(fd, "ATE0\r\n", "OK", 3)) step++; break;
	case 2:   if (commitCsq(fd, 3)) step++; break;

	case 3:  if (commitAT(fd, "AT+CPIN?\r\n", "+CPIN: READY", 3)) step++; break;
	case 4:  if (commitReg(fd, "AT+CREG?\r\n", "+CREG: 0,1", "+CREG: 0,5", 3)) step++; break;
	case 5:  clear(sys.imsi, IMSI_LEN);
		if (commitGet(fd, "AT+CIMI\r\n", sys.imsi, 0, 3)) step++; break;
	case 6:  clear(sys.imei, IMEI_LEN);
		if (commitGet(fd, "AT+GSN\r\n", sys.imei, 0, 3)) step++; break;

	case 7:  if (commitAT(fd, "AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r\n", "OK", 3)) step++; break;
	case 8:  if (commitAT(fd, "AT+SAPBR=3,1,\"APN\",\"CMNET\"\r\n", "OK", 3)) step++; break;

	case 9:   n = commitReg(fd, "AT+SAPBR=2,1\r\n", "+SAPBR: 1,3", "+SAPBR: 1,1", 3);
		if (n == 1) { if (commitAT(fd, "AT+SAPBR=1,1\r\n", "OK", 3)) step++; }
		else step++;
		break;
	case 10:  if (commitLbs(fd, 3)) step++; break;



	case 11: writeGsm();

		step = 0; sys.isReg = 1; break;
	}
}
//------------------------------------------------------------------
#define	LED	1

int main(void)
{
	int fd;
	if (wiringPiSetup() < 0)return 1;
	if ((fd = serialOpen("/dev/ttyAMA0", 115200)) < 0)
	{
		printf("error\n");
		return 1;
	}

	while (!sys.isReg)
	{
		reg(fd);
	}
	serialClose(fd);
	printf("OK\n");
	return 0;
}