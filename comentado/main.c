/*
 * ============================================================
 *  main.c  -  INTERFACE (totem) do MediCore Centro Clinico
 * ============================================================
 *
 *  Esse arquivo cuida SO da CLI: desenhar telas, ler input do
 *  usuario e decidir qual operacao da fila chamar. Nao tem
 *  logica de estrutura de dados aqui.
 */

#include <stdio.h>           /* printf, scanf, fgets, getchar  */
#include <stdlib.h>          /* system("clear") / system("cls")*/
#include <string.h>          /* strlen, strcpy, snprintf       */
#include "estrutura/lista.h" /* Tipos e funcoes da fila         */

/* ============================================================
 *  CONSTANTES DE LAYOUT
 *  Quantas colunas cada parte do "totem" ocupa.
 *  Mudando esses numeros, a tela inteira se ajusta.
 * ============================================================ */
#define MENU_W 38            /* Largura da coluna do MENU      */
#define FILA_W 32            /* Largura da coluna das SENHAS   */
#define MAX_LINHAS_FILA 16   /* Limite de linhas no painel     */
#define INDENT "  "          /* Recuo (2 espacos) antes das    */
                             /* bordas, so estetico             */

/* ============================================================
 *  Struct auxiliar para armazenar cada linha do painel
 *  lateral antes de imprimir. Usamos um array dessas
 *  estruturas para ter as linhas prontas e imprimir todas
 *  de uma vez junto com o menu.
 * ============================================================ */
typedef struct {
    char texto[FILA_W + 1];  /* +1 para o '\0' do fim da string*/
} LinhaFila;

/* ============================================================
 *  limparTela
 *  ------------------------------------------------------------
 *  Chama o comando do sistema operacional para limpar o
 *  terminal. No Windows e "cls", no Linux/Mac e "clear".
 * ============================================================ */
static void limparTela(void) {
#ifdef _WIN32                /* Se compilado no Windows... */
    system("cls");
#else                        /* Se for Linux ou Mac...     */
    system("clear");
#endif
}

/* ============================================================
 *  pausar
 *  ------------------------------------------------------------
 *  Mostra uma mensagem e espera o usuario apertar ENTER para
 *  continuar. Usado depois de cada operacao, para o usuario
 *  conseguir ler o resultado antes de voltar ao menu.
 * ============================================================ */
static void pausar(void) {
    printf("\n%s>> Pressione ENTER para voltar ao totem...", INDENT);
    int c;
    /* Consome todos os caracteres ate achar um '\n' (ENTER).  */
    /* Isso tambem limpa qualquer lixo que possa ter sobrado   */
    /* no buffer de input.                                     */
    while ((c = getchar()) != '\n' && c != EOF) { }
}

/* ============================================================
 *  lerLinha
 *  ------------------------------------------------------------
 *  Le uma linha de texto do usuario (com espacos!) e tira o
 *  '\n' do final. scanf("%s") nao serve porque ele para no
 *  primeiro espaco.
 * ============================================================ */
static void lerLinha(char *destino, int tamanho) {
    /* fgets le ate "tamanho-1" caracteres OU ate um '\n'.    */
    if (fgets(destino, tamanho, stdin) != NULL) {
        size_t len = strlen(destino);
        /* fgets deixa o '\n' no final da string. Removemos.   */
        if (len > 0 && destino[len - 1] == '\n') {
            destino[len - 1] = '\0';
        }
    } else {
        destino[0] = '\0'; /* Erro: deixa string vazia         */
    }
}

/* ============================================================
 *  prefixoPrioridade
 *  ------------------------------------------------------------
 *  Devolve a letra que identifica a prioridade no totem.
 *  Ex.: E0003, P0007, C0012.
 * ============================================================ */
static char prefixoPrioridade(int p) {
    switch (p) {
        case PRIORIDADE_EMERGENCIA:   return 'E';
        case PRIORIDADE_PREFERENCIAL: return 'P';
        case PRIORIDADE_COMUM:        return 'C';
        default:                      return '?';
    }
}

/* ============================================================
 *  nomePrioridade
 *  ------------------------------------------------------------
 *  Converte o numero da prioridade no nome por extenso.
 * ============================================================ */
static const char* nomePrioridade(int prioridade) {
    switch (prioridade) {
        case PRIORIDADE_EMERGENCIA:   return "EMERGENCIA";
        case PRIORIDADE_PREFERENCIAL: return "PREFERENCIAL";
        case PRIORIDADE_COMUM:        return "COMUM";
        default:                      return "DESCONHECIDA";
    }
}

