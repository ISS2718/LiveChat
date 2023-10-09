#include "msg.h"

const char cores[12][9] = {
    {"\x1B[32m"},
    {"\x1B[34m"},
    {"\x1B[35m"},
    {"\x1B[36m"},
    {"\x1B[91m"},
    {"\x1B[92m"},
    {"\x1B[93m"},
    {"\x1B[94m"},
    {"\x1B[95m"},
    {"\x1B[96m"},
    {"\x1B[37m"},
    {"\x1B[39m"}
};

void colocaCorUser(char *s, int cor) {

}

void tirabarran(char* msg) {
    int msg_tam = (int) strlen(msg);
    msg[msg_tam-1] = '\0';
}