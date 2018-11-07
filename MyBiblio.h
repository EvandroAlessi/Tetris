#ifndef GOTOYX_H_INCLUDED
#define GOTOYX_H_INCLUDED

void gotoyx(int linha, int coluna)//posição do cursor em tela
{
  COORD coord;
  coord.Y = linha;
  coord.X = coluna;
  SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}
/*************************************************************************************************************************************************/
void Hide_Cursor()//Evita que o cursor fique piscando
{
  CONSOLE_CURSOR_INFO cursor = {1, FALSE};
  SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor);
}
/*************************************************************************************************************************************************/
int Valida_Entrada(int opc)//valida a entrada do usuario obrigando ele a digitar uma opcao disponivel
{
	int ASC = 0, valor;
    do
	{
    	ASC = scanf("%d", &valor);//recebe o valor asc
    	while(getchar() != '\n');//enquanto não digitar enter
    	if(ASC != 1 || valor < 1 || valor > opc)//testa se não foi digitado espaço e se está entre as opções
    	{
    		printf("Entrada invalida. Digite novamente. \t");
		}
	}while(ASC != 1 || valor < 1 || valor > opc);//repete ate´ser valida a opcao
	return valor;//retorna a opcao validada
}
/*************************************************************************************************************************************************/
void Menu_Sair()//Dá ao usuário a opção de voltar ao menu ou sair do jogo//evita repetção desnecessária de código
{
    int resp;//resposta
    setbuf(stdin, NULL);//limpa o buffer
    printf("\n\nDigite 1 para volta ao MENU ou  2 para SAIR: ");
    resp = Valida_Entrada(2);//recebe o retorno da validação
	Sleep(200);
    system("Cls");
    switch(resp)
    {
    	case 1:
    		Menu();
    		break;
    	case 2:
    		puts("Finalizando o Game...");
        	exit(0);
        	break;
	}
}
/*************************************************************************************************************************************************/
void Apresenta_Teclas()//Introdução ao jogoS
{
	puts(">>>>>>>>>>TECLAS<<<<<<<<<<");
	puts("- ESC: Pausa o jogo;");
	puts("- W: Gira a peca no sentido antiorario;");
    puts("- A: Movimenta para a Esquerda;");
    puts("- S: Aumenta a velocidade;");
    puts("- D: Movimenta para a Direita.");
}

#endif // GOTOYX_H_INCLUDED
