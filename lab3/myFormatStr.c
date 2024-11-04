#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
 
int main (int argc, char **argv) 
{ 
    char secret[20]; 
    char buf[32]; 
 
    strncpy(secret, "secret!!", sizeof(secret)); 
 
    strncpy(buf, argv[1], sizeof(buf)); 
    printf(buf); 
 
    printf("\n"); 
    printf("locations: buf = %lu secret = %lu\n",  
             (unsigned long) buf, (unsigned long) secret); 
    fflush(stdout); 
}