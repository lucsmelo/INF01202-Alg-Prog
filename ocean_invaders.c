/*

Trabalho Final INF1202

Eduardo Fantini e Lucas Lima de Melo

*/

#include <stdio.h>
#include <conio2.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/timeb.h>

// CORES
#define AZUL 1
#define BRANCO 15
#define PRETO 0

// ASCII
#define asc0 48
#define asc1 49
#define asc2 50
#define asc3 51
#define asc4 52
#define LEFT 75
#define RIGHT 77
#define UP 72
#define DOWN 80
#define ESC 27
#define SPACE 32

// MOLDURA
#define L_INIT 2
#define L_FIM 24
#define COL_INIT 1
#define COL_FIM 80

// ESTRUTURAS
#define SUB_Xi 35
#define SUB_Yi 2
#define SUB_COMP 11
#define SUB_ALT 2
#define MERG_COMP 4
#define MERG_ALT 1
#define TORP_COMP 3
#define TORP_ALT 1
#define OXIxi   18
#define OXIyi   25
#define LIM_OXI   29
#define QTDOXIi 10

// TAMANHOS
#define TAM_NICK 9 //8 + \0

// VALORES PADRAO
#define VIDASi 3
#define OXIi 30
#define PONTOSi 0
#define RESCUEi 0
#define RESCUEmax 5
#define PONTOSsec 1
#define PONTOSrescue 20
#define PONTOSkill 10
#define MERG_MAX 7
#define SUB_MAX 5

//ARQUIVOS
#define RECORDS "recordes.txt"
#define RECORDS_temp "recordes_temp.txt"
#define RANK_MAX 10
//VELOCIDADES
#define VELOCIDADE1 100
#define VELOCIDADE2 79
#define VELOCIDADE3 1000
#define VELOCIDADECONTA 3000
#define VELOCIDADEMORTE 500


typedef struct {

        int x;
        int y;

} COORDENADA;

typedef struct {

        int status; //direcao do sub
        int cor;
        int comp; //comprimento do sub
        int alt; //altura do sub
        int visivel;//se esta visivel ou nao
        COORDENADA coord;//estrutura das coord
        int mov; // variavel para controlar a velocidade do sub

} SUBMARINO;

typedef struct {
        COORDENADA coord;
        int mov; //variavel para controlar a velocidade do oxi
        int oxi; //quantidade de oxigenio,come�a em 10 porque perde 1 a cada 3 segundos
}OXIGENIO;

typedef struct {

        int vidas;
        int pontos;
        char nome[15 + 1];
        int rescue; //mergulhadores salvos
        SUBMARINO sub;
        OXIGENIO oxi;
        int mov; //  variavel para controlar a velocidade de ganhar pontos do player

} PLAYER;

typedef struct {

        int status; //direcao do mergulhador
        COORDENADA coord;
        int cor;
        int mov; //  variavel para controlar a velocidade do mergulhador

} MERGULHADOR;

typedef struct {
        COORDENADA coord;
        int status;//direcao do torpedo
        int mov; // variavel para controlar a velocidade do torpedo
}TORPEDO;

typedef struct {  //estrutura para controlar o save do jogo

        PLAYER player;
        MERGULHADOR mergs[MERG_MAX];
        SUBMARINO subs[SUB_MAX];
        TORPEDO torp;

} GAMEDATA;

int main();
void hud(PLAYER *p);
int menu(int *opcao);
void novoJogo();
int creditos();
void exitGame();
void refreshPontos(int pontos);
void refreshVidas(int vidas);
void refreshOxi(int oxi);
void writeMerg(MERGULHADOR merg[]);
void refreshMerg(int merg);
void perdeVida(PLAYER *p, MERGULHADOR *mergs,SUBMARINO *subs);
void aumentaPont(PLAYER *p, int add);


void inicializatorp(TORPEDO *torp,PLAYER *p){

        if((*p).sub.status==1) {

                (*torp).status=1;
                (*torp).coord.x=(*p).sub.coord.x +11;
                (*torp).coord.y=(*p).sub.coord.y +1;



        }
        if((*p).sub.status==-1) {
                (*torp).coord.x=(*p).sub.coord.x-3;
                (*torp).coord.y=(*p).sub.coord.y+1;
                (*torp).status=-1;
        }

}
void write_torp(TORPEDO *torp){
        if((*torp).status==1)
                cputsxy((*torp).coord.x,(*torp).coord.y,"-->");
        if((*torp).status==-1)
                cputsxy((*torp).coord.x,(*torp).coord.y,"<--");
}
void apagatorp(TORPEDO *torp){
        cputsxy((*torp).coord.x, (*torp).coord.y, "   ");


}
void movetorp(TORPEDO *torp,int diff){
        if((diff/VELOCIDADE2)%2!=(*torp).mov)
        {
                (*torp).mov=(diff/VELOCIDADE2)%2;
                if((*torp).status!=0)
                {
                        apagatorp(torp);
                        (*torp).coord.x +=  (*torp).status;

                }
                if((*torp).coord.x < 3 || (*torp).coord.x > 77)
                        (*torp).status=0;
                apagatorp(torp);

        }
        write_torp(torp);
}
int detectaColisaoTorp(TORPEDO *torp, SUBMARINO *subs) {//colisao entre sub  e torp


        int Px = (*subs).coord.x,
            Py = (*subs).coord.y,
            Ix = (*torp).coord.x,
            Iy = (*torp).coord.y;

        if(Px == Ix) { //se coord x for igual

                if(Py == Iy) {//se coord y for igual
                        return 1;//verdadeiro
                }else if((Py > Iy) && (Py - Iy <= SUB_ALT)) { // se a coord y do sub > coord y do torp e se coord y do sub - coord y do inim for menor que altura do sub
                        return 1;//verdadeiro
                }else if((Py < Iy) && (Iy - Py <= TORP_ALT + 1)) {//se a coord y do sub > coord y do torp se coord y do sub - coord y do inim for menor que altura do torp+1
                        return 1;//verdadeiro
                }else{
                        return 0;//falso
                }

        }else if(Px > Ix) {//se x do sub for  maior que x do torp

                if(Px - Ix <= TORP_COMP) {//se x do sub - x do torp for menor que o comprimento do torp

                        if((Py > Iy) && (Py - Iy <= SUB_ALT)) {//se y do sub > y do torp e se y do sub - y do torp for menor que  altura do sub
                                return 1;//verdadeiro
                        }else if((Py < Iy) && (Iy - Py <= TORP_ALT + 1)) {//se a coord y do sub < coord y do torp e se coord y do torp - coord y do sub for menor que altura do torp+1
                                return 1;//verdadeiro
                        }else {
                                return 0;//senao falso
                        }

                }else{
                        return 0;//senao falso
                }

        }else if(Px < Ix) {//se x do sub for  menor que x do torp

                if(Ix - Px <= SUB_COMP) {//se x do torp - x do sub for menor que o comprimento do sub

                        if((Py > Iy) && (Py - Iy <= SUB_ALT)) {//se y do sub > y do torp e se y do sub - y do torp for menor que  altura do sub
                                return 1;//verdadeiro
                        }else if((Py < Iy) && (Iy - Py <= TORP_ALT + 1)) {//se a coord y do sub < coord y do merg e se coord y do torp - coord y do sub for menor que altura do torp +1
                                return 1;//verdadeiro
                        }else {
                                return 0;//falso
                        }

                }else{
                        return 0;//falso
                }

        }

}

