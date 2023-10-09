#ifndef _MSG_H_
#define _MSG_H_

#include <stdio.h>
#include <string.h>

#include "config.h"

#define CLIENTE_JA_CONECTADO "Cliente ja esta conectado! \n"
#define SUCESSO_CONEXAO_CLIENTE "Cliente foi conectado com sucesso.\n"
#define ERRO_CONECTAR_CLIENTE "Nao foi possivel conectar o cliente.\n"
#define ERRO_MENSAGEM "Nao foi possivel enviar a mensagem.\n"
#define CLIENTE_NAO_ENCONTRADO "Cliente nao foi encontrado! \n"

#define AVISO "\x1B[31mA\x1B[33mV\x1B[31mI\x1B[33mS\x1B[31mO\x1B[33m:\x1B[39m "
#define ERRO "\x1B[31mERRO:\x1B[39m "
#define MENSAGEM "\x1B[33mMENSAGEM:\x1B[39m "
#define SISTEMA  "\x1B[33mSISTEMA:\x1B[39m "

#define AMARELO "\x1B[33m"
#define RESET "\x1B[39m"

void tirabarran(char* msg);

#endif