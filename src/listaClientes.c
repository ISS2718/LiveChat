#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "listaClientes.h"
#include "config.h"
#include "msg.h"

ListaClientes * criaListaClientes(){
    ListaClientes * listaClientes = (ListaClientes*) malloc(sizeof(ListaClientes));
    if(listaClientes != NULL)
        *listaClientes = NULL;
    return listaClientes;
}

void liberaListaClientes(ListaClientes * listaClientes){
    if(listaClientes != NULL){
        Cliente * cliente;
        while(*listaClientes != NULL){
            cliente = *listaClientes;
            *listaClientes = (*listaClientes)->proximo;
            free(cliente);
        }
        free(listaClientes);
    }
}

void insereListaClientes(InfoCliente registro, struct sockaddr_in endereco, ListaClientes * listaClientes){
    if(listaClientes == NULL)
        return;
    
    Cliente * novoCliente = (Cliente*) malloc(sizeof(Cliente));
    novoCliente->endereco = endereco;
    novoCliente->registro = registro;
    novoCliente->proximo = NULL;

    if(*listaClientes == NULL)
        *listaClientes = novoCliente;
    else{
        Cliente * aux = *listaClientes;
        while(aux->proximo != NULL)
            aux = aux->proximo;
        
        aux->proximo = novoCliente;
    }
    return;
}

void imprimeListaClientes(ListaClientes * listaClientes){
    if(listaClientes == NULL){
        printf(SISTEMA);
        printf("nao ha lista de clientes para imprimir.\n");
        return;
    }

    printf(SISTEMA);
    printf("imprimindo todos os clientes da lista.\n");

    Cliente * cliente = *listaClientes;
    int i = 0;

    while(cliente != NULL){
        printf("Cliente %d:\n", i);
        printf("\t - %s\n", cliente->registro.nome);
        printf("\t - %s\n", cliente->registro.user);
        printf("\t - %d\n", cliente->registro.moderador);
        printf("\n");
        i++;
        cliente = cliente->proximo;
    }
}

// VERIFICA SE O CLIENTE ESTÁ NA LISTA DE CLIENTES ONLINE PELO SEU USER.
int existeClienteLista(InfoCliente registro, ListaClientes * listaClientes){
    if(listaClientes == NULL)
        return 0;

    Cliente * cliente = *listaClientes;
    while(cliente != NULL){
        printf(SISTEMA);
        printf("Verificando cliente: %s...\n", cliente->registro.user);
        if(strcmp(cliente->registro.user, registro.user) == 0)
            return 1;
        cliente = cliente->proximo;
    }
    return 0;
}

// VERIFICA SE O CLIENTE ESTÁ NA LISTA DE CLIENTES ONLINE PELO SEU ENDEREÇO.
int existeEnderecoLista(struct sockaddr_in endereco, ListaClientes * listaClientes){
    if(listaClientes == NULL)
        return 0;

    Cliente * cliente = *listaClientes;
    while(cliente != NULL){
        printf(SISTEMA);
        printf("Verificando cliente de IP: %d.\n", cliente->endereco.sin_addr.s_addr);
        struct sockaddr_in endClienteLista = cliente->endereco;
        int bEnderecosIguais = enderecosIguais(endereco, endClienteLista);
        if(bEnderecosIguais){
            return 1;
        }
        cliente = cliente->proximo;
    }
    return 0;
}

int enderecosIguais(struct sockaddr_in A, struct sockaddr_in B){
    int bEndereco = strncmp((char *) &A.sin_addr.s_addr, (char *) &B.sin_addr.s_addr, sizeof(unsigned int));

    int bPorta = strncmp((char *) &A.sin_port, (char *) &B.sin_port, sizeof(unsigned short));

    int bFamilia = strncmp((char *) &A.sin_family, (char *) &B.sin_family, sizeof(unsigned short));

    if(bEndereco == 0 && bPorta == 0 && bFamilia == 0)
        return 1;
    
    return 0;
}
