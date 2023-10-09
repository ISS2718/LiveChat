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

typedef struct  {
    int* socket_c;
    InfoCliente* cliente;
} ParametrosPthreads;


void* enviar(void * arg);
void* receber(void * arg);

#endif