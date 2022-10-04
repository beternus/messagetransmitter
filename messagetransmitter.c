#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <string.h>
#include <conio2.h>

//configurações de envio pela porta serial:
                
HANDLE AbreComm(char *nomecom, int baudrate)
{
    HANDLE hcom; 	//cria um ponteiro de nome hcom
    DCB cdcb;    	//nomeia a estrutura DCB (Device Control Block)
                		 //utilizada para definir todos os parâmetros da comunicação
    COMMTIMEOUTS comto; //nomeia a estrutura COMMTIMEOUTS (COMMon TIME OUTS)
                       	 //utilizada para definir os timeouts da comunicação
    hcom = CreateFile(
           nomecom, 	//nome do arquivo 
           GENERIC_READ | GENERIC_WRITE, //abre arquivo para leitura ou escrita.
           0, 		//indica que o arquivo não pode ser compartilhado
           NULL,	 //utiliza a estrutura default para as funções de segurança
           OPEN_EXISTING, //abre o arquivo, se não existir, retorna erro
           FILE_ATTRIBUTE_NORMAL, //o arquivo deve ser utilizado sozinho
           NULL); 	//sem gabarito de atributos
    
    if(hcom == INVALID_HANDLE_VALUE) 	// testa falha na abertura do arquivo
    {
      fprintf(stderr, "Nao abriu a %s\n", nomecom);
      return hcom;
    }
    
    GetCommState(hcom, &cdcb);	 //lê os parâmetros de comunicação atuais
    cdcb.BaudRate    = baudrate; 	//define a taxa de transmissão
    cdcb.ByteSize    = 8;		 //define o tamanho do dado - 8 bits
    cdcb.StopBits    = ONESTOPBIT; //define o tamanho do stop bit - 1 stop bit
    cdcb.Parity      = NOPARITY; 	//define o tipo de paridade - sem paridade
    
    if(!SetCommState(hcom,&cdcb)) 	//seta os novos parâmetros de comunicação
    {
      fputs("SetCommState", stderr);
      return INVALID_HANDLE_VALUE;
    }
    GetCommTimeouts(hcom, &comto); //Le os parâmetros atuais de COMMTIMEOUTS
    comto.ReadIntervalTimeout         = MAXDWORD; //tempo máximo entre a chegada 
                                                		             //de dois caracteres consecutivos(ms)
    comto.ReadTotalTimeoutMultiplier  =    0;
    comto.ReadTotalTimeoutConstant    =    0;
    comto.WriteTotalTimeoutMultiplier =    0;
    comto.WriteTotalTimeoutConstant   =    0;
    SetCommTimeouts(hcom, &comto); 	//seta os parâmetros de COMMTIMEOUTS
    return hcom;
}
//fim das configurações de envio pela porta serial

//funções de utilização do programa:
void tela(char *nome, int taxa){	//função que escreve a tela inicial
textcolor(15);	//branco brilhante
printf("\n\t\t! Programa de Transmissao de Mensagens !\n");		//título do programa
printf("________________________________________________________________________________");	//linha
textcolor(7);		//cinza
printf("\n Digite a mensagem:\t(maximo 16 caracteres)\n");
gotoxy(1,8);
textcolor(15);	//branco brilhante
printf("________________________________________________________________________________");	//linha.
gotoxy(1,10);
textcolor(7);		//cinza
printf(" Enviado:\n");
textcolor(15);	//branco brilhante
printf("________________________________________________________________________________");	//linha
textcolor(7);		//cinza
gotoxy(1,23);
printf("________________________________________________________________________________");	//linha.
gotoxy(1,24);
printf("Terminal %s %dbaud\t\t\t\t\t      Ctrl+C para sair", nome, taxa);	//indica a porta, a taxa de envio e a tecla de finalização
gotoxy(18,7);	//vai para a posição 17 na leitura da string
textcolor(10);	//amarelo
printf("]");		//coloca um ] para indicar que não é mais permitido escrever após aquela posição
textcolor(7);		//cinza
gotoxy(2,7);		//vai para a posição na qual será escrita a mensagem
}