/* ============================================================
 *  bordaCompleta
 *  ------------------------------------------------------------
 *  Desenha uma linha horizontal de borda na largura total.
 *  Exemplo: +===========================+
 * ============================================================ */
static void bordaCompleta(char ch) {
    int total = MENU_W + FILA_W + 5; /* 5 chars de separadores */
    printf("%s+", INDENT);            /* "  +"                  */
    for (int i = 0; i < total; i++) {
        putchar(ch);                  /* Repete o caractere     */
    }
    printf("+\n");                    /* Fecha com "+\n"        */
}

/* ============================================================
 *  bordaDividida
 *  ------------------------------------------------------------
 *  Desenha borda com um "+" no meio, separando as colunas
 *  do menu e do painel. Exemplo: +-----+-----+
 * ============================================================ */
static void bordaDividida(char ch) {
    printf("%s+", INDENT);
    for (int i = 0; i < MENU_W + 2; i++) putchar(ch); /* Esq.   */
    putchar('+');                                     /* Meio   */
    for (int i = 0; i < FILA_W + 2; i++) putchar(ch); /* Dir.   */
    printf("+\n");
}

/* ============================================================
 *  linhaCentralizada
 *  ------------------------------------------------------------
 *  Imprime um texto centralizado dentro da borda. Usado para
 *  o cabecalho "MediCore Centro Clinico".
 * ============================================================ */
static void linhaCentralizada(const char *texto) {
    int total = MENU_W + FILA_W + 5;
    int len = (int)strlen(texto);
    if (len > total) len = total;        /* Trunca se sobrar    */
    int leftPad  = (total - len) / 2;    /* Espacos a esquerda  */
    int rightPad = total - len - leftPad;/* Espacos a direita   */
    /* %*s com argumento numerico = imprime N espacos.          */
    printf("%s|%*s%.*s%*s|\n",
           INDENT, leftPad, "", len, texto, rightPad, "");
}

/* ============================================================
 *  linhaDupla
 *  ------------------------------------------------------------
 *  Imprime uma linha com DUAS colunas separadas por "|".
 *  Cada coluna e preenchida com espacos para manter o
 *  alinhamento.
 *
 *  Formato %-*.*s:
 *    -    = alinha a esquerda
 *    *    = largura vem como argumento
 *    .*   = limite maximo de caracteres (truncar se for maior)
 * ============================================================ */
static void linhaDupla(const char *esq, const char *dir) {
    printf("%s| %-*.*s | %-*.*s |\n",
           INDENT,
           MENU_W, MENU_W, esq,    /* Coluna da esquerda      */
           FILA_W, FILA_W, dir);   /* Coluna da direita        */
}

/* ============================================================
 *  cabecalho
 *  ------------------------------------------------------------
 *  Imprime o "banner" do MediCore. Usado em telas que NAO
 *  tem painel lateral (cadastro, chamadas, etc.).
 * ============================================================ */
static void cabecalho(void) {
    bordaCompleta('=');
    linhaCentralizada("");
    linhaCentralizada("[ + ]  M E D I C O R E  [ + ]");
    linhaCentralizada("Centro Clinico");
    linhaCentralizada("");
    bordaCompleta('=');
}

/* ============================================================
 *  montarLinhasFila
 *  ------------------------------------------------------------
 *  Percorre a fila e gera as strings que vao aparecer no
 *  painel lateral, INCLUINDO cabecalhos de grupo:
 *      -- EMERGENCIA --
 *       E0001  Joao
 *      -- PREFERENCIAL --
 *       P0002  Maria
 *  Devolve quantas linhas foram montadas.
 * ============================================================ */
