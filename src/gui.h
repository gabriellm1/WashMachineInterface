
#ifndef GUI_H
#define GUI_H

#include "tipos.h"

void draw_tela_inicial_logos(tImage laundry , tImage rpm, tImage bubble);
void draw_ciclo(t_ciclo *p_ciclo,char buffer[32]);
void clear_for_lavagem();
void draw_lavagem(int tempo);
void draw_porta_warning();


#endif