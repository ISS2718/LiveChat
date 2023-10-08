#ifndef _MSG_H_
#define _MSG_H_

#define CLIENTE_JA_CONECTADO "Cliente ja esta conectado! \n"
#define SUCESSO_CONEXAO_CLIENTE "Cliente foi conectado com sucesso.\n"
#define ERRO_CONECTAR_CLIENTE "Nao foi possivel conectar o cliente.\n"
#define ERRO_MENSAGEM "Nao foi possivel enviar a mensagem.\n"
#define CLIENTE_NAO_ENCONTRADO "Cliente nao foi encontrado! \n"

#define MENSAGEM "\x1B[33mMENSAGEM: \x1B[37m"
#define SISTEMA "\x1B[33mSISTEMA: \x1B[37m"


// Colors
#define NCOLORS 10
#define GREEN   "\x1B[32m"
#define BLUE   "\x1B[34m"
#define MAGENTA  "\x1B[35m"
#define CYAN   "\x1B[36m"
#define LRED   "\x1B[91m"
#define LGREEN   "\x1B[92m"
#define LYELLOW   "\x1B[93m"
#define LBLUE   "\x1B[94m"
#define LMAGENTA  "\x1B[95m"
#define LCYAN   "\x1B[96m"
#define WHITE  "\x1B[37m"
#define RESET "\x1B[39m"

void ERRO(char * s) {
    printf("\x1B[31mERRO:\x1B[39m %s\n", s);
}

void WARNING(char * s) {
    printf("\x1B[33mWARNING:\x1B[39m %s\n", s);
}

#endif