void ganhaPontoSec(PLAYER *p, int diff) { //funcao que a cada segundo atualiza a pontuacao do jogador em 1

        if((diff/VELOCIDADE3) %2 != p->mov) {
                p->mov = (diff/VELOCIDADE3) % 2;
                aumentaPont(p, 1);
        }

}

void perdeOxi(int *oxi) {

        *oxi -= 1;
        refreshOxi(*oxi);

}
void contaOxi(PLAYER *p,int diff){ //funcao que a cada 3 segundos atualiza o oxigenio do jogador e enche a barra de oxigenio

        if(p->sub.coord.y > L_INIT) {//se y do jogador for maior que a linha da agua executa a funcao

                if((diff/VELOCIDADECONTA)%2!=(*p).oxi.mov)
                {
                        putchxy((*p).oxi.coord.x,(*p).oxi.coord.y,'|');
                        (*p).oxi.mov=(diff/VELOCIDADECONTA)%2;
                        (*p).oxi.coord.x++;
                        (*p).oxi.oxi--;
                }
        }
}

void perdeVidaOxi(PLAYER *p, MERGULHADOR *mergs,SUBMARINO *subs)//se o oxigenio for menor que 0 perde uma vida e volta no come�o se tiver vidas restantes
{
        if((*p).oxi.oxi<0)
        {
                perdeVida(p, mergs,subs);
        }
}

void apagaSub(int x, int y) { //apaga o submarino

        cputsxy(x, y, "           ");
        cputsxy(x, y + 1, "           ");

}

void writeSub_R(int x, int y) {  //desenha submarino para direita

        textcolor(WHITE);

        cputsxy(x, y, "  ___|o|__ ");
        cputsxy(x, y + 1, "=(________)");
        // SUB 11x2

}

void writeSub_L(int x, int y) { //desenha submarino para esquerda

        textcolor(WHITE);

        cputsxy(x, y, " ___|o|__  ");
        cputsxy(x, y + 1, "(________)=");
        // SUB 11x2

}

void writeSubI_R(int x, int y) {   //desenha submarino inimigo para direita

        textcolor(5);

        cputsxy(x, y, "  ___|o|__ ");
        cputsxy(x, y + 1, "=(________)");

        textcolor(WHITE);
        // SUB 11x2

}

void writeSubI_L(int x, int y) { //desenha submarino inimigo para esquerda

        textcolor(5);

        cputsxy(x, y, " ___|o|__  ");
        cputsxy(x, y + 1, "(________)=");

        textcolor(WHITE);
        // SUB 11x2

}

int verificaSubY(SUBMARINO subs[],int y) {  //se a altura do sub for igual ao de um numero retorna verdadeiro

        int i;

        for(i = 0; i < SUB_MAX; i++) {
                if(subs[i].coord.y == y || subs[i].coord.y + 1 == y || subs[i].coord.y - 1 == y) {
                        return 1;
                }
        }

        return 0;

}

int verificaMergY(MERGULHADOR mergs[],int y) { //se a altura do merg for igual ao de um numero retorna verdadeiro

        int i;

        for(i = 0; i < MERG_MAX; i++) {
                if(mergs[i].coord.y == y) {
                        return 1;
                }
        }

        return 0;

}

void inicializaSubs(SUBMARINO sub[]) {  //inicializa  os submarinos inimigos para esquerda ou para direita com uma % de aproximadamente 50

        srand(time(0));
        int i, j;


        for(i = 0; i < SUB_MAX; i++) {

                do {
                        j = rand()%(L_FIM -L_INIT - SUB_ALT) + L_INIT + 2;
                } while(verificaSubY(sub, j));


                sub[i].coord.y = j;
                sub[i].status = (int) (rand() / (double) RAND_MAX * (2 + 1)) - 1;



                if(sub[i].status == 0) {

                        if(rand() / (double) RAND_MAX < 0.2) {

                                if(rand() / (double) RAND_MAX <= 0.5) {
                                        sub[i].status = 1;
                                }else{
                                        sub[i].status = -1;
                                }

                        }

                }

                switch(sub[i].status) {

                case -1:
                        sub[i].coord.x = 69;
                        break;
                case 0:
                        sub[i].coord.x = 0;
                        break;
                case 1:
                        sub[i].coord.x = 5;
                        break;
                default:
                        break;

                }

        }
}

