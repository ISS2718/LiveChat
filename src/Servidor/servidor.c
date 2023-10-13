#include "servidor.h"

int conectarCliente(int rSocket, struct sockaddr_in endCliente, InfoCliente registro, ListaClientes * listaClientes, char **cores){
    printf(SISTEMA "Conetando cliente %s!\n", registro.user);

    //Se a lista não foi alocada.
    if(listaClientes == NULL){
        printf(SISTEMA "Não há lista de clientes!\n");

        Cliente * cliente = criaCliente(endCliente, registro); //Cria um cliente com as suas informações.
        insereListaClientes(cliente, listaClientes); //Insere cliente na lista.

        printf("%s (%s): ", registro.nome, registro.user);
        printf(SUCESSO_CONEXAO_CLIENTE);
        return 0;
    }

    //Verifica se o cliente está na lista de clientes conectados.
    if(existeClienteLista(registro, listaClientes)){
        printf(SISTEMA CLIENTE_JA_CONECTADO);

        //Envia mensagem "USUARIO_CADASTRADO" de volta ao cliente. Retorna status em rMensagem.
        int rMensagem = sendto(rSocket, USUARIO_CADASTRADO, strlen(USUARIO_CADASTRADO), 0, (struct sockaddr *) &endCliente, sizeof(struct sockaddr));
        
        //Caso não seja possível enviar mensagem de volta ao cliente.
        if(rMensagem == -1){
            printf(ERRO ERRO_CONECTAR_CLIENTE);
            return -1;
        }

        return 0;
    }

    //Cria um cliente com as suas informações.
    Cliente * cliente = criaCliente(endCliente, registro);

    //Insere o cliente na lista de clientes.
    insereListaClientes(cliente, listaClientes);

    char * mensagem = mensagemServidor(cliente, " entrou no servidor!", COR_CONECTADO);
    if(mensagem != NULL){
        enviaMensagemParaTodos(rSocket, mensagem, listaClientes);
        printf(SISTEMA "%s", mensagem);
        free(mensagem);
    }

    return 0;
}

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

