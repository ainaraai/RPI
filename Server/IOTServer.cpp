/* Creates a datagram server.  The port
   number is passed as an argument.  This
   server runs forever */

#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <math.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>
#define TIMES 10
void error(const char *msg)
{
    perror(msg);
    exit(0);
}

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

void print_parsed_accel_data(const AccelData* data, size_t size){
	std::cout << "Parsed accelerometer data:" << std::endl;
/*	for(size_t i=0;i< size;i++){
		std::cout << "Sample " << i + 1 << ": X=" << data[i].x << " Y=" << data[i].y << " Z=" << data[i].z << std::endl;
	}*/
    std::cout << std::left << std::setw(10) << "Sample"
              << std::setw(10) << "X"
              << std::setw(10) << "Y"
              << std::setw(10) << "Z"
              << std::endl;

    // Print separator
    std::cout << std::string(40, '-') << std::endl;

    // Print table rows
    for(size_t i = 0; i < size; i++) {
        std::cout << std::left << std::setw(10) << i + 1
                  << std::setw(10) << std::fixed << std::setprecision(3) << data[i].x
                  << std::setw(10) << std::fixed << std::setprecision(3) << data[i].y
                  << std::setw(10) << std::fixed << std::setprecision(3) << data[i].z
                  << std::endl;
    }
    std::cout << "\n" << std::endl;

}

void print_parsed_color_data(const ColorData* data, size_t size){
    std::cout << "Parsed color sensor data:" << std::endl;
    for(size_t i=0;i< size;i++){
        std::cout << "Sample " << i + 1 << ": R=" << data[i].r << " G=" << data[i].g << " B=" << data[i].b << std::endl;
    }
}

void calculate_and_print_stats(const AccelData* data, size_t size) {
    float sum_x = 0, sum_y = 0, sum_z = 0;
    float max_x = data[0].x, max_y = data[0].y, max_z = data[0].z;
    float min_x = data[0].x, min_y = data[0].y, min_z = data[0].z;
    std::vector<float> x_values, y_values, z_values;

    for (size_t i = 0; i < size; i++) {
    	 const auto& d = data[i];
        sum_x += d.x;
        sum_y += d.y;
        sum_z += d.z;

        if (d.x > max_x) max_x = d.x;
        if (d.y > max_y) max_y = d.y;
        if (d.z > max_z) max_z = d.z;

        if (d.x < min_x) min_x = d.x;
        if (d.y < min_y) min_y = d.y;
        if (d.z < min_z) min_z = d.z;

        x_values.push_back(d.x);
        y_values.push_back(d.y);
        z_values.push_back(d.z);
    }

    float mean_x = sum_x / size;
    float mean_y = sum_y / size;
    float mean_z = sum_z / size;

    float variance_x = 0, variance_y = 0, variance_z = 0;
    for (const auto& val : x_values) variance_x += (val - mean_x) * (val - mean_x);
    for (const auto& val : y_values) variance_y += (val - mean_y) * (val - mean_y);
    for (const auto& val : z_values) variance_z += (val - mean_z) * (val - mean_z);

    variance_x /= size;
    variance_y /= size;
    variance_z /= size;

    float std_dev_x = std::sqrt(variance_x);
    float std_dev_y = std::sqrt(variance_y);
    float std_dev_z = std::sqrt(variance_z);

    std::cout << std::fixed << std::setprecision(3);
    int width = 10;
    int precision = 3;

    std::cout << "\nStatistics over the last 10 seconds for accelerometer sensor:" << std::endl;

    // Print the table header
    std::cout << std::left << std::setw(width) << "Stat"
              << std::setw(width) << "X"
              << std::setw(width) << "Y"
              << std::setw(width) << "Z" << std::endl;

    std::cout << std::string(3 * width, '-') << std::endl;

    // Print the mean values
    std::cout << std::left << std::setw(width) << "Mean"
              << std::fixed << std::setprecision(precision)
              << std::setw(width) << mean_x
              << std::setw(width) << mean_y
              << std::setw(width) << mean_z << std::endl;

    // Print the max values
    std::cout << std::left << std::setw(width) << "Max"
              << std::fixed << std::setprecision(precision)
              << std::setw(width) << max_x
              << std::setw(width) << max_y
              << std::setw(width) << max_z << std::endl;

    // Print the min values
    std::cout << std::left << std::setw(width) << "Min"
              << std::fixed << std::setprecision(precision)
              << std::setw(width) << min_x
              << std::setw(width) << min_y
              << std::setw(width) << min_z << std::endl;

    // Print the standard deviation values
    std::cout << std::left << std::setw(width) << "Std Dev"
              << std::fixed << std::setprecision(precision)
              << std::setw(width) << std_dev_x
              << std::setw(width) << std_dev_y
              << std::setw(width) << std_dev_z << std::endl;
}


