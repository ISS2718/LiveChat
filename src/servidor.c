#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

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
int clienteConectado(struct sockaddr_in endCliente, Cliente * listaClientes);
int conectarCliente(struct sockaddr_in endCliente, InfoCliente registro, Cliente * listaClientes);
int clientesIguais(struct sockaddr_in A, struct sockaddr_in B);
void enviaMensagemTodos(char mensagem[TAM_MSG], struct sockaddr_in mensageiro, Cliente * listaClientes);
InfoCliente retornaRegistroPorEndereco(struct sockaddr_in endCliente, Cliente * listaClientes);
void imprimeClientes(Cliente * listaClientes);

int main(){
    struct sockaddr_in endServidor;
    struct sockaddr_in endMensageiro;
    char mensagem[TAM_MSG];
    bzero(mensagem, TAM_MSG);

    Cliente * listaClientes = NULL;

    rSocket = socket(AF_INET, SOCK_DGRAM, 0); // Cria um socket UDP;
    if(rSocket == -1){
        fprintf(stderr, "Erro ao criar o socket.");
        return -1;
    }

    printf("Socket criado!\n");

    endServidor.sin_family = AF_INET;
    endServidor.sin_port = htons(PORTA);
    endServidor.sin_addr.s_addr = htonl(INADDR_ANY);

    memset(&(endServidor.sin_zero), '\0', 8);

    if(bind(rSocket, (struct sockaddr *) &endServidor, sizeof(struct sockaddr)) == -1){
        fprintf(stderr, "Erro ao criar o socket.");
        return -1;
    }

    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &endServidor,ip,INET_ADDRSTRLEN);
    printf("Socket esta online no IP %s e na porta %d!\n", ip, ntohs(endServidor.sin_port));


    while(1){
        imprimeClientes(listaClientes);
        int tamanhoEndereco = sizeof(struct sockaddr_in);
        int bytesRecebidos = recvfrom(rSocket,mensagem, TAM_MSG-1,0,(struct sockaddr *) &endMensageiro, (unsigned int *) &tamanhoEndereco);
        
        if(bytesRecebidos == -1){
            fprintf(stderr, "Erro ao receber mensagem.");
            return -1;
        }
        printf(MENSAGEM);
        printf(mensagem);
        printf("\n");

        //Se o cliente não está conectado, as mensagens que chegam são informações do usuário para conectá-lo.
        if(!clienteConectado(endMensageiro, listaClientes)){
            InfoCliente infoCliente = criaRegistroCliente(mensagem); //Cria o registro do cliente a partir da mensagem recebida.

            printf("Dados do cliente: \n");
            printf("\t - %s\n", infoCliente.nome);
            printf("\t - %s\n", infoCliente.user);
            printf("\t - %d\n", infoCliente.moderador);
            
            
            if(conectarCliente(endMensageiro, infoCliente, listaClientes)){
                printf("User %s conectado!\n", infoCliente.user);

                char statusCliente[TAM_MSG];
                strcat(statusCliente, infoCliente.user);
                strcat(statusCliente, " esta conectado!");
                strcat(statusCliente, "\0");
                
                enviaMensagemTodos(statusCliente, endMensageiro, listaClientes);
            }
        }
        else{
            printf("Cliente está conectado! \n");
            InfoCliente registroMensageiro = retornaRegistroPorEndereco(endMensageiro, listaClientes);
            
            char mensagemCompleta[TAM_MSG];
            strcat(mensagemCompleta, registroMensageiro.nome);
            strcat(mensagemCompleta, "(");
            strcat(mensagemCompleta, registroMensageiro.user);
            strcat(mensagemCompleta, "): ");
            strcat(mensagemCompleta, mensagem);
            strcat(mensagemCompleta, "\n\0");

            printf("%s", mensagemCompleta);
            enviaMensagemTodos(mensagemCompleta,endMensageiro, listaClientes);
        }

    }
}

