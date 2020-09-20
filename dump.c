//as usual...
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

//internet
#include <arpa/inet.h>
#include <sys/socket.h>

//port
uint16_t port_num=17000;		//default port

//UDP packet
uint8_t bits[54];

//internet
struct sockaddr_in si_me, si_other;
int s, i, slen = sizeof(si_other) , rcv_len;

//CRC
uint16_t CRC_LUT[256];
uint16_t poly=0x5935;

//test
//FILE *fp;

//stream info
struct moip_packet
{
	uint16_t sid;
	uint8_t src[10];
	uint8_t dst[10];
	uint16_t type;
	uint8_t nonce[14];
	uint16_t fn;
	uint8_t payload[16];
	uint16_t crc_udp;
} packet;

void CRC_Init(uint16_t *crc_table, uint16_t poly)
{
	uint16_t remainder;
	
	for(uint16_t dividend=0; dividend<256; dividend++)
	{
		remainder=dividend<<8;
		
		for(uint8_t bit=8; bit>0; bit--)
		{		
			if(remainder&(1<<15))
				remainder=(remainder<<1)^poly;
			else
				remainder=(remainder<<1);
		}
		
		crc_table[dividend]=remainder;
	}
}

uint16_t CRC_M17(uint16_t* crc_table, const uint8_t* message, uint16_t nBytes)
{
	uint8_t data;
	uint16_t remainder=0xFFFF;

	for(uint16_t byte=0; byte<nBytes; byte++)
	{
		data=message[byte]^(remainder>>8);
		remainder=crc_table[data]^(remainder<<8);
	}

	return(remainder);
}

uint8_t* decode_callsign_base40(uint64_t encoded, uint8_t *callsign)
{
	if(encoded >= 262144000000000)
	{
		*callsign=0;
		return callsign;
	}

	uint8_t *p = callsign;

	for (; encoded>0; p++)
	{
		*p = "xABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-/."[encoded % 40];
		encoded/=40;
	}

	*p = 0;

	return callsign;
}

// ./this.out port
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
	memset(bits, 0, 54);
	CRC_Init(CRC_LUT, poly);
	
	s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	
	if(s==-1)
	{
		fprintf(stderr, "Socket error\n");
		return 1;
	}
	
	//zero out the structure
	memset((char*)&si_me, 0, sizeof(si_me));
	
	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(port_num);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);
	
	if(bind(s, (struct sockaddr*)&si_me, sizeof(si_me))==-1)
	{
		fprintf(stderr, "Can't bind to port %d\n", port_num);
		return 1;
	}

	printf("Listening for M17 frames on port %d:\n", port_num);
	printf("Src\t\tDst\t\tType\tPld\n");

	while(1)
	{
		//receive packet via UDP
		if((rcv_len=recvfrom(s, bits, 54, 0, (struct sockaddr*)&si_other, &slen))==-1)
		{
			fprintf(stderr, "What the hell?\n");
			return 1;
		}

		if(rcv_len==54)
		{
			packet.sid=(bits[4]<<8)|bits[5];
			uint64_t tmp;
			tmp=(bits[6]<<(5*8))|(bits[7]<<(4*8))|(bits[8]<<(3*8))|(bits[9]<<(2*8))|(bits[10]<<(1*8))|bits[11];
			decode_callsign_base40(tmp, packet.src);
			tmp=(bits[12]<<(5*8))|(bits[13]<<(4*8))|(bits[14]<<(3*8))|(bits[15]<<(2*8))|(bits[16]<<(1*8))|bits[17];
			decode_callsign_base40(tmp, packet.dst);
			packet.type=(bits[18]<<8)|bits[19];

			memcpy(packet.nonce, &bits[20], 14);
			packet.fn=bits[34]<<8|bits[35];
			memcpy(packet.payload, &bits[36], 16);
			packet.crc_udp=bits[52]<<8|bits[53];

			//info
			printf("%s\t\t%s\t\t%04X\t", packet.src, packet.dst, packet.type);
			for(uint8_t i=0; i<128/8; i++)
			{
				printf("%02X", packet.payload[i]);
				if(i==(128/8-1))
					printf("\n");
			}
			
			/*fp=fopen("out.raw", "a");
			fwrite(speech_buff, 2, 160, fp);
			fclose(fp);*/
		}
	}

	
	return 0;
}
