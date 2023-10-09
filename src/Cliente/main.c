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

#include "cliente.h"

#include "../Includes/msg.h"
#include "../Includes/config.h"
#include "../Includes/listaClientes.h"

int main(){
    int socket_c;
    InfoCliente cliente;
    
    char usr_envia[TAM_NOME + TAM_USER + 1];

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
    printf("\n");
    printf(AMARELO"Nome Servidor:"RESET"%s\n", servidor->h_name);
    printf(AMARELO"Endereço IP do Servidor:"RESET"%s\n", inet_ntoa((struct in_addr)*((struct in_addr *)servidor->h_addr)));
    printf("\n");


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
    
    // criaparâmetros para enviar paras as treads
    ParametrosPthreads p;
    p.cliente = &cliente;
    p.socket_c = &socket_c;

    pthread_attr_init(&confg_thread);
    pthread_attr_setdetachstate(&confg_thread, PTHREAD_CREATE_JOINABLE);


    pthread_create(&thread_envia, &confg_thread, enviar, (void *) &p);
    pthread_create(&thread_recebe, &confg_thread, receber, (void *) &p);


    pthread_join(thread_envia, NULL);
    pthread_cancel(thread_recebe);

    close(socket_c);

    return 0;
}