#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>

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

    //Cria um socket por datagrama (UDP).
    rSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if(rSocket == -1){
        printf(ERRO "Erro ao criar o socket.\n");
        return 1;
    }

    printf(SISTEMA "Socket criado!\n");

    //Inicia as informações do endereço do servidor.
    endServidor.sin_family = AF_INET;
    endServidor.sin_port = htons(PORTA);
    endServidor.sin_addr.s_addr = htonl(INADDR_ANY);

    memset(&(endServidor.sin_zero), '\0', 8);

    //Conecta o socket no endereço especificado.
    if(bind(rSocket, (struct sockaddr *) &endServidor, sizeof(struct sockaddr)) == -1){
        printf(ERRO "Erro ao criar o socket.\n");
        return -1;
    }

    printf(AVISO "O socket esta conectado na porta %d e nos IPs:\n", ntohs(endServidor.sin_port));

    struct ifaddrs * ifap, * ifa;
    
    //Obtém e imprime todos os IPs que a máquina fornece. Todos serão IPs do servidor.
    if (getifaddrs(&ifap) == -1) {
        perror("getifaddrs");
        exit(EXIT_FAILURE);
    }
    ifa = ifap;
    while(ifa!=NULL){
        if (ifa->ifa_addr != NULL && ifa->ifa_addr->sa_family == AF_INET) {
            struct sockaddr_in *addr = (struct sockaddr_in *)ifa->ifa_addr;
            printf("\tIP: %s\n", inet_ntoa(addr->sin_addr));
        }
        ifa = ifa->ifa_next;
    }
    freeifaddrs(ifap);


    while(1){
        bzero(mensagem, TAM_MSG);

        int tamanhoEndereco = sizeof(struct sockaddr_in);
        int bytesRecebidos = recvfrom(rSocket,mensagem, TAM_MSG-1,0,(struct sockaddr *) &endMensageiro, (unsigned int *) &tamanhoEndereco);
        
        if(bytesRecebidos == -1){
            printf(ERRO "Erro ao receber mensagem.\n");
            return -1;
        }

        //Ignora mensagens vazias.
        if(strcmp(mensagem, "") == 0|| strcmp(mensagem, "\n") == 0 || strcmp(mensagem, " ") == 0)
            continue;

        printf(MENSAGEM "%s", mensagem); //Mostra a mensagem recebida

        //Se o cliente não está conectado, as mensagens que chegam são informações do usuário para conectá-lo.
        if(!clienteConectado(endMensageiro, listaClientes)){
            InfoCliente infoCliente = criaRegistroCliente(mensagem); //Cria o registro do cliente a partir da mensagem recebida.
            
            //Se o registro de clientes não for válido, volta ao início do laço.
            if(infoCliente.moderador == -1)
                continue;
            
            imprimeRegistro(infoCliente); //Imprime as informações do registro do cliente.
            
            //Verifica se o cliente é um moderador.
            if(strcmp(infoCliente.user, MODERADOR1) == 0 || strcmp(infoCliente.user, MODERADOR2) == 0){
                infoCliente.moderador = 1;
                infoCliente.cor = COR_MOD;
            }

            //Conecta o endereço e as informações recebidas em um cliente no servidor.
            if(conectarCliente(rSocket, endMensageiro, infoCliente, listaClientes, (char**) cores)){
                printf(AVISO "User %s conectado!\n", infoCliente.user);

                char statusCliente[TAM_MSG];
                strcat(statusCliente, infoCliente.user);
                strcat(statusCliente, " esta conectado!");
                strcat(statusCliente, "\0");
                
                enviaMensagemParaOutros(rSocket, statusCliente, endMensageiro, listaClientes); //Envia mensagem a todos os outros usuáriso de que o cliente foi conectado.
            }
        }
        //Se o cliente está conectado, sua mensagem pode ser enviada aos outros usuários.
        else{
            //Verifica se a mensagem é uma função executável.
            if(verificaExecutaFuncao(rSocket, endMensageiro, mensagem, listaClientes, (char**) cores))
                continue;

            Cliente * clienteMensageiro = retornaClientePorEndereco(endMensageiro, listaClientes); //Obtém o cliente pelo endereço da mensagem enviada.

            //Se o cliente não está mutado, sua mensagem é enviada.
            if(!clienteMensageiro->registro.mute){
                char * mensagemCompleta;
                //Verifica se o mensageiro é moderador, para mudar a cor do seu user.
                if(clienteMensageiro->registro.moderador)
                    mensagemCompleta = mensagemCliente(clienteMensageiro, mensagem, COR_MODERADOR,0);
                else
                    mensagemCompleta = mensagemCliente(clienteMensageiro, mensagem, COR_USER,0);

                //Envia a mensagem a todos os outros usuários.
                if(mensagemCompleta != NULL){
                    printf(MENSAGEM"%s", mensagemCompleta);
                    enviaMensagemParaOutros(rSocket, mensagemCompleta,endMensageiro, listaClientes);
                    free(mensagemCompleta);
                }
            }
            //Se o cliente está mutado, sua mensagem não é enviada.
            else{
                char * mensagemCompleta = mensagemServidorClientes(NULL, NULL, "Você está mutado", COR_SERVIDOR);
                //Envia mensagem ao cliente mensageiro sobre seu mudo.
                if(mensagemCompleta!=NULL){
                    enviaMensagemCliente(rSocket, clienteMensageiro, mensagemCompleta);
                    printf(AVISO "Usuário %s mutado\n", clienteMensageiro->registro.user);
                    free(mensagemCompleta);
                }
            }
        }

    }
}