void enviaMensagemParaOutros(int rSocket, char * mensagem, struct sockaddr_in mensageiro, ListaClientes * listaClientes){
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

int enviaMensagemCliente(int rSocket, Cliente * cliente, char * mensagem) {
    struct sockaddr_in destino = cliente->endereco;
    int ret = sendto(rSocket, mensagem, strlen(mensagem), 0, (const struct sockaddr *) &destino, sizeof(struct sockaddr));
    if(ret == -1){
        fprintf(stderr, "Nao foi possivel enviar mensagem ao endereco %d.", destino.sin_addr.s_addr);
        return -1;
    }
    return 0;
}


int verificaExecutaFuncao(int rSocket, struct sockaddr_in mensageiro, char * mensagem, ListaClientes * listaClientes, char ** cores){
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

    //Verifica se a função é um sussurro: envia uma mensagem privada a um cliente.
    if(strcmp(funcao, SUSSURRO) == 0){
        Cliente * clienteMensageiro = retornaClientePorEndereco(mensageiro, listaClientes); //Obtém o cliente mensageiro.
        Cliente * cliente = retornaClientePorUsuario(param1,listaClientes); //Obtém o cliente destino.
        
        //Caso o cliente destino não esteja conectado.
        if(cliente == NULL){
            //Envia mensagem de erro ao cliente-mensageiro.
            char * msg = mensagemErro("usuário buscado não foi encontrado!"); 
            if(msg != NULL){
                enviaMensagemCliente(rSocket, clienteMensageiro, msg); //Envia uma mensagem avisando ao cliente mensageiro.
                free(msg);
            }
            printf("%s", msg);
            return 1;
        }

        //Envia mensagem privada ao cliente destino.
        char * sussurro = mensagemCliente(clienteMensageiro, param2, COR_SUSSURRO, 1);
        if(sussurro != NULL){
            enviaMensagemCliente(rSocket, cliente, sussurro); //Envia mensagem privada ao cliente destino.
            printf(MENSAGEM"(para %s) %s", param1, sussurro);
            free(sussurro);
        }
        
        return 1;
    }

    //Verifica se a função é um mod: dá poder de moderador a um cliente.
    else if(strcmp(funcao, MOD) == 0){
        Cliente * clienteMensageiro = retornaClientePorEndereco(mensageiro, listaClientes); //Obtém o cliente mensageiro por endereço.
        Cliente * cliente = retornaClientePorUsuario(param1, listaClientes); //Obtém o cliente destino pelo seu usuário.

        //Se não há clientes na lista com o usuário do primeiro parâmetro.
        if(cliente == NULL){
            //Envia mensagem avisando ao cliente mensageiro que o usuário não foi encontrado.
            char * msg = mensagemErro("usuário buscado não foi encontrado!"); 
            if(msg != NULL){
                enviaMensagemCliente(rSocket, clienteMensageiro, msg); //Envia uma mensagem avisando ao cliente mensageiro.
                free(msg);
            }
            printf("%s", msg);
            return 1;
        }

        //Se o cliente mensageiro não é moderador, ele não pode executar essa função.
        if(clienteMensageiro->registro.moderador != 1){
            //Envia mensagem avisando ao cliente mensageiro que ele não pode executar essa função.
            char * msg = mensagemErro("você não pode executar essa função!"); 
            if(msg != NULL){
                enviaMensagemCliente(rSocket, clienteMensageiro, msg); //Envia uma mensagem avisando ao cliente mensageiro.
                free(msg);
            }
            printf(ERRO "usuario %s nao e moderador.\n", clienteMensageiro->registro.user);
            return 1;
        }

        //Se cliente destino não tem poder de moderador e tenta dar poder ao cliente destino.
        if((cliente->registro.moderador == 0) && (strcmp(param2, "true") == 0)){
            cliente->registro.moderador = 1; //Dá-se moderador ao cliente destino.
            cliente->registro.cor = COR_MOD; //Muda-se a cor do cliente destino para cor de moderador.

            //Envia para todos os clientes sobre o novo moderador.
            char * msg = mensagemServidorClientes(clienteMensageiro, cliente, " deu poder de moderador a ", COR_SERVIDOR);
            if(msg != NULL){
                enviaMensagemParaOutros(rSocket, msg, mensageiro, listaClientes); 
                free(msg);
            }
            
            //Envia para o cliente-mensageiro sobre o novo moderador.
            msg = mensagemServidorClientes(NULL, cliente, "Você deu poder de moderador a ", COR_SERVIDOR);
            if(msg != NULL){
                enviaMensagemCliente(rSocket, clienteMensageiro, msg);
                free(msg);
            }

            printf(SISTEMA "usuario %s deu poder de moderador a %s.\n", clienteMensageiro->registro.user, cliente->registro.user);
        } 

        //Se cliente destino não tem poder de moderador e tenta retirar o poder do cliente destino.
        else if((cliente->registro.moderador == 0) &&(strcmp(param2, "false") == 0)){
            //Envia mensagem avisando ao cliente mensageiro que o cliente-destino não é moderador para retirar o seu poder de moderador.
            char * msg = mensagemErro("Você não pode tirar poder de moderador de quem não é moderador.");
            if(msg != NULL){
                enviaMensagemCliente(rSocket, clienteMensageiro, msg); 
                free(msg);
            }
            printf(ERRO "usuario %s tentou retirar poder de moderador de %s.\n", clienteMensageiro->registro.user, cliente->registro.user);
        }

        //Se cliente destino tem poder de moderador e tenta dar poder ao cliente destino.
        else if((cliente->registro.moderador == 1) && (strcmp(param2, "true") == 0)){
            //Envia mensagem avisando ao cliente-mensageiro que o cliente destino já tem poder de moderador.
            char * msg = mensagemErro("Você não pode dar poder de moderador a quem já é moderador.");
            if(msg != NULL){
                enviaMensagemCliente(rSocket, clienteMensageiro, msg); 
                free(msg);
            } 
            printf(ERRO "usuario %s tentou dar poder de moderador a %s.\n", clienteMensageiro->registro.user, cliente->registro.user);
        } 
        //Se cliente destino tem poder de moderador e tenta retirar o poder do cliente destino.
        else if ((cliente->registro.moderador == 1) && (strcmp(param2, "false") == 0)){
            cliente->registro.moderador = 0; //Retira-se o poder de moderador do cliente-destino.
            srand(time(NULL)%7);
            cliente->registro.cor = rand() % NUM_CORES_USERS; //Atribui-se uma cor aleatória para o user do cliente-destino.

            //Envia mensagem a todos sobre a revogação do poder de moderador.
            char * msg = mensagemServidorClientes(clienteMensageiro, cliente, " retirou poder de moderador de ", COR_SERVIDOR);
            if(msg != NULL){
                enviaMensagemParaOutros(rSocket, msg, mensageiro, listaClientes); 
                free(msg);
            }
            
            //Envia para o cliente-mensageiro sobre o novo moderador.
            msg = mensagemServidorClientes(NULL, cliente, "Você retirou poder de moderador de ", COR_SERVIDOR);
            if(msg != NULL){
                enviaMensagemCliente(rSocket, clienteMensageiro, msg);
                free(msg);
            }

            printf(SISTEMA "usuario %s retirou poder de moderador a %s.\n", clienteMensageiro->registro.user, cliente->registro.user);
        }

        return 1;
    } 
    
    //Verifica se a função é de sair: um cliente deseja sair do servidor.
    else if(strcmp(funcao, FECHAR_CLIENTE) == 0) {
        Cliente * clienteMensageiro = retornaClientePorEndereco(mensageiro, listaClientes); //Obtém o cliente mensageiro por endereço.
        printf(SISTEMA "removendo usuario %s...\n", clienteMensageiro->registro.user);
        
        //Envia mensagem de desconexão a todos os outros usuários.
        char * msg = mensagemServidorClientes(clienteMensageiro, NULL, " foi desconectado", COR_SERVIDOR);
        if(msg != NULL){
            enviaMensagemParaOutros(rSocket, mensagem, mensageiro, listaClientes); //Envia mensagem a todos os outros clientes sobre a saída.
            free(msg);
        }

        removeClientePorUsuario(clienteMensageiro->registro.user, listaClientes);
        printf(SISTEMA "cliente removido.\n");
        return 1;
    } 

    //Verifica se a função é de fechar o servidor: um cliente deseja fechar o servidor.
    else if(strcmp(funcao, FECHAR_SERVIDOR) == 0) {
        Cliente * clienteMensageiro = retornaClientePorEndereco(mensageiro, listaClientes); //Obtém o cliente mensageiro por endereço.

        //Verifica se o cliente-mensageiro é moderador.
        if(clienteMensageiro->registro.moderador == 1) {
            //Envia aviso de encerramento para os outros usuários.
            char * mensagem = mensagemServidorClientes(clienteMensageiro, NULL, " encerrou o LiveChat", COR_SERVIDOR);
            if(mensagem != NULL){
                enviaMensagemParaOutros(rSocket, mensagem, mensageiro, listaClientes);
                free(mensagem);
            }

            //Envia aviso de encerramento para o cliente-mensageiro.
            mensagem = mensagemServidorClientes(NULL, NULL, "Você encerrou o LiveChat", COR_SERVIDOR);
            if(mensagem == NULL){
                enviaMensagemCliente(rSocket, clienteMensageiro, mensagem);
                free(mensagem);
            }

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
            //Envia mensagem avisando ao cliente mensageiro que ele não pode executar essa função.
            char * msg = mensagemErro("você não pode executar essa função!"); 
            if(msg != NULL){
                enviaMensagemCliente(rSocket, clienteMensageiro, msg); //Envia uma mensagem avisando ao cliente mensageiro.
                free(msg);
            }
            printf(ERRO "usuario %s nao e moderador.\n", clienteMensageiro->registro.user);
        }

        return 1;
    }
    
    //Verifica se a função é de mutar um cliente: um moderador deseja mutar um usuário.
    else if(strcmp(funcao, MUTE) == 0) {
        Cliente * clienteMensageiro = retornaClientePorEndereco(mensageiro, listaClientes); //Obtém o cliente-mensageiro pelo seu endereço.
        Cliente * cliente = retornaClientePorUsuario(param1, listaClientes);  //Obtém o cliente-destino pelo seu usuário.
        
        // Se o cliente mensageiro for moderador e tentar mutar o usuário destino.
        if((clienteMensageiro->registro.moderador == 1) && (strcmp(param2, "true") == 0)) {
            //Se o usuário está DESMUTADO.
            if(cliente->registro.mute == 0){
                cliente->registro.mute = 1; //Muta-se o cliente-destino.

                //Avisa a todos que o cliente-destino foi mutado.
                char * mensagem = mensagemServidorClientes(cliente, NULL, " foi mutado", COR_SERVIDOR);
                if(mensagem != NULL){
                    enviaMensagemParaOutros(rSocket, mensagem, mensageiro, listaClientes); 
                    free(mensagem);
                }

                //Avisa ao cliente-mensageiro que o cliente-destino foi mutado.
                mensagem = mensagemServidorClientes(NULL, cliente, " Você mutou ", COR_SERVIDOR);
                if(mensagem != NULL){
                    enviaMensagemCliente(rSocket, clienteMensageiro, mensagem); 
                    free(mensagem);
                }

            }
            //Se o usuário está MUTADO.
            else{
                //Avisa ao cliente-mensageiro que o cliente-destino já está mutado.
                char * mensagem = mensagemErro("Usuário já está mutado.");
                if(mensagem != NULL){
                    enviaMensagemCliente(rSocket, clienteMensageiro, mensagem);
                    free(mensagem);
                }
            }                 
        } 
        // Se o cliente mensageiro for moderador e tentar desmutar o usuário destino.
        else if((clienteMensageiro->registro.moderador == 1) && (strcmp(param2, "false") == 0)) {
            //Se o usuário está DESMUTADO
            if(cliente->registro.mute == 0){
                //Avisa ao cliente-mensageiro que o cliente-destino já está desmutado.
                char * mensagem = mensagemErro("Usuário não está mutado.");
                if(mensagem != NULL){
                    enviaMensagemCliente(rSocket, clienteMensageiro, mensagem);
                    free(mensagem);
                }
            }
            //Se o usuário está MUTADO.
            else{
                cliente->registro.mute = 0; //Desmuta-se o cliente-destino.

                //Avisa a todos que o cliente-destino foi desmutado.
                char * mensagem = mensagemServidorClientes(cliente, NULL, " foi desmutado", COR_SERVIDOR);
                if(mensagem != NULL){
                    enviaMensagemParaOutros(rSocket, mensagem, mensageiro, listaClientes); 
                    free(mensagem);
                }

                //Avisa ao cliente-mensageiro que o cliente-destino foi mutado.
                mensagem = mensagemServidorClientes(NULL, cliente, " Você desmutou ", COR_SERVIDOR);
                if(mensagem != NULL){
                    enviaMensagemCliente(rSocket, clienteMensageiro, mensagem); 
                    free(mensagem);
                }
            }
        } 
        // Se o cliente mensageiro não for moderador
        else if(clienteMensageiro->registro.moderador == 0) {
            //Envia mensagem avisando ao cliente mensageiro que ele não pode executar essa função.
            char * msg = mensagemErro("você não pode executar essa função!"); 
            if(msg != NULL){
                enviaMensagemCliente(rSocket, clienteMensageiro, msg); //Envia uma mensagem avisando ao cliente mensageiro.
                free(msg);
            }
            printf(ERRO "usuario %s nao e moderador.\n", clienteMensageiro->registro.user);
        }
            
        return 1;
    }
    
    //Verifica se a função é de mostrar clientes: um cliente deseja ver todos os clientes conectados.
    else if(strcmp(funcao, MOSTRAR_CLIENTES) == 0){
        Cliente * clienteMensageiro = retornaClientePorEndereco(mensageiro, listaClientes); //Obtém o cliente-mensageiro pelo seu endereço.
        
        //Se a lista de clientes não existe ou está vazia.
        if(listaClientes == NULL || *listaClientes == NULL){
            //Envia ao cliente-mensageiro que não há clientes conectados.
            char * mensagem = mensagemServidorClientes(NULL, NULL, "Não há clientes conectados", COR_SERVIDOR);
            if(mensagem != NULL){
                enviaMensagemCliente(rSocket, clienteMensageiro, mensagem);
                free(mensagem);
            }
            return 1;
        }
        
        //Envia ao cliente-mensageiro a lista de clientes-conectados.
        strcpy(mensagem, AMARELO);
        strcat(mensagem, "CLIENTES CONECTADOS: \n");
        strcat(mensagem, RESET);
        strcat(mensagem, "\0");
        enviaMensagemCliente(rSocket, clienteMensageiro, mensagem);
        
        //Percorre todos os clientes da lista.
        Cliente * cliente = *listaClientes;
        while(cliente != NULL){
            strcpy(mensagem, "- ");
            strcat(mensagem, COR_USER);
            strcat(mensagem, cliente->registro.user);
            
            //Se um cliente é moderador, ele possui uma indicação de moderador.
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
