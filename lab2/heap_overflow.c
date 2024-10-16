#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <ctype.h> 
 
int main (int argc, char **argv) 
{ 
    char *str = (char *) malloc(sizeof(char)*4); 
    char *critical = (char *) malloc(sizeof(char)*9); 
    char *tmp; 
 
    printf("Address of str is [%p, %lu]\n", str, (unsigned long) str); 
    printf("Address of critical is [%p, %lu]\n", critical, 
                                                (unsigned long) critical); 
    strcpy(critical, "secret"); 
    strcpy(str, argv[1]); 
    tmp = str; 
    while (tmp < critical+9){ 
        printf("[%p, %lu]: %c (0x%x)\n", tmp, (unsigned long) tmp, 
                             isprint(*tmp) ? *tmp : '?', (unsigned) (*tmp)); 
        tmp +=1; 
    } 
    printf("critical = %s\n", critical);  
} 