static int montarLinhasFila(Fila *fila, LinhaFila *linhas, int max) {
    /* Caso 1: fila vazia => mostra uma linha unica. */
    if (filaVazia(fila)) {
        snprintf(linhas[0].texto, FILA_W + 1, "Nenhuma senha cadastrada");
        return 1;
    }

    int count = 0;       /* Quantas linhas ja montamos            */
    int prevPrior = -1;  /* Ultima prioridade vista (para detectar*/
                         /* mudanca e inserir cabecalho de grupo). */
    No *atual = fila->inicio;  /* Comeca pelo primeiro paciente   */

    /* "max - 1" pra reservar a ultima linha para uma possivel    */
    /* mensagem de "+N aguardando" se faltar espaco.              */
    while (atual != NULL && count < max - 1) {
        int p = atual->paciente.prioridade;

        /* Se mudou de grupo de prioridade, insere um cabecalho.  */
        if (p != prevPrior) {
            snprintf(linhas[count++].texto, FILA_W + 1,
                     "-- %s --", nomePrioridade(p));
            prevPrior = p;
            if (count >= max - 1) break;
        }

        /* Linha do paciente. O "%.*s" trunca o nome se for       */
        /* maior do que cabe (evita estourar a coluna).           */
        snprintf(linhas[count++].texto, FILA_W + 1,
                 " %c%04d  %.*s",
                 prefixoPrioridade(p),
                 atual->paciente.senha,
                 FILA_W - 8,
                 atual->paciente.nome);

        atual = atual->proximo;  /* Vai pro proximo no            */
    }

    /* Se sobraram pacientes que nao couberam, mostra "+N..."     */
    if (atual != NULL) {
        int remaining = 0;
        for (No *n = atual; n != NULL; n = n->proximo) remaining++;
        snprintf(linhas[count++].texto, FILA_W + 1,
                 "... +%d aguardando", remaining);
    }

    return count;
}

/* ============================================================
 *  menuPrincipal
 *  ------------------------------------------------------------
 *  Desenha a tela inicial do totem, com o menu de opcoes na
 *  esquerda e o painel de senhas na direita.
 * ============================================================ */
static void menuPrincipal(Fila *fila) {
    /* Texto fixo do menu (lado esquerdo). Cada string e uma   */
    /* linha. "" representa linha em branco.                   */
    const char *menuLinhas[] = {
        "[1] Retirar senha de atendimento",
        "[2] Chamar proximo paciente",
        "[3] Visualizar paciente da frente",
        "[4] Visualizar fila completa",
        "[5] Total de pacientes aguardando",
        "[6] Verificar se a fila esta vazia",
        "",
        "[0] Encerrar sessao do totem",
    };
    /* sizeof(array) / sizeof(array[0]) = numero de elementos. */
    int menuCount = (int)(sizeof(menuLinhas) / sizeof(menuLinhas[0]));

    /* Monta as linhas do painel lateral.                       */
    LinhaFila linhasFila[MAX_LINHAS_FILA];
    int filaCount = montarLinhasFila(fila, linhasFila, MAX_LINHAS_FILA);

    /* Desenha o cabecalho (banner com fundo "="):              */
    bordaCompleta('=');
    linhaCentralizada("");
    linhaCentralizada("[ + ]  M E D I C O R E  [ + ]");
    linhaCentralizada("Centro Clinico");
    linhaCentralizada("");

    /* Borda dividida para a area de duas colunas:              */
    bordaDividida('=');
    linhaDupla("MENU DE ATENDIMENTO", "SENHAS NA FILA");
    bordaDividida('-');

    /* Numero de linhas a imprimir = o maior dos dois.          */
    int rows = (menuCount > filaCount) ? menuCount : filaCount;

    /* Imprime linha por linha; se uma coluna acabou, usa "".   */
    for (int i = 0; i < rows; i++) {
        const char *esq = (i < menuCount) ? menuLinhas[i] : "";
        const char *dir = (i < filaCount) ? linhasFila[i].texto : "";
        linhaDupla(esq, dir);
    }

    bordaDividida('-');
    printf("\n%s>> Sua escolha: ", INDENT);
}

/* ============================================================
 *  menuPrioridade
 *  ------------------------------------------------------------
 *  Tela secundaria mostrada quando o usuario quer retirar uma
 *  senha. Pergunta qual o nivel de prioridade.
 * ============================================================ */
static void menuPrioridade(void) {
    cabecalho();
    printf("%s\n", INDENT);
    printf("%s        CLASSIFICACAO DE PRIORIDADE\n", INDENT);
    printf("%s\n", INDENT);
    printf("%s   [ 1 ]  EMERGENCIA     (atendimento imediato)\n", INDENT);
    printf("%s   [ 2 ]  PREFERENCIAL   (idoso, gestante, PCD, crianca)\n", INDENT);
    printf("%s   [ 3 ]  COMUM          (consultas em geral)\n", INDENT);
    printf("%s\n", INDENT);
    printf("%s   [ 0 ]  Cancelar e voltar\n", INDENT);
    printf("\n%s>> Selecione a prioridade: ", INDENT);
}

/* ============================================================
 *  retirarSenha
 *  ------------------------------------------------------------
 *  Fluxo da opcao [1]: pergunta prioridade, pede nome, gera
 *  numero de senha e insere o paciente na fila.
 * ============================================================ */