void calculate_and_print_color_stats(const ColorData* data, size_t size){
    float sum_r = 0, sum_g = 0, sum_b = 0;
    float max_r = data[0].r, max_g = data[0].g, max_b = data[0].b;
    float min_r = data[0].r, min_g = data[0].g, min_b = data[0].b;
    std::vector<int> r_values, g_values, b_values;

    for (size_t i = 0; i < size; i++) {
    	const auto& d = data[i];
        sum_r += d.r;
        sum_g += d.g;
        sum_b += d.b;

        if (d.r > max_r) max_r = d.r;
        if (d.g > max_g) max_g = d.g;
        if (d.b > max_b) max_b = d.b;

        if (d.r < min_r) min_r = d.r;
        if (d.g < min_g) min_g = d.g;
        if (d.b < min_b) min_b = d.b;

        r_values.push_back(d.r);
        g_values.push_back(d.g);
        b_values.push_back(d.b);
    }

    int mean_r = sum_r / size;
    int mean_g = sum_g / size;
    int mean_b = sum_b / size;

    float variance_r = 0, variance_g = 0, variance_b = 0;
    for (const auto& val : r_values) variance_r += (val - mean_r) * (val - mean_r);
    for (const auto& val : g_values) variance_g += (val - mean_g) * (val - mean_g);
    for (const auto& val : b_values) variance_b += (val - mean_b) * (val - mean_b);

    variance_r /= size;
    variance_g /= size;
    variance_b /= size;

    int std_dev_r = sqrt(variance_r);
    int std_dev_g = sqrt(variance_g);
    int std_dev_b = sqrt(variance_b);

    std::cout << std::fixed << std::setprecision(0);

    std::cout << "\nStatistics over the last 10 seconds for color sensor:" << std::endl;
    
    int width = 10;
    int precision = 0;

    // Print the table header
    std::cout << std::left << std::setw(width) << "Stat"
              << std::setw(width) << "R"
              << std::setw(width) << "G"
              << std::setw(width) << "B" << std::endl;

    std::cout << std::string(3 * width, '-') << std::endl;

    // Print the mean values
    std::cout << std::left << std::setw(width) << "Mean"
              << std::fixed << std::setprecision(precision)
              << std::setw(width) << mean_r
              << std::setw(width) << mean_g
              << std::setw(width) << mean_b << std::endl;

    // Print the max values
    std::cout << std::left << std::setw(width) << "Max"
              << std::fixed << std::setprecision(precision)
              << std::setw(width) << max_r
              << std::setw(width) << max_g
              << std::setw(width) << max_b << std::endl;

    // Print the min values
    std::cout << std::left << std::setw(width) << "Min"
              << std::fixed << std::setprecision(precision)
              << std::setw(width) << min_r
              << std::setw(width) << min_g
              << std::setw(width) << min_b << std::endl;

    // Print the standard deviation values
    std::cout << std::left << std::setw(width) << "Std Dev"
              << std::fixed << std::setprecision(precision)
              << std::setw(width) << std_dev_r
              << std::setw(width) << std_dev_g
              << std::setw(width) << std_dev_b << std::endl;
    std::cout << "\n+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n" << std::endl;
}




int main(int argc, char *argv[])
{
   int sock, length, n;
   int contador=0;
   socklen_t fromlen;
   Data data;
   struct sockaddr_in server;
   struct sockaddr_in from;

   char buf[24];

   if (argc < 2) {
      fprintf(stderr, "ERROR, no port provided\n");
      exit(0);
   }

   sock=socket(AF_INET, SOCK_DGRAM, 0);
   if (sock < 0) error("Opening socket");
   length = sizeof(server);
   bzero(&server,length);
   server.sin_family=AF_INET;
   server.sin_addr.s_addr=INADDR_ANY;
   server.sin_port=htons(atoi(argv[1]));
   if (bind(sock,(struct sockaddr *)&server,length)<0)
       error("binding");
   fromlen = sizeof(struct sockaddr_in);


   while (1) {
       n = recvfrom(sock,&data,sizeof(data),0,(struct sockaddr *)&from,&fromlen);
       if (n < 0) error("recvfrom");

		if(n==sizeof(data)){

			print_parsed_accel_data(data.accel,TIMES);
			print_parsed_color_data(data.color,TIMES);
			calculate_and_print_stats(data.accel, TIMES);
			calculate_and_print_color_stats(data.color,TIMES);

		}else{
			std::cerr << "Received data size mismatch" << std::endl;
		}

		std::strcpy(buf,"Data received");
	 	n = sendto(sock, buf, strlen(buf) + 1, 0, (struct sockaddr *)&from, fromlen);
       	if (n < 0) error("sendto");
		
   }
   close(sock);
   return 0;
 }
