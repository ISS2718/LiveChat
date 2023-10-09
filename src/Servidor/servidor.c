#include "servidor.h"

int conectarCliente(int rSocket, struct sockaddr_in endCliente, InfoCliente registro, ListaClientes * listaClientes, char **cores){
    printf(SISTEMA "Conetando cliente %s!\n", registro.user);

    if(listaClientes == NULL){
        printf(SISTEMA "Não há lista de clientes!\n");

        insereListaClientes(registro, endCliente, listaClientes);

        printf("%s (%s): ", registro.nome, registro.user);
        printf(SUCESSO_CONEXAO_CLIENTE);
        return 0;
    }

    if(existeClienteLista(registro, listaClientes)){
        printf(SISTEMA CLIENTE_JA_CONECTADO);

        int rMensagem = sendto(rSocket, USUARIO_CADASTRADO, strlen(CLIENTE_JA_CONECTADO), 0, (struct sockaddr *) &endCliente, sizeof(struct sockaddr));
        
        if(rMensagem == -1){
            printf(ERRO ERRO_CONECTAR_CLIENTE);
            return -1;
        }

        return 0;
    }

    insereListaClientes(registro, endCliente, listaClientes);

    char mensagem[TAM_MSG];
    strcpy(mensagem, cores[0]);
    strcat(mensagem, registro.user);
    strcat(mensagem, " entrou no servidor!");
    strcat(mensagem, RESET);
    strcat(mensagem, "\n\0");

    enviaMensagemParaOutros(rSocket, mensagem, endCliente, listaClientes);

    printf(SISTEMA "%s (%s): ", registro.nome, registro.user);
    printf(SUCESSO_CONEXAO_CLIENTE);
    return 0;
}

int clienteConectado(struct sockaddr_in endCliente, ListaClientes * listaClientes){    
    if(existeEnderecoLista(endCliente, listaClientes)){
        printf(SISTEMA "Usuário de IP %d ja esta conectado!\n",endCliente.sin_addr.s_addr);
        return 1;
    }
    else{
        printf(SISTEMA "nao ha clientes com o endereco de IP %d conectados.\n",endCliente.sin_addr.s_addr);
        return 0;
    }
}

void enviaMensagemParaOutros(int rSocket, char mensagem[TAM_MSG], struct sockaddr_in mensageiro, ListaClientes * listaClientes){
    Cliente * cliente = *listaClientes;
    
    while(cliente != NULL){
        int bEnderecosIguais = enderecosIguais(mensageiro,cliente->endereco);
        if(!bEnderecosIguais){
            int ret = sendto(rSocket, mensagem, strlen(mensagem),0, (struct sockaddr *)&cliente->endereco, sizeof(struct sockaddr));
            
            if(ret == -1){
                printf(ERRO "%s (%s): ", cliente->registro.nome, cliente->registro.user);
                printf(ERRO ERRO_MENSAGEM);
                break;
            }
        }
        cliente = cliente->proximo;
    }
}

void enviaMensagemParaTodos(int rSocket, char mensagem[TAM_MSG], ListaClientes * listaClientes) {
    Cliente * cliente = *listaClientes;
    
    while(cliente != NULL){
        int ret = sendto(rSocket, mensagem, strlen(mensagem),0, (struct sockaddr *)&cliente->endereco, sizeof(struct sockaddr));
            
        if(ret == -1){
            printf(ERRO "%s (%s): ", cliente->registro.nome, cliente->registro.user);
            printf(ERRO ERRO_MENSAGEM);
            break;
        }

        cliente = cliente->proximo;
    }
}

int enviaMensagemCliente(int rSocket, Cliente * cliente, char mensagem[TAM_MSG]) {
    struct sockaddr_in destino = cliente->endereco;
    int ret = sendto(rSocket, mensagem, strlen(mensagem), 0, (const struct sockaddr *) &destino, sizeof(struct sockaddr));
    if(ret == -1){
        fprintf(stderr, "Nao foi possivel enviar mensagem ao endereco %d.", destino.sin_addr.s_addr);
        return -1;
    }
    return 0;
}

