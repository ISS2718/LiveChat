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
    ListaClientes * listaClientes = (ListaClientes*) malloc(sizeof(ListaClientes)); //Aloca dinamicamente uma lista de clientes.
    //Se foi possível alocar a lista de clientes, inicializa como vazia.
    if(listaClientes != NULL)
        *listaClientes = NULL;
    return listaClientes;
}

InfoCliente criaRegistroCliente(char * infoGeral){
    int tamanho = strlen(infoGeral);
    int j = -1;
    int a = 0;
    char mRegistro[2][64];

    //Lê a string recebida até o fim, indicado por um '\n' ou um '\0'
    for(int i = 0; infoGeral[i] != '\n' && infoGeral[i] != '\0'; i++){
        //Se encontrado um byte CODIGO_REGISTRO.
        if(infoGeral[i] == CODIGO_REGISTRO){
            //Inicia-se a contagem de parâmetros;
            j++;
            if(j>=2)
                break;
            a = 0;
        }
        //Se não foi encontrado o byte CODIGO_REGISTRO.
        else{
            //O byte é um valor dos parâmetros e guarda-se numa matriz.
            mRegistro[j][a] = infoGeral[i];
            mRegistro[j][a+1] = '\0';
            a++;
        }
    }

    InfoCliente infoCliente;
    //Se a contagem de parâmetros não se iniciou.
    if(j == -1){
        //Inicia-se o registro com valores nulos.
        strcpy(infoCliente.nome, "\0");
        strcpy(infoCliente.user, "\0");
        infoCliente.moderador = -1;
        infoCliente.cor = -1;
        infoCliente.mute = -1;
    }
    //Se a contagem de parâmetros foi iniciado.
    else{
        //Inicia-se o registro com os valores encontrados pela linha 'infoGeral'.
        strcpy(infoCliente.nome, mRegistro[0]);
        strcat(infoCliente.nome, "\0");

        strcpy(infoCliente.user, mRegistro[1]);
        strcat(infoCliente.user, "\0");
        srand(time(NULL)%7);
        infoCliente.cor = rand() % NUM_CORES_USERS;
        infoCliente.moderador = 0;
        infoCliente.mute = 0;
    }

    return infoCliente; //Retorna o registro criado, que pode ser nulo ou pode conter as informações obtidas.

}

void liberaListaClientes(ListaClientes * listaClientes){
    //Se existe lista de clientes.
    if(listaClientes != NULL){
        Cliente * cliente;
        //Percorre toda a lista de clientes conectados.
        while(*listaClientes != NULL){
            //Retira-se um cliente da lista e o desaloca da memória.
            cliente = *listaClientes;
            *listaClientes = (*listaClientes)->proximo;
            free(cliente);
        }
        free(listaClientes); //Desaloca a lista de clientes.
    }
}

void insereListaClientes(InfoCliente registro, struct sockaddr_in endereco, ListaClientes * listaClientes){
    //Se não há lista de clientes, retorna.
    if(listaClientes == NULL)
        return;
    
    //Cria um novo cliente, juntando seu endereço e informações de registro.
    Cliente * novoCliente = (Cliente*) malloc(sizeof(Cliente));
    novoCliente->endereco = endereco;
    novoCliente->registro = registro;
    novoCliente->proximo = NULL;

    //Se não há clientes na lista, adiciona o cliente na primeira posição.
    if(*listaClientes == NULL)
        *listaClientes = novoCliente;
    //Se existe clientes na lista, adiciona o cliente na última posição.
    else{
        Cliente * aux = *listaClientes;
        while(aux->proximo != NULL)
            aux = aux->proximo;
        
        aux->proximo = novoCliente;
    }
    return;
}

void imprimeListaClientes(ListaClientes * listaClientes){
    //Se não há lista de clientes alocada, imprime-se o aviso.
    if(listaClientes == NULL){
        printf(SISTEMA);
        printf("nao ha lista de clientes para imprimir.\n");
        return;
    }

    printf(SISTEMA);
    printf("imprimindo todos os clientes da lista.\n");

    Cliente * cliente = *listaClientes;
    int i = 0;

    //Percorre toda a lista de clientes, imprimindo o nome, o user, seu poder de moderador e sua cor.
    while(cliente != NULL){
        printf("Cliente %d:\n", i);
        printf("\t - %s\n", cliente->registro.nome);
        printf("\t - %s\n", cliente->registro.user);
        printf("\t - %d\n", cliente->registro.moderador);
        printf("\t - %d\n", cliente->registro.cor);
        printf("\t - %d\n", cliente->registro.mute);
        printf("\n");
        i++;
        cliente = cliente->proximo;
    }
}

