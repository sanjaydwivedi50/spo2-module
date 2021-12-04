#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <asm/ioctls.h>
#include <linux/ioctl.h>

#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include "spo2-module.h"

#define UC500_GET_UART_TYPE	0xe001  
#define UC500_SET_UART_TYPE	0xe002	

#define aTime(a,b) (((b.tv_sec - a.tv_sec) > 0 ? (b.tv_sec - a.tv_sec) : 0) * 1000000) + (b.tv_usec - a.tv_usec)

/*
 * 	Send data to serial port
 *  	fd      => file descriptor.
 *  	buffer  => the data that is sent.
 * 		size    => max size of data.
 * 		speed 	=> selected baudrate.
 */
int send_data(int fd, unsigned char *buffer, int size, int speed) {
	struct timeval start, stop;
	unsigned long times = 0;
	int i, rtn = 0;

	times = (1100000 / (speed / 10)) * (size);

	rtn = write(fd, buffer, size);
	gettimeofday(&start, NULL);

	if (times < 20000) {
		for (i = 0; i < 100; i++);

		while (aTime(start, stop) < times) {
			for (i = 0; i < 100; i++);
			gettimeofday(&stop, NULL);
		}
	}
	else {
		tcdrain(fd);
		gettimeofday(&stop, NULL);
	}

	if (rtn == -1)
		perror("send_data");

	return rtn;
}

/*
 * 	Recieve data from serial port
 * 		fd 		=> file descriptor.
 * 		buffer 	=> container of data.
 * 		size 	=> max size of data.
 * 		timeout => limit of time that wait to recieve data complete. unit: micro second
 */
int recv_data(int fd, char *buffer, int size, unsigned long timeout) {
	int recv = 0, recv_len = 0, read_size = 0, remain_len = size;
	struct timeval start, stop;
	int i;

	gettimeofday(&start, NULL);

	while (remain_len) {
		read_size = remain_len < size ? remain_len : size;
		if ((recv = read(fd, buffer + recv_len, read_size)) == -1) {
			perror("recv_data");
			return -1;
		}

		if (recv > 0) {
			recv_len += recv;
			remain_len -= recv;
		}

		for (i = 0; i < 100; i++)
			;

		gettimeofday(&stop, NULL);

		if (aTime(start, stop) > timeout)
			break;
	}

	return recv_len;
}


/*
 * Get baudrate value
 */
long baudrate(long speed) {
	if(speed ==  0)
		return  B0;
	if(speed ==  50)
		return B50;
	if(speed ==  75)			
		return B75;
	if(speed ==  110)			
           	return B110;
	if(speed ==  134)			
           	return B134;
	if(speed ==  150)			
            return B150;
	if(speed ==  200)			
           	return B200;
	if(speed ==  300)			
           	return B300;
	if(speed ==  600)			
            return B600;
	if(speed ==  1200)			
          	return B1200;
	if(speed ==  1800)			
         	return B1800;
	if(speed ==  2400)			
          	return B2400;
	if(speed ==  4800)			
           	return B4800;
	if(speed ==  9600)			
           	return B9600;
	if(speed ==  19200)			
          	return B19200;
	if(speed ==  38400)			
            return B38400;
	if(speed ==  57600)			
          	return B57600;
	if(speed ==  115200)			
            return B115200;
	if(speed ==  230400)			
          	return B230400;
	if(speed ==  460800)			
          	return B460800;
	if(speed ==  921600)			
          	return B921600;
          	
	return speed;
	
}