void moveSubs(SUBMARINO sub[],int diff) {  //move o submarino com uma velocidade de 100 ms
        int i;
        for(i = 0; i < SUB_MAX; i++) {
                if((diff/VELOCIDADE1)%2!=sub[i].mov)
                {
                        sub[i].mov=(diff/VELOCIDADE1)%2;
                        if(sub[i].status != 0) {

                                apagaSub(sub[i].coord.x, sub[i].coord.y);
                                sub[i].coord.x += sub[i].status; //aumenta x em 1 ou -1;

                                if(sub[i].coord.x < 2 || sub[i].coord.x > 69) { //se a coordenada passar dos limites muda a direcao

                                        sub[i].status = sub[i].status * -1;



                                }
                        }
                }
                if(sub[i].status == 1) { // se o status for 1 desenha pra direita
                        writeSubI_R(sub[i].coord.x, sub[i].coord.y);
                }else if(sub[i].status == -1) {// se o status for -1 desenha pra esquerda
                        writeSubI_L(sub[i].coord.x, sub[i].coord.y);
                }
        }
}

void apagaMerg(MERGULHADOR *merg) {  //apaga mergulhador

        cputsxy((*merg).coord.x, (*merg).coord.y, "    ");

}

void inicializaMergs(MERGULHADOR merg[]) { //inicializa  os mergulhadores para esquerda ou para direita com uma % de aproximadamente 50

        srand(time(0));
        int i, j;


        for(i = 0; i < MERG_MAX; i++) {

                do {
                        j = rand()%(L_FIM -L_INIT)+L_INIT +1;
                } while(verificaMergY(merg, j));


                merg[i].coord.y = j;
                merg[i].status = (int) (rand() / (double) RAND_MAX * (2 + 1)) - 1;



                if(merg[i].status == 0) {

                        if(rand() / (double) RAND_MAX < 0.2) {

                                if(rand() / (double) RAND_MAX <= 0.5) {
                                        merg[i].status = 1;
                                }else{
                                        merg[i].status = -1;
                                }

                        }

                }

                switch(merg[i].status) {

                case -1:
                        merg[i].coord.x = 78;
                        break;
                case 0:
                        merg[i].coord.x = 0;
                        break;
                case 1:
                        merg[i].coord.x = 2;
                        break;
                default:
                        break;

                }

        }
}

void writeMerg(MERGULHADOR merg[]){//desenha os mergulhadores

        int i;

        for(i = 0; i < MERG_MAX; i++) {

                if(merg[i].status == 1) {
                        cputsxy(merg[i].coord.x,merg[i].coord.y, ">->O");//se o status for 1 desenha para direita


                }

                else if((merg[i].status == -1)) {
                        cputsxy(merg[i].coord.x, merg[i].coord.y, "O<-<");//se o status for -1 desenha para esquerda



                }

        }
}

void moveMergs(MERGULHADOR merg[],int diff) { //move os mergulhadores com uma velocidade de 79 ms
        int i;

        for(i = 0; i < MERG_MAX; i++) {
                if((diff/VELOCIDADE2)%2!=merg[i].mov)
                {
                        merg[i].mov=(diff/VELOCIDADE2)%2;
                        if(merg[i].status != 0) {

                                apagaMerg(&merg[i]);
                                merg[i].coord.x += merg[i].status;//aumenta ou decresce de acordo com status

                                if(merg[i].coord.x < 3 || merg[i].coord.x > 77) {//se passar dos limites muda a direcao

                                        merg[i].status = merg[i].status * -1;



                                }
                        }
                }

        }
        writeMerg(merg);
}


void exitGame() {  //tela final do jogo

        clrscr();
        textcolor(BRANCO);
        printf("\n\n\n\n");
        printf("\t     ___  __ ___ __ _ _ _   (_)_ ___ ____ _ __| |___ _ _ ___\n");
        printf("\t    / _ \\/ _/ -_) _` | '  \\ | | ' \\ V / _` / _` / -_) '_(_-<\n");
        printf("\t    \\___/\\__\\___\\__,_|_||_| |_|_||_\\_/\\__,_\\__,_\\___|_| /__/\n");
        printf("\n");
        printf("\t    []=====================================================[]\n\n\n");
        printf("\t\t        ___   _   ___ _  _ ___   ___  \n");
        printf("\t\t       / __| /_\\ |_ _| \\| |   \\ / _ \\ \n");
        printf("\t\t       \\__ \\/ _ \\ | || .` | |) | (_) | _ _ \n");
        printf("\t\t       |___/_/ \\_\\___|_|\\_|___/ \\___(_|_|_)\n");
        printf("\t\t                                     \n\n\n");
        printf("\t\t\t    Obrigado por jogar!\n\n");

        exit(0);
        return;
}

void aumentaPont(PLAYER *p, int add) { //aumenta a pontuacao de acordo com a entrada

        (*p).pontos += add;
        refreshPontos((*p).pontos);

}

