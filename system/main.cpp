#include <stdio.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <string.h>
#include "cJSON.h"
#include "data.h"

data_t sys;

//----------------------------------------------------------------------------------
void writeSystem()
{
	FILE *fp = NULL;
	char* json;
	cJSON* root = cJSON_CreateObject();


	cJSON_AddItemToObject(root, "firmwareVersion", cJSON_CreateString(sys.strFir));
	cJSON_AddItemToObject(root, "appVersion", cJSON_CreateString(sys.strApp));
	json = cJSON_PrintUnformatted(root);
	cJSON_Delete(root);

	fp = fopen("../data/system.json", "w");
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
	writeSystem();


//	while (true)
	{

	}
	serialClose(fd);
	printf("OK\n");
	return 0;
}