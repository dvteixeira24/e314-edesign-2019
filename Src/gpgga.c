/*
 * File: gpgga.c
 * Author: 20976313
 *
 * Description: Handles the decoding and checking of gpgga messages.
 *
 */

#include<prog_def.h>
#include<stdint.h>
#include<string.h>
#include<stdlib.h>

int isGPGGA(void);
int checksumGPGGA(void);
void build_fields(void);
char parse_field_as_char(int fieldNum);
float parse_field_as_float(int fieldNum);
float lat_to_deg(float lat, char dir);
float lon_to_deg(float lon, char dir);
float extract_float(char string[]);
void set_utc_vars(int UTC);
int parse_hex_string_int(char in[]);

void process_GPGGA(void) {
	uint8_t valid = 1;
	if (!isGPGGA())
		valid = 0;
	if (!checksumGPGGA())
		valid = 0;
	if(valid) {
		build_fields();
		boundary_check();
	}
}

int isGPGGA(void) {
	if(stringCompare(bufferIn, "$GPGGA", 6)) return 1;
	if(stringCompare(bufferIn, "$CMDIN", 6)) {
		system_cmd_handler();
		return 0;
	}
	return 0;
}

int checksumGPGGA(void) {  //algorithm and code credit Anya Nutt from demo 3
	int out = 0;
	int crc = 0;
	int src_crc = 0;
	char hex[2];
	int i = 1; //start checking after $
	//XOR all characters between '$' and '*'
	while ((bufferIn[i] != '*') && (bufferIn[i] != 13)) // if '*' wasn't transmitted, loop will stop at last character of message <CR> (= 13)
	{
		if (bufferIn[i] == 13)
			return 0; // no '*' received, not valid message
		crc ^= bufferIn[i++];
	}
	//i will now be equal to to pos of '*' (if exists)
	i = i + 1; //first digit of received checksum
	hex[0] = bufferIn[i++];
	hex[1] = bufferIn[i];
	src_crc = parse_hex_string_int(hex);
	if (src_crc == crc) {
		out = 1;
	}
	return out; //1 for valid, 0 for not valid
}

void build_fields(void){

	//UTC
	int UTC = parse_field_as_float(1);
	set_utc_vars(UTC);

	//latitude
	latitude = lat_to_deg(parse_field_as_float(2), parse_field_as_char(3));

	//long
	longitude = lon_to_deg(parse_field_as_float(4), parse_field_as_char(5));

	//altitude
	altitude = parse_field_as_float(9);

}

char parse_field_as_char(int fieldNum){
		int i = 0;
		int d = 0;
		char ch;
		while(d<fieldNum) {
			if(bufferIn[i] == ',') d++; //comma counting
			i++;
		}
		ch = bufferIn[i];
		return ch;
}

float parse_field_as_float(int fieldNum){
	int i = 0;
	int d = 0;
	int j = 0;
	float value;
	while(d<fieldNum) {
		if(bufferIn[i] == ',') d++; //comma counting
		i++;
	}
	j = i;
	i = 0;
	d = 0;
	while(d<(fieldNum+1)){
		if(bufferIn[i] == ',') d++; //comma counting
		i++;
		}
	char in[i-j+1];
	for (int q = 0; q<(i-j+1); q++){
		in[q] = '\0';
		}
	for (int k = 0; k<(i-j); k++){
	in[k] = bufferIn[j + k];
	}
	value = extract_float(in);
	return value;
}

float lat_to_deg(float lat, char dir){
	float latDeg = 0;
	int deg = (int) (lat/100);
	float min = lat - (deg*100);
	latDeg = deg + min/60;
	if (dir == 'S') latDeg = latDeg*(-1);
    return latDeg;
}

float lon_to_deg(float lon, char dir){
	float lonDeg = 0;
	int deg = (int) (lon/100);
	float min = lon - (deg*100);
	lonDeg = deg + min/60;
	if (dir == 'W') lonDeg = lonDeg*(-1);
    return lonDeg;
}

float extract_float(char string[]){
	   return strtof(string, NULL); //string to double from std lib
}

void set_utc_vars(int UTC){
	UTChh = (int)(double)UTC/(double)10000;
	UTCmm = (int)((double)UTC/(double)100)%100;
	UTCss = UTC%100;
}

//pull a int value from a section of a string representing a hex number
int parse_hex_string_int(char in[]){
	return (int)strtol(in, NULL, 16);
}

int stringCompare(char str1[], char str2[], int len){
	uint8_t f = 1;
	for (int i = 0; i<len; i++){
		if (!(str1[i] == str2 [i])) f = 0;
	}
	return f;
}