static void retirarSenha(Fila *fila, int *contadorSenha) {
    int opcao = -1;
    Paciente novo;   /* Novo paciente a ser criado     */

    limparTela();
    menuPrioridade();

    /* scanf devolve quantos valores conseguiu ler.    */
    /* Se nao for 1, deu erro (usuario digitou letra). */
    if (scanf("%d", &opcao) != 1) {
        while (getchar() != '\n') { }  /* Limpa buffer */
        printf("\n%s[!] Entrada invalida.\n", INDENT);
        pausar();
        return;
    }
    /* Mesmo quando scanf funciona, o '\n' do ENTER    */
    /* fica no buffer. Precisamos consumi-lo antes     */
    /* de chamar fgets depois.                          */
    while (getchar() != '\n') { }

    /* Converte a opcao do usuario na constante de prioridade. */
    switch (opcao) {
        case 1: novo.prioridade = PRIORIDADE_EMERGENCIA;   break;
        case 2: novo.prioridade = PRIORIDADE_PREFERENCIAL; break;
        case 3: novo.prioridade = PRIORIDADE_COMUM;        break;
        case 0:
            /* Usuario cancelou: sai sem cadastrar.         */
            printf("\n%sOperacao cancelada.\n", INDENT);
            pausar();
            return;
        default:
            printf("\n%s[!] Prioridade invalida.\n", INDENT);
            pausar();
            return;
    }

    /* Pede o nome do paciente.                              */
    limparTela();
    cabecalho();
    printf("\n%sInforme o nome do paciente: ", INDENT);
    lerLinha(novo.nome, sizeof(novo.nome));

    /* Se o usuario nao digitou nada, usa um placeholder.    */
    if (novo.nome[0] == '\0') {
        strcpy(novo.nome, "Paciente sem identificacao");
    }

    /* Gera o numero da senha. *contadorSenha eh o valor da  */
    /* variavel apontada por contadorSenha (no main).         */
    novo.senha = ++(*contadorSenha);

    /* Insere o paciente na fila (com prioridade).            */
    enfileirar(fila, novo);

    /* Mostra um "comprovante" da senha gerada.               */
    printf("\n%s+--------------------------------------------------+\n", INDENT);
    printf("%s|           SENHA GERADA COM SUCESSO               |\n", INDENT);
    printf("%s+--------------------------------------------------+\n", INDENT);
    printf("%s   Senha       : %c%04d\n", INDENT,
           prefixoPrioridade(novo.prioridade), novo.senha);
    printf("%s   Paciente    : %s\n", INDENT, novo.nome);
    printf("%s   Prioridade  : %s\n", INDENT, nomePrioridade(novo.prioridade));
    printf("%s+--------------------------------------------------+\n", INDENT);
    pausar();
}

/* ============================================================
 *  chamarProximo
 *  ------------------------------------------------------------
 *  Opcao [2]: remove o paciente da frente da fila e exibe os
 *  dados na tela ("chamar pro consultorio").
 * ============================================================ */
static void chamarProximo(Fila *fila) {
    Paciente atendido;
    limparTela();
    cabecalho();

    /* desenfileirar retorna 1 se conseguiu remover.           */
    if (desenfileirar(fila, &atendido)) {
        /* Tem paciente: mostra os dados dele.                  */
        printf("\n%s+--------------------------------------------------+\n", INDENT);
        printf("%s|           CHAMANDO PROXIMO PACIENTE              |\n", INDENT);
        printf("%s+--------------------------------------------------+\n", INDENT);
        printf("%s   Senha       : %c%04d\n", INDENT,
               prefixoPrioridade(atendido.prioridade), atendido.senha);
        printf("%s   Paciente    : %s\n", INDENT, atendido.nome);
        printf("%s   Prioridade  : %s\n", INDENT, nomePrioridade(atendido.prioridade));
        printf("%s+--------------------------------------------------+\n", INDENT);
        printf("\n%s>> Dirija-se ao consultorio indicado.\n", INDENT);
    } else {
        /* Fila estava vazia.                                   */
        printf("\n%s[i] Nao ha pacientes aguardando no momento.\n", INDENT);
    }
    pausar();
}

/* ============================================================
 *  verFrente
 *  ------------------------------------------------------------
 *  Opcao [3]: espia (sem remover) o paciente da frente.
 * ============================================================ */
