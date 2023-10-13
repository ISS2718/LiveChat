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
    // Variável de informações do cliente.
    InfoCliente cliente;
    
    // Buffer para envio do nome e usuário para servidor .
    char usr_envia[TAM_NOME + TAM_USER + 1];
    
    // Buffer para o ip do servidor.
    char servidor_ip[TAM_IP];

    // Zera todos os buffers (strings).
    bzero(cliente.nome, TAM_NOME);
    bzero(cliente.user, TAM_USER);
    bzero(servidor_ip, TAM_IP);
    bzero(usr_envia, TAM_NOME + TAM_USER + 1);

    // Requisita nome do usuário
    printf(AMARELO"Digite seu nome real (tam_máx %d):\n"RESET, TAM_NOME - 1);
    while(fgets(cliente.nome, TAM_NOME, stdin) == NULL){
        printf(ERRO"Digite novamente o seu nome\n");
    }

    // Requisita nome de usuário.
    printf(AMARELO"Digite seu usuário (tam_máx %d):\n"RESET, TAM_USER - 1);
    while(fgets(cliente.user, TAM_USER, stdin) == NULL){
        printf(ERRO"Digite novamente o seu usuário\n");
    }

    // Trira o "\n" das Strings recebidas pelo fgets
    tirabarran(cliente.nome);
    tirabarran(cliente.user);

    // Monta a String para envio do nome e usuário no padrão #nome#usuário. 
    usr_envia[0] = CODIGO_REGISTRO;
    usr_envia[1] = '\0';
    strcat(usr_envia, cliente.nome);
    usr_envia[strlen(usr_envia)] = CODIGO_REGISTRO;
    usr_envia[strlen(usr_envia) + 1] = '\0';
    strcat(usr_envia, cliente.user);
    strcat(usr_envia, "\n\0");

    // Requisita ip do servidor (hostname)
    printf(AMARELO"Digite o ip do servidor (tam_máx %d):\n"RESET, TAM_IP - 1);
    while(!scanf("%s", servidor_ip)) {
        printf(ERRO"Digite novamente o seu usuário\n");
    }

    // Pegando informações do servidor
    struct hostent *servidor = gethostbyname(servidor_ip);
    if(servidor == NULL) {
        // Se não foi possível conseguir as informações do servidor pelo ip recebido, printa erro
        printf(ERRO"Não foi possível obter as informações do servidor pelo ip recebido.\n");
        return 1;
    }
    // Se foi possível conseguir as informações do servidor pelo ip recebido, printa informações
    printf("\n");
    printf(AMARELO"Nome Servidor:"RESET"%s\n", servidor->h_name);
    printf(AMARELO"Endereço IP do Servidor:"RESET"%s\n", inet_ntoa((struct in_addr)*((struct in_addr *)servidor->h_addr)));
    printf("\n");


    // Criando e configurando o socket como IPV4, UDP e IPROTO_UDP
    int socket_c;
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

    // Criando Threads para enviar e receber mensagens.
    pthread_t thread_envia, thread_recebe;
    pthread_attr_t confg_thread;
    
    // Cria a estrutura de parâmetros para enviar paras as pthreads.
    ParametrosPthreads p;
    p.cliente = &cliente;
    p.socket_c = &socket_c;

    // Inicializa e configura a thread para ser joinable.
    pthread_attr_init(&confg_thread);
    pthread_attr_setdetachstate(&confg_thread, PTHREAD_CREATE_JOINABLE);

    // Cria a Thread de envio (Joinable).
    pthread_create(&thread_envia, &confg_thread, enviar, (void *) &p);

    // Inicializa e configura a thread para ser não joinable.
    pthread_attr_init(&confg_thread);
    pthread_attr_setdetachstate(&confg_thread, PTHREAD_CREATE_DETACHED);

    // Cria a Thread de recebimento (Não Joinable).
    pthread_create(&thread_recebe, &confg_thread, receber, (void *) &p);

    // Espera a thread a de envio terminar.
    pthread_join(thread_envia, NULL);

    // Encerra a thread de recebimento.
    pthread_cancel(thread_recebe);

    // fecha o socket.
    close(socket_c);

    // Encerra o programa
    return 0;
}