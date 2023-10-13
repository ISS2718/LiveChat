![LiveChat](img/livechat.png)

<p align="center">
<img src="https://img.shields.io/github/license/ISS2718/LiveChat"/>
<img src="https://img.shields.io/badge/Language-C-blue"/>
<img src="https://img.shields.io/badge/Socket-UDP-blue"/>
<img src="https://img.shields.io/badge/Server/Cliente-C-blue"/>
</p>

## **Resumo.**

Trabalho da disciplina de Redes de Computadores (SSC0641), lecionada pelo Professor Rodolfo Ipolito Meneguette, para o curso de Engenharia de Computação - USP São Carlos.

LiveChat é uma aplicação Servidor-Cliente de chat de "LiveStream" com comunicação UDP. Nele temos funções de usuários moderadores (mute, encerramento de chat) e "sussurros" para todos os usuários.

### **Autores.**

* **[Beatriz Lomes da Silva](https://github.com/bealomes)**;
* **[Hugo H. Nakamura](https://github.com/ikuyorih9)**;
* **[Isaac S. Soares](https://github.com/ISS2718)**;
* **[João Pedro Gonçalves](https://github.com/JoaoHardline)**;
* **[Nicholas Estevão P. de O. Rodrigues Bragança](https://github.com/nicholasestevao)**.

## **Tabela de conteúdos.**

   * [**Resumo.**](#resumo)
       * [**Autores.**](#autores)     
   * [**Tabela de conteúdos.**](#tabela-de-conte%C3%BAdos)
   * [**1. Composição do projeto.**](#1-composi%C3%A7%C3%A3o-do-projeto)
   * [**2. Pré-Requisitos**](#2-pr%C3%A9-requisitos)
   * [**3. Instalação dos Pré-requisitos.**](#3-instala%C3%A7%C3%A3o-dos-pr%C3%A9-requisitos)
      * [**3.1.Ubuntu**](#31-ubuntu) 
   * [**4. Guia de execução.**](#4-guia-de-execu%C3%A7%C3%A3o)
   * [**5. Resumo do Servidor.**](#5-resumo-do-driver)
   * [**6. Resumo do Cliente.**](#6-resumo-do-driver)
   * [**7. Tecnologias.**](#7-tecnologias)
   * [**8. Licença.**](#8-licença)

---

## **1. Composição do projeto.**

## **2. Pré-Requisitos**

* GCC;
* GNU make;

## **3. Instalação dos Pré-requisitos.**

### **3.1.Ubuntu**

## **4. Guia de execução.**

   ### **4.X. Funções dos usuários.**

   **1. Sussurro**: um usuário pode enviar uma *mensagem* privada a outro *usuário*, executando a linha

   ```
   /priv usuário mensagem
   ```

   **2. Fechar cliente**: um usuário pode se desconectar do servidor, executando a linha

   ```
   /quit
   ```

   **3. Mostrar clientes**: um usuário pode obter uma lista com todos os clientes conectados, com indicação especial aos moderadores, executando a linha

   ```
   /users
   ```

   **4. Poder de moderador**: um *moderador* por dar poder de moderador a outro usuário, executando a linha

   ```
   /mod usuario true
   ```
   e, ainda, retirá-lo com
   ```
   /mod usuario false
   ```
   * **OBS**: essa função só pode ser executada por um moderador. Também não é possível dar poder de moderador a um usuário já moderador, ou retirar de um usuário que não é moderador.


   **5. Mutar**: um moderador pode mutar um outro usuário e impedir que suas mensagens sejam enviadas aos usuários conectados. Executa-se
   ```
   /mute usuário true
   ```
   Para desmutá-lo, executa-se
   ```
   /mute usuário false
   ```

   * **OBS**: essa função só pode ser executada por um moderador. Também não é possível mutar um usuário já mutado, ou desmutar um usuário já desmutado.

   **6. Fechar servidor**: um moderador pode fechar o servidor. Esse comando desconecta todos os outros usuários e finaliza o servidor. Executa-se:
   ```
   /fservidor
   ```
   * **OBS**: essa função só pode ser executada por um moderador.

## **5. Resumo do Servidor.**

   ### 5.1. Lista de clientes conectados.
   
   Conforme os clientes são conectados, eles passam a ser identificados, dentro do servidor, como uma estrutura básica de uma lista encadeada.

   ```
   typedef struct cliente {
      struct sockaddr_in endereco;
      InfoCliente registro;
      struct cliente * proximo;
   }Cliente;
   ```
   Assim, é guardado o seu **endereço**, seu **registro** de informações e o cliente encadeado. O endereço é formado pela combinação do endereço de IP, a porta e a família da conexão.
   
   O registro de informações, por sua vez, é formado pelo **nome do cliente**, o **nome de usuário**, se **moderador** e se **mutado**.

   ```
   typedef struct InfoCliente {
      char nome[TAM_NOME];
      char user[TAM_USER];
      int moderador;
      int mute;
   }InfoCliente;
   ```

   ### 5.2. Socket.

   O socket é criado utilizando a biblioteca <sys/socket.h>, em C. Primeiro, cria-se o socket através da função

   ```
   socket(AF_INET, SOCK_DGRAM, 0);
   ```
   onde **AF_INET** indica o uso do protocolo IPV4 e **SOCK_DGRAM** indica o uso de uma conexão por datagramas (UDP). O último argumento utiliza o protocolo padrão da familía de endereço. Se o socket não for criado, a função retorna -1.

   Depois que o socket é criado, configura-se o endereço do servidor, selecionando sua **família de endereços**, **porta** e **endereço de IP**.

   ```
   struct sockaddr_in endServidor;

   endServidor.sin_family = AF_INET;
   endServidor.sin_port = htons(PORTA);
   endServidor.sin_addr.s_addr = htonl(INADDR_ANY);
   ```

   Por fim, é preciso conectar o servidor ao socket, utilizando a função *bind*.

   ```
   bind(rSocket, (struct sockaddr *) &endServidor, sizeof(struct sockaddr))
   ``` 

   Caso alguma informação do endereço do servidor esteja sendo utilizada, ou não exista, a conexão entre servidor e o socket retornará -1, logo, não sendo capaz de ligar o servidor.

   ### 5.3. Conexão do cliente.

   Uma vez que o servidor está conectado ao socket, o servidor passa a poder receber mensagens. Quando um cliente se conecta ao servidor e manda uma mensagem, ela não é imediatamente enviada a todos os conectados, pois é preciso registrar o cliente. Do ponto de vista do servidor, o cliente é apenas um endereço, sem informações de registro. Para se comunicar com outros usuários, é preciso que o cliente envie, primeiro, uma mensagem específica necessária para cadastrá-lo na lista de clientes.

   #### 5.3.1. Identificação dos clientes e moderadores.

   A primeira mensagem enviada por um cliente deve ser
   ```
   #nome_cliente#nome_usuario
   ```

   Ao receber essa linha, o servidor separa o nome do cliente (mRegistro[0]) e o nome de usuário (mRegistro[1]), criando um registro de informações do cliente. Todo usuário entra no servidor desmutado e não-moderador.

   ```
   strcpy(registro.nome, mRegistro[0]);
   strcpy(registro.user, mRegistro[1]);
   registro.moderador = 0;
   registro.mute = 0;
   ``` 

   Os moderadores são identificados pelo seu nome de usuário, que são fixos. Logo após criar o registro, através da primeira mensagem, o servidor verifica se o nome de usuário é de um moderador, trocando o valor de moderador no registro, se positivo.

   ```
   #define MODERADOR1 "ikuyorih9"
   #define MODERADOR2 "iss2718"

   if(strcmp(registro.user, MODERADOR1) == 0 || strcmp(registro.user, MODERADOR2) == 0)
      infoCliente.moderador = 1;
   ``` 

   Um cliente é criado, juntando as informações de registro com o endereço recebido pelo servidor no envio da mensagem. Logo em seguida, o cliente é salvo na lista de clientes conectados.

   ```
   Cliente * novoCliente = (Cliente*) malloc(sizeof(Cliente));
   novoCliente->endereco = endereco;
   novoCliente->registro = registro;
   novoCliente->proximo = NULL;
   ```
   #### 5.3.2. Envio de mensagens aos clientes.

   Quando o cliente já está conectado, ele passa a ser buscado na lista através do seu endereço, obtendo as informações do cliente diretamente da lista. Assim, a sua mensagem é formada por

   ```
   Nome(usuário): mensagem.
   ```

   Obviamente, essa mensagem será enviada aos outros usuários somente se o mensageiro não estiver mutado, ou seja

   ```
   if(!clienteMensageiro->registro.mute){
      enviaMensagemParaOutros(rSocket, mensagemCompleta,endMensageiro, listaClientes);
   }
   ```
   
### 5.4. Identificação das funções.

   As funções são identificadas através de
   ```
   /função param1 param2
   ```
   Toda mensagem enviada para o servidor passa pelo filtro da identificação das funções. Assim, uma linha de mensagem é separada em três strings: 

   ```
   char funcao[TAM_MSG];
   char param1[TAM_MSG];
   char param2[TAM_MSG];
   ```

   Caso a string *funcao* esteja vazia, após a passagem do filtro, então a mensagem não indica uma função e pode ser enviada aos clientes. Os parâmetros podem ser vazios, pois algumas funções não necessitam de parâmetros para serem executados. Assim, uma série de if-else direciona a execução de cada função.

   ```
   if(strcmp(funcao, SUSSURRO) == 0){...}
   else if(strcmp(funcao, MOD) == 0){...}
   else if(strcmp(funcao, FECHAR_CLIENTE) == 0){...}
   else if(strcmp(funcao, FECHAR_SERVIDOR) == 0){...}
   else if(strcmp(funcao, MUTE) == 0){...}
   else if(strcmp(funcao, MOSTRAR_CLIENTES) == 0){...}
   ```
## **6. Resumo do Cliente.**

## **7. Tecnologias.**

As seguintes ferramentas foram usadas na construção do projeto:

- [C](https://devdocs.io/c/)
- [GCC](https://gcc.gnu.org/)
- [GNU make](https://www.gnu.org/software/make/manual/make.html)

## **8. Licença.**

[MIT License](https://github.com/ISS2718/Whisper.Driver/blob/main/LICENSE) © [Beatriz Lomes da Silva](https://github.com/bealomes), [Hugo H. Nakamura](https://github.com/ikuyorih9), [Isaac S. Soares](https://github.com/ISS2718), [João Pedro Gonçalves](https://github.com/JoaoHardline), [Nicholas Estevão P. de O. Rodrigues Bragança](https://github.com/nicholasestevao).


