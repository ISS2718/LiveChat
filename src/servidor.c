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

const char cores[12][9] = {
    {"\x1B[32m"},
    {"\x1B[34m"},
    {"\x1B[35m"},
    {"\x1B[36m"},
    {"\x1B[91m"},
    {"\x1B[92m"},
    {"\x1B[93m"},
    {"\x1B[94m"},
    {"\x1B[95m"},
    {"\x1B[96m"},
    {"\x1B[37m"},
    {"\x1B[39m"}
};

int rSocket;

InfoCliente criaRegistroCliente(char * infoGeral);
int clienteConectado(struct sockaddr_in endCliente, ListaClientes * listaClientes);
int conectarCliente(struct sockaddr_in endCliente, InfoCliente registro, ListaClientes * listaClientes);
void enviaMensagemTodos(char mensagem[TAM_MSG], struct sockaddr_in mensageiro, ListaClientes * listaClientes);
InfoCliente retornaRegistroPorEndereco(struct sockaddr_in endCliente, ListaClientes * listaClientes);

int main(){
    struct sockaddr_in endServidor;
    struct sockaddr_in endMensageiro;
    char mensagem[TAM_MSG];

    ListaClientes * listaClientes = criaListaClientes();

    rSocket = socket(AF_INET, SOCK_DGRAM, 0); // Cria um socket UDP;
    if(rSocket == -1){
        printf(ERRO "Erro ao criar o socket.\n");
        return 1;
    }

    printf(SISTEMA "Socket criado!\n");

    endServidor.sin_family = AF_INET;
    endServidor.sin_port = htons(PORTA);
    endServidor.sin_addr.s_addr = htonl(INADDR_ANY);

    memset(&(endServidor.sin_zero), '\0', 8);

    if(bind(rSocket, (struct sockaddr *) &endServidor, sizeof(struct sockaddr)) == -1){
        printf(ERRO "Erro ao criar o socket.\n");
        return -1;
    }

    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &endServidor,ip,INET_ADDRSTRLEN);
    printf(AVISO"Socket esta online no IP %s e na porta %d!\n", ip, ntohs(endServidor.sin_port));


    while(1){
        bzero(mensagem, TAM_MSG);

        int tamanhoEndereco = sizeof(struct sockaddr_in);
        int bytesRecebidos = recvfrom(rSocket,mensagem, TAM_MSG-1,0,(struct sockaddr *) &endMensageiro, (unsigned int *) &tamanhoEndereco);
        
        if(bytesRecebidos == -1){
            printf(ERRO "Erro ao receber mensagem.\n");
            return -1;
        }

        if(strcmp(mensagem, "") == 0|| strcmp(mensagem, "\n") == 0 || strcmp(mensagem, " ") == 0)
            continue;

        printf(MENSAGEM"%s", mensagem);

        //Se o cliente não está conectado, as mensagens que chegam são informações do usuário para conectá-lo.
        if(!clienteConectado(endMensageiro, listaClientes)){
            InfoCliente infoCliente = criaRegistroCliente(mensagem); //Cria o registro do cliente a partir da mensagem recebida.

            if(infoCliente.moderador == -1)
                continue;
            
            printf(SISTEMA "Dados do cliente: \n");
            printf("\t\t NOME: %s\n", infoCliente.nome);
            printf("\t\t USER: %s\n", infoCliente.user);
            printf("\t\t MODERADOR: %d\n", infoCliente.moderador);
            printf("\t\t COR: %d\n", infoCliente.cor);
            
            
            if(conectarCliente(endMensageiro, infoCliente, listaClientes)){
                printf(AVISO"User %s conectado!\n", infoCliente.user);

                char statusCliente[TAM_MSG];
                strcat(statusCliente, infoCliente.user);
                strcat(statusCliente, " esta conectado!");
                strcat(statusCliente, "\0");
                
                enviaMensagemTodos(statusCliente, endMensageiro, listaClientes);
            }
        }
        else{
            InfoCliente registroMensageiro = retornaRegistroPorEndereco(endMensageiro, listaClientes);
            
            char mensagemCompleta[TAM_MSG];
            bzero(mensagemCompleta, TAM_MSG);

            strcat(mensagemCompleta, cores[registroMensageiro.cor]);
            strcat(mensagemCompleta, registroMensageiro.nome);
            strcat(mensagemCompleta, "(");
            strcat(mensagemCompleta, registroMensageiro.user);
            strcat(mensagemCompleta, "): ");
            strcat(mensagemCompleta, cores[11]);
            strcat(mensagemCompleta, mensagem);
            strcat(mensagemCompleta, "\n\0");

            printf(MENSAGEM"%s", mensagemCompleta);
            enviaMensagemTodos(mensagemCompleta,endMensageiro, listaClientes);
        }

    }
}

int conectarCliente(struct sockaddr_in endCliente, InfoCliente registro, ListaClientes * listaClientes){
    printf(SISTEMA "Conetando cliente %s!\n", registro.user);

    if(listaClientes == NULL){
        printf(SISTEMA "Não há lista de clientes!\n");

        insereListaClientes(registro, endCliente, listaClientes);

        printf("%s (%s): ", registro.nome, registro.user);
        printf(SUCESSO_CONEXAO_CLIENTE);
        return 0;
    }

    if(existeClienteLista(registro, listaClientes)){
        printf(SISTEMA CLIENTE_JA_CONECTADO);

        int rMensagem = sendto(rSocket, ERROR, strlen(CLIENTE_JA_CONECTADO), 0, (struct sockaddr *) &endCliente, sizeof(struct sockaddr));
        
        if(rMensagem == -1){
            printf(ERRO ERRO_CONECTAR_CLIENTE);
            return -1;
        }

        return 0;
    }

    insereListaClientes(registro, endCliente, listaClientes);

    printf("%s (%s): ", registro.nome, registro.user);
    printf(SISTEMA SUCESSO_CONEXAO_CLIENTE);
    return 0;
}

int clienteConectado(struct sockaddr_in endCliente, ListaClientes * listaClientes){    
    if(existeEnderecoLista(endCliente, listaClientes)){
        printf(SISTEMA "Usuário de IP %d ja esta conectado!\n",endCliente.sin_addr.s_addr);
        return 1;
    }
    else{
        printf(SISTEMA "nao ha clientes com o endereco de IP %d conectados.\n",endCliente.sin_addr.s_addr);
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
                printf(ERRO "%s (%s): ", cliente->registro.nome, cliente->registro.user);
                printf(ERRO ERRO_MENSAGEM);
                break;;
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