void leitura(char *p,int c,int l){	//função de leitura da string
gets(p);		//lê a string a ser enviada
gotoxy(c,l);		//vai para a posição onde escreve a mensagem já enviada
printf(" %s",p);	//escreve a última string enviada
}

void limpa1(void){	//função que limpa a coluna esquerda (da linha 12 a 22)
int t=12;		//vai para a linha 12
     while(t<23){	//enquanto linha é menor que 23
     gotoxy(1,t);	//vai para a coluna 1 e para a linha t
     printf("                     ");	//coloca uma linha em branco sobre o texto escrito.
     t++;	//incrementa a linha t
     }
}

void limpa2(void){	//função que limpa a coluna direita
int t=12;		//vai para a linha 12
     while(t<23){	//enquanto linha é menor que 23
     gotoxy(40,t);	//vai para a coluna 40 e para a linha t
     printf("                     ");	//coloca uma linha em branco sobre o texto escrito.
     t++;		//incrmenta a linha t
     }
}

int main(void)	//////////////////////////////MAIN//////////////////////////////
{
    HANDLE hcom; 	//cria o ponteiro, área de memória
    unsigned long n;
    char *ncom="COM1"; 	//define nome do arquivo, no nosso caso a porta usada
    int baud=2400;	 //define a taxa de transmissão
    char dado[1]; 	//cria o buffer
    hcom=AbreComm(ncom,baud);	 //abre o aquivo e relaciona com ponteiro
    if(hcom==INVALID_HANDLE_VALUE) {getch();return 1;} 	//se não abriu arquivo/porta,
    int c=1,i=0,;	//variável c e i , tipo inteiro
    char a[16];	//vetor que arazena a string digitada pelo usuário
    tela(ncom,baud);	//chama a função que escreve a tela inicial
    int l=12;		//variável linha, tipo inteiro
    while(1)		//sempre
    {     
        while(l<22)	//enquanto linha é menor que 22
        {
            l++;		//incrementa linha
            leitura(a,c,l);//chamada da função de leitura
            *dado='u';	//envio de 3 u
            WriteFile(hcom,dado, 1, &n, NULL);	//escreve conteúdo do buffer na porta serial
            WriteFile(hcom,dado, 1, &n, NULL);	//escreve conteúdo do buffer na porta serial
            WriteFile(hcom,dado, 1, &n, NULL);	//escreve conteúdo do buffer na porta serial
            *dado='A';	//envio de um A (para ser reconhecido pelo PIC)
            WriteFile(hcom,dado, 1, &n, NULL);	//escreve conteúdo do buffer na porta serial
            *dado='z';	//envio de em z (para ser reconhecido pelo PIC)
            WriteFile(hcom,dado, 1, &n, NULL);	//escreve conteúdo do buffer na porta serial
            i=0;			//vai para a posição zero do vetor    
            while(a[i]) 		//enquanto a string não terminar
            {        
          	   *dado=a[i];	//coloca a posição da string em dado
               WriteFile(hcom,dado, 1, &n, NULL); //escreve conteúdo do buffer na porta serial
               i++;		//incrementa a posição da string a
            }
            gotoxy(2,7);		//vai para a posição da string escrita e enviada
            textcolor(10);	//amarelo
            printf("                ]\t\t");//apaga a string anteriormente escrita e enviada
            textcolor(7);		//cinza
            gotoxy(2,7);		//retorna à posição inicial
        }
        l=12;
        switch(c){		//c
           case 1:		//se c=1
                c=40;		//vai para c=40.
                limpa2();		//chama a função de limpeza da coluna
                break;
           case 40:		//se c=40 
                c=1;		//vai para c=1
                limpa1();		//chama a função de limpeza da coluna
                break;
        }
        gotoxy(2,7);		//retorna a posição de leitura da string
    }
}				//fim do Programa

3.2 Sistema de recepção e exibição de mensagens

Software do programa de recepção de mensagens, desenvolvido do MPLAB v8.60, executado pelo microcontrolador PIC 16F690:

