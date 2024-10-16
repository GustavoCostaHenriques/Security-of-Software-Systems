#include <stdio.h> 
#include <string.h> 
void test(char *s) { 
	char buf[10]; 
	strcpy(buf, s); 
} 

int main (int argc, char** argv) { 
	test(argv[1]); 
	printf("I’m OK!\n"); 
} 