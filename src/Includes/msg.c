#include "msg.h"

void tirabarran(char* msg) {
    // Calcula o tamanho da string
    int msg_tam = (int) strlen(msg);  
    // Substitui o último caractere (nova linha) por um caractere nulo
    msg[msg_tam-1] = '\0';  
}

int mensagemTemFim(char * mensagem){
    if (mensagem == NULL)
        return 0;
    
    for(int i = 0; i < TAM_MSG; i++){
        if(mensagem[i] == '\0')
            return 1;
    }
    return 0;
}

/**
 * Cria mensagem de aviso ao servidor
 * @param cliente cliente alvo de uma execução do servidor.
 * @param complemento mensagem objeto que acompanha o nome do usuário.
*/
char * mensagemUsuario(Cliente * cliente, char * objeto){
    //Se não há mensagem de complemento, retorna NULL.
    if(objeto == NULL)
        return NULL;
    
    char * mensagem = (char *) malloc(sizeof(char*)); // Aloca a memória da mensagem dinamicamente.
    
    strcat(mensagem, COR_USER); //Insere a cor do usuário.
    strcat(mensagem, cliente->registro.user); //Adiciona o user do cliente na mensagem.
    strcat(mensagem, objeto); //Adiciona a mensagem objeto na mensagem.
    strcat(mensagem, RESET); //Reseta a cor do personagem.
    strcat(mensagem, "\n\0");

    return mensagem;
}

char * mensagemErro(char * objeto){
    //Se não há mensagem objeto, retorna NULL.
    if(objeto == NULL)
        return NULL;
    
    char * mensagem = (char *) malloc(sizeof(char*)); // Aloca a memória da mensagem dinamicamente.

    strcpy(msg, ERRO);
    strcat(msg, objeto);
    if(!mensagemTemFim(objeto))
        strcat(msg, "\0");

    return mensagem;
}
