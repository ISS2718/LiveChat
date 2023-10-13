#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "servidor.h"

#include "../Includes/msg.h"
#include "../Includes/config.h"
#include "../Includes/listaClientes.h"

const char cores[12][9] = {
    {"\x1B[32m"}, //VERDE
    {"\x1B[35m"}, //MAGENTA
    {"\x1B[36m"}, //CIANO
    {"\x1B[91m"}, //VERMELHO CLARO
    {"\x1B[92m"}, //VERDE CLARO
    {"\x1B[93m"}, //AMARELO CLARO
    {"\x1B[94m"}, //AZUL CLARO
    {"\x1B[95m"}, //MAGENTA CLARO
    {"\x1B[96m"}, //CIANO CLARO
    {"\x1B[37m"}, //BRANCO
    {"\x1B[34m"}, //AZUL (MOD)
    {"\x1B[39m"}  //RESET
};

int main(){
    int rSocket;
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
            
            imprimeRegistro(infoCliente); //Imprime as informações do registro do cliente.
            
            if(strcmp(infoCliente.user, MODERADOR1) == 0 || strcmp(infoCliente.user, MODERADOR2) == 0){
                infoCliente.moderador = 1;
                infoCliente.cor = COR_MOD;
            }

            if(conectarCliente(rSocket, endMensageiro, infoCliente, listaClientes, (char**) cores)){
                printf(AVISO "User %s conectado!\n", infoCliente.user);

                char statusCliente[TAM_MSG];
                strcat(statusCliente, infoCliente.user);
                strcat(statusCliente, " esta conectado!");
                strcat(statusCliente, "\0");
                
                enviaMensagemParaOutros(rSocket, statusCliente, endMensageiro, listaClientes);
            }
        }
        else{
            if(verificaExecutaFuncao(rSocket, endMensageiro, mensagem, listaClientes, (char**) cores))
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
                enviaMensagemParaOutros(rSocket, mensagemCompleta,endMensageiro, listaClientes);
            } else {
                printf(AVISO "Usuário %s mutado\n", registroMensageiro.user);
            }
        }

    }
}