void perdeVida(PLAYER *p, MERGULHADOR *merg,SUBMARINO *subs) {  //perde vida  e inicializa o jogo com uma vida a menos

        int i;

        p->vidas--;
        refreshVidas(p->vidas);
        p->oxi.oxi = QTDOXIi;
        p->oxi.coord.x = OXIxi;
        p->rescue = 0;
        apagaSub(p->sub.coord.x, p->sub.coord.y);
        p->sub.coord.x = SUB_Xi;
        p->sub.coord.y = SUB_Yi;
        writeSub_R(p->sub.coord.x, p->sub.coord.y);//reinicia o player para coordenada inicial
        refreshOxi(p->oxi.oxi);//volta o oxi para 10
        refreshMerg(p->rescue);//reinicia o resgate
        for(i = 0; i < MERG_MAX; i++) {
                if(merg[i].status != 0) {
                        apagaMerg(&merg[i]);
                }
        }
        for(i = 0; i < SUB_MAX; i++)
        {
                if(subs[i].status != 0) {
                        apagaSub(subs[i].coord.x, subs[i].coord.y);

                }
        }
        inicializaMergs(merg);//inicializa os mergulhadores novamente
        inicializaSubs(subs);

}

void entregaMerg(PLAYER *p, MERGULHADOR *merg) { //se o player entregar o mergulhador para superficie ,ganha os pontos

        int i = 0, j = 0;

        aumentaPont(p, p->rescue * PONTOSrescue);
        refreshPontos(p->pontos);
        p->rescue = 0;
        refreshMerg(0);
        for(i = 0; i < MERG_MAX; i++) {
                if(merg[i].status == 0) {
                        j++;
                }
        }
        if(j == MERG_MAX) {
                inicializaMergs(merg);//se pegar todos mergulhadores,inicializa eles de novo
        }
}

void moveSub(SUBMARINO *sub, int *on, PLAYER *p, MERGULHADOR *merg,TORPEDO *torp) {//move o submarino de acordo com a tecla apertada pelo jogador

        int i;
        char tecla;

        if(kbhit()) {

                tecla = getch();

                if(tecla == ESC) {
                        *on = 0;

                }else if(tecla == -32) {

                        tecla = getch();
                        switch(tecla) {

                        case LEFT://mover para esquerda
                                if((*sub).coord.x > COL_INIT) {
                                        putchxy((*sub).coord.x + SUB_COMP - 1, (*sub).coord.y, ' ');
                                        putchxy((*sub).coord.x + SUB_COMP - 1, (*sub).coord.y + 1, ' ');
                                        if((*sub).coord.y == 2) {
                                                putchxy((*sub).coord.x + SUB_COMP, (*sub).coord.y, '~');
                                        }
                                        (*sub).coord.x -= 1;
                                        writeSub_L((*sub).coord.x, (*sub).coord.y);
                                        (*sub).status = -1;
                                }
                                break;
                        case RIGHT://move para direita
                                if((*sub).coord.x + SUB_COMP < COL_FIM) {
                                        putchxy((*sub).coord.x, (*sub).coord.y, ' ');
                                        putchxy((*sub).coord.x, (*sub).coord.y + 1, ' ');
                                        if((*sub).coord.y == 2) {
                                                putchxy((*sub).coord.x, (*sub).coord.y, '~');
                                        }
                                        (*sub).coord.x += 1;
                                        writeSub_R((*sub).coord.x, (*sub).coord.y);
                                        (*sub).status = 1;;
                                }
                                break;
                        case UP:// move para cima
                                if((*sub).coord.y > L_INIT) {
                                        for(i = (*sub).coord.x; i <= (*sub).coord.x + SUB_COMP + 1; i++) {
                                                putchxy(i, (*sub).coord.y + 1, ' ');
                                        }
                                        (*sub).coord.y -= 1;
                                        if(sub->coord.y == 2) {
                                                entregaMerg(p, merg);
                                                p->oxi.oxi = 10;
                                                p->oxi.coord.x = 18;
                                                refreshOxi(p->oxi.oxi);
                                        }
                                        if((*sub).status == -1) {
                                                writeSub_L((*sub).coord.x, (*sub).coord.y);
                                        }else if ((*sub).status == 1) {
                                                writeSub_R((*sub).coord.x, (*sub).coord.y);
                                        }
                                }
                                break;
                        case DOWN://move para baixo
                                if((*sub).coord.y < L_FIM - SUB_ALT + 1) {
                                        for(i = (*sub).coord.x; i <= (*sub).coord.x + SUB_COMP + 1; i++) {
                                                putchxy(i, (*sub).coord.y, ' ');
                                        }
                                        if((*sub).coord.y == 2) {
                                                for(i = 1; i < COL_FIM; i++) {
                                                        putchxy(i, 2, '~');
                                                }
                                        }
                                        (*sub).coord.y += 1;
                                        if((*sub).status == -1) {
                                                writeSub_L((*sub).coord.x, (*sub).coord.y);
                                        }else if((*sub).status == 1) {
                                                writeSub_R((*sub).coord.x, (*sub).coord.y);
                                        }
                                }
                                break;
                        default:
                                break;

                        }
                }else if(tecla == SPACE) {//se for espaco solta o torpedo
                    if(torp->status == 0) {
                        inicializatorp(torp,p);
                    }
                }
        }
}

void startGame(PLAYER *p) { //carrega hud e inicializa  o submarino na pos inicial

        clrscr();
        hud(p);
        writeSub_L((*p).sub.coord.x, (*p).sub.coord.y);

}

void refreshVidas(int vidas) {  //desenha na tela as vidas do jogador

        int i = 0;
        char vidas_text[15] = "Vidas";
        for(i = 0; i <= 9; i++) {
                putchxy(9 + i, 1, ' ');
        }
        for(i = vidas; i > 0; i--) {
                if(i <= VIDASi) {
                        strcat(vidas_text, " <3");
                }
        }
        cputsxy(3, 1, vidas_text);

}

