#ifndef _LISTA_CLIENTES_H_
#define _LISTA_CLIENTES_H_

#include <netinet/in.h>
#include "msg.h"
#include "config.h"

typedef struct infoCliente{
    char nome[TAM_NOME];
    char user[TAM_USER];
    int moderador;
    int cor;
    int mute;
}InfoCliente;

typedef struct cliente{
    struct sockaddr_in endereco;
    InfoCliente registro;
    struct cliente * proximo;
}Cliente;

typedef struct cliente * ListaClientes;

ListaClientes * criaListaClientes();
InfoCliente criaRegistroCliente(char * infoGeral);
void liberaListaClientes(ListaClientes * listaClientes);
void insereListaClientes(InfoCliente registro, struct sockaddr_in endereco, ListaClientes * listaClientes);
void imprimeListaClientes(ListaClientes * listaClientes);
int enderecosIguais(struct sockaddr_in A, struct sockaddr_in B);
int existeClienteLista(InfoCliente registro, ListaClientes * listaClientes);
int existeEnderecoLista(struct sockaddr_in endereco, ListaClientes * listaClientes);
InfoCliente retornaRegistroPorEndereco(struct sockaddr_in endCliente, ListaClientes * listaClientes);
Cliente * retornaClientePorUsuario(char * usuario, ListaClientes * listaClientes);
void removeListaPorUsuario(char * usuario, ListaClientes * listaClientes);
void removeListaPorEndereco(struct sockaddr_in endCliente, ListaClientes * listaClientes);
#endif