#include <stdio.h>
#include <stdlib.h>
#include <windows.h>//Funcões de sistema

FILE *file;

int main(void)//apenas chama a função menu
{
    keybd_event ( VK_MENU, 0x36, 0, 0 );
    keybd_event ( VK_RETURN, 0x1C, 0, 0 );
    keybd_event ( VK_RETURN, 0x1C, KEYEVENTF_KEYUP, 0 );
    keybd_event ( VK_MENU, 0x38, KEYEVENTF_KEYUP, 0 );//comandos de systema para deixar Full screen//Não funciona no windows 7 '-'

    file = fopen("ranking.txt","a");// cria um arquivo de texto
    if (file == NULL) //Teste se possivel abrir o arquivo
	{
       printf("Houve um erro ao abrir o arquivo.\n");
       return 1;
    }
    fclose(file);// fecha o mesmo para nÃ£o dar conflito nas outras função
    Menu();
    return 0;
}