static void verFrente(Fila *fila) {
    Paciente atual;
    limparTela();
    cabecalho();

    if (frenteFila(fila, &atual)) {
        printf("\n%s+--------------------------------------------------+\n", INDENT);
        printf("%s|             PROXIMO A SER CHAMADO                |\n", INDENT);
        printf("%s+--------------------------------------------------+\n", INDENT);
        printf("%s   Senha       : %c%04d\n", INDENT,
               prefixoPrioridade(atual.prioridade), atual.senha);
        printf("%s   Paciente    : %s\n", INDENT, atual.nome);
        printf("%s   Prioridade  : %s\n", INDENT, nomePrioridade(atual.prioridade));
        printf("%s+--------------------------------------------------+\n", INDENT);
    } else {
        printf("\n%s[i] A fila esta vazia.\n", INDENT);
    }
    pausar();
}

/* ============================================================
 *  verFilaCompleta
 *  ------------------------------------------------------------
 *  Opcao [4]: imprime todos os pacientes em ordem.
 * ============================================================ */
static void verFilaCompleta(Fila *fila) {
    limparTela();
    cabecalho();
    printf("\n%s------------- FILA DE ATENDIMENTO -------------\n\n", INDENT);
    if (filaVazia(fila)) {
        printf("%s[i] Nenhum paciente na fila.\n", INDENT);
    } else {
        listarFila(fila);  /* Funcao implementada em lista.c   */
    }
    pausar();
}

/* ============================================================
 *  verTamanho
 *  ------------------------------------------------------------
 *  Opcao [5]: mostra quantos pacientes ha na fila.
 * ============================================================ */
static void verTamanho(Fila *fila) {
    limparTela();
    cabecalho();
    printf("\n%sPacientes aguardando atendimento: %d\n",
           INDENT, tamanhoFila(fila));
    pausar();
}

/* ============================================================
 *  verificarVazia
 *  ------------------------------------------------------------
 *  Opcao [6]: avisa se a fila esta vazia ou nao.
 * ============================================================ */
static void verificarVazia(Fila *fila) {
    limparTela();
    cabecalho();
    if (filaVazia(fila)) {
        printf("\n%s[i] A fila esta VAZIA. Nenhum paciente aguardando.\n", INDENT);
    } else {
        printf("\n%s[i] A fila NAO esta vazia. Ha %d paciente(s) aguardando.\n",
               INDENT, tamanhoFila(fila));
    }
    pausar();
}

/* ============================================================
 *  main
 *  ------------------------------------------------------------
 *  Ponto de entrada do programa. Inicializa a fila, fica num
 *  laco mostrando o menu e despachando para a funcao certa.
 * ============================================================ */
int main(void) {
    Fila fila;             /* A fila do totem               */
    int contadorSenha = 0; /* Contador para gerar senhas    */
    int opcao = -1;        /* Opcao escolhida no menu        */

    inicializarFila(&fila); /* Sempre primeiro! Zera a fila. */

    /* Laco principal: mostra menu, le opcao, executa.       */
    do {
        limparTela();
        menuPrincipal(&fila);

        /* Tenta ler o numero. Se o usuario digitar letra,    */
        /* scanf retorna 0 (nao conseguiu ler 1 inteiro).     */
        if (scanf("%d", &opcao) != 1) {
            while (getchar() != '\n') { } /* Limpa lixo       */
            printf("\n%s[!] Opcao invalida. Digite um numero do menu.\n", INDENT);
            pausar();
            continue;   /* Volta pro inicio do do-while        */
        }
        /* Consome o '\n' deixado pelo scanf.                  */
        while (getchar() != '\n') { }

        /* Despacha pra funcao certa de acordo com a opcao.    */
        switch (opcao) {
            case 1: retirarSenha(&fila, &contadorSenha); break;
            case 2: chamarProximo(&fila);                break;
            case 3: verFrente(&fila);                    break;
            case 4: verFilaCompleta(&fila);              break;
            case 5: verTamanho(&fila);                   break;
            case 6: verificarVazia(&fila);               break;
            case 0:
                /* Tela de despedida.                          */
                limparTela();
                cabecalho();
                printf("\n%sObrigado por utilizar o totem do MediCore Centro Clinico.\n", INDENT);
                printf("%sTenha um excelente atendimento!\n\n", INDENT);
                break;
            default:
                printf("\n%s[!] Opcao inexistente. Tente novamente.\n", INDENT);
                pausar();
                break;
        }
    } while (opcao != 0);   /* Repete ate o usuario escolher 0 */

    /* IMPORTANTE: libera toda a memoria antes de sair.        */
    /* Sem isso, os nos da fila ficam "abandonados" na memoria.*/
    liberarFila(&fila);

    return 0;   /* 0 = programa terminou sem erro              */
}
