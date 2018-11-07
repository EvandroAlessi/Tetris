#include <stdio.h>// printf
#include <conio.h>
#include <stdlib.h>
#include <time.h>//hor�rio
#include <windows.h>//Func�es de sistema
#include <stdbool.h>//permite o retorno de valores logicos
#include <string.h>//Func�es de string como strcomp, str
#include <ctype.h>//toupper, lower, isalpha
#include "MyBiblio.h"

#define H 28 //Altura do campo
#define L 18 //Largura do campo

struct Desenha//posibilita a escolha de uma rota��o do mesmo desennho
{
    char desenho[4][5];//matriz que armazena os desenhos
};

struct Peca//dimens�es das pe�as
{
    int largura;//largura da pe�a
    int altura;//altura da pe�a
    struct Desenha rotacao[4];//vetor da struct rota��o
};

struct RANK//Guara os valores para que ele possam ser atribuidos ao arquivo
{
	int pontos;
	char apelido[15];
};

struct Teste//usada para realizar testes multiplos, evitando gasto desnecessario de memoria
{
	int linha, coluna;
};

char nome[20];//nome do jogador
char campo[H][L];//Matriz de controle
int pontos = 0;
int level = 1;

FILE *file;//ranking
FILE *TETRIS;//TETRIS escrito de um jeito maneiro
FILE *GAME;//Game Over escrito de um jeito maneiro

