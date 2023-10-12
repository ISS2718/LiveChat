#include "servidor.h"

/**
 * Conecta um cliente no servidor.
 * 
 * @param rSocket resultado da conexão.
 * @param endCliente endereço do cliente que será conectado.
 * @param registro informações de registro do cliente.
 * @param listaClientes lista de clientes conectados no servidor.
 * 
 * @return conexão bem sucedida (0) ou falha na conexão (1).
 */
int conectarCliente(int rSocket, struct sockaddr_in endCliente, InfoCliente registro, ListaClientes * listaClientes, char **cores){
    printf(SISTEMA "Conetando cliente %s!\n", registro.user);

    //Se a lista não foi alocada.
    if(listaClientes == NULL){
        printf(SISTEMA "Não há lista de clientes!\n");

        insereListaClientes(registro, endCliente, listaClientes); //Insere cliente na lista.

        printf("%s (%s): ", registro.nome, registro.user);
        printf(SUCESSO_CONEXAO_CLIENTE);
        return 0;
    }

    //Verifica se o cliente está na lista de clientes conectados.
    if(existeClienteLista(registro, listaClientes)){
        printf(SISTEMA CLIENTE_JA_CONECTADO);

        //Envia mensagem "USUARIO_CADASTRADO" de volta ao cliente. Retorna status em rMensagem.
        int rMensagem = sendto(rSocket, USUARIO_CADASTRADO, strlen(CLIENTE_JA_CONECTADO), 0, (struct sockaddr *) &endCliente, sizeof(struct sockaddr));
        
        //Caso não seja possível enviar mensagem de volta ao cliente.
        if(rMensagem == -1){
            printf(ERRO ERRO_CONECTAR_CLIENTE);
            return -1;
        }

        return 0;
    }

    //Insere o cliente na lista de clientes.
    insereListaClientes(registro, endCliente, listaClientes);

    char mensagem[TAM_MSG];
    strcpy(mensagem, cores[0]);
    strcat(mensagem, registro.user);
    strcat(mensagem, " entrou no servidor!");
    strcat(mensagem, RESET);
    strcat(mensagem, "\n\0");

    //Envia mensagem de conexão no servidor para todos os conectados, exceto ao recém conectado.
    enviaMensagemParaOutros(rSocket, mensagem, endCliente, listaClientes);

    printf(SISTEMA "%s (%s): ", registro.nome, registro.user);
    printf(SUCESSO_CONEXAO_CLIENTE);
    return 0;
}

/**
 * Verifica se um cliente está conectado a partir de seu endereço.
 * @param endCliente endereço do cliente a se verificar.
 * @param listaClientes lista de clientes conectados no servidor.
 * @return conectado (1) ou não conectado (0).
*/
int clienteConectado(struct sockaddr_in endCliente, ListaClientes * listaClientes){  
    //Se o endereço está na lista. Retorna conectado (1). 
    if(existeEnderecoLista(endCliente, listaClientes)){
        printf(SISTEMA "Usuário de IP %d ja esta conectado!\n",endCliente.sin_addr.s_addr);
        return 1;
    }
    //Se o endereço não está na lista. Retorna não conectado (0).
    else{
        printf(SISTEMA "nao ha clientes com o endereco de IP %d conectados.\n",endCliente.sin_addr.s_addr);
        return 0;
    }
}

/**
 * Envia mensagem para todos os clientes conectados, exceto àquele que enviou.
 * @param rSocket feedback da conexão.
 * @param mensagem mensagem a se enviar para os clientes conectados.
 * @param mensageiro endereço do cliente que envia a mensagem.
 * @param listaClientes lista de clientes conectados no servidor.
*/
void enviaMensagemParaOutros(int rSocket, char mensagem[TAM_MSG], struct sockaddr_in mensageiro, ListaClientes * listaClientes){
    Cliente * cliente = *listaClientes;
    //Percorre a lista de clientes.
    while(cliente != NULL){
        int bEnderecosIguais = enderecosIguais(mensageiro,cliente->endereco); //Verifica os endereços entre cliente mensageiro e cliente da lista.
        //Se os endereços do cliente mensageiro com o cliente da lista não forem iguais.
        if(!bEnderecosIguais){
            int ret = sendto(rSocket, mensagem, strlen(mensagem),0, (struct sockaddr *)&cliente->endereco, sizeof(struct sockaddr)); //Envia a mensagem para o cliente da lista.
            
            //Se não foi possível enviar a mensagem para o cliente da lista.
            if(ret == -1){
                printf(ERRO "%s (%s): ", cliente->registro.nome, cliente->registro.user);
                printf(ERRO ERRO_MENSAGEM);
                break;
            }
        }
        cliente = cliente->proximo;
    }
}

