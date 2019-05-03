/*
 * gui.c
 *
 * Created: 02/05/2019 11:23:00
 *  Author: gmonteiro42
 */ 
#include "gui.h"
#include "asf.h"
#include "icones/door_open.h"

void draw_tela_inicial_logos(tImage laundry , tImage rpm, tImage bubble){
	
	ili9488_draw_pixmap(20,
	20,
	64,
	64,
	laundry.data);

	ili9488_draw_pixmap(20,
	128,
	64,
	64,
	rpm.data);

	ili9488_draw_pixmap(20,
	240,
	64,
	64,
	bubble.data);
}

void draw_ciclo(t_ciclo *p_ciclo,char buffer[32]){
	

	ili9488_set_foreground_color(COLOR_CONVERT(COLOR_WHITE));
	ili9488_draw_filled_rectangle(272,260,400,320);
	ili9488_draw_filled_rectangle(100,40,130,70);
	ili9488_draw_filled_rectangle(100,140,170,170);
	ili9488_draw_filled_rectangle(100,260,145,290);
	ili9488_draw_filled_rectangle(312,150,376,214);
	


	ili9488_set_foreground_color(COLOR_CONVERT(COLOR_BLACK));
	
	ili9488_draw_pixmap(312,
	150,
	64,
	64,
	p_ciclo->image->data);

	ili9488_draw_string(272,260,p_ciclo->nome);
	sprintf(buffer,"%d",p_ciclo->enxagueQnt);
	ili9488_draw_string(100,40,buffer);
	sprintf(buffer,"%d",p_ciclo->centrifugacaoRPM);
	ili9488_draw_string(100,140,buffer);

	if (p_ciclo->bubblesOn){
		ili9488_draw_string(100,260,"ON");
		}else{
		ili9488_draw_string(100,260,"OFF");
		
	}
	
}

void draw_porta_warning(){
	
	ili9488_draw_pixmap(340,
	0,
	64,
	64,
	door_open.data);
	ili9488_set_foreground_color(COLOR_CONVERT(COLOR_BLACK));
	ili9488_draw_string(300,72,"PORTA ABERTA");
}