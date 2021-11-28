#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

#include <termios.h>

#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include "uart-comm.h"
#include "spo2-module.h"

#define SERIAL_DEVICE "/dev/ttyUSB0"

int serial_fd = 0;
int serialOpen(char *, int);
int serialTxData (int serial_fd, unsigned char tx_data[], size_t tx_size);
void sighandler (int sig) {
	close(serial_fd);
	exit(0);
}

unsigned char handshake[] = {
			0xFA, 0x0B, 0x03, 
			0x01, 0x01, 0x2F, 
			0x00, 0x00, 0x00, 
			0x00, 0x3F
			};

int main(int argc, char ** argv) {
	int i = 0;
	char data;
	unsigned char rx_buffer[20];
	unsigned char tx_buffer[20];
	int rx_length;
	int tx_length;

	signal(SIGINT, sighandler);

	if ((serial_fd = serialOpen (SERIAL_DEVICE, 9600)) < 0) {
		fprintf(stderr, "Unable to upen serial device %s: %s ",
				SERIAL_DEVICE, strerror(errno));
		return 1;
	}

	while(1) {
		printf("Handshaking in progress..");
		prepare_egress_pkt(HANDSHAKE_RES_ID, tx_buffer);
		printf("Egress Packet prepared\n");
		//parse_ingress_pkt(tx_buffer, 11);

		usleep(1500);
		set_blocking(serial_fd, 1);
		tx_length = serialTxData (serial_fd, tx_buffer, 11);

		if(tx_length != 11) {
			printf("Tx failed for Handshake.\n");
		}else {
			printf("Handshake data sent.\n");
		}
		for (i=0;i<tx_length;i++)
		    printf(" 0x%x ", tx_buffer[i]);
		printf("\n");

		usleep(1500);
		set_blocking(serial_fd, 0);
		break;
#if 0
		rx_length = read(serial_fd, rx_buffer, 12);
		if(rx_length < 0) {
			continue;
		}else {
			printf("%i bytes read \n", rx_length);
			for (i=0;i<rx_length;i++)
			    printf(" 0x%x ", rx_buffer[i]);
			printf("\n");
		}
		parse_ingress_pkt(rx_buffer, rx_length);
		if (rx_buffer[PACKET_ID] != HANDSHAKE_REQ_ID)
			break;
#endif
	}
#if 1
	while (1) {
		usleep(1500);
		rx_length = read(serial_fd, rx_buffer, 20);
		usleep(1500);
		if(rx_length < 0) {
			continue;
		}else {
			printf("%i bytes read \n", rx_length);
			for (i=0;i<rx_length;i++)
			    printf(" 0x%x ", rx_buffer[i]);
			printf("\n \n \n ");
		}
		parse_ingress_pkt(rx_buffer, rx_length);
		printf("\n=======================================\n");
		rx_length = 0;
		//break;
	}
#endif
	close(serial_fd);

	return 0;
}

int serialOpen(char *serial_dev, int baudrate) {
	int fd = -1;
	struct termios term;
	fd = open(serial_dev, O_RDWR | O_NOCTTY);
	if (fd == -1) {
		printf("Error - Unable to open UART.\n");
		return fd;
	}
	
	set_interface_attribs(fd, B9600, 1);
/*	
	tcgetattr(serial_fd, &term);
	term.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
	term.c_iflag = IGNPAR;
	term.c_lflag = 0;
	tcflush(serial_fd, TCIFLUSH);
	tcsetattr(serial_fd, TCSANOW, &term);
*/
	return fd;
}

int serialTxData (int fd, unsigned char tx_data[], size_t tx_size) {
	int count, i;
	unsigned char *p_tx_buffer;

	p_tx_buffer = &tx_data[0];

	for(i=0; i<tx_size; i++){
		*p_tx_buffer++ = tx_data[i];
	}
	count = write(fd, tx_data, (p_tx_buffer - &tx_data[0]));
	return count;
}
