#ifndef _SERVIDOR_H_
#define _SERVIDOR_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#include "../Includes/msg.h"
#include "../Includes/config.h"
#include "../Includes/listaClientes.h"

InfoCliente criaRegistroCliente(char * infoGeral);
int clienteConectado(struct sockaddr_in endCliente, ListaClientes * listaClientes);
int conectarCliente(int rSocket, struct sockaddr_in endCliente, InfoCliente registro, ListaClientes * listaClientes, char **cores);
void enviaMensagemParaOutros(int rSocket, char mensagem[TAM_MSG], struct sockaddr_in mensageiro, ListaClientes * listaClientes);
void enviaMensagemParaTodos(int rSocket, char mensagem[TAM_MSG], ListaClientes * listaClientes);
int enviaMensagemCliente(int rSocket, Cliente * cliente, char mensagem[TAM_MSG]);
int verificaExecutaFuncao(int rSocket, struct sockaddr_in mensageiro, char mensagem[TAM_MSG], ListaClientes * listaClientes, char **cores);

#endif