/*
 * tipos.h
 *
 * Created: 02/05/2019 11:46:57
 *  Author: gmonteiro42
 */ 


#ifndef TIPOS_H_
#define TIPOS_H_

#include "asf.h"

// Image struct
typedef struct {
	const uint8_t *data;
	uint16_t width;
	uint16_t height;
	uint8_t dataSize;
} tImage;

typedef struct ciclo t_ciclo;

struct ciclo{
	char nome[32];           // nome do ciclo, para ser exibido
	int  enxagueTempo;       // tempo que fica em cada enxague
	int  enxagueQnt;         // quantidade de enxagues
	int  centrifugacaoRPM;   // velocidade da centrifugacao
	int  centrifugacaoTempo; // tempo que centrifuga
	char heavy;              // modo pesado de lavagem
	char bubblesOn;          // smart bubbles on (???)
	tImage *image;           // imagem do cilco
	t_ciclo *previous;
	t_ciclo *next;
};



#endif /* TIPOS_H_ */