/**
 * Envia mensagem para todos os clientes da lista, incluindo ao que enviou a mensagem.
 * @param rSocket feedback da conexão.
 * @param mensagem mensagem a se enviar para os clientes conectados.
 * @param listaClientes lista de clientes conectados no servidor.
*/
void enviaMensagemParaTodos(int rSocket, char mensagem[TAM_MSG], ListaClientes * listaClientes) {
    Cliente * cliente = *listaClientes;
    //Percorre a lista de clientes.
    while(cliente != NULL){
        int ret = sendto(rSocket, mensagem, strlen(mensagem),0, (struct sockaddr *)&cliente->endereco, sizeof(struct sockaddr)); //Envia a mensagem para o cliente da lista.
        
        //Se não foi possível enviar a mensagem para o cliente da lista.
        if(ret == -1){
            printf(ERRO "%s (%s): ", cliente->registro.nome, cliente->registro.user);
            printf(ERRO ERRO_MENSAGEM);
            break;
        }

        cliente = cliente->proximo;
    }
}

/**
 * Envia mensagem para um cliente em específico.
 * @param rSocket feedback do envio da mensagem.
 * @param cliente cliente a que se deseja enviar a mensagem.
 * @param mensagem mensagem a se enviar ao cliente.
 * @return mensagem enviada (0), erro no envio (-1).
*/
int enviaMensagemCliente(int rSocket, Cliente * cliente, char mensagem[TAM_MSG]) {
    struct sockaddr_in destino = cliente->endereco;
    int ret = sendto(rSocket, mensagem, strlen(mensagem), 0, (const struct sockaddr *) &destino, sizeof(struct sockaddr));
    if(ret == -1){
        fprintf(stderr, "Nao foi possivel enviar mensagem ao endereco %d.", destino.sin_addr.s_addr);
        return -1;
    }
    return 0;
}

