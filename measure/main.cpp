#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <string.h>
#include "cJSON.h"
#include "data.h"

data_t sys;

float getValue() {
	srand(micros());
	return rand()%500+2.3;
}
//----------------------------------------------------------------------------------
void writeMes()
{
	FILE *fp = NULL;
	char* json;
	cJSON* root = cJSON_CreateObject();

	sys.t1 = getValue();
	sys.t2 = getValue();
	sys.t3 = getValue();
	sys.t4 = getValue();

	sys.p1 = getValue();
	sys.p2 = getValue();
	sys.p3 = getValue();
	sys.p4 = getValue();

	sys.cp = getValue();
	sys.ct = getValue();

	sys.wf1 = getValue();
	sys.wf2 = getValue();
	sys.wf3 = getValue();
	sys.wf4 = getValue();

	sys.wt1 = getValue();
	sys.wt2 = getValue();
	sys.wt3 = getValue();
	sys.wt4 = getValue();

	cJSON_AddItemToObject(root, "t1", cJSON_CreateNumber(sys.t1));
	cJSON_AddItemToObject(root, "t2", cJSON_CreateNumber(sys.t2));
	cJSON_AddItemToObject(root, "t3", cJSON_CreateNumber(sys.t3));
	cJSON_AddItemToObject(root, "t4", cJSON_CreateNumber(sys.t4));

	cJSON_AddItemToObject(root, "p1", cJSON_CreateNumber(sys.p1));
	cJSON_AddItemToObject(root, "p2", cJSON_CreateNumber(sys.p2));
	cJSON_AddItemToObject(root, "p3", cJSON_CreateNumber(sys.p3));
	cJSON_AddItemToObject(root, "p4", cJSON_CreateNumber(sys.p4));

	cJSON_AddItemToObject(root, "cp", cJSON_CreateNumber(sys.cp));
	cJSON_AddItemToObject(root, "ct", cJSON_CreateNumber(sys.ct));

	cJSON_AddItemToObject(root, "wf1", cJSON_CreateNumber(sys.wf1));
	cJSON_AddItemToObject(root, "wf2", cJSON_CreateNumber(sys.wf2));
	cJSON_AddItemToObject(root, "wf3", cJSON_CreateNumber(sys.wf3));
	cJSON_AddItemToObject(root, "wf4", cJSON_CreateNumber(sys.wf4));

	cJSON_AddItemToObject(root, "wt1", cJSON_CreateNumber(sys.wt1));
	cJSON_AddItemToObject(root, "wt2", cJSON_CreateNumber(sys.wt2));
	cJSON_AddItemToObject(root, "wt3", cJSON_CreateNumber(sys.wt3));
	cJSON_AddItemToObject(root, "wt4", cJSON_CreateNumber(sys.wt4));

	json = cJSON_PrintUnformatted(root);
	cJSON_Delete(root);

	fp = fopen("../data/mes.json", "w");
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
	writeMes();

	



//	while (true)
	{
		
	}
	serialClose(fd);
	printf("OK\n");
	return 0;
}