/*
 * acelerometer.c
 * 
 * Copyright 2024  <rpi-student@raspberrypi>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */

#include "sensors.h"
#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <iostream>
#include <cstdio>
#include <cstdint>
#include <cmath>



int init_i2c_device(int device){
	char i2cFile[15];
	sprintf(i2cFile,"/dev/i2c-%d",device);
	int fd= open(i2cFile,O_RDWR);
	if(fd<0){
		std::cerr << "Error opening I2C address" << std::endl;
		return -1;
	}
	
	return fd;

}

int init_accel(int fd){
	unsigned char buffer_w[2]={0x6B,0x28};
	struct i2c_rdwr_ioctl_data packets;
	struct i2c_msg messages[2]; //Set size depending on number of messages in one transaction

	int f=ioctl(fd,I2C_SLAVE,ACCEL_ADDR); 
	if(f<0){
		std::cerr << "Error setting I2C address" << std::endl;
		close(fd);
		return -1;
	}

	messages[0].addr= ACCEL_ADDR; //address del slave
	messages[0].flags = 0; //means write
	messages[0].len = 2; //1 byte de escritura
	messages[0].buf = buffer_w ; //direccion del power managment

	packets.msgs = messages;
	packets.nmsgs = 1;

	if(ioctl(fd,I2C_RDWR,&packets)<0){
		std::cerr << "Error writting to I2C device" << std::endl;
		close(fd);
		return -1;
	}

	return fd;

}


AccelData read_accel_data(int fd){

	AccelData data;
	unsigned char write_bytes=0x3B;
	unsigned char read_bytes[6];
	struct i2c_rdwr_ioctl_data packets;
	struct i2c_msg messages[2]; //Set size depending on number of messages in one transaction

	messages[0].addr = ACCEL_ADDR;
	messages[0].flags = 0; //means write
	messages[0].len = 1;
	messages[0].buf =&write_bytes; //Pointer to the data bytes to be written

	messages[1].addr = ACCEL_ADDR;
	messages[1].flags = I2C_M_RD;
	messages[1].len = 6;
	messages[1].buf = read_bytes; //Pointer for reading the data

	packets.msgs = messages;
	packets.nmsgs = 2;

	if(ioctl(fd,I2C_RDWR,&packets)<0){
		std::cerr << "Error reading from I2C device" << std::endl;
		return data;
	}

	float x= (read_bytes[0] <<8) | read_bytes[1];
    float y= (read_bytes[2] <<8) | read_bytes[3];
    float z= (read_bytes[4] <<8) | read_bytes[5];

    data.x = round((int16_t)x / (float)16384 * 1000) / 1000.0;
    data.y = round((int16_t)y / (float)16384 * 1000) / 1000.0;
    data.z = round((int16_t)z / (float)16384 * 1000) / 1000.0;

    return data;

}

void write_register(int fd, unsigned char reg,unsigned char value){
	struct i2c_rdwr_ioctl_data packets;
	struct i2c_msg messages[1];
	unsigned char buffer_w[2];

	buffer_w[0]= reg;
	buffer_w[1]= value;

	messages[0].addr= COLOR_ADDR;
	messages[0].flags= 0;
	messages[0].len= 2;
	messages[0].buf= buffer_w;

	packets.msgs = messages;
	packets.nmsgs = 1;


	if(ioctl(fd,I2C_RDWR,&packets)<0){
		std::cerr << "Error writting to I2C device for the color sensor" << std::endl;
		close(fd);
	}
}

int init_color_sensor(int fd){

	int f=ioctl(fd,I2C_SLAVE,COLOR_ADDR); //
	if(f<0){
		std::cerr << "Error setting I2C address" << std::endl;
		close(fd);
		return -1;
	}

	write_register(fd,0x80,0x03);
	usleep(5000);
	write_register(fd,0x81,0x00);
	usleep(500);
	write_register(fd,0x83,0x07);
	usleep(500);
	write_register(fd,0x8F,0x00);
	usleep(500);
	write_register(fd,0x8D,0x11);
	usleep(500);

	return fd;
}

ColorData read_color_data(int fd){

	ColorData data;
	unsigned char read_bytes[6];
	unsigned char write_bytes=0x96;

	struct i2c_rdwr_ioctl_data packets;
	struct i2c_msg messages[2];

	messages[0].addr= COLOR_ADDR;
	messages[0].flags= 0;
	messages[0].len= 1;
	messages[0].buf= &write_bytes;

	messages[1].addr= COLOR_ADDR;
	messages[1].flags= I2C_M_RD;
	messages[1].len= 6;
	messages[1].buf= read_bytes;

	packets.msgs = messages;
	packets.nmsgs = 2;

	if(ioctl(fd,I2C_RDWR,&packets)<0){
		std::cerr << "Error reading from color sensors' I2C device" << std::endl;
		return data;
	}

	int red=   read_bytes[1] << 8 | read_bytes[0];
	int green= read_bytes[3] << 8 | read_bytes[2];
	int blue=  read_bytes[5] << 8 | read_bytes[4];

	float r = (float)red*255/65535.0;
	float g = (float)green*255/65535.0;
	float b = (float)blue*255/65535.0;

	data.r=(int)r;
	data.g=(int)g;
	data.b=(int)b;
	
	return data;
}



void close_i2c_device(int fd){
	close(fd);
}