/*
m5_uart_Config : example to config uart port

fd : file descriptor
baud : baudrate
serial_mode : n81 , o81 , e81 , n71 , o71 .........
serial_type : set to RS232 , RS485 , RS422
*/
int m5_uart_Config (int fd, int baud , char * serial_mode , int serial_type){
	struct termios term;
	long BAUD , DATABITS , STOPBITS , PARITYON , PARITY;
	int flag = 0 , i , j = 0 , interface;
	
	BAUD = baudrate(baud);
	
	if(tcgetattr(fd,&term) != 0) {
		close(fd);
		perror("m5_uart_Config");
		return -1;
	}
	
	switch (serial_mode[1])
	{
		case '8':
			DATABITS = CS8;
		break;
		case '7':
			DATABITS = CS7;
		break;
		case '6':
			DATABITS = CS6;
		break;
		case '5':
			DATABITS = CS5;
		break;
		default :
			flag = 1;
		break;
	}  //end of switch data_bits
	
	switch (serial_mode[2])
	{
		case '1':
			STOPBITS = 0;
		break;
		case '2':
			STOPBITS = CSTOPB;
		break;
		default :
			flag = 1;
		break;
	}  //end of switch stop bits
	
	switch (serial_mode[0])
	{
		case 'N':
		case 'n':
			PARITYON = 0;
			PARITY = 0;
		break;
		case 'O': 
		case 'o': //odd
			PARITYON = PARENB;
			PARITY = PARODD;
		break;
		case 'E': //even
		case 'e':
			PARITYON = PARENB;
			PARITY = 0;
		break;
		default :
			flag = 1;
		break;
	}  //end of switch parity
	
	if(serial_type != 232) {
		term.c_iflag &= ~(IXON|IXOFF|IXANY);
		term.c_cflag &= ~CRTSCTS;
	}
	
	if(flag == 1) {
		printf("m5_uart_Config : Error config %s.\n",serial_mode);
		close(fd);
		return -1;
	}
	
	term.c_cflag = BAUD | DATABITS | STOPBITS | PARITYON | PARITY | CLOCAL | CREAD | HUPCL;
	term.c_iflag = IGNPAR;
	term.c_oflag = 0;
	term.c_lflag = 0;       //ICANON;
	term.c_cc[VMIN]= 0;
	term.c_cc[VTIME]= 0;
	
	switch(serial_type) {
		case 422:
			interface = 422;
			break;
		case 485:
			interface = 485;
			break;
		default:
			interface = 232;
		break;
	}
	
	tcflush(fd, TCIFLUSH); // clean recv

	if(tcsetattr(fd,TCSANOW,&term) != 0) {
		perror("m5_uart_Config");
		close(fd);
		return -1;
	} 
	
	tcflush(fd, TCIFLUSH); // clean recv
	return fd;
}

int main(int argc, char *argv[]){
	int fd = -1, speed = 9600;
	char buf[256];
	unsigned char handshake[13] = "\xfa\x0b\x03\x01\x01\x2f\x00\x00\x00\x00\x3f\n";
	int recv;
	int i = 0, j = 0;

	if(argc < 2) {
		printf("command : %s /dev/ttySx\n",argv[0]);
		return 1;
	}

	printf("Open %s\n",argv[1]);
	fd = open(argv[1], O_RDWR | O_NOCTTY | O_NDELAY | O_SYNC);

	if (fd == -1) {
		printf("open %s Failed, errno: %d\r\n", argv[1], errno);
		return 0;
	}

	if(m5_uart_Config(fd, speed, "o81", 232) == -1)
		return -1;
	
	while (1){
		bzero(buf, 256);
		recv = recv_data(fd, buf, 255, 1000000); // wait 1 second
	
		if (buf[PACKET_ID] == HANDSHAKE_REQ_ID)
			send_data(fd, handshake, 13, speed);

		sleep(5);

		printf("recieved data : %s\n", buf);
		printf("data length: %d %s\n", recv, recv > 1 ? "bytes" : "byte");

		if(recv < 0) {
			continue;
		}else {
			printf("%i bytes read \n", recv);
			for (i=0;i<recv;i++)
			    printf(" 0x%x ", buf[i]);
			printf("\n \n \n ");
		}

		parse_ingress_pkt(buf, recv);
	
		printf("\n");
	}
	close(fd);	
	printf("Done.\n");

	return 0;
}
