#include <stdint.h>

typedef struct
{
	int16_t firV; //firmwareVersion
	int16_t appV;   //appVersion
	char strFir[10];
	char strApp[10];


}data_t;
