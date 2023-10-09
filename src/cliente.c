#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "msg.h"
#include "config.h"
#include "listaClientes.h"


InfoCliente cliente;

int socket_c;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

char bufferEnviar[TAM_MSG];
char bufferReceber[TAM_MSG];

void* enviar() {
    while (1) {
        // Zera buffer para enviar nova mensagem
        bzero(bufferEnviar, TAM_MSG);

        // Recebe do usuário a mensagem a ser enviada para o servidor
        while(fgets(bufferEnviar, TAM_MSG, stdin) == NULL);

        // Enviando mensagem para servidor utilizando o socket
        if(send(socket_c, bufferEnviar, strlen(bufferEnviar), 0) == -1) {
            // Se não conseguiu enviar...

            // Printa erro de envio
            printf(ERRO"Não foi possível enviar mensagem!\n"); 
            break;
        } else if((strcmp(bufferEnviar, FECHAR_CLIENTE) == 0) || (strcmp(bufferEnviar, FECHAR_SERVIDOR) == 0)) {
            // Se o que enviou foi um código de saída...
            
            // Printa que está saindo
            printf(AMARELO"Saindo...\n"RESET);
            break;
        }
    }

    // Fecha thread
    pthread_exit(NULL);
}

void* receber() {
    int mensagem_tam;
    while (1) {
        bzero(bufferReceber, TAM_MSG);

        // Recebe mensagens do servidor
        if((mensagem_tam = recv(socket_c, bufferReceber, TAM_MSG - 1, 0)) == -1) {
            // Se não foi possível receber...

            // Printa erro de recebimento
            printf(ERRO"Não foi possível receber mensagem!");
            break;
        } else {
            // Garante que a mensagem tem o '\0'
            bufferReceber[mensagem_tam] = '\0';

            // Verifica se não foi uma mensagem de erro do servidor
            //****************MUDAR ESSA MACRO DE ERRO*******************
            // ERRO DE USUÁRIO JÁ UTILIZADO
            if(strcmp(ERROR, bufferReceber) == 0) {
                // Se usuário já estiver sendo utilizado...

                // Printa aviso de usuário já utilizado
                printf(ERRO"O usuário %s já está sendo utlizado, mude para conectar.\n", cliente.user);
                break;
                
            } else {
                printf("%s", bufferReceber);
            }
        }
    }

    // Fecha programa pois deu erro
    exit(1);
}

int main(){
    char usr_envia[TAM_NOME + TAM_USER + 1];

    bzero(bufferEnviar, TAM_MSG);
    bzero(bufferReceber, TAM_MSG);
    bzero(cliente.nome, TAM_NOME);
    bzero(cliente.user, TAM_USER);
    bzero(usr_envia, TAM_NOME + TAM_USER + 1);

    
    //**********************CRIAR MACRO TAMANHO IP**********************
    char servidor_ip[TAM_NOME];

    // pegando informações do usuário
    printf(AMARELO"Digite seu nome:\n"RESET);
    while(fgets(cliente.nome, TAM_NOME, stdin) == NULL){
        printf(ERRO"Digite novamente o seu nome\n");
    }

    printf(AMARELO"Digite seu usuário:\n"RESET);
    while(fgets(cliente.user, TAM_USER, stdin) == NULL){
        printf(ERRO"Digite novamente o seu usuário\n");
    }

    tirabarran(cliente.nome);
    tirabarran(cliente.user);

    strcpy(usr_envia, "#");
    strcat(usr_envia, cliente.nome);
    strcat(usr_envia, "#");
    strcat(usr_envia, cliente.user);
    strcat(usr_envia, "\n\0");

    printf(AMARELO"Digite o ip do servidor:\n"RESET);
    while(!scanf("%s", servidor_ip)) {
        printf(ERRO"Digite novamente o seu usuário\n");
    }

    // pegando informações do servidor
    struct hostent *servidor = gethostbyname(servidor_ip);
    if(servidor == NULL) {        // get the host info
        printf(ERRO"Não foi possível obter as informações do socket_c.\n");
        return 1;
    }

    printf(AMARELO"Nome Servidor:"RESET"%s\n", servidor->h_name);
    printf(AMARELO"Endereço IP do Servidor:"RESET"%s\n", inet_ntoa((struct in_addr)*((struct in_addr *)servidor->h_addr)));


    // Criando e configurando o socket como IPV4, UDP e IPROTO_UDP
    if((socket_c = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        close(socket_c);
        printf(ERRO"Não foi possível criar o descritor do socket.\n");
        return 1;
    }

    // Guardando as configurações do socket e o endereço de conexão
    struct sockaddr_in socket_endereço;

    // Salvando as configurações do socket
    socket_endereço.sin_family = AF_INET; // IPV4
    socket_endereço.sin_port = htons(PORTA); // PORTA
    socket_endereço.sin_addr = *((struct in_addr *) servidor->h_addr); // IP
    memset(&(socket_endereço.sin_zero), '\0', 8); // utlimos 8 bytes == 0

    // Realizando a conexão com o servidor
    if(connect(socket_c, (struct sockaddr*) &socket_endereço, sizeof(struct sockaddr)) == -1) {
        printf(ERRO"Não foi possível conetctar no servidor.\n");
        close(socket_c);
        return 1;
    }

    // Enviando dados do usuário pro servidor utilizando o socket
    if(send(socket_c, usr_envia, strlen(usr_envia), 0) == -1) {
        printf(ERRO"Fallha no envio do login!\n");
        close(socket_c);
        return 1;
    }

    pthread_t thread_envia, thread_recebe;
    pthread_attr_t confg_thread;

    pthread_attr_init(&confg_thread);
    pthread_attr_setdetachstate(&confg_thread, PTHREAD_CREATE_JOINABLE);


    pthread_create(&thread_envia, &confg_thread, enviar, NULL);
    pthread_create(&thread_recebe, &confg_thread, receber, NULL);


    pthread_join(thread_envia, NULL);
    pthread_cancel(thread_recebe);

    pthread_mutex_destroy(&mutex);

    close(socket_c);

    return 0;
}