void refreshMerg(int merg) {  //desenha na tela as informa�oes dos mergulhadores resgatados

        int i = 0;
        char merg_text[38] = "Mergulhadores ";
        for(i = merg; i > 0; i--) {
                if(i <= RESCUEmax) {
                        strcat(merg_text, "\\o/ ");
                }
        }
        cputsxy(40, 25, "                                        ");
        cputsxy(COL_FIM - strlen(merg_text) - 1, 25, merg_text);

}

void refreshOxi(int oxi) { //desenha na tela as informacoes do oxigenio

        char oxi_text[10] = "Oxig�nio";
        char oxi_tank[26] = " 0s [|||||||||||] 30s ";

        cputsxy(3, 25, oxi_text);
        textbackground(BRANCO);
        textcolor(PRETO);
        cputsxy(13, 25, oxi_tank);
        textbackground(PRETO);
        textcolor(BRANCO);

}

void refreshPontos(int pontos) { //desenha na tela as informacoes dos pontos do jogador

        char pontos_s[16];
        itoa(pontos, pontos_s, 10);
        char pontos_text[8] = " Pontos";
        strcat(pontos_s, pontos_text);
        cputsxy(COL_FIM - strlen(pontos_s) - 1, 1, pontos_s);

}

void hud(PLAYER *p) {  //faz a hud do jogo com o as informacoes das vidas,dos mergulhadores,do oxigenio,e dos pontos

        int i = 0;

        char c = '~';

        for(i = COL_INIT; i <= COL_FIM; i++) {

                putchxy(i, L_INIT, c);

        }

        refreshVidas((*p).vidas);
        refreshMerg((*p).rescue);
        refreshOxi(QTDOXIi);
        refreshPontos((*p).pontos);

}

int menu(int *opcao) { //menu inicial do jogo

        clrscr();
        textcolor(BRANCO);
        printf("\n\n\n\n");
        printf("\t     ___  __ ___ __ _ _ _   (_)_ ___ ____ _ __| |___ _ _ ___\n");
        printf("\t    / _ \\/ _/ -_) _` | '  \\ | | ' \\ V / _` / _` / -_) '_(_-<\n");
        printf("\t    \\___/\\__\\___\\__,_|_||_| |_|_||_\\_/\\__,_\\__,_\\___|_| /__/\n");
        printf("\n");
        printf("\t    []=====================================================[]\n\n\n\n\n\n");
        printf("\t\t[1]   Novo Jogo\n");
        printf("\t\t[2]   Carregar Jogo\n");
        printf("\t\t[3]   Recordes\n");
        printf("\t\t[4]   Creditos\n");
        printf("\t\t[ESC] Sair\n\n\n\n\n\n");

        *opcao = getch();
        return *opcao;//retorna  1,2,3 ou 4 ou esc
}

int creditos() { // escreve os creditos na tela

        int opcao = 0;

        clrscr();
        textcolor(BRANCO);
        printf("\n\n\n");
        printf("\t     ___  __ ___ __ _ _ _   (_)_ ___ ____ _ __| |___ _ _ ___\n");
        printf("\t    / _ \\/ _/ -_) _` | '  \\ | | ' \\ V / _` / _` / -_) '_(_-<\n");
        printf("\t    \\___/\\__\\___\\__,_|_||_| |_|_||_\\_/\\__,_\\__,_\\___|_| /__/\n");
        printf("\n\n");
        printf("\t    []=====================================================[]\n\n\n");
        printf("\t\t\t    JOGO DESENVOLVIDO POR\n\n");
        printf("\t\t\t>      Eduardo Fantini    <\n");
        printf("\t\t\t>    Lucas Lima de Melo   <\n\n");
        printf("\t\t\t   COMO TRABALHO FINAL DE\n");
        printf("\t\t\t  ALGORITMOS & PROGRAMACAO\n");
        printf("\t\t\t   DA UNIVERSIDADE FEDERAL\n");
        printf("\t\t\tDO RIO GRANDE DO SUL (UFRGS)\n\n\n");
        printf("\t\t\t   [1]   Voltar ao menu\n");
        printf("\t\t\t   [ESC] Sair");

        opcao = getch(); // usuario pode sair ou voltar ao menu inicial
        if(opcao == ESC) {
                exitGame();
        }else if(opcao == asc1) {
                main();
        }else{
                creditos();
        }

}

int detectaColisaoSUB(PLAYER *p, SUBMARINO *inim) { // colisao entre o submarino do player e o inimigo

        int Px = (*p).sub.coord.x,
            Py = (*p).sub.coord.y,
            Ix = (*inim).coord.x,
            Iy = (*inim).coord.y;
        if((*inim).status!=0) {
                if(Px == Ix) { //se x for igual

                        if(Py == Iy) {//se y for igual
                                return 1;//verdadeiro
                        }else if(abs(Py - Iy) <= SUB_ALT) { //se coord y do player - coord y do inim for menor que altura do sub
                                return 1;//verdadeiro
                        }else{
                                return 0;//caso n�o for nenhum falso
                        }

                }else if(Px > Ix) { //se coord x do player for > que a coord x do inim

                        if(abs(Px - Ix) <= SUB_COMP) { //se coord x do player - coord x do inim for menor que comprimento do sub

                                if(abs(Py - Iy) <= SUB_ALT) { //se coord y do player - coord y do inim for menor que altura do sub
                                        return 1; //verdadeiro
                                }else {
                                        return 0;// senao falso
                                }

                        }else{
                                return 0;//senao falso
                        }

                }else if(Px < Ix) {//se coord x do player for < que a coord x do inim

                        if(abs(Px - Ix) <= SUB_COMP) { //se coord x do player - coord x do inim for menor que comprimento do sub

                                if(abs(Py - Iy) <= SUB_ALT) {//se coord y do player - coord y do inim for menor que altura do sub
                                        return 1;//verdadeiro
                                }else {
                                        return 0;// senao falso
                                }

                        }else{
                                return 0;// senao falso
                        }

                }

        }
}