/**********************************************************************************************************************************************************************************************/
void Escreva_GAME_OVER()//printa o arquivo substituindo pelo valor asc 254
{
	int i = 0;
	char show;
	GAME = fopen("GAME.txt", "r");
	HANDLE hConsole;
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 12);//vermelho '-'
	while((fscanf(GAME, "%c", &show)) != EOF)//L�
    {
    	if(show == '1')//Tranforma
    	{
    		show = 254;
		}
		else if(show == ' ')//pula linha ao chega ao fim da linha
		{
			i++;
			puts("");
			gotoyx(15 + i, 50);
		}
		else//atribui espa�o vazio para manter o desenho
		{
			show = 0;
		}
		printf("%c", show);//apresentar� a mesma//setbuf(stdin, NULL);
	}
	SetConsoleTextAttribute(hConsole, 15);
	fclose(GAME);
}
/**********************************************************************************************************************************************************************************************/
void GameOver()//encerra o jogo
{
    file = fopen("ranking.txt","a");//abre para ser adicionado ao fim do arquivo sem apagar o que ja est� no arquivo
    system("cls");
    Escreva_GAME_OVER();
  	gotoyx(34,62);
    printf("Level Final: %d", level);
  	gotoyx(35,62);
    printf("Score Final: %d", pontos);
    fprintf(file, "%s\t%d\n", nome, pontos);//Guarda no arquivo
    Sleep(1000);
    fclose(file);
    Menu_Sair();
}
/**********************************************************************************************************************************************************************************************/
void Exclui_Linha(int *l)//Exclui a linha, caso a mesma esteja completa
{
    int i, j;//controladores de la�o

    for(i = 1; i < L; i++)//exclui linha
    {
        campo[*l][i] = 0;
    }
    for(i = *l; i > 1; i--)//muda posi��o das pe�as no campo, assim como dos 0
    {
        for(j = 1; j < L; j++)
        {
            campo[i][j] = campo[i-1][j];
        }
    }
    Imprime_Campo();
}
/**********************************************************************************************************************************************************************************************/
void Verifica_Linha()//verifica se uma das linha est� completa
{
    int cont, i, j;//Contador para verifica��o de linha, poderia ser uma flag tbm, mas quis inovar kkk

    for(i = H - 1; i > 0; i--)
    {
        cont = 0;
        for(j = 1; j < L; j++)//Testa se a linha est� preenchida
        {
            if(campo[i][j] == '\xFE')//==254, mas em hexa
            {
                cont++;
            }
        }
        if(cont == (L - 2))//caso a linha esteja completa
        {
            pontos = pontos + cont;//soma dos pontos com base na quantia de quadrados excluidos por linha
            Exclui_Linha(&i);
        }
    }
}
/**********************************************************************************************************************************************************************************************/
void Score()//apresenta os pontos feitos pelo jogador
{
  	gotoyx(L + 3, 1);
    Verifica_Linha();
  	gotoyx(6, L + 4);
    printf("SCORE: %d", pontos);//Apresenta os pontos
}
/**********************************************************************************************************************************************************************************************/
void Controla_Level(int *T)//Verifica o level, assim como aumenta a velocidade de acordo com o level//Ajeitar
{
    if(pontos == (L - 2)*3)//Testa a quantia de pontos realizados para indicar o level e aumentar a velocidade
    {//-2 devido as bordas
        level = 2;//level
        *T = *T * 0.90;//aumento de velocidade//ou seja, diminui o tempo de espera entre desenhar e apagar
    }
    else if(pontos == (L - 2)*5)
    {
    	level = 3;
        *T = *T * 0.75;
    }
    else if(pontos == (L - 2)*7)
    {
        level = 4;
        *T = *T * 0.60;
    }
    else if(pontos == (L - 2)*9)
    {
        level = 5;
        *T = *T * 0.55;
    }
    else if(pontos >= (L - 2)*12)//Level HARD
    {
        level = 1000;
        *T = *T * 0.30;
    }
  	gotoyx(3, L + 4);//posi��o em que vau aparecer escrito level
    printf("LEVEL: %d", level);//printa o level atual
    Score();
}
/**********************************************************************************************************************************************************************************************/
void Atribui(int linha, int coluna, int altura, int largura, struct Desenha peca)// Atribui��o geral de todas as pe�as
{
    int i, j;
    setbuf(stdin, NULL);
    for(i = linha; i < (altura + linha); i++)
    {
        for(j = coluna; j < (largura + coluna); j++)
        {
        	if(peca.desenho[i - linha][j - coluna] == '\xFE')
        	{
                gotoyx(i, j);
            	campo[i][j] = 254;//guarda o caractere 254 na matriz de controle
        	}
        }
    }
}
/**********************************************************************************************************************************************************************************************/
void Imprime_Apaga_Peca(struct Desenha rotacao, int altura, int largura, int pos_i, int pos_x, int alternador)//printa o desenho na tela
{
    int i, j, aux = 0;//controladores de la�o
    setbuf(stdin, NULL);
    for(i = 0; i < altura; i++)//fixa o come�o de cada linha para que a pe�a seja desenhada corretamente
    {
    	aux = pos_x;//evitar que o aux seja incrementado mais de uma vez
        for(j = 0; j < largura; j++)
        {
        	if(rotacao.desenho[i][j] == 0)
        	{
        		++aux;
                gotoyx(pos_i, aux);//seria o espa�o, por�m sem printar isso em tela, para isso apenas fomos para a proxima posi��o da coluna
        	}
        	else
        	{
        		if(alternador == 1)
        		{
        			printf("%c", rotacao.desenho[i][j]);//desenha a pe�a
				}
        		else
        		{
        			printf("%c", 0);//apaga a pe�a
				}
        	}
        }
        rotacao.desenho[i][j] = '\0';//Caractere de termino de string//evita aparecimento de lixo
        ++pos_i;//Altera a linha
      	gotoyx(pos_i, pos_x);//volta sempre na mesma coluna e varia as linhas
    }
}
/**********************************************************************************************************************************************************************************************/
struct Peca Gerar_Peca_Vazia(int altura, int largura)//Contro� uma matriz vazia do tamnho das pe�as
{
    struct Peca peca;//struct para retorno
    int i, j, k;//controladores de la�o
    peca.largura = largura;//controla a largura da pe�a na hora de desenhar ela
    peca.altura = altura;//controla a altura da pe�a sna hora de desenhar ela
    setbuf(stdin, NULL);
    for(i = 0; i < 4; i++)
    {
        for(j = 0; j < altura; j++)
        {
            for(k = 0; k < largura; k++)
            {
                peca.rotacao[i].desenho[j][k] = 0;
            }
            peca.rotacao[i].desenho[j][k] = '\0';
        }
    }
    return peca;//restorna a struct peca do tipo Peca
}
/**********************************************************************************************************************************************************************************************/
void Desenha_Peca(struct Peca pecas[4], int r, int s)//depois de chamar a fun��o que contro� a matriz vazia desenha a pe�a nas posições correspondentes
{
    int i;//controlador de la�o
    setbuf(stdin, NULL);
    switch(s)//seleciona a pe�a
    {
        case 0://Desenho e rota��o do L
            if(r == 0)//manipulando um vetor por isso surge efeito nas demais fun��es
            {
                pecas[0] = Gerar_Peca_Vazia(3, 2);

                pecas[0].rotacao[0].desenho[0][0] = 254;
                pecas[0].rotacao[0].desenho[1][0] = 254;
                pecas[0].rotacao[0].desenho[2][0] = 254;
                pecas[0].rotacao[0].desenho[2][1] = 254;
            }
            else if(r == 1)
            {
                pecas[0] = Gerar_Peca_Vazia(2, 3);

				pecas[0].rotacao[1].desenho[0][2] = 254;
        		pecas[0].rotacao[1].desenho[1][0] = 254;
        		pecas[0].rotacao[1].desenho[1][1] = 254;
        		pecas[0].rotacao[1].desenho[1][2] = 254;
            }
            else if(r == 2)
            {
            	pecas[0] = Gerar_Peca_Vazia(3, 2);

            	pecas[0].rotacao[2].desenho[0][0] = 254;
                for(i = 0; i < pecas[0].altura; i++)
                {
                    pecas[0].rotacao[2].desenho[i][1] = 254;//01,11,21
                }
            }
            else //if(r == 3)
            {
            	pecas[0] = Gerar_Peca_Vazia(2, 3);

            	for(i = 0; i < pecas[0].largura; i++)
                {
                    pecas[0].rotacao[3].desenho[0][i] = 254;//00, 01, 02
                }
        		pecas[0].rotacao[3].desenho[1][0] = 254;
            }
            break;
        case 1://Desenho e rota��o do Quadrado
            pecas[1] = Gerar_Peca_Vazia(2, 2);//mesmo tamanho em todas as rotações
            for(i = 0 ; i < 4; i++)//mesmo desenho para todas as rotações
            {
                pecas[1].rotacao[i].desenho[0][0] = 254;
                pecas[1].rotacao[i].desenho[0][1] = 254;
                pecas[1].rotacao[i].desenho[1][0] = 254;
                pecas[1].rotacao[i].desenho[1][1] = 254;
            }
            break;
        case 2://Desenho e rota��o do I
            if(r == 0 || r == 2)
            {
                pecas[2] = Gerar_Peca_Vazia(1, 4);
                for(i = 0 ; i < 4; i++)
                {
                    pecas[2].rotacao[0].desenho[0][i] = 254;//----
                    pecas[2].rotacao[2].desenho[0][i] = 254;//----
                }
            }
            else //if(r == 1 || r == 3)
            {
                pecas[2] = Gerar_Peca_Vazia(4, 1);
                for(i = 0 ; i < 4; i++)
                {
                    pecas[2].rotacao[1].desenho[i][0] = 254;// I
                    pecas[2].rotacao[3].desenho[i][0] = 254;// I
                }
            }
            break;
        case 3://Desenho e rota��o do T
            if(r == 0)
            {
                pecas[3] = Gerar_Peca_Vazia(2, 3);
                for(i = 0; i < pecas[3].largura; i++)
                {
                    pecas[3].rotacao[0].desenho[0][i] = 254;//00, 01, 02
                }
                pecas[3].rotacao[0].desenho[1][1] = 254;
            }
            else if(r == 1)
            {
            	pecas[3] = Gerar_Peca_Vazia(3, 2);
                for(i = 0; i < pecas[3].altura; i++)
                {
                    pecas[3].rotacao[1].desenho[i][0] = 254;//00, 10, 20
                }
                pecas[3].rotacao[1].desenho[1][1] = 254;
            }
            else if(r == 2)
            {
                pecas[3] = Gerar_Peca_Vazia(2, 3);

				for(i = 0; i < pecas[3].largura; i++)
                {
                    pecas[3].rotacao[2].desenho[1][i] = 254;//10, 11, 12
                }
                pecas[3].rotacao[2].desenho[0][1] = 254;
            }
            else
            {
                pecas[3] = Gerar_Peca_Vazia(3, 2);
                pecas[3].rotacao[3].desenho[1][0] = 254;
				for(i = 0; i < pecas[3].altura; i++)
                {
                    pecas[3].rotacao[3].desenho[i][1] = 254;//01, 11, 21
                }
            }
            break;
        case 4://Desenho e rota��o do L espelhado
            if(r == 0)//manipulando um vetor por isso surge efeito nas demais fun��es
            {
                pecas[4] = Gerar_Peca_Vazia(3, 2);

                pecas[4].rotacao[0].desenho[0][1] = 254;
                pecas[4].rotacao[0].desenho[1][1] = 254;
                pecas[4].rotacao[0].desenho[2][1] = 254;
                pecas[4].rotacao[0].desenho[2][0] = 254;
            }
            else if(r == 1)//todas as pe�as s�o desenhadas de cima pra baixo, ou seja, da posi��o atual para as inferiores, jamais usurpando um espa�o j� percorrido
            {
                pecas[4] = Gerar_Peca_Vazia(2, 3);

                pecas[4].rotacao[1].desenho[1][0] = 254;
                pecas[4].rotacao[1].desenho[1][1] = 254;
                pecas[4].rotacao[1].desenho[1][2] = 254;
                pecas[4].rotacao[1].desenho[0][0] = 254;
            }
            else if(r == 2)
            {
                pecas[4] = Gerar_Peca_Vazia(3, 2);

                pecas[4].rotacao[2].desenho[0][1] = 254;
                for(i = 0; i < pecas[4].altura; i++)
                {
                    pecas[4].rotacao[2].desenho[i][0] = 254;//00,10,20
                }
            }
            else //if(r == 3)
            {
                pecas[4] = Gerar_Peca_Vazia(2, 3);
                for(i = 0; i < pecas[4].largura; i++)
                {
                    pecas[4].rotacao[3].desenho[0][i] = 254;//00, 01, 02
                }
                pecas[4].rotacao[3].desenho[1][2] = 254;
            }
            break;
        case 5://the flash//Raio//Ac/Dc
            if(r == 0 || r == 2)
            {
                pecas[5] = Gerar_Peca_Vazia(3, 2);

                pecas[5].rotacao[r].desenho[0][1] = 254;
                pecas[5].rotacao[r].desenho[1][0] = 254;
                pecas[5].rotacao[r].desenho[1][1] = 254;
                pecas[5].rotacao[r].desenho[2][0] = 254;
            }
            else //if(r == 1 || r == 3)
            {
                pecas[5] = Gerar_Peca_Vazia(2, 3);

                pecas[5].rotacao[r].desenho[0][0] = 254;
                pecas[5].rotacao[r].desenho[0][1] = 254;
                pecas[5].rotacao[r].desenho[1][1] = 254;
                pecas[5].rotacao[r].desenho[1][2] = 254;
            }
    }
    Hide_Cursor();
}
/**********************************************************************************************************************************************************************************************/
int Move_Test(int linha,int coluna,int Altura_Peca,int Largura_peca)
{
	int i, j, Flag = 1;
	for(i = linha; i <= (linha + Altura_Peca); i++)//Controla para que a rota��o n�o exarcebe as bordas e tbm n�o sobreponha as pe�as
    {
        for(j = coluna; j < (coluna + Largura_peca); j++)//invertido por considerar a proxima e n�o a atual
        {
            if(campo[i][j] == 0)
            {
                Flag = Flag * 1;//Multiplicado para que sejam validadas todas e n�o apenas um posi��o
            }
            else
            {
                Flag = 0;
            }
        }
    }
    return Flag;
}
/**********************************************************************************************************************************************************************************************/
void Percorre_Campo(int *R, int l, int *X, int Altura_Peca, int Largura_peca, int *Tempo)//Movimenta��o das pe�as
{
    int Flag = 1, i, j, resp;// flag para controle de rota��o
    setbuf(stdin, NULL);
    if(kbhit())//Testa se foi digita alguma tecla
    {
        switch(toupper(getch()))//Primeiro pega a tecla digitada, depois deixa ela em maiusculo e testa no switch
        {
            case '\x1B'://pause hehe//ESC
                setbuf(stdin, NULL);
                gotoyx(H + 2, 0);
                printf("Digite 1 para volta ao Menu ou 2 para continuar o jogo: ");
                resp = Valida_Entrada(2);
   				Sleep(200);
   				system("cls");
   				switch(resp)
   				{
                    case 1:
                        file = fopen("ranking.txt", "a");
                        fprintf(file, "%s\t%d\n", nome, pontos);
                        fclose(file);
                        Menu();
                        break;
                    case 2:
                        Imprime_Campo();
                        break;
   				}
            case 'W':
                Flag = 1;//usei a altura devido a caracteristica de a proxima rota��o futura ter largura igual a altura da atual
                for(i = l + 1; i < (l + 1 + Largura_peca ); i++)//Controla para que a rota��o n�o exarcebe as bordas e tbm n�o sobreponha as pe�as
                {
                    for(j = *X; j < (*X + Altura_Peca); j++)//invertido por considerar a proxima e n�o a atual
                    {
                        if(campo[i][j] == 0)
                        {
                            Flag = Flag * 1;
                        }
                        else
                        {
                            Flag = 0;
                        }
                    }
                }
                if(Flag == 1)//evita que passe da quarta rota��o da pe�a
                {
                    if(*R < 3)
                    {
                        ++*R;//muda a rota��o das pe�as
                    }
                    else
                    {
                        *R = 0;//ao atingir a quarta rota��o volta para a primeira
                    }
                }
                break;
            case 'S':
                *Tempo = *Tempo/2;//aumenta a velocidade de descida da pe�a
                break;
            case 'A':
            	Flag = Move_Test(l, *X - 1, Altura_Peca, Largura_peca);//Testa se pode mover a pe�a
                if((*X != 1) && Flag == 1)//controla para que n�o ultrapasse a borda esquerda
                {
                    --*X;//muda a pe�a para a posi��o imediatamente a esquerda
                }
                break;
            case 'D':
            	Flag = Move_Test(l, *X + 1, Altura_Peca, Largura_peca);//Testa se pode mover a pe�a
                if(((*X + Largura_peca  +  1) < L) && Flag == 1)//controla para que n�o ultrapasse a borda direita, considerando a posi��o atual + a largura da pe�a na proxima posi��o e tirando a borda
                {
                    ++*X;//muda a pe�a para a posi��o imediatamente a direita
                }
                break;
        }
    }
}
/**********************************************************************************************************************************************************************************************/
int Testa_posicao(struct Teste pos[3])
{
	if(campo[pos[0].linha][pos[0].coluna] == 0 && campo[pos[1].linha][pos[1].coluna] == 0 && campo[pos[2].linha][pos[2].coluna] == 0)//Testa se a pr�xima posi��o sequencial a altura da pe�a � igual a 0 e se a posi��o seguinte depois do desenho na primeira coluna � igual a 0
    {
        return 1;
	}
    else
    {
        return 0;
    }
}
/**********************************************************************************************************************************************************************************************/
void Manipula_Pecas()//Controla tudo que � feito com as pe�as, desde desenha, apagar e mover at� atribuir
{
    int j, k, l, Flag, linha = 1, c = 0, Alternador, rota = 0, sorte = 0, coluna = 0, T = 550;
    struct Peca pecas[7];//vetor de struct do tipo Peca
	struct Teste Pos[3];

    srand((unsigned) time(NULL));// parametros para a rand, usando tempo
    sorte = rand() % 6;// rand: gera aleatoriedade para escolher uma pe�a aleatoria de 0~3 no inicio
    rota = rand() % 4;//rota��o inicial aleatoria
    coluna = 1 + (rand() % (L - 5));//Posis�o horizontal aleat�ria// -5 devido ao tamanho da maior peca + a borda

	HANDLE hConsole;
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 12);
    Controla_Level(&T);//Chama a fun��o de level, que chamd Score, que chama verifica linha que chama exclui linha
    do//De 1 enquanto linha < altura - altura da pe�a - borda inferior
    {
        Flag = 1;
        Desenha_Peca(pecas, rota, sorte);//constroi as pe�as
      	gotoyx(linha, coluna);//posi��o inicial da pe�a para impres�o
        if(sorte == 5 || sorte == 6 || (sorte == 4 && (rota == 2 || rota == 3)) || (sorte == 0 && (rota == 2 || rota == 3)) || (sorte == 3 && (rota == 0 || rota == 1 || rota == 3)))
        {
            if(sorte == 0 && (rota == 2 || rota == 3))
            {
            	switch(rota)
            	{
            		case 2:
            			Pos[0].linha = linha + pecas[sorte].altura;
            			Pos[0].coluna = coluna + 1;

            			Pos[1].linha = linha + 1;
            			Pos[1].coluna = coluna;

						Pos[2].linha = linha + 1;//repete para dar certo os paramentros
            			Pos[2].coluna = coluna;//repete para dar certo os paramentros
                    	break;
                   case 3:
                   		Pos[0].linha = linha + pecas[sorte].altura;
            			Pos[0].coluna = coluna;

            			Pos[1].linha = linha + 1;
            			Pos[1].coluna = coluna + 1;

						Pos[2].linha = linha + 1;
            			Pos[2].coluna = coluna + 2;
                    	break;
            	}
            }
            else if(sorte == 3 && (rota == 0 || rota == 1 || rota == 3))
            {
            	switch(rota)
            	{
            		case 0:
            			Pos[0].linha = linha + pecas[sorte].altura;
            			Pos[0].coluna = coluna + 1;

            			Pos[1].linha = linha + 1;
            			Pos[1].coluna = coluna;

						Pos[2].linha = linha + 1;
            			Pos[2].coluna = coluna + pecas[sorte].largura - 1;
                    	break;
                    case 1:
                    	Pos[0].linha = linha + pecas[sorte].altura;
            			Pos[0].coluna = coluna;

            			Pos[1].linha = linha;
            			Pos[1].coluna = coluna + 1;

						Pos[2].linha = linha + pecas[sorte].altura - 1;//repete para dar certo os paramentros
            			Pos[2].coluna = coluna + 1;//repete para dar certo os paramentros
                    	break;
                   case 3:
                   		Pos[0].linha = linha + pecas[sorte].altura;
            			Pos[0].coluna = coluna + 1;

            			Pos[1].linha = linha + 1;
            			Pos[1].coluna = coluna;

						Pos[2].linha = linha + pecas[sorte].altura - 1;//repete para dar certo os paramentros
            			Pos[2].coluna = coluna;//repete para dar certo os paramentros
                    	break;
            	}
            }
            else if(sorte == 4)
            {
            	switch(rota)
            	{
            		case 2:
            			Pos[0].linha = linha + pecas[sorte].altura;
            			Pos[0].coluna = coluna;

            			Pos[1].linha = linha + 1;
            			Pos[1].coluna = coluna + 1;

						Pos[2].linha = linha + 1;//repete para dar certo os paramentros
            			Pos[2].coluna = coluna + 1;//repete para dar certo os paramentros
                    	break;
                   case 3:
                   		Pos[0].linha = linha + pecas[sorte].altura;
            			Pos[0].coluna = coluna + 2;

            			Pos[1].linha = linha + 1;
            			Pos[1].coluna = coluna;

						Pos[2].linha = linha + 1;
            			Pos[2].coluna = coluna + 1;
                    	break;
            	}
            }
            else if(sorte == 5)
            {
            	switch(rota)
            	{
            		case 0:
            			case 2:
            				Pos[0].linha = linha + pecas[sorte].altura;
            				Pos[0].coluna = coluna;

            				Pos[1].linha = linha + 2;
            				Pos[1].coluna = coluna + 1;

							Pos[2].linha = linha + 2;//repete para dar certo os paramentros
            				Pos[2].coluna = coluna + 1;//repete para dar certo os paramentros
                    		break;
                    case 1:
                   		case 3:
                   			Pos[0].linha = linha + pecas[sorte].altura;
            				Pos[0].coluna = coluna + 1;

            				Pos[1].linha = linha + 1;
            				Pos[1].coluna = coluna;

							Pos[2].linha = linha + pecas[sorte].altura;//repete para dar certo os paramentros
            				Pos[2].coluna = coluna + pecas[sorte].largura - 1;//repete para dar certo os paramentros
                    		break;
            	}
            }
            Flag = Testa_posicao(Pos);
		}
        else
        {
        	for(k = linha; k <= (linha + pecas[sorte].altura ); k++)//Controla para que a rota��o n�o exarcebe as bordas e tbm n�o sobreponha as pe�as
            {
                for(l = coluna; l < (coluna + pecas[sorte].largura ); l++)//invertido por considerar a proxima e n�o a atual
                {
                    if(campo[k][l] == 0)
                    {
                        Flag = Flag * 1;
                    }
                    else
                    {
                        Flag = 0;
                    }
                }
            }
        }
        if(Flag == 0)
        {
            SetConsoleTextAttribute(hConsole, 15);
            if(linha == 1)
            {
            	GameOver();
            }
		}
        Alternador = 1;
        Imprime_Apaga_Peca(pecas[sorte].rotacao[rota], pecas[sorte].altura, pecas[sorte].largura, linha, coluna, Alternador);
        Sleep(T);//Faz com que o pragrama pare por T mls
      	gotoyx(linha, coluna);//volta para a posi��o onde foi criada a pe�a para apaga-la
        if(Flag == 1)
        {
            Alternador = 0;
        	Imprime_Apaga_Peca(pecas[sorte].rotacao[rota], pecas[sorte].altura, pecas[sorte].largura, linha, coluna, Alternador);
        }
        else
        {
            Atribui(linha, coluna, pecas[sorte].altura, pecas[sorte].largura, pecas[sorte].rotacao[rota]);

        }
        Percorre_Campo(&rota, linha, &coluna, pecas[sorte].altura, pecas[sorte].largura, &T);//controla a movimenta��o
        linha++;
    }while(Flag == 1 && linha < (H - pecas[sorte].altura));//Impede que passe do limite inferior, e permite Encaixar as pe�as
    Manipula_Pecas();//Chama ela mesma pra cria a pe�a seguinte
}
/**********************************************************************************************************************************************************************************************/
void Ordena_Ranking(struct RANK ficha[30], int i)//odena��o do arquivo ranking
{
    char aux2[15];
    int aux = 0, j=0, k = 0, l = 0;

    for (j = 0; j <= i - 1; j++)//incrementa
    {
    	for (k = i - 2; k >= j; k--)//testa at� a posi��o do outro for
    	{
    		if(ficha[k + 1].pontos > ficha[k].pontos)
    		{
    			aux = ficha[k + 1].pontos;//trocas os pontos de lugar
            	ficha[k + 1].pontos = ficha[k].pontos;
        		ficha[k].pontos = aux;

        		strcpy(aux2, ficha[k + 1].apelido);//troca os nomes de lugar
            	strcpy(ficha[k + 1].apelido, ficha[k].apelido);
            	strcpy(ficha[k].apelido, aux2);
			}
		}
	}
	file = fopen("ranking.txt", "w+");//abre o arquivo e apaga o conteudo presente nele
    while(l < i)
    {
        fprintf(file, "%s\t%d\n", ficha[l].apelido, ficha[l].pontos);//escreve novamente de forma ordenada
        l++;
    }
    fclose(file);
}
/**********************************************************************************************************************************************************************************************/
void Ranking()// apresenta um Ranking, t� faltando ajusta a posis�o dos nomes e pontos, assim como ordenar
{
	struct RANK Armazem[30];
    char resp[8], showup[50];//resposta e variavel que guarda os caracteres presentes no arquivo para que posteriormente seja possivel apresentalos
    int i = 0, j = 0;

    HANDLE hConsole;//cores
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 12);//vermelho '-'

    file = fopen("ranking.txt","r");//abre o arquivo de texto no modo leitura

    while((fscanf(file, "%s%d", Armazem[i].apelido, &Armazem[i].pontos)) != EOF)
    {
        i++;

	}
	fclose(file);
    Ordena_Ranking(Armazem, i);
    setbuf(stdin, NULL);//limpar o buffer do teclado
  	gotoyx(0, 50);
    printf(">>>>>>>>>>>>>Ranking<<<<<<<<<<<<");
  	gotoyx(2, 50);
    printf("  APELIDO\t\tPONTUACAO");
    SetConsoleTextAttribute(hConsole, 15);//vermelho '-'
     if(i == 0)
    {
        puts("Ranking vazio...");//Caso o arquivo esteja vazio
	}
   	else
	{
		while(j < i)//enquanto tiver caracteres dentro do file, ira passar toda uma string para a variavel showup, até o tamanho do vetor
   	 	{
    	gotoyx(j + 4, 50);
        	printf("%d - %s\t\t    %d", j + 1, Armazem[j].apelido, Armazem[j].pontos);
        	j++;
    	}
	}
    Menu_Sair();
}
/**********************************************************************************************************************************************************************************************/
void Como_Jogar()// explica��o de como funciona o jogo//Bem inutil
{
    puts("O Objetivo do tetris eh completar linhas, para assim farmar pontos");
    puts("Para isso devesse encaixar as pecas, umas as outras");
    puts("Durante o percurso da peca eh possivel efitivar 4 tipos de movimentos:");
    Apresenta_Teclas();
    puts("O jogo contem as pecas: L, L espelhado, T, Raio, Quadrado e I");
    puts("Estao presentes quatro Leveis, quanto mais alto o level, maior a velocidade e consequentemente maior a dificuldade");
    puts("A passagem de level eh efetuada pela pontuacao, quanto maior a pontuacao maior o level");
    Menu_Sair();
}
/**********************************************************************************************************************************************************************************************/
void Imprime_Campo()//Printa tudo o que et� presente na matriz de controle
{
    int i, j;//controladores de la�o
    gotoyx(0,0);//caso precise ser redesenhado
    HANDLE hConsole;//cores
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, 15);
    for(i = 0; i < H; i++)
    {
        for(j= 0; j < L; j++)
        {
            printf("%c", campo[i][j]);
        }
        puts("");//pula para a pr�xima linha
    }
    Manipula_Pecas();
}
/**********************************************************************************************************************************************************************************************/
void Campo_De_Jogo()//Constroi o campo de jogo e atribui os valores a matriz de controle
{
    int i, j;//controladores de la�o
    for(i = 0; i < H; i++)//0 at� 25 - 2 das bordas = 24 de altura interna
    {
        setbuf(stdin, NULL);
        if(i > 0 && i < H - 1)//bordas laterais
        {
            campo[i][0] = 186;
            for(j = 1; j < L-1; j++)//0 at� 20 - 2 das bordas = 19 de largura interna
            {
                campo[i][j] = 0;
            }
            campo[i][L-1] = 186;
        }
        else if(i == 0)//borda superior
        {
            campo[0][0] = 201;
            for(j = 1; j < (L - 1); j++)
            {
                campo[0][j] = 205;
            }
            campo[0][L-1] = 187;
        }
        else//borda inferior
        {
            campo[H-1][0] = 200;
            for(j = 1; j < (L - 1); j++)
            {
                campo[H-1][j] = 205;
            }
            campo[H-1][L-1] = 188;
        }
    }
    Imprime_Campo();
}
/**********************************************************************************************************************************************************************************************/
void Escreve_TETRIS()//Criada devido a incapacidade de imprimir o caracter 254 da tabela ascII
{
	int i = 0;
	char showTETRIS;
	TETRIS = fopen("Tetris.txt", "r");
	HANDLE hConsole;
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 12);//vermelho '-'
	while((fscanf(TETRIS, "%c", &showTETRIS)) != EOF)//L�
    {
    	if(showTETRIS == '1')//Tranforma
    	{
    		showTETRIS = 254;
		}
		else if(showTETRIS == ' ')//pula linha ao chega ao fim da linha
		{
			i++;
			puts("");
			gotoyx(15 + i, 40);
		}
		else//atribui espa�o vazio para manter o desenho
		{
			showTETRIS = 0;
		}
		printf("%c", showTETRIS);//apresentar� a mesma//setbuf(stdin, NULL);
	}
	SetConsoleTextAttribute(hConsole, 15);//retorna para cor anterior
	fclose(TETRIS);
}
/**********************************************************************************************************************************************************************************************/
void Menu(void)//Mostra as op��es ao usuario assim como chama as demais fun��es
{
    int resp;
    gotoyx(15, 40);
    Escreve_TETRIS();
    setbuf(stdin, NULL);
  	gotoyx(30,60);
    printf("1 - Iniciar o Jogo;");
  	gotoyx(31,60);
    printf("2 - Como jogar;");
  	gotoyx(32,60);
    printf("3 - Ranking;");
  	gotoyx(33,60);
    printf("4 - Sair.");
  	gotoyx(34,60);
  	printf("Opcao: ");
  	resp = Valida_Entrada(4);
    system("cls");
    switch(resp)
    {
        case 1:
            printf("Digite seu Apelido: ");//nome do jogador
            scanf("%s", nome);
            system("cls");
            Apresenta_Teclas();
            printf("Digite Alguma tecla para continuar!");
            getch();
            system("cls");
            Campo_De_Jogo();//constroi o campo
            break;
        case 2:
            Como_Jogar();//explica o jogo
            break;
        case 3:
            Ranking();//Ranking
            break;
        case 4:
            puts("Saindo do Jogo...");
            Sleep(500);
            exit(0);
            break;
    }
}
