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

### **3.1. Ubuntu.**

### **3.1.1 GCC.**

Para comkpilar e executar o LiveChat devemos ter o GCC. Primeiro, verifique se o gcc já não está instalado. Execute

```
$ gcc -v
```

Caso não haja, execute

```
$ sudo apt install build-essential
```

Se o gcc foi instalado com êxito, então ao executar ```$ gcc -v```, deve aparecer a última versão do gcc.
Caso não esteja você pode forçar a instalação do gcc com

```
$ sudo apt install gcc
```

#### **3.1.2. GNU make.**

Caso você tenha instalado o ```build-essential``` o GNU make, também conhecido como Makefile, provavelmente já está instalado. Para descobrir se realmente esté instalado, basta seguir um processo parecido ao do gcc. Execute

```
$ make -v
```

Caso não haja, execute

```
$ sudo apt install make
```

Se o make foi instalado com êxito, então ao executar ```$ make -v```, deve aparecer a última versão do make.

### **3.2. Outras Distribuições Linux.**

Para outras distribuições linux teste se não há o gcc e o make já instalado com

```
$ gcc -v
```

```
$ make -v
```

Caso não tenha um, ou os dois, basta procurar por pacotes equivalentes ao do ubunto. Pois certamente existirá.

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



## **6. Resumo do Cliente.**

## **7. Tecnologias.**

As seguintes ferramentas foram usadas na construção do projeto:

- [C](https://devdocs.io/c/)
- [GCC](https://gcc.gnu.org/)
- [GNU make](https://www.gnu.org/software/make/manual/make.html)

## **8. Licença.**

[MIT License](https://github.com/ISS2718/Whisper.Driver/blob/main/LICENSE) © [Beatriz Lomes da Silva](https://github.com/bealomes), [Hugo H. Nakamura](https://github.com/ikuyorih9), [Isaac S. Soares](https://github.com/ISS2718), [João Pedro Gonçalves](https://github.com/JoaoHardline), [Nicholas Estevão P. de O. Rodrigues Bragança](https://github.com/nicholasestevao).


