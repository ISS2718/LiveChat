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

InfoCliente criaRegistroCliente(char * infoGeral){

    int tamanho = strlen(infoGeral);
    int j = -1;
    int a = 0;
    char mRegistro[2][64];

    for(int i = 0; infoGeral[i] != '\n'; i++){
        if(infoGeral[i] == CODIGO_REGISTRO){
            j++;
            if(j>=2)
                break;
            a = 0;
        }
        else{
            mRegistro[j][a] = infoGeral[i];
            mRegistro[j][a+1] = '\0';
            a++;
        }
    }


    InfoCliente infoCliente;
    if(j == -1){ //A entrada não foi codigo de registro.
        
        strcpy(infoCliente.nome, "\0");
        strcpy(infoCliente.user, "\0");
        infoCliente.moderador = -1;
        infoCliente.cor = -1;
    }
    else{
        strcpy(infoCliente.nome, mRegistro[0]);
        strcat(infoCliente.nome, "\0");

        strcpy(infoCliente.user, mRegistro[1]);
        strcat(infoCliente.user, "\0");
        srand(time(NULL)%7);
        infoCliente.cor = rand()%10;
        infoCliente.moderador = 0;
        infoCliente.mute = 0;
    }

    return infoCliente;

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

Cliente * retornaClientePorUsuario(char * usuario, ListaClientes * listaClientes){
    Cliente * cliente = *listaClientes;
    while(cliente != NULL){
        if(strcmp(cliente->registro.user, usuario) == 0)
            return cliente;
        cliente = cliente->proximo;
    }
    return NULL;
}

InfoCliente retornaRegistroPorEndereco(struct sockaddr_in endCliente, ListaClientes * listaClientes){
    InfoCliente infoCliente;
    strcat(infoCliente.nome, "\0");
    strcat(infoCliente.user, "\0");
    infoCliente.moderador = -1;
    
    Cliente * cliente = *listaClientes;
    while(cliente!=NULL){
        struct sockaddr_in endClienteLista = cliente->endereco;
        int bClientesIguais = enderecosIguais(endCliente, endClienteLista);
        if(bClientesIguais){
            infoCliente = cliente->registro;
            return infoCliente;
        }
            
        cliente = cliente->proximo;
    }

    printf(CLIENTE_NAO_ENCONTRADO);
    return infoCliente;
}

void removeListaPorEndereco(struct sockaddr_in endCliente, ListaClientes * listaClientes) {
    Cliente * cliente_anterior = NULL;
    Cliente * cliente_atual = *listaClientes;
    Cliente * cliente_sucessor = cliente_atual->proximo;
    while(cliente_atual!=NULL){
        struct sockaddr_in endClienteLista = cliente_atual->endereco;
        int bClientesIguais = enderecosIguais(endCliente, endClienteLista);
        if(bClientesIguais){
            if(cliente_anterior == NULL) {
                if(cliente_sucessor == NULL) {
                    free(cliente_atual);
                } else {
                    cliente_atual->registro = cliente_sucessor->registro;
                    cliente_atual->proximo = cliente_sucessor->proximo;
                    free(cliente_sucessor);
                }
            } else {
                if(cliente_sucessor == NULL) {
                    cliente_anterior->proximo = NULL;
                } else {
                    cliente_anterior->proximo = cliente_sucessor;
                }
                free(cliente_atual);
            }
        }
        cliente_anterior = cliente_atual;
        cliente_atual = cliente_atual->proximo;
        cliente_sucessor = cliente_atual->proximo;
    }
}

void removeListaPorUsuario(char * usuario, ListaClientes * listaClientes) {
    Cliente * cliente_anterior = NULL;
    Cliente * cliente_atual = *listaClientes;
    Cliente * cliente_sucessor = cliente_atual->proximo;
    while(cliente_atual != NULL){
        if(strcmp(cliente_atual->registro.user, usuario) == 0) {
            if(cliente_anterior == NULL) {
                if(cliente_sucessor == NULL) {
                    free(cliente_atual);
                } else {
                    cliente_atual->registro = cliente_sucessor->registro;
                    cliente_atual->proximo = cliente_sucessor->proximo;
                    free(cliente_sucessor);
                }
            } else {
                if(cliente_sucessor == NULL) {
                    cliente_anterior->proximo = NULL;
                } else {
                    cliente_anterior->proximo = cliente_sucessor;
                }
                free(cliente_atual);
            }
        }
        cliente_anterior = cliente_atual;
        cliente_atual = cliente_atual->proximo;
        cliente_sucessor = cliente_atual->proximo;
    }
}