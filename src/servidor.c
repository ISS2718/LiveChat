#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "config.h"
#include "msg.h"



typedef struct infoCliente{
    char nome[TAM_NOME];
    char user[TAM_USER];
    int moderador;
}InfoCliente;

typedef struct cliente{
    struct sockaddr_in endereco;
    InfoCliente registro;
    struct cliente * proximo;
}Cliente;

int rSocket;

InfoCliente criaRegistroCliente(char * infoGeral);
int clienteConectado(struct sockaddr_in endCliente, Cliente listaClientes);
int conectarCliente(struct sockaddr_in endCliente, InfoCliente registro, Cliente listaClientes);

int main(){
    struct sockaddr_in endServidor;
    struct sockaddr_in endMensageiro;

    Cliente listaClientes;

    rSocket = socket(AF_INET, SOCK_DGRAM, 0); // Cria um socket UDP;
    if(rSocket == -1){
        fprintf(stderr, "Erro ao criar o socket.");
        return -1;
    }

    endServidor.sin_family = AF_INET;
    endServidor.sin_port = htons(PORTA);
    endServidor.sin_addr.s_addr = htonl(INADDR_ANY);

    memset(&(endServidor.sin_zero), '\0', 8);

    if(bind(rSocket, (struct sockaddr *) &endServidor, sizeof(struct sockaddr)) == -1){
        fprintf(stderr, "Erro ao criar o socket.");
        return -1;
    }

    while(1){
        char mensagem[TAM_MSG];
        int bytesRecebidos = recvfrom(rSocket,mensagem, TAM_MSG-1,0,(struct sockaddr *) &endMensageiro, (unsigned int *) sizeof(struct sockaddr_in));
        
        if(bytesRecebidos == -1){
            fprintf(stderr, "Erro ao receber mensagem.");
            return -1;
        }

        if(!clienteConectado(endMensageiro, listaClientes)){
            InfoCliente infoCliente = criaRegistroCliente(mensagem);
            if(conectarCliente(endMensageiro, infoCliente, listaClientes)){
                printf("User conectado!\n");
                //MANDAR MENSAGEM DE VOLTA DIZENDO QUE FOI CONECTADO.
            }
                
        }

    }
}

int conectarCliente(struct sockaddr_in endCliente, InfoCliente registro, Cliente listaClientes){
    Cliente * cliente = &listaClientes;

    // VERIFICA SE JÁ HÁ CLIENTE COM O MESMO USER NA REDE.
    while(cliente != NULL){
        if(strcmp(cliente->registro.user, registro.user) == 0){
            printf(CLIENTE_JA_CONECTADO);
            int rMensagem = sendto(rSocket, CLIENTE_JA_CONECTADO, strlen(CLIENTE_JA_CONECTADO), 0, (struct sockaddr *) &endCliente, sizeof(struct sockaddr));
            
            if(rMensagem == -1){
                fprintf(stderr, ERRO_CONECTAR_CLIENTE);
                return -1;
            }
        }
        cliente = cliente->proximo;
    }

    cliente = &listaClientes;
    while(cliente->proximo != NULL)
        cliente = cliente->proximo;

    cliente->proximo = malloc(sizeof(cliente));
    (cliente->proximo)->endereco = endCliente;
    (cliente->proximo)->registro = registro;
    (cliente->proximo)->proximo = NULL;

    printf("%s (%s): ", registro.nome, registro.user);
    printf(SUCESSO_CONEXAO_CLIENTE);

    return 0;
}

int clienteConectado(struct sockaddr_in endCliente, Cliente listaClientes){
    Cliente * cliente = &listaClientes;
    struct sockaddr_in endClienteLista = cliente->endereco;

    while(cliente != NULL){
        int bEndereco = strncmp((char *) &endCliente.sin_addr.s_addr, (char *) &endClienteLista.sin_addr.s_addr, sizeof(unsigned long));

        int bPorta = strncmp((char *) &endCliente.sin_port, (char *) &endClienteLista.sin_port, sizeof(unsigned short));

        int bFamilia = strncmp((char *) &endCliente.sin_family, (char *) &endClienteLista.sin_family, sizeof(unsigned short));

        if(bEndereco == 0 && bPorta == 0 && bFamilia == 0)
            return 1;
        
        return 0;
    }
}

InfoCliente criaRegistroCliente(char * infoGeral){

    int tamanho = strlen(infoGeral);
    int j = -1;
    int a = 0;
    char mRegistro[2][64];

    for(int i = 0; i < strlen(infoGeral); i++){
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
    strcpy(infoCliente.nome, mRegistro[0]);

    strcpy(infoCliente.user, mRegistro[1]);

    infoCliente.moderador = 0;

    return infoCliente;

}