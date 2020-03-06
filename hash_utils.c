#include <string.h>

void hashfunc(char *buf, int len){
		buf[0] = 'z';
		buf[len/2] = '0';
		buf[len/3] = '9';
		buf[len-3] = 'F';
}
