#include <stdint.h>

#define  MSG_LEN   50
#define  CSQ_LEN   10
#define  IMSI_LEN    30
#define  IMEI_LEN  30
#define  LOG_LEN    40
#define  LAT_LEN     40



typedef struct
{
	uint8_t eId;
          uint8_t isReg;
          char msg[MSG_LEN];
	char csq[CSQ_LEN];
	char imsi[IMSI_LEN];      //�����ƶ��豸��ʶ
          char imei[IMEI_LEN];
	char longitude[LOG_LEN];  //����
	char latitude[LAT_LEN];   //γ��

}data_t;
