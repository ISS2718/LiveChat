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

#define PORTA 8080
#define TAM_MSG 256
#define TAM_NOME 64
#define TAM_USER 16

#define ERROR "/ERROR\n"
#define FECHAR_CLIENTE "/QUIT\n"
#define FECHAR_SERVIDOR "/SEVERKILL\n"


#define CODIGO_REGISTRO '#'

typedef struct {
    char nome[TAM_NOME];
    char user[TAM_USER];
    int moderador;
} InfoCliente;


InfoCliente cliente;

int socket_c;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

char bufferEnviar[TAM_MSG];
char bufferReceber[TAM_MSG];

int fechar = 0; 

void tirabarran(char* msg) {
    int msg_tam = (int) strlen(msg);
    msg[msg_tam-1] = '\0';
}

void* enviar() {
    while (!fechar) {
        // Zera buffer para enviar nova mensagem
        bzero(bufferEnviar, TAM_MSG);

        // Recebe do usuário a mensagem a ser enviada para o servidor
        while(fgets(bufferEnviar, TAM_MSG, stdin) == NULL);

        // Enviando mensagem para servidor utilizando o socket
        if(send(socket_c, bufferEnviar, strlen(bufferEnviar), 0) == -1) {
            // Se não conseguiu enviar...

            // Printa erro de envio
            perror("\x1B[31msend\x1B[39m"); 

            // Muda variável de thread para fechar servidor
            pthread_mutex_lock(&mutex); 
                fechar = 1;
            pthread_mutex_unlock(&mutex);
        } else if((strcmp(bufferEnviar, FECHAR_CLIENTE) == 0) || (strcmp(bufferEnviar, FECHAR_SERVIDOR) == 0)) {
            // Se o que enviou foi um código de saída...
            
            // Printa que está saindo
            printf("\x1B[31mSaindo...\n\x1B[39m");

            // Muda variável de thread para fechar servidor
            pthread_mutex_lock(&mutex);
                fechar = 1;
            pthread_mutex_unlock(&mutex);
        }
    }

    // Fecha thread
    pthread_exit(NULL);
}

void* receber() {
    int mensagem_tam;
    while (!fechar) {
        bzero(bufferReceber, TAM_MSG);

        // Recebe mensagens do servidor
        if((mensagem_tam = recv(socket_c, bufferReceber, TAM_MSG - 1, 0)) == -1) {
            // Se não foi possível receber...

            // Printa erro de recebimento
            perror("\x1B[31msend\x1B[39m");

            // Muda variável de thread para fechar servidor
            pthread_mutex_lock(&mutex);
                fechar = 1;
            pthread_mutex_unlock(&mutex);
        } else {
            // Garante que a mensagem tem o '\0'
            bufferReceber[mensagem_tam] = '\0';

            // Verifica se não foi uma mensagem de erro do servidor
            //****************MUDAR ESSA MACRO DE ERRO*******************
            // ERRO DE USUÁRIO JÁ UTILIZADO
            if(strcmp(ERROR, bufferReceber) == 0) {
                // Se usuário já estiver sendo utilizado...

                // Printa aviso de usuário já utilizado
                printf("\x1B[31Error:\x1B[39m O usuário %s já está sendo utlizado, mude para conectar.\n", cliente.nome);
                
                // Muda variável de thread para fechar servidor
                pthread_mutex_lock(&mutex);
                fechar = 1;
                pthread_mutex_unlock(&mutex);
            } else {
                printf("%s", bufferReceber);
            }
        }
    }

    // Fecha thread
    pthread_exit(NULL);
}

int main(){
    bzero(bufferEnviar, TAM_MSG);
    bzero(bufferReceber, TAM_MSG);

    char usr_envia[TAM_NOME + TAM_USER + 1];
    
    //**********************CRIAR MACRO TAMANHO IP**********************
    char servidor_ip[TAM_NOME];

    // pegando informações do usuário
    printf("\x1B[33mDigite seu nome:\x1B[39m \n");
    while(fgets(cliente.nome, TAM_NOME, stdin) == NULL){
        printf("\x1B[31mERRO:\x1B[39m Digite novamente o seu nome\n");
    }
    
    tirabarran(cliente.nome);
    strcat(usr_envia, "#");
    strcat(usr_envia, cliente.nome);

    printf("\x1B[33mDigite seu usuário:\x1B[39m \n");
    while(fgets(cliente.user, TAM_USER, stdin) == NULL){
        printf("\x1B[31mERRO:\x1B[39m Digite novamente o seu usuário\n");
    }

    tirabarran(cliente.user);
    strcat(usr_envia, "#");
    strcat(usr_envia, cliente.user);
    strcat(usr_envia, "\0");

    //**********************TIRAR DPS**********************
    printf("usuário: %s\n", usr_envia);

    printf("\x1B[33mDigite o ip do servidor:\x1B[39m \n");
    while(!scanf("%s", servidor_ip)) {
        printf("\x1B[31mERRO:\x1B[39m Digite novamente o seu usuário\n");
    }

    // pegando informações do servidor
    struct hostent *servidor;
    if((servidor = gethostbyname(servidor_ip)) == NULL) {        // get the host info
        printf("\x1B[31mERRO:\x1B[39m Não foi possível obter as informações do socket_c.\n");
        exit(EXIT_FAILURE);
    }

    printf("\x1B[33mNome Servidor:\x1B[39m %s\n", servidor->h_name);
    printf("\x1B[33mEndereço IP do Servidor:\x1B[39m %s\n", inet_ntoa((struct in_addr)*((struct in_addr *)servidor->h_addr)));


    // Criando e configurando o socket como IPV4, UDP e IPROTO_UDP
    if((socket_c = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        close(socket_c);
        printf("\x1B[31mERRO:\x1B[39m Não foi possível criar o descritor do socket.\n");
        exit(EXIT_FAILURE);
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
        close(socket_c);
        printf("\x1B[31mERRO:\x1B[39m Não foi possível conetctar no servidor.\n");
        exit(EXIT_FAILURE);
    }

    // Enviando dados do usuário pro servidor utilizando o socket
    if(send(socket_c, usr_envia, strlen(usr_envia), 0) == -1) {
        perror("\x1B[31msend\x1B[39m");
        close(socket_c);
        exit(EXIT_FAILURE);
    }

    pthread_t thread_envia, thread_recebe;
    pthread_attr_t confg_thread;

    pthread_attr_init(&confg_thread);
    pthread_attr_setdetachstate(&confg_thread, PTHREAD_CREATE_JOINABLE);


    pthread_create(&thread_envia, &confg_thread, enviar, NULL);
    pthread_create(&thread_recebe, &confg_thread, receber, NULL);

    pthread_join(thread_envia, NULL);
    pthread_join(thread_recebe, NULL);

    pthread_mutex_destroy(&mutex);

    close(socket_c);
    return 0;
}