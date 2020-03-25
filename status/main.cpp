#include <stdio.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <string.h>
#include "cJSON.h"
#include "data.h"

data_t sys;

//----------------------------------------------------------------------------------
void writeStatus()
{
	FILE *fp = NULL;
	char* json;
	cJSON* root = cJSON_CreateObject();

	if (sys.time < 240) sys.working = 1; else sys.working = 0;

	cJSON_AddItemToObject(root, "moldClosed", cJSON_CreateNumber(sys.closed));
	cJSON_AddItemToObject(root, "moldWorking", cJSON_CreateNumber(sys.working));
	json = cJSON_PrintUnformatted(root);
	cJSON_Delete(root);

	fp = fopen("../data/status.json", "w");
	fputs(json, fp);
	fclose(fp);
}

int main(void)
{
	int fd;
	if (wiringPiSetup() < 0)return 1;
	if ((fd = serialOpen("/dev/ttyAMA0", 115200)) < 0)
	{
		printf("error\n");
		return 1;
	}
	writeStatus();


//	while (true)
	{

	}
	serialClose(fd);
	printf("OK\n");
	return 0;
}