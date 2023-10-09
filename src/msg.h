#ifndef _MSG_H_
#define _MSG_H_

#include <stdio.h>

#include "config.h"

#define CLIENTE_JA_CONECTADO "Cliente ja esta conectado! \n"
#define SUCESSO_CONEXAO_CLIENTE "Cliente foi conectado com sucesso.\n"
#define ERRO_CONECTAR_CLIENTE "Nao foi possivel conectar o cliente.\n"
#define ERRO_MENSAGEM "Nao foi possivel enviar a mensagem.\n"
#define CLIENTE_NAO_ENCONTRADO "Cliente nao foi encontrado! \n"

void AVISO();
void ERRO();
void MENSAGEM();
void SISTEMA();
void colocaCorUser(char *s, int cor);
void tirabarran(char* msg);

#endif