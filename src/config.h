#ifndef _CONFIG_H_
#define _CONFIG_H_

#define PORTA 8080
#define TAM_MSG 256
#define TAM_NOME 64
#define TAM_USER 16

#define CODIGO_REGISTRO '#'
#define ERROR "/ERROR\n"
#define FECHAR_CLIENTE "/QUIT\n"
#define FECHAR_SERVIDOR "/SEVERKILL\n"

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

#endif