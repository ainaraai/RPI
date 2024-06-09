/* UDP client in the internet domain */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <iomanip>

#include "sensors.h"

#define SECONDS 1

void error(const char *);
int main(int argc, char *argv[])
{
   int sock, n;
   unsigned int length;
   struct sockaddr_in server, from;
   struct hostent *hp;
   char buffer[1024];

   if (argc != 3) { printf("Usage: server port\n");
                    exit(1);
   }
   sock= socket(AF_INET, SOCK_DGRAM, 0);
   if (sock < 0) error("socket");

   server.sin_family = AF_INET;
   hp = gethostbyname(argv[1]);
   if (hp==0) error("Unknown host");

   bcopy((char *)hp->h_addr,
        (char *)&server.sin_addr,
         hp->h_length);
   server.sin_port = htons(atoi(argv[2]));
   length=sizeof(struct sockaddr_in);

   int i2c_device=1;
   int fd_accel=init_i2c_device(i2c_device);
   printf("Accel's I2C device initialized successfully\n");

   if(fd_accel<0){
	   std::cerr << "Failed to initialize accel's I2C device" << std::endl;
	   close(sock);
	   return -1;
   }

   int fd_color=init_i2c_device(i2c_device);
   printf("Color's I2C device initialized successfully\n\n");

   if(fd_color<0){
	   std::cerr << "Failed to initialize color's I2C device" << std::endl;
	   close(sock);
	   return -1;
   }

   init_accel(fd_accel);
   init_color_sensor(fd_color);

   Data combinedData;

   while(1){

	   for(int i=0;i<10;i++){

		   combinedData.accel[i]=read_accel_data(fd_accel);
		   std::cout << "\nSample " << i + 1 << ": X=" << combinedData.accel[i].x << " Y=" << combinedData.accel[i].y << "  Z=" << combinedData.accel[i].z  << std::endl;

		   combinedData.color[i]= read_color_data(fd_color);

		   std::cout << "          SAMPLE NUMBER " << i + 1 << std::endl;
		   std::cout << "+----------+----------+----------+" << std::endl;
		   std::cout << "|   Color R|   Color G|   Color B|" << std::endl;
		   std::cout << "+----------+----------+----------+" << std::endl;
		   std::cout << "|"
					 << std::setw(10) << combinedData.color[i].r << "|"
					 << std::setw(10) << combinedData.color[i].g << "|"
					 << std::setw(10) << combinedData.color[i].b << "|"
					 << std::endl;
		   std::cout << "+----------+----------+----------+" << std::endl;
		   sleep(SECONDS);
	   }

	   n=sendto(sock,&combinedData,sizeof(combinedData),0,(const struct sockaddr *)&server,length);
	   if (n < 0){
		   error("Sendto");
		   printf("error send");
	   }


	   n = recvfrom(sock,buffer,256,0,(struct sockaddr *)&from, &length);
	   if (n < 0) error("recvfrom");

	   buffer[n]='\0';
	   std::cout << "Received data: " << buffer << std::endl;

   }

   close_i2c_device(fd_accel);
   close_i2c_device(fd_color);
   close(sock);
   return 0;
}

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