/**
 * Verifica se uma mensagem é uma função dentro do servidor. Caso seja, executa a função escrita pelo cliente.
 * @param rSocket feedback do socket.
 * @param mensageiro endereço do mensageiro.
 * @param mensagem mensagem que pode conter a diretiva da função do servidor.
 * @param listaClientes lista de clientes conectados no servidor.
 * @return mensagem com uma função no servidor (1) ou mensagem normal (0).
 * 
 * @note uma mensagem-função é definida por "/funcao param1 param2". Uma função sem parâmetros ou sem um deles apenas resultam em strings vazias.
*/
int verificaExecutaFuncao(int rSocket, struct sockaddr_in mensageiro, char mensagem[TAM_MSG], ListaClientes * listaClientes, char ** cores){
    char funcao[TAM_MSG]; //String da diretiva da função.
    funcao[0] = '\0';
    char param1[TAM_MSG]; //String do primeiro parâmetro.
    param1[0] = '\0';
    char param2[TAM_MSG]; //String do segundo parâmetro.
    param2[0] = '\0';

    int bFuncao = 0; //booleano da função.
    int bParam1 = 0; //booleano do primeiro parâmetro.
    int bParam2 = 0; //booleano do segundo parâmetro.

    int j = 0;//Iterador das strings de diretiva e parâmetros.

    //Itera a mensagem até encontrar o fim da mensagem no '\n' ou '\0'.
    for(int i = 0; mensagem[i] != '\n' && mensagem[i] != '\0'; i++){
        //Se a mensagem contém '/', começa a leitura da diretiva da função.
        if(mensagem[i] == '/'){
            bFuncao = 1; //Identificada uma função.
            bParam1 = 0; //Não identificado um parâmetro 1.
            bParam2 = 0; //Não identificado um parâmetro 2.
        }
        //Se encontrado um espaço.
        else if (mensagem[i] == ' '){
            bFuncao = 0; //A diretiva da função termina.
            //Se nenhum parâmetro estava identificado.
            if(bParam1 == 0 && bParam2 == 0){
                bParam1 = 1; //O primeiro parâmetro é escolhido.
                bParam2 = 0; //O segundo parâmetro não é escolhido.
                j=0;
                continue;
            }
            //Se o primeiro parâmetro estava sendo escrito.
            else if(bParam2 == 0 && bParam1 == 1){
                bParam1 = 0; //Desseleciona a escrita no primeiro parâmetro.
                bParam2 = 1; //Seleciona a escrita no segundo parâmetro.
                j=0;
                continue;
            }
        }
        //Se a função está selecionada para escrita.
        if(bFuncao){
            funcao[j] = mensagem[i]; //Copia-se a mensagem para a string da função.
            funcao[j+1] = '\0'; //Pondo o fim da string após a ultima letra escrita.
            j++;
        }
        //Se o primeiro parâmetro está selecionado para escrita.
        if(bParam1){
            param1[j] = mensagem[i]; //Copia-se a mensagem para a string do primeiro parâmetro.
            param1[j+1] = '\0'; //Pondo o fim da string após a ultima letra da escrita.
            j++;
        }
        //Se o segundo parâmetro está selecionado para escrita.
        if(bParam2){
            param2[j] = mensagem[i]; //Copia-se a mensagem para a string do segundo parâmetro.
            param2[j+1] = '\0'; //Pondo o fim da string após a ultima letra da escrita.
            j++;
        }
    }

    printf(SISTEMA "FUNCAO: %s\n", funcao);
    printf(SISTEMA "PARAMETRO 1: %s\n", param1);
    printf(SISTEMA "PARAMETRO 2: %s\n", param2);

    //Verifica se a função é um sussurro: envia uma mensagem privada a um cliente.
    if(strcmp(funcao, SUSSURRO) == 0){
        Cliente * clienteMensageiro = retornaClientePorEndereco(mensageiro, listaClientes); //Obtém o cliente mensageiro.
        Cliente * cliente = retornaClientePorUsuario(param1,listaClientes); //Obtém o cliente destino.
        
        //Caso o cliente destino não esteja conectado.
        if(cliente == NULL){
            char msg[TAM_MSG];
            strcpy(msg, ERRO);
            strcat(msg, "usuario nao foi encontrado!\n");
            strcat(msg, "\0");
            printf(ERRO "o usuario buscado nao esta conectado!\n");
            enviaMensagemCliente(rSocket, clienteMensageiro, msg); //Envia uma mensagem avisando ao cliente mensageiro.
            return 1;
        }

        InfoCliente registroMensageiro = retornaRegistroPorEndereco(mensageiro, listaClientes); //Obtém o registro do cliente mensageiro

        //Mensagem privada ao cliente destino.
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
        enviaMensagemCliente(rSocket, cliente, mensagemCompleta); //Envia mensagem privada ao cliente destino.
        return 1;
    }

    //Verifica se a função é um mod: dá poder de moderador a um cliente.
    else if(strcmp(funcao, MOD) == 0){
        Cliente * clienteMensageiro = retornaClientePorEndereco(mensageiro, listaClientes); //Obtém o cliente mensageiro por endereço.
        Cliente * cliente = retornaClientePorUsuario(param1, listaClientes); //Obtém o cliente destino pelo seu usuário.

        //Se não há clientes na lista com o usuário do primeiro parâmetro.
        if(cliente == NULL){
            char msg[TAM_MSG];
            strcpy(msg, ERRO);
            strcat(msg, "usuario nao foi encontrado!\n");
            strcat(msg, "\0");
            printf(ERRO "o usuario buscado nao esta conectado!\n");
            enviaMensagemCliente(rSocket, clienteMensageiro, msg); //Envia mensagem avisando ao cliente mensageiro.
            return 1;
        }

        //Se o cliente mensageiro não é moderador, ele não pode executar essa função.
        if(clienteMensageiro->registro.moderador != 1){
            char msg[TAM_MSG];
            strcpy(msg, ERRO);
            strcat(msg, "você não pode executar essa função!\n");
            strcat(msg, "\0");
            enviaMensagemCliente(rSocket, clienteMensageiro, msg); //Envia mensagem avisando ao cliente mensageiro.
            printf(ERRO "usuario %s nao e moderador.\n", clienteMensageiro->registro.user);

            return 1;
        }

        char msg[TAM_MSG];
        bzero(msg, TAM_MSG);

        //Se cliente destino não tem poder de moderador.
        if(cliente->registro.moderador == 0){
            //Caso tenta-se dar poder ao cliente destino.
            if(strcmp(param2, "true") == 0){
                cliente->registro.moderador = 1; //Dá-se moderador ao cliente destino.
                cliente->registro.cor = COR_MOD; //Muda-se a cor do cliente destino para cor de moderador.
                strcpy(msg, AMARELO);
                strcat(msg, clienteMensageiro->registro.user);
                strcat(msg, " deu poder de moderador a ");
                strcat(msg, cliente->registro.user);
                strcat(msg, ".");
                strcat(msg, RESET);
                strcat(msg, "\n\0");
                enviaMensagemParaOutros(rSocket, msg, mensageiro, listaClientes); //Envia para todos os clientes sobre o novo moderador.

                strcpy(msg, AMARELO);
                strcat(msg, "você deu poder de moderador a ");
                strcat(msg, cliente->registro.user);
                strcat(msg, RESET);
                strcat(msg, ".");
                strcat(msg, " \n\0");
                enviaMensagemCliente(rSocket, clienteMensageiro, msg);
                printf(SISTEMA "usuario %s deu poder de moderador a %s.\n", clienteMensageiro->registro.user, cliente->registro.user);
        
            }
            //Caso tenta-se retirá-lo poder.
            else if(strcmp(param2, "false") == 0){
                //O cliente-destino não é moderador para retirar o seu poder de moderador.
                char msg[TAM_MSG];
                strcpy(msg, ERRO);
                strcat(msg, cliente->registro.user);
                strcat(msg, " não é moderador.\n");
                strcat(msg, RESET);
                strcat(msg, "\0");
                enviaMensagemCliente(rSocket, clienteMensageiro, msg); //Envia mensagem avisando ao cliente mensageiro.
                printf(SISTEMA "usuario %s tentou retirar poder de moderador de %s.\n", clienteMensageiro->registro.user, cliente->registro.user);
            }
        }
        //Caso o cliente destino tenha poder de moderador.
        else{
            //Caso tenta-se dá-lo poder de moderador.
            if(strcmp(param2, "true") == 0){
                //O cliente destino já tem poder de moderador.
                char msg[TAM_MSG];
                strcpy(msg, ERRO);
                strcat(msg, cliente->registro.user);
                strcat(msg, " ja possui poder de moderador.");
                strcat(msg, RESET);
                strcat(msg, "\n\0");
                enviaMensagemCliente(rSocket, clienteMensageiro, msg); //Envia mensagem avisando ao cliente-mensageiro. 
                printf(SISTEMA "usuario %s tentou dar poder de moderador a %s.\n", clienteMensageiro->registro.user, cliente->registro.user);
            }
            //Caso tenta-se tirá-lo o poder.
            else if (strcmp(param2, "false") == 0 ){
                cliente->registro.moderador = 0; //Retira-se o poder de moderador do cliente-destino.
                srand(time(NULL)%7);
                cliente->registro.cor = rand() % NUM_CORES_USERS; //Atribui-se uma cor aleatória para o user do cliente-destino.
                strcpy(msg, AMARELO);
                strcat(msg, clienteMensageiro->registro.user);
                strcat(msg, " retirou o poder de moderador de ");
                strcat(msg, cliente->registro.user);
                strcat(msg, ".");
                strcat(msg, RESET);
                strcat(msg, "\n\0");
                enviaMensagemParaOutros(rSocket, msg, mensageiro, listaClientes); //Envia mensagem a todos sobre a revogação do poder de moderador.

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
    
    //Verifica se a função é de sair: um cliente deseja sair do servidor.
    else if(strcmp(funcao, FECHAR_CLIENTE) == 0) {
        Cliente * clienteMensageiro = retornaClientePorEndereco(mensageiro, listaClientes); //Obtém o cliente mensageiro por endereço.
        printf(SISTEMA "removendo usuario %s...\n", clienteMensageiro->registro.user);
        char mensagem[TAM_USER + 50];
        strcpy(mensagem, "\x1B[31mUsuário: ");
        strcat(mensagem, clienteMensageiro->registro.user);
        strcat(mensagem, " desconectado");
        strcat(mensagem, RESET);
        strcat(mensagem, " \n\0");
        //enviaMensagemCliente(rSocket, clienteMensageiro, mensagem);
        enviaMensagemParaOutros(rSocket, mensagem, mensageiro, listaClientes); //Envia mensagem a todos os outros clientes sobre a saída.
        removeClientePorUsuario(clienteMensageiro->registro.user, listaClientes);
        printf(SISTEMA "cliente removido.\n");
        return 1;
    } 

    //Verifica se a função é de fechar o servidor: um cliente deseja fechar o servidor.
    else if(strcmp(funcao, FECHAR_SERVIDOR) == 0) {
        Cliente * clienteMensageiro = retornaClientePorEndereco(mensageiro, listaClientes); //Obtém o cliente mensageiro por endereço.
        char mensagem[TAM_USER + 50];

        //Verifica se o cliente-mensageiro é moderador.
        if(clienteMensageiro->registro.moderador == 1) {
            //Envia aviso de encerramento para os usuários, que o LiveChat foi encerrado pelo cliente-mensageiro.
            strcpy(mensagem, AMARELO);
            strcat(mensagem, clienteMensageiro->registro.user);
            strcat(mensagem, " encerrou o LiveChat.");
            strcat(mensagem, RESET);
            strcat(mensagem, " \n\0");
            enviaMensagemParaOutros(rSocket, mensagem, mensageiro, listaClientes);

            //Envia aviso de encerramento para o moderador que encerrou o LiveChat.
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
        //Caso o cliente-mensageiro não é moderador.
        else{
            //Envia aviso ao cliente-mensageiro sobre não ser permitido encerrar o LiveChat.
            strcpy(mensagem, ERRO);
            strcat(mensagem, "você não pode executar essa função!\n");
            strcat(mensagem, "\0");
            enviaMensagemCliente(rSocket, clienteMensageiro, mensagem);
            printf(ERRO "usuario %s nao e moderador.\n", clienteMensageiro->registro.user);
        }

        return 1;
    }
    
    //Verifica se a função é de mutar um cliente: um moderador deseja mutar um usuário.
    else if(strcmp(funcao, MUTE) == 0) {
        Cliente * clienteMensageiro = retornaClientePorEndereco(mensageiro, listaClientes); //Obtém o cliente-mensageiro pelo seu endereço.
        Cliente * mutado = retornaClientePorUsuario(param1, listaClientes);  //Obtém o cliente-destino pelo seu usuário.
        if(clienteMensageiro->registro.moderador == 1) {
            //Se deseja-se MUTAR o usuario.
            if(strcmp(param2, "true") == 0) {
                //Se o usuário está DESMUTADO.
                if(mutado->registro.mute == 0){
                    mutado->registro.mute = 1; //Muta-se o cliente-destino.

                    //Avisa a todos que o cliente-destino foi mutado.
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
                    //Avisa ao cliente-mensageiro que o cliente-destino já está mutado.
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
                    //Avisa ao cliente-mensageiro que o cliente-destino já está desmutado.
                    char mensagem[TAM_USER + 50];
                    strcpy(mensagem, ERRO);
                    strcat(mensagem, mutado->registro.user);
                    strcat(mensagem, " não está mutado."); 
                    strcat(mensagem, " \n\0");
                    enviaMensagemCliente(rSocket, clienteMensageiro, mensagem);
                }
                //Se o usuário está MUTADO.
                else{
                    mutado->registro.mute = 0; //Desmuta-se o cliente-destino.

                    //Avisa a todos que o cliente-destino foi desmutado.
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
    
    //Verifica se a função é de mostrar clientes: um cliente deseja ver todos os clientes conectados.
    else if(strcmp(funcao, MOSTRAR_CLIENTES) == 0){
        Cliente * clienteMensageiro = retornaClientePorEndereco(mensageiro, listaClientes); //Obtém o cliente-mensageiro pelo seu endereço.
        
        char mensagem[TAM_MSG];
        //Se a lista de clientes não existe ou está vazia.
        if(listaClientes == NULL || *listaClientes == NULL){
            //Envia ao cliente-mensageiro que não há clientes conectados.
            strcpy(mensagem, AMARELO);
            strcat(mensagem, "Não há clientes conectados.\n");
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
