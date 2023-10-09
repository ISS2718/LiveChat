#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#include "listaClientes.h"
#include "config.h"
#include "msg.h"

int rSocket;

InfoCliente criaRegistroCliente(char * infoGeral);
int clienteConectado(struct sockaddr_in endCliente, ListaClientes * listaClientes);
int conectarCliente(struct sockaddr_in endCliente, InfoCliente registro, ListaClientes * listaClientes);
void enviaMensagemTodos(char mensagem[TAM_MSG], struct sockaddr_in mensageiro, ListaClientes * listaClientes);
InfoCliente retornaRegistroPorEndereco(struct sockaddr_in endCliente, ListaClientes * listaClientes);
int enviaMensagemCliente(Cliente * cliente, char mensagem[TAM_MSG]);
Cliente * retornaClientePorUsuario(char usuario[TAM_USER], ListaClientes * listaClientes);
int verificaExecutaFuncao(char mensagem[TAM_MSG], ListaClientes * listaClientes);

int main(){
    struct sockaddr_in endServidor;
    struct sockaddr_in endMensageiro;
    char mensagem[TAM_MSG];

    ListaClientes * listaClientes = criaListaClientes();

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
        bzero(mensagem, TAM_MSG);

        int tamanhoEndereco = sizeof(struct sockaddr_in);
        int bytesRecebidos = recvfrom(rSocket,mensagem, TAM_MSG-1,0,(struct sockaddr *) &endMensageiro, (unsigned int *) &tamanhoEndereco);
        
        if(bytesRecebidos == -1){
            fprintf(stderr, "Erro ao receber mensagem.");
            return -1;
        }

        if(strcmp(mensagem, "") == 0|| strcmp(mensagem, "\n") == 0 || strcmp(mensagem, " ") == 0)
            continue;

        printf(MENSAGEM);
        printf("%s", mensagem);

        //Se o cliente não está conectado, as mensagens que chegam são informações do usuário para conectá-lo.
        if(!clienteConectado(endMensageiro, listaClientes)){
            InfoCliente infoCliente = criaRegistroCliente(mensagem); //Cria o registro do cliente a partir da mensagem recebida.

            if(infoCliente.moderador == -1)
                continue;
            
            printf(SISTEMA);
            printf("Dados do cliente: \n");
            printf("\t NOME: %s\n", infoCliente.nome);
            printf("\t USER: %s\n", infoCliente.user);
            printf("\t MODERADOR: %d\n", infoCliente.moderador);
            printf("\t COR: %d\n", infoCliente.cor);
            
            
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
            if(verificaExecutaFuncao(mensagem, listaClientes))
                continue;

            InfoCliente registroMensageiro = retornaRegistroPorEndereco(endMensageiro, listaClientes);
            
            char mensagemCompleta[TAM_MSG];
            bzero(mensagemCompleta, TAM_MSG);

            strcat(mensagemCompleta, registroMensageiro.nome);
            strcat(mensagemCompleta, "(");
            strcat(mensagemCompleta, registroMensageiro.user);
            strcat(mensagemCompleta, "): ");
            strcat(mensagemCompleta, mensagem);
            strcat(mensagemCompleta, "\n\0");

            printf(MENSAGEM);
            printf("%s", mensagemCompleta);
            enviaMensagemTodos(mensagemCompleta,endMensageiro, listaClientes);
        }

    }
}

int conectarCliente(struct sockaddr_in endCliente, InfoCliente registro, ListaClientes * listaClientes){
    printf(SISTEMA);
    printf("Conetando cliente %s!\n", registro.user);

    if(listaClientes == NULL){
        printf(SISTEMA);
        printf("nao ha lista de clientes!\n");

        insereListaClientes(registro, endCliente, listaClientes);

        printf("%s (%s): ", registro.nome, registro.user);
        printf(SUCESSO_CONEXAO_CLIENTE);
        return 0;
    }

    if(existeClienteLista(registro, listaClientes)){
        printf(SISTEMA);
        printf(CLIENTE_JA_CONECTADO);

        int rMensagem = sendto(rSocket, ERROR, strlen(CLIENTE_JA_CONECTADO), 0, (struct sockaddr *) &endCliente, sizeof(struct sockaddr));
        
        if(rMensagem == -1){
            fprintf(stderr, ERRO_CONECTAR_CLIENTE);
            return -1;
        }

        return 0;
    }

    insereListaClientes(registro, endCliente, listaClientes);

    printf("%s (%s): ", registro.nome, registro.user);
    printf(SUCESSO_CONEXAO_CLIENTE);
    return 0;
}