int conectarCliente(struct sockaddr_in endCliente, InfoCliente registro, Cliente * listaClientes){
    printf(SISTEMA);
    printf("Conetando cliente %s!\n", registro.user);
    Cliente * cliente = listaClientes;

    // VERIFICA SE JÁ HÁ CLIENTE COM O MESMO USER NA REDE.
    while(cliente != NULL){
        printf(SISTEMA);
        printf("Verificando cliente: %s...\n", cliente->registro.user);
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
    
    cliente->proximo = malloc(sizeof(Cliente));
    (cliente->proximo)->endereco = endCliente;
    (cliente->proximo)->registro = registro;
    (cliente->proximo)->proximo = NULL;

    printf("%s (%s): ", registro.nome, registro.user);
    printf(SUCESSO_CONEXAO_CLIENTE);
    return 0;
}

int clienteConectado(struct sockaddr_in endCliente, Cliente * listaClientes){
    Cliente * cliente = listaClientes;
    
    while(cliente != NULL){
        printf(SISTEMA);
        printf("Buscando cliente: %s.\n", cliente->registro.user);
        struct sockaddr_in endClienteLista = cliente->endereco;
        int bClientesIguais = clientesIguais(endCliente, endClienteLista);
        if(bClientesIguais){
            printf(SISTEMA);
            printf("Usuário %s ja esta conectado!\n",cliente->registro.user);
            return 1;
        }
        cliente = cliente->proximo;
    }
    printf(SISTEMA);
    printf("nao ha clientes com o endereco de IP %u.\n",endCliente.sin_addr.s_addr);
    return 0;
}

int clientesIguais(struct sockaddr_in A, struct sockaddr_in B){
    int bEndereco = strncmp((char *) &A.sin_addr.s_addr, (char *) &B.sin_addr.s_addr, sizeof(unsigned long));

    int bPorta = strncmp((char *) &A.sin_port, (char *) &B.sin_port, sizeof(unsigned short));

    int bFamilia = strncmp((char *) &A.sin_family, (char *) &B.sin_family, sizeof(unsigned short));

    if(bEndereco == 0 && bPorta == 0 && bFamilia == 0)
        return 1;
    
    return 0;
}

void imprimeClientes(Cliente * listaClientes){
    printf(SISTEMA);
    printf("imprimindo todos os clientes da lista.\n");

    Cliente * cliente = listaClientes;
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
            a++;
        }
    }

    InfoCliente infoCliente;
    strcpy(infoCliente.nome, mRegistro[0]);
    strcat(infoCliente.nome, "\0");

    strcpy(infoCliente.user, mRegistro[1]);
    strcat(infoCliente.user, "\0");

    infoCliente.moderador = 0;

    return infoCliente;

}

void enviaMensagemTodos(char mensagem[TAM_MSG], struct sockaddr_in mensageiro, Cliente * listaClientes){
    Cliente * cliente = listaClientes;
    
    while(cliente != NULL){
        int bClientesIguais = clientesIguais(mensageiro,cliente->endereco);
        if(!bClientesIguais){
            int ret = sendto(rSocket, mensagem, strlen(mensagem),0, (struct sockaddr *)&cliente->endereco, sizeof(struct sockaddr));
            
            if(ret == -1){
                fprintf(stderr, "%s (%s): ", cliente->registro.nome, cliente->registro.user);
                fprintf(stderr, ERRO_MENSAGEM);
                return;
            }
        }
        cliente = cliente->proximo;
    }
}

InfoCliente retornaRegistroPorEndereco(struct sockaddr_in endCliente, Cliente * listaClientes){
    InfoCliente infoCliente;
    strcat(infoCliente.nome, "\0");
    strcat(infoCliente.user, "\0");
    infoCliente.moderador = -1;
    
    Cliente * cliente = listaClientes;
    while(cliente!=NULL){
        struct sockaddr_in endClienteLista = cliente->endereco;
        int bClientesIguais = clientesIguais(endCliente, endClienteLista);
        if(bClientesIguais){
            infoCliente = cliente->registro;
            return infoCliente;
        }
            
        cliente = cliente->proximo;
    }

    printf(CLIENTE_NAO_ENCONTRADO);
    return infoCliente;
}

int removeCliente(struct sockaddr_in endCliente, Cliente * listaClientes){
    Cliente * cliente_anterior = NULL
    Cliente * cliente_atual = listaClientes;
    Cliente * cliente_sucessor = cliente_atual->proximo;
    
    while(cliente != NULL){
        printf(SISTEMA);
        printf("Buscando cliente: %s.\n", cliente->registro.user);
        struct sockaddr_in endClienteLista = cliente->endereco;
        int bClientesIguais = clientesIguais(endCliente, endClienteLista);
        if(bClientesIguais){
            Cliente * remover = cliente_atual;
            if(cliente_anterior == NULL) {
                if(cliente_sucessor == NULL) {
                    free(listaClientes);
                    listaClientes == NULL
                } else {
                    cliente_atual->endereco = cliente_sucessor->endereco;
                    cliente_atual->registro = cliente_sucessor->registro;
                    cliente_atual->proximo = cliente_sucessor->proximo;
                    free(cliente_sucessor);
                }
            } else {
                if(cliente_sucessor == NULL) {
                    free(cliente_atual);
                    cliente_anterior->proximo == NULL
                } else {
                    cliente_anterior->proximo = cliente_sucessor;
                    free(cliente_atual);
                }
            }
            return 1;
        }
        cliente_anterior = cliente_atual;
        cliente_atual = cliente->proximo;
    }
    printf(SISTEMA);
    printf("nao ha clientes com o endereco de IP %u.\n",endCliente.sin_addr.s_addr);
    return 0;
}