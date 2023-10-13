#include "msg.h"

void tirabarran(char* msg) {
    // Calcula o tamanho da string
    int msg_tam = (int) strlen(msg);  
    // Substitui o último caractere (nova linha) por um caractere nulo
    msg[msg_tam-1] = '\0';  
}

/**
 * Cria mensagem de aviso do servidor, envolvendo um cliente.
 * @param cliente cliente alvo de uma execução do servidor.
 * @param objeto mensagem objeto (principal) da informação.
 * @param cor cor da mensagem.
 * @return mensagem criada.
*/
char * mensagemServidor(Cliente * cliente, const char objeto[], const char cor[]){    
    char * mensagem = (char *) malloc((TAM_MSG+1) * sizeof(char)); // Aloca a memória da mensagem dinamicamente.

    if(mensagem == NULL){
        return NULL;
    }
    
    strcpy(mensagem, cor); //Insere a cor do usuário.
    
    if(cliente != NULL)
        strcat(mensagem, cliente->registro.user); //Adiciona o user do cliente na mensagem.
    
    strcat(mensagem, objeto); //Adiciona a mensagem objeto na mensagem.
    strcat(mensagem, RESET); //Reseta a cor do personagem.
    strcat(mensagem, "\n\0");

    return mensagem;
}

/**
 * Cria mensagem de aviso do servidor, envolvendo dois clientes.
 * @param cliente cliente remetente de uma execução do servidor.
 * @param cliente cliente destinatário de uma execução do servidor.
 * @param objeto mensagem objeto (principal) da informação.
 * @param cor cor da mensagem.
 * @return mensagem criada.
*/
char * mensagemServidorClientes(Cliente * remetente, Cliente * destinatario, const char objeto[], const char cor []){
    char * mensagem = (char *) malloc((TAM_MSG+1) * sizeof(char)); // Aloca a memória da mensagem dinamicamente.

    //Se não foi possível alocar a mensagem, retorna nulo.
    if(mensagem == NULL){
        return NULL;
    }

    strcpy(mensagem, cor);
    strcat(mensagem, remetente->registro.user);
    strcat(mensagem, objeto);
    strcat(mensagem, destinatario->registro.user);
    strcat(mensagem, ".");
    strcat(mensagem, RESET);
    strcat(mensagem, "\n\0");

    return mensagem;
}

/**
 * Cria mensagem de um cliente no servidor.
 * @param cliente cliente que envia a mensagem.
 * @param objeto informação principal da mensagem.
 * @return mensagem criada.
*/
char * mensagemCliente(Cliente * cliente, const char objeto[]){
    char * mensagem = (char *) malloc((TAM_MSG+1) * sizeof(char)); // Aloca a memória da mensagem dinamicamente.

    if(mensagem == NULL){
        return NULL;
    }
    
    strcpy(mensagem, COR_USER); //Insere a cor do usuário.
    strcat(mensagem, cliente->registro.nome);
    strcat(mensagem, "(");
    strcat(mensagem, cliente->registro.user); //Adiciona o user do cliente na mensagem.
    strcat(mensagem, "): ");
    strcat(mensagem, RESET); //Reseta à cor padrão.
    strcat(mensagem, objeto); //Adiciona a mensagem objeto na mensagem.
    strcat(mensagem, "\n\0");

    return mensagem;
}


char * mensagemErro(const char objeto[]){
    char * mensagem = (char *) malloc(TAM_MSG * sizeof(char)); // Aloca a memória da mensagem dinamicamente.
    if(mensagem == NULL)
        return NULL;
        
    strcpy(mensagem, ERRO); //Adiciona constante de ERRO no inicio da mensagem.
    strcat(mensagem, objeto); //Adiciona a mensagem objeto.
    strcat(mensagem, "\n\0");

    return mensagem;
}
