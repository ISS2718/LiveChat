#include "msg.h"

void AVISO() {
    printf("\x1B[31mA\x1B[33mV\x1B[31mI\x1B[33mV\x1B[31mI\x1B[33mS\x1B[31mO\x1B[33m:\x1B[39m ");
}

void ERRO() {
    printf("\x1B[31ERRO:\x1B[39m ");
}

void MESAGEM() {
    printf("\x1B[33mMENSAGEM:\x1B[39m ");
}

void SISTEMA(){
    printf("\x1B[33mSISTEMA:\x1B[39m ");
}

void colocaCorUser(char *s, int cor) {

}

void tirabarran(char* msg) {
    int msg_tam = (int) strlen(msg);
    msg[msg_tam-1] = '\0';
}