int detectaColisaoMERG(PLAYER *p, MERGULHADOR *merg) {//colisao entre sub do player e mergulhador

        int Px = (*p).sub.coord.x,
            Py = (*p).sub.coord.y,
            Ix = (*merg).coord.x,
            Iy = (*merg).coord.y;

        if(Px == Ix) { //se coord x for igual

                if(Py == Iy) {//se coord y for igual
                        return 1;//verdadeiro
                }else if((Py > Iy) && (Py - Iy <= SUB_ALT)) { // se a coord y do player > coord y do merg e se coord y do player - coord y do inim for menor que altura do sub
                        return 1;//verdadeiro
                }else if((Py < Iy) && (Iy - Py <= MERG_ALT + 1)) {//se a coord y do player > coord y do merg se coord y do player - coord y do inim for menor que altura do mergulhador +1
                        return 1;//verdadeiro
                }else{
                        return 0;//falso
                }

        }else if(Px > Ix) {//se x do player for  maior que x do mergulhador

                if(Px - Ix <= MERG_COMP) {//se x do player - x do mergulhador for menor que o comprimento do mergulhador

                        if((Py > Iy) && (Py - Iy <= SUB_ALT)) {//se y do player > y do mergulhador e se y do player - y do mergulhador for menor que  altura do sub
                                return 1;//verdadeiro
                        }else if((Py < Iy) && (Iy - Py <= MERG_ALT + 1)) {//se a coord y do player < coord y do merg e se coord y do mergulhador - coord y do player for menor que altura do mergulhador +1
                                return 1;//verdadeiro
                        }else {
                                return 0;//senao falso
                        }

                }else{
                        return 0;//senao falso
                }

        }else if(Px < Ix) {//se x do player for  menor que x do mergulhador

                if(Ix - Px <= SUB_COMP) {//se x do mergulhador - x do player for menor que o comprimento do sub

                        if((Py > Iy) && (Py - Iy <= SUB_ALT)) {//se y do player > y do mergulhador e se y do player - y do mergulhador for menor que  altura do sub
                                return 1;//verdadeiro
                        }else if((Py < Iy) && (Iy - Py <= MERG_ALT + 1)) {//se a coord y do player < coord y do merg e se coord y do mergulhador - coord y do player for menor que altura do mergulhador +1
                                return 1;//verdadeiro
                        }else {
                                return 0;//falso
                        }

                }else{
                        return 0;//falso
                }

        }

}

void resgataMerg(PLAYER *p, MERGULHADOR merg[]) {//resgata os mergulhadores

        int i;

        for(i = 0; i < MERG_MAX; i++) {

                if(merg[i].status != 0) {
                        if(detectaColisaoMERG(p, &merg[i])) {//se tiver colisao

                                if(p->rescue < RESCUEmax) {//se os mergulhadores resgatados forem menor que o limite

                                        merg[i].status = 0;
                                        apagaMerg(&merg[i]);//apaga o mergulhador
                                        (*p).rescue++; //aumenta em 1 os mergulhadores resgatados
                                        refreshMerg((*p).rescue);

                                        if((*p).sub.status == -1) {//desenha o sub denovo
                                                writeSub_L((*p).sub.coord.x, (*p).sub.coord.y);
                                        }else if ((*p).sub.status == 1) {
                                                writeSub_R((*p).sub.coord.x, (*p).sub.coord.y);
                                        }
                                        //putchxy(15, 24, '1');
                                }else{
                                        if((*p).sub.status == -1) {//desenha o sub denovo
                                                writeSub_L((*p).sub.coord.x, (*p).sub.coord.y);
                                        }else if ((*p).sub.status == 1) {
                                                writeSub_R((*p).sub.coord.x, (*p).sub.coord.y);
                                        }
                                }
                        }
                }

        }

}
void destroi_sub(PLAYER *p, TORPEDO *torp,SUBMARINO subs[])
{
        int i,cont=0;
        int algum = 0;

        for(i = 0; i < SUB_MAX; i++) {
                if(subs[i].status != 0)
                {
                    algum = 1;
                        if(detectaColisaoTorp(torp,&subs[i]))
                        {
                                cont++;
                                (*torp).status=0;
                                apagatorp(torp);
                                subs[i].status=0;
                                apagaSub(subs[i].coord.x, subs[i].coord.y);
                                aumentaPont(p, 10);
                        }
                }
        }

        if(algum == 0) {
            inicializaSubs(subs);
        }

}

int saveGame(FILE *arq, PLAYER *p, MERGULHADOR *mergs, SUBMARINO *subs, TORPEDO *torp) {

        clrscr();

        char nick[TAM_NICK];
        char saveName[TAM_NICK + 4];

        strcpy(nick, p->nome);
        strcpy(saveName, strcat(nick, ".bin"));

        //printf(" %s", saveName);

        GAMEDATA data = {*p, {*mergs}, {*subs}, *torp};

        if(!(arq = fopen(saveName, "wb"))) {
                printf("\nErro.");
                return 1;
        }else{
                if(!(fwrite(&data, sizeof(GAMEDATA), 1, arq))) {
                        printf("\nErro.");
                        fclose(arq);
                        return 1;
                }else{
                        fclose(arq);
                        printf("\nSalvo.");
                        return 0;
                }
        }

}