int verificaExecutaFuncao(int rSocket, struct sockaddr_in mensageiro, char mensagem[TAM_MSG], ListaClientes * listaClientes, char ** cores){
    char funcao[TAM_MSG];
    funcao[0] = '\0';
    char param1[TAM_MSG];
    param1[0] = '\0';
    char param2[TAM_MSG];
    param2[0] = '\0';

    int bFuncao = 0;
    int bParam1 = 0;
    int bParam2 = 0;

    int j = 0;
    for(int i = 0; mensagem[i] != '\n' && mensagem[i] != '\0'; i++){
        if(mensagem[i] == '/'){
            bFuncao = 1;
            bParam1 = 0;
            bParam2 = 0;
        }
        else if (mensagem[i] == ' '){
            bFuncao = 0;
            if(bParam1 == 0 && bParam2 == 0){
                bParam1 = 1;
                bParam2 = 0;
                j=0;
                continue;
            }
            else if(bParam2 == 0 && bParam1 == 1){
                bParam1 = 0;
                bParam2 = 1;
                j=0;
                continue;
            }
        }

        if(bFuncao){
            funcao[j] = mensagem[i];
            funcao[j+1] = '\0';
            j++;
        }
        if(bParam1){
            param1[j] = mensagem[i];
            param1[j+1] = '\0';
            j++;
        }
        if(bParam2){
            param2[j] = mensagem[i];
            param2[j+1] = '\0';
            j++;
        }
    }

    printf(SISTEMA "FUNCAO: %s\n", funcao);
    printf(SISTEMA "PARAMETRO 1: %s\n", param1);
    printf(SISTEMA "PARAMETRO 2: %s\n", param2);

    //ENVIA UMA MENSAGEM PRIVADA A UM USUÁRIO.
    if(strcmp(funcao, SUSSURRO) == 0){
        Cliente * clienteMensageiro = retornaClientePorEndereco(mensageiro, listaClientes);
        Cliente * cliente = retornaClientePorUsuario(param1,listaClientes);
        if(cliente == NULL){
            char msg[TAM_MSG];
            strcpy(msg, ERRO);
            strcat(msg, "usuario nao foi encontrado!\n");
            strcat(msg, "\0");
            printf(ERRO "o usuario buscado nao esta conectado!\n");
            enviaMensagemCliente(rSocket, clienteMensageiro, msg);
            return 1;
        }

        InfoCliente registroMensageiro = retornaRegistroPorEndereco(mensageiro, listaClientes);

        char mensagemCompleta[TAM_MSG];
        bzero(mensagemCompleta, TAM_MSG);

        strcat(mensagemCompleta, cores[2]);
        strcat(mensagemCompleta, registroMensageiro.nome);
        strcat(mensagemCompleta, "(");
        strcat(mensagemCompleta, registroMensageiro.user);
        strcat(mensagemCompleta, "): ");
        strcat(mensagemCompleta, param2);
        strcat(mensagemCompleta, RESET);
        strcat(mensagemCompleta, "\n\0");

        printf(MENSAGEM"(para %s) %s", param1, mensagemCompleta);
        enviaMensagemCliente(rSocket, cliente, mensagemCompleta);
        return 1;
    }
    //CONTROLA O PODER DE MOD DOS USUÁRIOS.
    else if(strcmp(funcao, MOD) == 0){
        Cliente * clienteMensageiro = retornaClientePorEndereco(mensageiro, listaClientes);

        Cliente * cliente = retornaClientePorUsuario(param1, listaClientes);
        if(cliente == NULL){
            char msg[TAM_MSG];
            strcpy(msg, ERRO);
            strcat(msg, "usuario nao foi encontrado!\n");
            strcat(msg, "\0");
            printf(ERRO "o usuario buscado nao esta conectado!\n");
            enviaMensagemCliente(rSocket, clienteMensageiro, msg);
            return 1;
        }

        if(clienteMensageiro->registro.moderador != 1){
            char msg[TAM_MSG];
            strcpy(msg, ERRO);
            strcat(msg, "você não pode executar essa função!\n");
            strcat(msg, "\0");
            enviaMensagemCliente(rSocket, clienteMensageiro, msg);
            printf(ERRO "usuario %s nao e moderador.\n", clienteMensageiro->registro.user);

            return 1;
        }

        char msg[TAM_MSG];
        bzero(msg, TAM_MSG);

        //Alvo não tem poder de moderador.
        if(cliente->registro.moderador == 0){
            //Caso o alvo não tenha poder de moderador e tenta-se dá-lo poder.
            if(strcmp(param2, "true") == 0){
                cliente->registro.moderador = 1;
                cliente->registro.cor = COR_MOD;
                strcpy(msg, AMARELO);
                strcat(msg, clienteMensageiro->registro.user);
                strcat(msg, " deu poder de moderador a ");
                strcat(msg, cliente->registro.user);
                strcat(msg, ".");
                strcat(msg, RESET);
                strcat(msg, "\n\0");
                enviaMensagemParaOutros(rSocket, msg, mensageiro, listaClientes);

                strcpy(msg, AMARELO);
                strcat(msg, "você deu poder de moderador a ");
                strcat(msg, cliente->registro.user);
                strcat(msg, RESET);
                strcat(msg, ".");
                strcat(msg, " \n\0");
                enviaMensagemCliente(rSocket, clienteMensageiro, msg);
                printf(SISTEMA "usuario %s deu poder de moderador a %s.\n", clienteMensageiro->registro.user, cliente->registro.user);
        
            }
            //Caso o alvo não tenha poder de moderador e tenta-se retirá-lo poder.
            else if(strcmp(param2, "false") == 0){
                char msg[TAM_MSG];
                strcpy(msg, ERRO);
                strcat(msg, cliente->registro.user);
                strcat(msg, " não é moderador.\n");
                strcat(msg, RESET);
                strcat(msg, "\0");
                enviaMensagemCliente(rSocket, clienteMensageiro, msg);
                printf(SISTEMA "usuario %s tentou retirar poder de moderador de %s.\n", clienteMensageiro->registro.user, cliente->registro.user);
            }
        }
        //Alvo tem poder de moderador.
        else{
            //Caso o alvo já tenha moderador e tenta-se dá-lo poder de moderador.
            if(strcmp(param2, "true") == 0){
                char msg[TAM_MSG];
                strcpy(msg, ERRO);
                strcat(msg, cliente->registro.user);
                strcat(msg, " ja possui poder de moderador.");
                strcat(msg, RESET);
                strcat(msg, "\n\0");
                enviaMensagemCliente(rSocket, clienteMensageiro, msg);
                printf(SISTEMA "usuario %s tentou dar poder de moderador a %s.\n", clienteMensageiro->registro.user, cliente->registro.user);
            }
            //Caso o alvo tenha poder de moderador e tenta-se tirá-lo o poder.
            else if (strcmp(param2, "false") == 0 ){
                cliente->registro.moderador = 0;
                srand(time(NULL)%7);
                cliente->registro.cor = rand() % NUM_CORES_USERS;
                strcpy(msg, AMARELO);
                strcat(msg, clienteMensageiro->registro.user);
                strcat(msg, " retirou o poder de moderador de ");
                strcat(msg, cliente->registro.user);
                strcat(msg, ".");
                strcat(msg, RESET);
                strcat(msg, "\n\0");
                enviaMensagemParaOutros(rSocket, msg, mensageiro, listaClientes);

                strcpy(msg, AMARELO);
                strcat(msg, "você retirou poder de moderador de ");
                strcat(msg, cliente->registro.user);
                strcat(msg, ".");
                strcat(msg, RESET);
                strcat(msg, " \n\0");
                enviaMensagemCliente(rSocket, clienteMensageiro, msg);

                printf(SISTEMA "usuario %s retirou poder de moderador a %s.\n", clienteMensageiro->registro.user, cliente->registro.user);
            }
        }
        
        return 1;
    } 
    //CLIENTE SAI DO SERVIDOR.
    else if(strcmp(funcao, FECHAR_CLIENTE) == 0) {
        Cliente * clienteMensageiro = retornaClientePorEndereco(mensageiro, listaClientes);
        printf(SISTEMA "removendo usuario %s...\n", clienteMensageiro->registro.user);
        char mensagem[TAM_USER + 50];
        strcpy(mensagem, "\x1B[31mUsuário: ");
        strcat(mensagem, clienteMensageiro->registro.user);
        strcat(mensagem, " desconectado");
        strcat(mensagem, RESET);
        strcat(mensagem, " \n\0");
        //enviaMensagemCliente(rSocket, clienteMensageiro, mensagem);
        enviaMensagemParaOutros(rSocket, mensagem, mensageiro, listaClientes);
        removeClientePorUsuario(clienteMensageiro->registro.user, listaClientes);
        printf(SISTEMA "cliente removido.\n");
        return 1;
    } 
    //CLIENTE FECHA O SERVIDOR.
    else if(strcmp(funcao, FECHAR_SERVIDOR) == 0) {
        Cliente * clienteMensageiro = retornaClientePorEndereco(mensageiro, listaClientes);
        char mensagem[TAM_USER + 50];
        if(clienteMensageiro->registro.moderador == 1) {
            // Envia aviso de encerramento para os usuários, que o LiveChat foi encerrado pelo moderador "tal".
            strcpy(mensagem, AMARELO);
            strcat(mensagem, clienteMensageiro->registro.user);
            strcat(mensagem, " encerrou o LiveChat.");
            strcat(mensagem, RESET);
            strcat(mensagem, " \n\0");
            enviaMensagemParaOutros(rSocket, mensagem, mensageiro, listaClientes);

            // Envia aviso de encerramento para o moderador que encerrou o LiveChat.
            strcpy(mensagem, AMARELO);
            strcat(mensagem, "Você encerrou o LiveChat.");
            strcat(mensagem, RESET);
            strcat(mensagem, " \n\0");
            enviaMensagemCliente(rSocket, clienteMensageiro, mensagem);

            // Desconecta todos os clientes
            enviaMensagemParaTodos(rSocket, DESCONECTAR, listaClientes);
            liberaListaClientes(listaClientes);

            // Avisa no terminal do servidor que ele foi finalizado
            printf(SISTEMA "o servidor foi finalizado.\n");

            // Finaliza sem erro o servidor; 
            exit(0);
        } 
        else{
            strcpy(mensagem, ERRO);
            strcat(mensagem, "você não pode executar essa função!\n");
            strcat(mensagem, "\0");
            enviaMensagemCliente(rSocket, clienteMensageiro, mensagem);
            printf(ERRO "usuario %s nao e moderador.\n", clienteMensageiro->registro.user);
        }

        return 1;
    }
    //FUNÇÃO QUE MUTA O USUÁRIO SELECIONADO.
    else if(strcmp(funcao, MUTE) == 0) {
        Cliente * clienteMensageiro = retornaClientePorEndereco(mensageiro, listaClientes); 
        Cliente * mutado = retornaClientePorUsuario(param1, listaClientes); 
        if(clienteMensageiro->registro.moderador == 1) {
            

            //Se deseja-se MUTAR o usuario.
            if(strcmp(param2, "true") == 0) {
                //Se o usuário está DESMUTADO.
                if(mutado->registro.mute == 0){
                    mutado->registro.mute = 1;
                    char mensagem[TAM_USER + 50];
                    strcpy(mensagem, AMARELO);
                    strcat(mensagem, mutado->registro.user);
                    strcat(mensagem, " foi mutado."); 
                    strcat(mensagem, RESET);
                    strcat(mensagem, " \n\0");
                    enviaMensagemParaOutros(rSocket, mensagem, mensageiro, listaClientes);

                    strcpy(mensagem, AMARELO);
                    strcat(mensagem, "você mutou ");
                    strcat(mensagem, mutado->registro.user);
                    strcat(mensagem, RESET);
                    strcat(mensagem, " \n\0");
                    enviaMensagemCliente(rSocket, clienteMensageiro, mensagem);
                }
                //Se o usuário está MUTADO.
                else{
                    char mensagem[TAM_USER + 50];
                    strcpy(mensagem, ERRO);
                    strcat(mensagem, mutado->registro.user);
                    strcat(mensagem, " já está mutado.");
                    strcat(mensagem, " \n\0");
                    enviaMensagemCliente(rSocket, clienteMensageiro, mensagem);
                }
                                
            } 
            //Se deseja-se DESMUTAR o usuario.
            else if(strcmp(param2, "false") == 0) {
                //Se o usuário está DESMUTADO
                if(mutado->registro.mute == 0){
                    char mensagem[TAM_USER + 50];
                    strcpy(mensagem, ERRO);
                    strcat(mensagem, mutado->registro.user);
                    strcat(mensagem, " não está mutado."); 
                    strcat(mensagem, " \n\0");
                    enviaMensagemCliente(rSocket, clienteMensageiro, mensagem);
                }
                //Se o usuário está MUTADO.
                else{
                    mutado->registro.mute = 0;
                    char mensagem[TAM_USER + 50];
                    strcpy(mensagem, AMARELO);
                    strcat(mensagem, mutado->registro.user);
                    strcat(mensagem, " foi desmutado.");
                    strcat(mensagem, RESET);
                    strcat(mensagem, " \n\0");
                    enviaMensagemParaOutros(rSocket, mensagem, mensageiro, listaClientes);

                    strcpy(mensagem, AMARELO);
                    strcat(mensagem, "você desmutou ");
                    strcat(mensagem, mutado->registro.user);
                    strcat(mensagem, RESET);
                    strcat(mensagem, " \n\0");
                    enviaMensagemCliente(rSocket, clienteMensageiro, mensagem);
                }
                               
            } 
            
            return 1;
        }
    }
    //FUNÇÃO QUE IMPRIME TODOS OS USUÁRIOS.
    else if(strcmp(funcao, MOSTRAR_CLIENTES) == 0){
        Cliente * clienteMensageiro = retornaClientePorEndereco(mensageiro, listaClientes);
        char mensagem[TAM_MSG];

        if(listaClientes == NULL || *listaClientes == NULL){
            strcpy(mensagem, AMARELO);
            strcat(mensagem, "Não há clientes conectados.: \n");
            strcat(mensagem, RESET);
            strcat(mensagem, "\0");
            enviaMensagemCliente(rSocket, clienteMensageiro, mensagem);
        }
        
        strcpy(mensagem, AMARELO);
        strcat(mensagem, "CLIENTES CONECTADOS: \n");
        strcat(mensagem, RESET);
        strcat(mensagem, "\0");
        enviaMensagemCliente(rSocket, clienteMensageiro, mensagem);
        
        Cliente * cliente = *listaClientes;
        while(cliente != NULL){
            strcpy(mensagem, "- ");
            strcat(mensagem, cores[cliente->registro.cor]);
            strcat(mensagem, cliente->registro.user);
            if(cliente->registro.moderador == 1){
                strcat(mensagem, " (moderador)");
            }
            strcat(mensagem, RESET);
            strcat(mensagem, "\n\0");
            enviaMensagemCliente(rSocket, clienteMensageiro, mensagem);

            cliente = cliente->proximo;
        }
        return 1;
    }
    
    return 0;
}
