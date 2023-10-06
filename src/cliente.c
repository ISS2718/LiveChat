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


#define CODIGO_REGISTRO '#'

typedef struct {
    char nome[TAM_NOME];
    char user[TAM_USER];
    int moderador;
} InfoCliente;

int socket_c;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* enviar();
void* receber();

char bufferEnviar[TAM_MSG];
char bufferReceber[TAM_MSG];

int main(){
    bzero(bufferEnviar, TAM_MSG);
    bzero(bufferReceber, TAM_MSG);

    InfoCliente cliente;

    char usr_envia[TAM_NOME + TAM_USER + 1];
    
    //**********************CRIAR MACRO TAMANHO IP**********************
    char servidor_ip[TAM_NOME];

    // pegando informações do usuário
    printf("\x1B[33mDigite seu nome:\x1B[39m \n");
    while(!scanf("%s", cliente.nome)) {
        printf("\x1B[31mERRO:\x1B[39m Digite novamente o seu nome\n");
    }
    
    strcat(usr_envia, "#");
    strcat(usr_envia, cliente.nome);

    printf("\x1B[33mDigite seu usuário:\x1B[39m \n");
    while(!scanf("%s", cliente.user)) {
        printf("\x1B[31mERRO:\x1B[39m Digite novamente o seu usuário\n");
    }

    strcat(usr_envia, "#");
    strcat(usr_envia, cliente.user);

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

    return 0;
}