int loadGame(FILE *arq, PLAYER *p, MERGULHADOR *mergs, SUBMARINO *subs, TORPEDO *torp) {

        clrscr();

        GAMEDATA buffer;
        char nick[TAM_NICK];
        char saveName[TAM_NICK + 4];
        int i;

        printf("Digite seu nome:");
        scanf(" %s", nick);
        strcpy(saveName, strcat(nick, ".bin"));

        if(!(arq = fopen(saveName, "rb"))) {
                printf("\nErro.");
                return 1;
        }else{
                while(!feof(arq)) {
                        if(fread(&buffer, sizeof(GAMEDATA), 1, arq) == 1) {

                                printf("\n\n%s %d", buffer.player.nome, buffer.player.pontos);
                                *p = buffer.player;
                                mergs = buffer.mergs;
                                subs = buffer.subs;
                                *torp = buffer.torp;
                                fclose(arq);
                                return 0;

                        }else{
                                printf("\nErro.");
                                fclose(arq);
                                return 1;
                        }
                }
                fclose(arq);
                return 0;
        }

}

int existeRecordes(FILE *arq) {
        if(access(RECORDS,F_OK) != -1) {
                // file exists
                return 1;
        } else {
                // file doesn't exist
                return 0;
        }
}

int criaRecordes(FILE *arq, PLAYER *p) {

        if(!(arq = fopen(RECORDS, "w"))) {
                return 0;
        }else{
                fprintf(arq, "1;\n1;%d;%s;\n", p->pontos, p->nome);
                fclose(arq);
                return 1;
        }

}

// 0: Nao entra ou ERRO; OUTRO: lugar
int checaSeRecorde(PLAYER *p, FILE *arq) {

        char buffer[81];
        int i, n, pontos, pos;

        if(!(arq = fopen(RECORDS, "r"))) {
                return 0;
        }else{
                fgets(buffer, sizeof(buffer), arq);
                n = atoi(buffer);

                while(!feof(arq)) {

                    fgets(buffer, sizeof(buffer), arq);
                    pos = atoi(strtok(buffer, ";"));
                    pontos = atoi(strtok(NULL, ";"));
                    if(p->pontos > pontos) {
                            return pos;
                    }
                    if(pos == n && n != 10) {
                        return pos +1;
                    }
                }
                fclose(arq);
                return 0;
        }

}

int salvaRecordes(PLAYER *p, FILE *arq, int pos) {

        char buffer[81], nick[TAM_NICK], nick_aux[TAM_NICK];
        int i, n, pos_file, pontos, pos_aux, pontos_aux;

        if(pos < 1 || pos > 10) {
                return 1;
        }else{

                if(!(arq = fopen(RECORDS, "r+"))) {
                        return 1;
                }else{

                        fgets(buffer, sizeof(buffer), arq);
                        n = atoi(strtok(buffer, ";"));
                        //printf("n: %d\n", n);
                        if(n < pos) {
                            fseek(arq, 0, SEEK_END);
                            fprintf(arq, "%d;%d;%s;\n", pos, p->pontos, p->nome);
                        }else {
                            while(!feof(arq)) {
                                    fgets(buffer, sizeof(buffer), arq);
                                    //printf("Buffer: %s\n", buffer);
                                    pos_file = atoi(strtok(buffer, ";"));
                                    //printf("Pos_file: %d\n", pos_file);
                                    if(pos_file == pos) {
                                            pos_aux = pos_file;
                                            pontos_aux = atoi(strtok(NULL, ";"));
                                            //printf("Pontos_aux: %d\n", pontos_aux);
                                            strcpy(nick_aux, strtok(NULL, ";"));
                                            //printf("Nick_aux: %s\n", nick_aux);
                                            fprintf(arq, "%d;%d;%s;\n", pos, p->pontos, p->nome);
                                            //printf("Printado\n");
                                            fgets(buffer, sizeof(buffer), arq);
                                            //printf("Buffer: %s\n", buffer);

                                            for(i = pos_aux; i <= n; i++) {
                                                //printf("a\n");
                                                fprintf(arq, "%d;%d;%s;\n", pos_aux, pontos_aux, nick_aux);
                                                pos_aux = atoi(strtok(buffer, ";"));
                                                pontos_aux = atoi(strtok(NULL, ";"));
                                                //printf("P: %s", strtok(NULL, ";"));
                                                strcpy(nick_aux, strtok(NULL, ";"));
                                            }
                                            fprintf(arq, "%d;%d;%s;\n", pos_aux, pontos_aux, nick_aux);
                                    }

                            }
                        }
                        fseek(arq, 0, SEEK_SET);
                        fprintf(arq, "%d;", n + 1);
                        fclose(arq);
                        return 0;

                }

        }

}



int recordes(PLAYER *p, FILE *arq) {

        int i;

        if(existeRecordes(arq)) {
                i = checaSeRecorde(p, arq);
                //printf("\n%d\n", i);
                if(i) {
                        //printf("Entrou\n");
                        if(salvaRecordes(p, arq, i)) {
                                printf("Erro ao modificar arquivo.");
                                return 1;
                        }
                        return 0;
                }
        }else{
                if(!criaRecordes(arq, p)) {
                    printf("Erro ao gerar arquivo.");
                    return 1;
                }
                return 0;
        }
}

void perdeVida_colisao(PLAYER *p,SUBMARINO *subs,MERGULHADOR *merg)
{
        int i;
        for(i = 0; i < SUB_MAX; i++) {

                if(detectaColisaoSUB(p, &subs[i])) {

                        perdeVida(p, merg,subs);
                }

        }
}

