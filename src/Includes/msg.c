#include "msg.h"

void tirabarran(char* msg) {
    // Calcula o tamanho da string
    int msg_tam = (int) strlen(msg);  
    // Substitui o último caractere (nova linha) por um caractere nulo
    msg[msg_tam-1] = '\0';  
}