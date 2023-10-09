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
        infoCliente.mute = -1;
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

Cliente * retornaClientePorEndereco(struct sockaddr_in endereco, ListaClientes * listaClientes){
    Cliente * cliente = *listaClientes;
    while(cliente!=NULL){
        struct sockaddr_in endClienteLista = cliente->endereco;
        int bClientesIguais = enderecosIguais(endereco, endClienteLista);
        if(bClientesIguais){
            return cliente;
        }
            
        cliente = cliente->proximo;
    }

    printf(CLIENTE_NAO_ENCONTRADO);
    return NULL;
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
    infoCliente.mute = -1;
    
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

//REMOVE UM CLIENTE DA LISTA ATRAVÉS DO SEU ENDEREÇO
void removeClientePorEndereco(struct sockaddr_in endCliente, ListaClientes * listaClientes) {

    //Se não há lista ou não há itens na lista, retorna.
    if(listaClientes == NULL || *listaClientes == NULL){
        return;
    }

    Cliente * anterior = NULL;
    Cliente * atual = *listaClientes;

    //Percorre todos os itens existentes da lista de clientes.
    while(atual->proximo != NULL){
        //Compara os endereços do cliente atual com o buscado.
        if(enderecosIguais(atual->endereco, endCliente)){
            //Se o endereço buscado foi o primeiro item, então a lista começa com o proximo do atual.
            if(anterior == NULL){
                *listaClientes = atual->proximo;
            }
            //Se o endereço buscado não for o primeiro item, o item anterior tem o próximo do atual como sucessor.
            else{
                anterior->proximo = atual->proximo;
            }
            //Libera o nó atual.
            free(atual);
            break;
        }

        anterior = atual;
        atual = atual->proximo;
    }
}

//REMOVE UM CLIENTE DA LISTA ATRAVÉS DO SEU USUÁRIO.
void removeClientePorUsuario(char * usuario, ListaClientes * listaClientes) {
    //Se não há lista ou não há itens na lista, retorna.
    if(listaClientes == NULL || *listaClientes == NULL){
        return;
    }

    Cliente * anterior = NULL;
    Cliente * atual = *listaClientes;

    //Percorre todos os itens existentes da lista de clientes.
    while(atual->proximo != NULL){
        //Compara o user do cliente atual com o buscado.
        if(strcmp(atual->registro.user, usuario) == 0){
            //Se o endereço buscado foi o primeiro item, então a lista começa com o proximo do atual.
            if(anterior == NULL){
                *listaClientes = atual->proximo;
            }
            //Se o endereço buscado não for o primeiro item, o item anterior tem o próximo do atual como sucessor.
            else{
                anterior->proximo = atual->proximo;
            }
            //Libera o nó atual.
            free(atual);
            break;
        }

        anterior = atual;
        atual = atual->proximo;
    }
}