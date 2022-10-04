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
