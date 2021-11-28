#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

enum module_pkt_index{
	START_CHARACTER,
	PACKET_LENGTH,
	PARAMETER_TYPE,
	PACKET_TYPE,
	PACKET_ID,
	SERIAL_BYTE_1,
	SERIAL_BYTE_2,
	SERIAL_BYTE_3,
	SERIAL_BYTE_4,
	DATA,
	CHECKSUM
};

#define START_CHAR 		0xFA
#define PARAM_TYPE		0x03
#define DC 			0x01
#define DR 			0x02
#define DA 			0x03
#define DD 			0x04
#define DATA_DEFAULT		0x00
#define CMD_SUCCESS		0x07
#define CMD_FAILURE		0x06

//patient type data
#define PATIENT_TYPE_ADULT		0x00
#define PATIENT_TYPE_CHILD		0x01
#define PATIENT_TYPE_NEONATE		0x02

//sesitivity setting data
#define LOW_SENSITIVITY			0x00
#define MIDDLE_SENSITIVITY		0x00
#define HIGH_SENSITIVITY		0x00
#define HIGHEST_SENSITIVITY		0x00

//packet ID
#define ANSWER_ID			0x80 //DA
#define HANDSHAKE_REQ_ID		0x81 //DD
#define VERSION_INFO_ID			0x82 //DA
#define MODULE_SELF_TEST_RES_ID		0x83 //DA
#define REALTIME_WAVEFORM_ID		0x84 //DD
#define MESUREMENT_RESULT_ID		0x85 //DD
#define VARIATION_INDEX_RESULT_ID	0x86 //DD
#define REPORT_SENSOR_ERR_ID		0x87 //DA
#define TEMPERATURE_PKT_ID		0x80 //DD
#define HANDSHAKE_RES_ID		0x01
#define PKT_SELF_TEST_RES_ENQ_ID	0x03
#define PATIENT_TYPE_SETTING_ID		0x04
#define SESITIVITY_SETTING_ID		0x05

void get_host_serial_no(unsigned char *);
unsigned char FromHex(char c);
void prepare_egress_pkt(uint8_t pkt_id, unsigned char *pkt);
uint8_t checksum(uint8_t *data, int len);
void parse_ingress_pkt(uint8_t *pkt, int rx_bytes);