int existeClienteLista(InfoCliente registro, ListaClientes * listaClientes){
    //Se não há lista, retorna que não há clientes na lista.
    if(listaClientes == NULL)
        return 0;

    Cliente * cliente = *listaClientes;
    //Percorre toda a lista de clientes conectados.
    while(cliente != NULL){
        printf(SISTEMA);
        printf("Verificando cliente: %s...\n", cliente->registro.user);

        //Se encontrado um cliente com o user buscado, retorna sucesso.
        if(strcmp(cliente->registro.user, registro.user) == 0)
            return 1;
        cliente = cliente->proximo;
    }

    return 0; //Caso a lista tenha sido percorrida inteira sem encontrar, retorna insucesso.
}

int existeEnderecoLista(struct sockaddr_in endereco, ListaClientes * listaClientes){
    //Se não há lista, retorna que não há clientes na lista.
    if(listaClientes == NULL)
        return 0;

    Cliente * cliente = *listaClientes;
    //Percorre toda a lista de clientes conectados.
    while(cliente != NULL){
        printf(SISTEMA);
        printf("Verificando cliente de IP: %d.\n", cliente->endereco.sin_addr.s_addr);
        struct sockaddr_in endClienteLista = cliente->endereco;        
        int bEnderecosIguais = enderecosIguais(endereco, endClienteLista);
        //Se encontrado um cliente com o user buscado, retorna sucesso.
        if(bEnderecosIguais){
            return 1;
        }
        cliente = cliente->proximo;
    }
    return 0; //Caso a lista tenha sido percorrida inteira sem encontrar, retorna insucesso.
}

int enderecosIguais(struct sockaddr_in A, struct sockaddr_in B){
    int bEndereco = strncmp((char *) &A.sin_addr.s_addr, (char *) &B.sin_addr.s_addr, sizeof(unsigned int)); //Compara o endereço de IP.

    int bPorta = strncmp((char *) &A.sin_port, (char *) &B.sin_port, sizeof(unsigned short)); //Compara a porta.

    int bFamilia = strncmp((char *) &A.sin_family, (char *) &B.sin_family, sizeof(unsigned short)); //Compara a família do endereço.

    //Se as três comparações forem iguais, retorna endereços iguais.
    if(bEndereco == 0 && bPorta == 0 && bFamilia == 0)
        return 1;
    
    //Caso contrário, retorna endereços diferentes.
    return 0;
}

Cliente * retornaClientePorEndereco(struct sockaddr_in endereco, ListaClientes * listaClientes){
    Cliente * cliente = *listaClientes;
    //Percorre toda a lista de clientes conectados.
    while(cliente!=NULL){
        printf(SISTEMA "procurando por %d na lista de clientes...\n", endereco.sin_addr.s_addr);
        struct sockaddr_in endClienteLista = cliente->endereco; //Obtém endereço do cliente da lista.
        int bClientesIguais = enderecosIguais(endereco, endClienteLista); //Obtém se os endereços são iguais.
        
        //Se o endereço do cliente da lista for igual o cliente buscado, retorna o cliente da lista.
        if(bClientesIguais){
            printf(SISTEMA "usuario de endereço %d encontrado: %s\n", endereco.sin_addr.s_addr, cliente->registro.user);
            return cliente;
        }   
        cliente = cliente->proximo;
    }

    //Retorna nulo se o cliente não foi encontrado.
    printf(CLIENTE_NAO_ENCONTRADO);
    return NULL;
}

Cliente * retornaClientePorUsuario(char * usuario, ListaClientes * listaClientes){
    Cliente * cliente = *listaClientes;
    //Percorre toda a lista de clientes conectados.
    while(cliente != NULL){
        //Se o cliente da lista tiver o mesmo usuário buscado, retorna o cliente da lista.
        if(strcmp(cliente->registro.user, usuario) == 0)
            return cliente;
        cliente = cliente->proximo;
    }
    //Caso a lista inteira seja percorrida sem encontrar, retorna NULL.
    return NULL;
}

InfoCliente retornaRegistroPorEndereco(struct sockaddr_in endCliente, ListaClientes * listaClientes){
    //Inicia um registro com valores nulos.
    InfoCliente infoCliente;
    strcat(infoCliente.nome, "\0");
    strcat(infoCliente.user, "\0");
    infoCliente.moderador = -1;
    infoCliente.mute = -1;
    
    Cliente * cliente = *listaClientes;
    //Percorre toda a lista de clientes.
    while(cliente!=NULL){
        struct sockaddr_in endClienteLista = cliente->endereco; //Obtém o endereço de um cliente da lista.
        int bClientesIguais = enderecosIguais(endCliente, endClienteLista); //Compara os endereços do cliente da lista e o buscado.
        //Se o cliente foi encontrado, troca-se o registro nulo pelo registro do cliente encontrado.
        if(bClientesIguais){
            infoCliente = cliente->registro;
            return infoCliente;
        }
            
        cliente = cliente->proximo;
    }

    //Retorna o registro nulo, caso a lista tenha sido percorrida sem encontrar.
    printf(CLIENTE_NAO_ENCONTRADO);
    return infoCliente;
}

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