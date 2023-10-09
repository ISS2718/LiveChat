#ifndef _CLIENTES_H_
#define _CLIENTES_H_

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "../Includes/msg.h"
#include "../Includes/config.h"
#include "../Includes/listaClientes.h"

/**
 * @struct ParametrosPthreads
 * @brief Estrutura para armazenar os parâmetros para as funções de thread.
 *
 * Esta estrutura é usada para passar múltiplos parâmetros para as funções que são executadas em threads separadas.
 *
 * @var int* ParametrosPthreads::socket_c
 * Ponteiro para o socket do cliente. Este é o canal de comunicação entre o cliente e o servidor.
 *
 * @var InfoCliente* ParametrosPthreads::cliente
 * Ponteiro para uma estrutura InfoCliente que contém informações sobre o cliente.
 */
typedef struct  {
    int* socket_c;
    InfoCliente* cliente;
} ParametrosPthreads;

/**
 * @fn void* enviar(void * arg)
 * @brief Função para enviar mensagens para o servidor.
 *
 * Esta função é responsável por enviar mensagens para o servidor. Ela deve ser executada em uma thread separada.
 * A função recebe um único argumento, que é um ponteiro para uma estrutura ParametrosPthreads.
 * A função lê uma mensagem do usuário, envia a mensagem para o servidor e repete esse processo até que uma condição de saída seja atendida.
 *
 * @param arg Um ponteiro para uma estrutura ParametrosPthreads. Esta estrutura deve conter todas as informações necessárias para a função.
 *
 * @note Esta função deve ser utilizada em uma thread.
 */
void* enviar(void * arg);

/**
 * @fn void* receber(void * arg)
 * @brief Função para receber mensagens do servidor.
 *
 * Esta função é responsável por receber mensagens do servidor. Ela deve ser executada em uma thread separada.
 * A função recebe um único argumento, que é um ponteiro para uma estrutura ParametrosPthreads.
 * A função lê as mensagens recebidas do servidor e realiza ações apropriadas com base no conteúdo da mensagem.
 *
 * @param arg Um ponteiro para uma estrutura ParametrosPthreads. Esta estrutura deve conter todas as informações necessárias para a função.
 *
 * @note Esta função deve ser utilizada em uma thread.
 */
void* receber(void * arg);

#endif