#include "cliente.h"

void* enviar(void * arg) {
    ParametrosPthreads * p = (ParametrosPthreads *) arg;

    char bufferEnviar[TAM_MSG];

    while (1) {
        // Zera buffer para enviar nova mensagem
        bzero(bufferEnviar, TAM_MSG);

        // Recebe do usuário a mensagem a ser enviada para o servidor
        while(fgets(bufferEnviar, TAM_MSG, stdin) == NULL);
        tirabarran(bufferEnviar);
        // Enviando mensagem para servidor utilizando o socket
        if(send((*p->socket_c), bufferEnviar, strlen(bufferEnviar), 0) == -1) {
            // Se não conseguiu enviar...

            // Printa erro de envio
            printf(ERRO"Não foi possível enviar mensagem!\n"); 
            break;
        } else if((strcmp(bufferEnviar, FECHAR_CLIENTE) == 0)) {
            // Se o que enviou foi um código de saída...
            
            // Printa que está saindo
            printf(AMARELO"Saindo...\n"RESET);
            break;
        }
    }

    // Fecha thread
    pthread_exit(NULL);
}

void* receber(void * arg) {
    ParametrosPthreads * p = (ParametrosPthreads *) arg;

    char bufferReceber[TAM_MSG];
    
    int real_msg_tam;
    while (1) {
        bzero(bufferReceber, TAM_MSG);

        // Recebe mensagens do servidor
        if((real_msg_tam = recv((*p->socket_c), bufferReceber, TAM_MSG - 1, 0)) == -1) {
            // Se não foi possível receber...

            // Printa erro de recebimento
            printf(ERRO"Não foi possível receber mensagem!");
            exit(1);
        } else {
            // Garante que a mensagem tem o '\0'
            bufferReceber[real_msg_tam] = '\0';

            // Verifica se não foi um código de servidor
            if(strcmp(USUARIO_CADASTRADO, bufferReceber) == 0) {
                // Se usuário já estiver sendo utilizado...

                // Printa aviso de usuário já utilizado
                printf(ERRO"O usuário %s já está sendo utlizado, mude para conectar.\n", (*p->cliente).user);
                
                // Fecha cliente com erro, pois deu erro de usuário já conectado
                exit(1);
            } else if(strcmp(DESCONECTAR, bufferReceber) == 0) {
                // Se usuário foi desconectado...

                //Printa aviso que o servidor desconectou este usuário
                printf(SISTEMA"O servidor te desconectou");

                // Fecha cliente sem erro, pois o servidor que o desconectou.
                exit(0);
            } else {
                printf("%s", bufferReceber);
            }
        }
    }

    // Fecha thread
    pthread_exit(NULL);
}