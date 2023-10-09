#include "msg.h"

void tirabarran(char* msg) {
    int msg_tam = (int) strlen(msg);
    msg[msg_tam-1] = '\0';
}