#include<htc.h>		//inclui as bibliotecas necessárias
#include <lcd.h>
#include <stdio.h>
//***CONFIGURAÇÃO PADRÃO:***
__CONFIG(WDTDIS & MCLRDIS & INTIO & PWRTEN);	

#ifndef _XTAL_FREQ
#define _XTAL_FREQ   4000000
#endif
#define LED RA0

char BYTE_RX,conta;
char nB=0, vetor[17];	//O vetor grava a string recebida do transmissor

void putch(char x){
	lcd_putch(x);
}

void interrupt serial(void)
{
	if(RCIF==1)			//quando ocorre uma interrupção RCIF é setado automaticamente, pois é a flag de interrupções
	{
	   RCIF=0;			//entao ele é zerado
	   if(FERR==1||OERR==1) {	//testa para ver se não ocorreu erro de overfloaw, ou outro erro
	        SPEN=0;			//desabilita essa interrupção
	        SPEN=1;
	   }
	   else    BYTE_RX=RCREG;	//se não tem erro, o bit recebido é atribuído ao BYTE_RX	
	}
}   

void reconhecimento_sinal(char conta)	//Quando o pic receber 3 u, 1 A e um z ele vai reconhecer o transmissor,
{                                          		//só depois disso ele vai começar a considerar as informações.
	switch(conta)                          	//O transmissor sempre manda uuuAz antes de qualquer nova mensagem.
	{                                      	 //O transmissor manda  as strings usando um while que incrementa a posição da string,
		case 0:                     	//Cada caracter é enviado, taxa de 2400 bits/s.
		if(BYTE_RX=='A') conta=1;
			else
			break;
		case 1:
			if(BYTE_RX=='z') conta=2;
			else conta=0;
			break;
		case 2:
			if(BYTE_RX=='\0') conta=3;	//Coloca cada caracter recebido no vetor , incrementando a posição.
			else{
			vetor[nB]=BYTE_RX;
			nB++;
			}
			break;
		case 3:
			nB=0;
			break;
	}
}
//manipular dado, no fim colocar conta=0

void main(void){

	SPBRGH=0;
	SPBRG=103;		//SELECIONA A VELOCIDADE DE RECEPÇÃO PARA 2400 BITS/S
	SYNC=0;		//MODO ASSÍNCRONO
	BRGH=1;
	BRG16=0;
	
	SPEN=1;		//HABILITA A PORTA SERIAL

	TXEN=0;		//HABILITA A TRANSMISSÃO
	TXIE=0;		//DESABILITA A INTERRUPÇÃO POR TRANSMISSÃO

	//CONFIGURAÇÕES PARA RECEPÇÃO
	RCIE=1;		//HABILITA A INTERRUPÇÃO POR RECEPÇÃO SERIAL
	PEIE=1;
	GIE=1;
	RX9=0;			//MODO 8 BITS
	CREN=1;


   ANSEL=0;
   ANSELH=0;
   TRISA=0;
   TRISB=0b00100000;	//ENTRADA RB5 - RX
   TRISC=0;
   	
lcd_init();		//PROCEDIMENTOS DE INICIALIZAÇÃO DO LCD
lcd_goto(0);		//SELECIONA A PRIMEIRA LINHA	
	lcd_puts("STRING RECEBIDA:");



 while(1)
	{
		   reconhecimento_sinal(conta);		//CHAMA FUNÇÃO PARA MANDAR CARACTERES ANTES DA MENSAGEM
		   if(conta==3)				//QUANDO A RAJADA DE CARACTERES CHEGAR AO TERCEIRO ESTÁGIO, INICIA A PRÓXIMA CONDIÇÃO
		{
			   if   (RCREG==1)		//CASO ALGUM DADO SEJA RECEBIDO:
		      	{
				LED=1;				//SETA O LED DE INDICAÇÃO DE RECEPÇÃO
				lcd_goto(0x40);			// SELECIONA A SEGUNDA LINHA
				printf("%c",vetor[16]); 		//ESCREVE STRING RECEBIDA
	      		}else LED=0;  				//SE NENHUM DADO FOI RECEBIDO, O LED DE INDICAÇÃO DE RECEPÇÃO É ZERADO
	 	}      
	}
}
