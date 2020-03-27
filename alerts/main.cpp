#include <stdio.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <string.h>
#include "cJSON.h"
#include "data.h"

data_t sys;
//----------------------------------------------------------------------------------
void writeAlerts()
{
	FILE *fp = NULL;
	char* json;
	cJSON* root = cJSON_CreateObject();

	cJSON_AddItemToObject(root, "alertCode", cJSON_CreateString(sys.msg));
	json = cJSON_PrintUnformatted(root);
	cJSON_Delete(root);

	fp = fopen("../data/alerts.json", "w");
	fputs(json, fp);
	cJSON_free(json);
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
	writeAlerts();


//	while (true)
	{

	}
	serialClose(fd);
	printf("OK\n");
	return 0;
}