int clienteConectado(struct sockaddr_in endCliente, ListaClientes * listaClientes){    
    if(existeEnderecoLista(endCliente, listaClientes)){
        printf(SISTEMA);
        printf("Usuário de IP %d ja esta conectado!\n",endCliente.sin_addr.s_addr);
        return 1;
    }
    else{
        printf(SISTEMA);
        printf("nao ha clientes com o endereco de IP %d conectados.\n",endCliente.sin_addr.s_addr);
        return 0;
    }
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
    }

    return infoCliente;

}

void enviaMensagemTodos(char mensagem[TAM_MSG], struct sockaddr_in mensageiro, ListaClientes * listaClientes){
    Cliente * cliente = *listaClientes;
    
    while(cliente != NULL){
        int bEnderecosIguais = enderecosIguais(mensageiro,cliente->endereco);
        if(!bEnderecosIguais){
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

int enviaMensagemCliente(Cliente * cliente, char mensagem[TAM_MSG]){
    struct sockaddr_in destino = cliente->endereco;
    int ret = sendto(rSocket, mensagem, strlen(mensagem), 0, (struct sockaddr_in *) &destino, sizeof(struct sockaddr));
    if(ret == -1){
        fprintf(stderr, "Nao foi possivel enviar mensagem ao endereco %d.", destino.sin_addr.s_addr);
        return -1;
    }
    return 0;
}

Cliente * retornaClientePorUsuario(char usuario[TAM_USER], ListaClientes * listaClientes){
    Cliente * cliente = *listaClientes;
    while(cliente != NULL){
        if(strcmp(cliente->registro.user, usuario) == 0)
            return cliente;
        cliente = cliente->proximo;
    }
    return NULL;
}

int verificaExecutaFuncao(char mensagem[TAM_MSG], ListaClientes * listaClientes){
    char funcao[TAM_MSG];
    funcao[0] = '\0';
    char param1[TAM_MSG];
    param1[0] = '\0';
    char param2[TAM_MSG];
    param2[0] = '\0';

    int bFuncao = 0;
    int bParam1 = 0;
    int bParam2 = 0;

    int j = 0;
    for(int i = 0; mensagem[i] != '\n' && mensagem[i] != '\0'; i++){
        if(mensagem[i] == '/'){
            bFuncao = 1;
            bParam1 = 0;
            bParam2 = 0;
        }
        else if (mensagem[i] == ' '){
            bFuncao = 0;
            if(bParam1 == 0 && bParam2 == 0){
                bParam1 = 1;
                bParam2 = 0;
                j=0;
                continue;
            }
            else if(bParam2 == 0 && bParam1 == 1){
                bParam1 = 0;
                bParam2 = 1;
                j=0;
                continue;
            }
        }

        if(bFuncao){
            funcao[j] = mensagem[i];
            funcao[j+1] = '\0';
            j++;
        }
        if(bParam1){
            param1[j] = mensagem[i];
            param1[j+1] = '\0';
            j++;
        }
        if(bParam2){
            param2[j] = mensagem[i];
            param2[j+1] = '\0';
            j++;
        }
    }

    printf(SISTEMA "FUNCAO: %s\n", funcao);
    printf(SISTEMA "PARAMETRO 1: %s\n", param1);
    printf(SISTEMA "PARAMETRO 2: %s\n", param2);

    /*
    if(strcmp(funcao, SUSSURRO) == 0){
        Cliente * cliente = retornaClientePorUsuario(param,listaClientes);
        printf("%s\n", cliente->registro.user);
        enviaMensagemCliente(cliente, param);
        return 1;
    }*/
    return 0;
}
