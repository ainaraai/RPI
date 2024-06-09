#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#define ACCEL_ADDR 0x68
#define COLOR_ADDR 0x29



struct AccelData{
	float x;
	float y;
	float z;
};

struct ColorData{
	int r;
	int g;
	int b;
};

struct Data{
	AccelData accel[TIMES];
	ColorData color[TIMES];
};
//function to initialize the I2C device
int init_i2c_device(int device);

//initialization accelerometer sensor
int init_accel(int fd);

//function to read acceleration data
AccelData read_accel_data(int fd);

//function to close the I2C device
void close_i2c_device(int fd);

void write_register_color(int file,unsigned char reg,unsigned char value);
ColorData read_color_data(int fd);
int init_color_sensor(int fd);

#endif //ACCELEROMETER_H