void mostraRecordes(FILE *arq) {

    char buffer[81], nick[TAM_NICK];
    int i, n;
    int pos, pontos;
    int opcao;

    printf("\n\n\n");
    printf("\t     ___  __ ___ __ _ _ _   (_)_ ___ ____ _ __| |___ _ _ ___\n");
    printf("\t    / _ \\/ _/ -_) _` | '  \\ | | ' \\ V / _` / _` / -_) '_(_-<\n");
    printf("\t    \\___/\\__\\___\\__,_|_||_| |_|_||_\\_/\\__,_\\__,_\\___|_| /__/\n");
    printf("\n");
    printf("\t    []=====================================================[]\n\n\n");
    printf("\t\t\t     MELHORES PONTUACOES\n\n");

    if(!(arq = fopen(RECORDS, "r"))) {
        printf("Erro ao carregar!");
        return;
    }else{
        fgets(buffer, sizeof(buffer), arq);
        n = atoi(strtok(buffer, ";"));
        for(i = 0; i < n; i++) {
            fgets(buffer, sizeof(buffer), arq);
            pos = atoi(strtok(buffer, ";"));
            pontos = atoi(strtok(NULL, ";"));
            strcpy(nick, strtok(NULL, ";"));
            printf("\t\t\t%d.\t%d Pontos\t%s\n", pos, pontos, nick);
        }
        printf("\n\n\n\n\t\t\t   [1]   Voltar ao menu\n");
        printf("\t\t\t   [ESC] Sair");
        opcao = getch(); // usuario pode sair ou voltar ao menu inicial
        if(opcao == ESC) {
                exitGame();
        }else if(opcao == asc1) {
                main();
        }else{
                creditos();
        }
    }

}


int gameLoop(PLAYER *p,int *on, MERGULHADOR *merg, SUBMARINO *subs,TORPEDO *torp) { //funcao que controla o  jogo
        struct timeb start, end; //estrutura para controlar o tempo e a velocidade do jogo
        int t_diff, i;
        ftime(&start);
        do {
                ftime(&end);
                t_diff=(1000.0 * (end.time - start.time)
                        + (end.millitm - start.millitm));
                moveMergs(merg,t_diff);
                moveSub(&(p->sub), on, p, merg,torp);
                moveSubs(subs, t_diff);
                movetorp(torp,t_diff);
                resgataMerg(p, merg);

                destroi_sub(p, torp,subs);
                contaOxi(p,t_diff);
                perdeVidaOxi(p, merg,subs);
                perdeVida_colisao(p,subs,merg);
                ganhaPontoSec(p, t_diff);
                if(p->vidas == 0) {
                        clrscr();
                        return 1;
                }

        } while(*on == 1);

}

void recebeNick(PLAYER *p) {

    char nick[TAM_NICK];

    do {

        clrscr();
        printf("\n\n\n");
        printf("\t     ___  __ ___ __ _ _ _   (_)_ ___ ____ _ __| |___ _ _ ___\n");
        printf("\t    / _ \\/ _/ -_) _` | '  \\ | | ' \\ V / _` / _` / -_) '_(_-<\n");
        printf("\t    \\___/\\__\\___\\__,_|_||_| |_|_||_\\_/\\__,_\\__,_\\___|_| /__/\n");
        printf("\n");
        printf("\t    []=====================================================[]\n\n\n\n\n\n\n\n");
        printf("\t\t\t\tDigite seu nome:\n\t\t\t\t(max. 8 carac.)\n\n\t\t\t\t   ________");
        gotoxy(36, 19);
        scanf("%s", nick);

    } while(strlen(nick) > TAM_NICK);

    strcpy((*p).nome, nick);

    return;

}

int init(FILE *arq_i, FILE *arq_o, PLAYER *p,int *on, MERGULHADOR *merg, SUBMARINO *subs,TORPEDO *torp){//funcao que controla a incicializao do jogo

        int opcao = 0;
        opcao = menu(&opcao);

        inicializaMergs(merg);//inicializa os merg
        inicializaSubs(subs);//inicializa os subs inimigos


        switch(opcao) { //pega a tecla pressionado pelo usuario

        case ESC:
                exitGame(); //se for esc fecha o jogo
                break;

        case asc1://se for 1 comeca um novo jogo
                //novoJogo();
                *on = 1;
                startGame(p);
                gameLoop(p, on, merg,subs,torp);
                recebeNick(p);
                if(p->vidas != 0) {//se as vidas forem maior que 0
                        saveGame(arq_i, p, merg, subs, torp);//salva o jogo
                }
                recordes(p, arq_o);
                exitGame();
                break;

        case asc2://se for 2 carrega um jogo existente
                if(loadGame(arq_i, p, merg, subs, torp)) {
                        printf("Erro.");
                        break;
                }else{
                        *on = 1;
                        startGame(p);
                        gameLoop(p, on, merg,subs,torp);
                        recebeNick(p);
                        if(p->vidas != 0) {
                                saveGame(arq_i, p, merg, subs, torp);
                        }
                        recordes(p, arq_o);
                        exitGame();
                }
                break;

        case asc3://se for 3 mostra os recordes
                clrscr();
                mostraRecordes(arq_i);
                break;

        case asc4://se for 4 mostra os creditos
                creditos();
                break;

        default:
                main();
                break;

        }

        return 0;


}

int main() {//main

        //writeSub_L(SUB_Xi, SUB_Yi);
        PLAYER p  = {VIDASi, PONTOSi,"ASD", RESCUEi,
                     {1, BRANCO, SUB_COMP, SUB_ALT, 1, {SUB_Xi, SUB_Yi}},{{OXIxi,OXIyi},1,QTDOXIi} };//comeca o player com valores padroes

        MERGULHADOR merg[MERG_MAX] = {0};//mergs em 0

        SUBMARINO subs[SUB_MAX] = {0};// subs em 0
        TORPEDO torp={0};

        FILE *arq_i, *arq_o;

        int on;
        init(arq_i, arq_o, &p, &on, merg, subs,&torp);//chama inicializacao

}
