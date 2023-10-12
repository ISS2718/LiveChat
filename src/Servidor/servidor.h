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

/**
 * @fn int clienteConectado(struct sockaddr_in endCliente, ListaClientes * listaClientes)
 * @brief Verifica se um cliente está conectado a partir de seu endereço.
 * @param endCliente endereço do cliente a se verificar.
 * @param listaClientes lista de clientes conectados no servidor.
 * @return conectado (1) ou não conectado (0).
*/
int clienteConectado(struct sockaddr_in endCliente, ListaClientes * listaClientes);

/**
 * @fn int conectarCliente(int rSocket, struct sockaddr_in endCliente, InfoCliente registro, ListaClientes * listaClientes, char **cores)
 * @brief Conecta um cliente no servidor.
 * 
 * @param rSocket resultado da conexão.
 * @param endCliente endereço do cliente que será conectado.
 * @param registro informações de registro do cliente.
 * @param listaClientes lista de clientes conectados no servidor.
 * 
 * @return conexão bem sucedida (0) ou falha na conexão (1).
 */
int conectarCliente(int rSocket, struct sockaddr_in endCliente, InfoCliente registro, ListaClientes * listaClientes, char **cores);

/**
 * @fn void enviaMensagemParaOutros(int rSocket, char * mensagem, struct sockaddr_in mensageiro, ListaClientes * listaClientes)
 * @brief Envia mensagem para todos os clientes conectados, exceto àquele que enviou.
 * @param rSocket feedback da conexão.
 * @param mensagem mensagem a se enviar para os clientes conectados.
 * @param mensageiro endereço do cliente que envia a mensagem.
 * @param listaClientes lista de clientes conectados no servidor.
*/
void enviaMensagemParaOutros(int rSocket, char * mensagem, struct sockaddr_in mensageiro, ListaClientes * listaClientes);

/**
 * @fn void enviaMensagemParaTodos(int rSocket, char * mensagem, ListaClientes * listaClientes)
 * @brief Envia mensagem para todos os clientes da lista, incluindo ao que enviou a mensagem.
 * @param rSocket feedback da conexão.
 * @param mensagem mensagem a se enviar para os clientes conectados.
 * @param listaClientes lista de clientes conectados no servidor.
*/
void enviaMensagemParaTodos(int rSocket, char * mensagem, ListaClientes * listaClientes);

/**
 * @fn int enviaMensagemCliente(int rSocket, Cliente * cliente, char * mensagem)
 * @brief Envia mensagem para um cliente em específico.
 * @param rSocket feedback do envio da mensagem.
 * @param cliente cliente a que se deseja enviar a mensagem.
 * @param mensagem mensagem a se enviar ao cliente.
 * @return mensagem enviada (0), erro no envio (-1).
*/
int enviaMensagemCliente(int rSocket, Cliente * cliente, char * mensagem);

/**
 * @fn int verificaExecutaFuncao(int rSocket, struct sockaddr_in mensageiro, char * mensagem, ListaClientes * listaClientes, char ** cores)
 * @brief Verifica se uma mensagem é uma função dentro do servidor. Caso seja, executa a função escrita pelo cliente.
 * @param rSocket feedback do socket.
 * @param mensageiro endereço do mensageiro.
 * @param mensagem mensagem que pode conter a diretiva da função do servidor.
 * @param listaClientes lista de clientes conectados no servidor.
 * @return mensagem com uma função no servidor (1) ou mensagem normal (0).
 * 
 * @note uma mensagem-função é definida por "/funcao param1 param2". Uma função sem parâmetros ou sem um deles apenas resultam em strings vazias.
*/
int verificaExecutaFuncao(int rSocket, struct sockaddr_in mensageiro, char * mensagem, ListaClientes * listaClientes, char **cores);

#endif