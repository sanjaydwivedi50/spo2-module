#include "spo2-module.h"


void prepare_egress_pkt(uint8_t pkt_id, unsigned char *pkt) {
	uint8_t host_serial_no[4] = {0x0, 0x0, 0x0, 0x12};
	//get_host_serial_no(host_serial_no);
	
	//printf("Host Serial No : 0x%x%x%x%x\n",
	//			host_serial_no[0],
	//		       host_serial_no[1],
	//		       host_serial_no[2],
	//		       host_serial_no[3]);

	printf("Preparing handshake pkt\n");
	if (pkt_id == HANDSHAKE_RES_ID) {
		pkt[START_CHARACTER] 	= START_CHAR; 
		pkt[PACKET_LENGTH]	= 0xb;
		pkt[PARAMETER_TYPE]	= PARAM_TYPE; 
		pkt[PACKET_TYPE]	= DC;
		pkt[PACKET_ID]		= HANDSHAKE_RES_ID; 
		pkt[SERIAL_BYTE_1]	= host_serial_no[0];
		pkt[SERIAL_BYTE_2]	= host_serial_no[1];
		pkt[SERIAL_BYTE_3]	= host_serial_no[2];
		pkt[SERIAL_BYTE_4]	= host_serial_no[3];
		pkt[DATA]		= 0;
		pkt[CHECKSUM]		= checksum(pkt, 0xB);
	}
}

void parse_ingress_pkt(uint8_t *pkt, int rx_bytes) {

	int i = 0;
	int pkt_len = pkt[PACKET_LENGTH];
	int data_start_index = DATA;
	int data_end_index = pkt_len - 1;

	printf("pkt_len = %d\n", pkt_len);
	if (rx_bytes != pkt_len)
		return;

	printf("\nstart char : 0x%x\n", pkt[START_CHARACTER]); 
	printf("\npacket length : 0x%x\n", pkt[PACKET_LENGTH]);
	printf("\nparameter_type : 0x%x\n", pkt[PARAMETER_TYPE]); 
	printf("\npacket type : 0x%x\n", pkt[PACKET_TYPE]);
	printf("\npacket id : 0x%x\n", pkt[PACKET_ID]); 
	printf("\npacket serial number : 0x%x", pkt[SERIAL_BYTE_1]);
	printf("%x", pkt[SERIAL_BYTE_2]);
	printf("%x", pkt[SERIAL_BYTE_3]);
	printf("%x", pkt[SERIAL_BYTE_4]);

	printf("\npacket_data : ");
	if (pkt_len > 10) {
		for (i = data_start_index; i < data_end_index; i++)
			printf("0x%x ", pkt[i]);
	}else {
		printf("Empty");
	}
	printf("\npacket checksum : 0x%x\n", pkt[pkt_len - 1]);
}

void get_host_serial_no(unsigned char *serial_no) {
    FILE *cmd;
    char result[30];
    int i = 0, j = 0;

    cmd = popen("grep Serial /proc/cpuinfo | sed 's/.*://'", "r");
    if (cmd == NULL) {
        perror("popen");
        exit(EXIT_FAILURE);
    }
    while (fgets(result, sizeof(result), cmd)) {
	    printf(result);

	for (i=strlen(result) - 2; i>0; i--) {
		if (j == 8)
			break;
		serial_no[j++] = (FromHex(result[i])) |
			         (FromHex(result[--i]) << 4);
	}
    }

    pclose(cmd);
}

unsigned char FromHex(char c){
   switch(c){
      case '0': return 0;
      case '1': return 1;
      case '2': return 2;
      case '3': return 3;
      case '4': return 4;
      case '5': return 5;
      case '6': return 6;
      case '7': return 7;
      case '8': return 8;
      case '9': return 9;
      case 'a': return 10;
      case 'b': return 11;
      case 'c': return 12;
      case 'd': return 13;
      case 'e': return 14;
      case 'f': return 15;
   }
   return -1;
}

uint8_t checksum(uint8_t *data, int len) {
    uint16_t sum = 0x0;
    int pkt_len = len;
    while(len) {
	if ((len == 1) || (len == pkt_len)) {
        	len--;
		continue;
	}
        sum += *data++;
        len--;
    };
    return sum;
}
