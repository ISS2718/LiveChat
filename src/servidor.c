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
int enviaMensagemCliente(Cliente * cliente, char mensagem[TAM_MSG]);
Cliente * retornaClientePorUsuario(char usuario[TAM_USER], ListaClientes * listaClientes);
int verificaExecutaFuncao(struct sockaddr_in mensageiro, char mensagem[TAM_MSG], ListaClientes * listaClientes);

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
            if(verificaExecutaFuncao(endMensageiro, mensagem, listaClientes))
                continue;

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

            if(registroMensageiro.mute == 0) {
                enviaMensagemTodos(mensagemCompleta,endMensageiro, listaClientes);
            } else {
                printf(AVISO "Usuário %s mutado\n", registroMensageiro.user);
            }
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

int enviaMensagemCliente(Cliente * cliente, char mensagem[TAM_MSG]){
    struct sockaddr_in destino = cliente->endereco;
    int ret = sendto(rSocket, mensagem, strlen(mensagem), 0, (struct sockaddr_in *) &destino, sizeof(struct sockaddr));
    if(ret == -1){
        fprintf(stderr, "Nao foi possivel enviar mensagem ao endereco %d.", destino.sin_addr.s_addr);
        return -1;
    }
    return 0;
}

int verificaExecutaFuncao(struct sockaddr_in mensageiro, char mensagem[TAM_MSG], ListaClientes * listaClientes){
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

    
    if(strcmp(funcao, SUSSURRO) == 0){
        //Cliente * cliente = retornaClientePorUsuario(param,listaClientes);
        //printf("%s\n", cliente->registro.user);
        //enviaMensagemCliente(cliente, param);
        return 1;
    } else if(strcmp(funcao, FECHAR_CLIENTE) == 0) {
        InfoCliente desconectado = retornaRegistroPorEndereco(mensageiro, listaClientes);
        char mensagem[TAM_USER + 50];
        strcpy(mensagem, "\x1B[31mUsuário: ");
        strcat(mensagem, desconectado.user);
        strcat(mensagem, " desconectado\n\0\x1B[39m");
        enviaMensagemTodos(mensagem, mensageiro, listaClientes);
        //removeListaPorUsuario(desconectado.user, listaClientes);
    } else if(strcmp(funcao, FECHAR_SERVIDOR) == 0) {
        InfoCliente desconectado = retornaRegistroPorEndereco(mensageiro, listaClientes);
        char mensagem[TAM_USER + 50];
        if(desconectado.moderador == 1) {
            strcpy(mensagem, "\x1B[31mModerador: ");
            strcat(mensagem, desconectado.user);
            strcat(mensagem, " encerrou o LiveChat\n\0\x1B[39m");
            enviaMensagemTodos(mensagem, mensageiro, listaClientes);
            liberaListaClientes(listaClientes);
            exit(0);
        } else {
            strcpy(mensagem, "\x1B[31mUsuário: ");
            strcat(mensagem, desconectado.user);
            strcat(mensagem, " desconectado\n\0\x1B[39m");
            enviaMensagemTodos(mensagem, mensageiro, listaClientes);
            //removeListaPorUsuario(desconectado.user, listaClientes);
        }
    }  else if(strcmp(funcao, MUTE) == 0) {
        InfoCliente moderador = retornaRegistroPorEndereco(mensageiro, listaClientes);
        if(moderador.moderador == 1) {
            Cliente * mutado = retornaClientePorUsuario(param1, listaClientes); 
            char mensagem[TAM_USER + 50];

            strcpy(mensagem, "\x1B[31mUsuário: ");
            strcat(mensagem, mutado->registro.user);

            if(strcmp(param2, "true")) {
                mutado->registro.mute = 1;
                strcat(mensagem, " mutado\n\0\x1B[39m");
            } else if(strcmp(param2, "false")) {
                mutado->registro.mute = 0;
                strcat(mensagem, " desmutado\n\0\x1B[39m");
            } 
        
            enviaMensagemTodos(mensagem, mensageiro, listaClientes);
        }
    }
    
    return 0;
}
