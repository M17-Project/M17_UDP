//as usual...
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

//internet
#include <arpa/inet.h>
#include <sys/socket.h>

//Codec2
#include "codec2.h"

//port
uint16_t port_num=17000;		//default port

//UDP packet
uint8_t bits[40];

//for reconstructed speech samples, 2x160 (2x20ms)
uint16_t speech_buff[320];

//Codec2 structure
struct CODEC2 *cod;

//internet
struct sockaddr_in si_me, si_other;
int s, i, slen = sizeof(si_other) , rcv_len;

//test
//FILE *fp;

int main(int argc, char *argv[])
{
	if(argc==2)
	{
		port_num=atoi(argv[1]);
	}
	else
	{
		fprintf(stderr, "Not enough args\n");
		return 1;
	}

	//init
	cod = codec2_create(CODEC2_MODE_3200);
	memset(bits, 0, 40);
	memset(speech_buff, 0, 320);
	
	s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	
	if(s==-1)
	{
		return 1;
	}
	
	//zero out the structure
	memset((char*)&si_me, 0, sizeof(si_me));
	
	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(port_num);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);
	
	if(bind(s, (struct sockaddr*)&si_me, sizeof(si_me))==-1)
	{
		
		return 1;
	}

	while(1)
	{
		//receive packet via UDP
		if((rcv_len=recvfrom(s, bits, 40, 0, (struct sockaddr*)&si_other, &slen))==-1)
		{
			return 1;
		}

		if(rcv_len==34)
		{/*
			//reconstruct speech
			codec2_decode(cod, &speech_buff[0], &bits[0]);
			codec2_decode(cod, &speech_buff[160], &bits[8]);
			
			//send to stdout for playback
			for(uint16_t i=0; i<320*2; i++)
			{
				//signed 16bit, little endian
				if(!(i%2))//lsb
					printf("%c", (uint8_t)(speech_buff[i/2]&0xFF));
				else
					printf("%c", (uint8_t)(speech_buff[i/2]>>8));
			}
			/*fp=fopen("out.raw", "a");
			fwrite(speech_buff, 2, 160, fp);
			fclose(fp);*/

			printf("Frame recv'd!\n");
		}
		else if(rcv_len==40)
		{
			printf("LICH recv'd!\n");
		}
	}
	
	return 0;
}
