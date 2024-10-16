#include <stdio.h> 
#include <string.h> 
#include <stdlib.h> 
 
void test(char *s) 
{ 
    char buf[16]; 
    strcpy(buf, s); 
} 
 
void cannot()  
{ 
    printf("This function should not be executed! Was there a BO (-; ?\n"); 
    exit(0); 
} 
 
int main (int argc, char** argv) 
{ 
    printf(" &cannot = %p\n", &cannot); 
    test(argv[1]); 
    printf("I’m OK!\n"); 
} 