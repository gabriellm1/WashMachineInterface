/*
	Wash Machine Interface by Gabriel Monteiro and Hugo Carl

	APS2- Computa??o Embarcada - Insper 2019

	S?o Paulo - Brasil

*/


// Board configuration
#include <asf.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "conf_board.h"
#include "conf_example.h"
#include "conf_uart_serial.h"

#include "tipos.h"

// Images includes
#include "icones/next.h"
#include "icones/back.h"
#include "icones/locked.h"
#include "icones/unlocked.h"
#include "icones/play_butt.h"
#include "icones/para.h"
#include "icones/laundry.h"
#include "icones/rpm.h"
#include "icones/bubble.h"

// Cicles include
#include "maquina1.h"

//GUI
#include "gui.h"

// Example default configuration
#define MAX_ENTRIES        3
#define STRING_LENGTH     70
#define USART_TX_MAX_LENGTH     0xff

// Door sensor definition
#define BUT_PIO      PIOA
#define BUT_PIO_ID   ID_PIOA
#define BUT_IDX  11
#define BUT_IDX_MASK (1 << BUT_IDX)

// Returned status when user simple touches
#define TOUCHED_STATUS 32

// LCD ili9488 struct prototype
struct ili9488_opt_t g_ili9488_display_opt;

// Cicle prototype
t_ciclo *p_ciclo;

// Touch button struct
struct botao {
	uint16_t x;
	uint16_t y;
	uint16_t size_x;
	uint16_t size_y;
	tImage *image;
	void (*p_handler)(void);
};

// Default variables
int tempo;
char buffer[32];


// Flags
volatile Bool lavando;
volatile Bool blocked;
volatile Bool f_rtt_alarme = false;

// Callbacks
void prox_callback(void){
	p_ciclo = p_ciclo->next;

}
void ant_callback(void){
	p_ciclo = p_ciclo->previous;
}

void lock_callback(void){
	printf("entrou callback");
	if (blocked){
		printf("true pra false\n");
		blocked = false;
	} else{
		printf("false pra true\n");
		blocked = true;
	}
}

void inicia_lavagem();
void play_callback(void){
	lavando = true;
	inicia_lavagem();
}

void tela_inicial();
void para_callback(void){
	lavando = false;
	f_rtt_alarme = false;
	tela_inicial();


}


// Touch buttons declaration
struct botao prox = {.x=416, .y=150, .size_x = 64, .size_y = 64,.p_handler=prox_callback, .image=&next};

struct botao ant = {.x=200, .y=150, .size_x = 64, .size_y = 64, .p_handler=ant_callback, .image=&back};

struct botao lock = {.x=400,.y=0, .size_x = 64, .size_y = 64,.p_handler=lock_callback,.image=&unlocked};

struct botao play = {.x = 410,.y=240, .size_x = 64, .size_y = 64,.p_handler=play_callback,.image=&play_butt};

struct botao cancela = {.x=200, .y=180, .size_x = 64, .size_y = 64, .p_handler=para_callback, .image=&para};

// Function to process touch of the button
int processa_touch(struct botao b[], struct botao *rtn, uint N ,uint x, uint y ){
	for (int i=0; i<N; i++){
		if (((x >= b->x) && (x <= b->x + b->size_x)) && ((y >= b->y) && (y <= b->y + b->size_y))){
			*rtn = *b;
			return 1;
		}
		b++;
	}
	return 0;
}

// LCD configuration
static void configure_lcd(void){
	/* Initialize display parameter */
	g_ili9488_display_opt.ul_width = ILI9488_LCD_WIDTH;
	g_ili9488_display_opt.ul_height = ILI9488_LCD_HEIGHT;
	g_ili9488_display_opt.foreground_color = COLOR_CONVERT(COLOR_WHITE);
	g_ili9488_display_opt.background_color = COLOR_CONVERT(COLOR_WHITE);

	/* Initialize LCD */
	ili9488_init(&g_ili9488_display_opt);


}

// Convertion of axis
uint32_t convert_axis_system_x(uint32_t touch_y) {
	// entrada: 4096 - 0 (sistema de coordenadas atual)
	return ILI9488_LCD_WIDTH - ILI9488_LCD_WIDTH*touch_y/4096;
}

uint32_t convert_axis_system_y(uint32_t touch_x) {
	// entrada: 0 - 4096 (sistema de coordenadas atual)
	return ILI9488_LCD_HEIGHT - ILI9488_LCD_HEIGHT*touch_x/4096;
}

/**
* \brief Set maXTouch configuration
*
* This function writes a set of predefined, optimal maXTouch configuration data
* to the maXTouch Xplained Pro.
*
* \param device Pointer to mxt_device struct
*/
static void mxt_init(struct mxt_device *device)
{
	enum status_code status;

	/* T8 configuration object data */
	uint8_t t8_object[] = {
		0x0d, 0x00, 0x05, 0x0a, 0x4b, 0x00, 0x00,
		0x00, 0x32, 0x19
	};

	/* T9 configuration object data */
	uint8_t t9_object[] = {
		0x8B, 0x00, 0x00, 0x0E, 0x08, 0x00, 0x80,
		0x32, 0x05, 0x02, 0x0A, 0x03, 0x03, 0x20,
		0x02, 0x0F, 0x0F, 0x0A, 0x00, 0x00, 0x00,
		0x00, 0x18, 0x18, 0x20, 0x20, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x02,
		0x02
	};

	/* T46 configuration object data */
	uint8_t t46_object[] = {
		0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x03,
		0x00, 0x00
	};

	/* T56 configuration object data */
	uint8_t t56_object[] = {
		0x02, 0x00, 0x01, 0x18, 0x1E, 0x1E, 0x1E,
		0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E,
		0x1E, 0x1E, 0x1E, 0x1E, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00
	};

	/* TWI configuration */
	twihs_master_options_t twi_opt = {
		.speed = MXT_TWI_SPEED,
		.chip  = MAXTOUCH_TWI_ADDRESS,
	};

	status = (enum status_code)twihs_master_setup(MAXTOUCH_TWI_INTERFACE, &twi_opt);
	Assert(status == STATUS_OK);

	/* Initialize the maXTouch device */
	status = mxt_init_device(device, MAXTOUCH_TWI_INTERFACE,
	MAXTOUCH_TWI_ADDRESS, MAXTOUCH_XPRO_CHG_PIO);
	Assert(status == STATUS_OK);

	/* Issue soft reset of maXTouch device by writing a non-zero value to
	* the reset register */
	mxt_write_config_reg(device, mxt_get_object_address(device,
	MXT_GEN_COMMANDPROCESSOR_T6, 0)
	+ MXT_GEN_COMMANDPROCESSOR_RESET, 0x01);

	/* Wait for the reset of the device to complete */
	delay_ms(MXT_RESET_TIME);

	/* Write data to configuration registers in T7 configuration object */
	mxt_write_config_reg(device, mxt_get_object_address(device,
	MXT_GEN_POWERCONFIG_T7, 0) + 0, 0x20);
	mxt_write_config_reg(device, mxt_get_object_address(device,
	MXT_GEN_POWERCONFIG_T7, 0) + 1, 0x10);
	mxt_write_config_reg(device, mxt_get_object_address(device,
	MXT_GEN_POWERCONFIG_T7, 0) + 2, 0x4b);
	mxt_write_config_reg(device, mxt_get_object_address(device,
	MXT_GEN_POWERCONFIG_T7, 0) + 3, 0x84);

	/* Write predefined configuration data to configuration objects */
	mxt_write_config_object(device, mxt_get_object_address(device,
	MXT_GEN_ACQUISITIONCONFIG_T8, 0), &t8_object);
	mxt_write_config_object(device, mxt_get_object_address(device,
	MXT_TOUCH_MULTITOUCHSCREEN_T9, 0), &t9_object);
	mxt_write_config_object(device, mxt_get_object_address(device,
	MXT_SPT_CTE_CONFIGURATION_T46, 0), &t46_object);
	mxt_write_config_object(device, mxt_get_object_address(device,
	MXT_PROCI_SHIELDLESS_T56, 0), &t56_object);

	/* Issue recalibration command to maXTouch device by writing a non-zero
	* value to the calibrate register */
	mxt_write_config_reg(device, mxt_get_object_address(device,
	MXT_GEN_COMMANDPROCESSOR_T6, 0)
	+ MXT_GEN_COMMANDPROCESSOR_CALIBRATE, 0x01);
}
int touch_counter = 0;
void mxt_handler(struct mxt_device *device, struct botao botoes[], uint Nbotoes)
{
	/* USART tx buffer initialized to 0 */
	char tx_buf[STRING_LENGTH * MAX_ENTRIES] = {0};
	uint8_t i = 0; /* Iterator */

	/* Temporary touch event data struct */
	struct mxt_touch_event touch_event;

	/* Collect touch events and put the data in a string,
	* maximum 2 events at the time */
	do {
		/* Temporary buffer for each new touch event line */
		char buf[STRING_LENGTH];

		/* Read next next touch event in the queue, discard if read fails */
		if (mxt_read_touch_event(device, &touch_event) != STATUS_OK) {
			continue;
		}

		// eixos trocados (quando na vertical LCD)
		uint32_t conv_x = convert_axis_system_x(touch_event.x);
		uint32_t conv_y = convert_axis_system_y(touch_event.y);

		/* Format a new entry in the data string that will be sent over USART */
		//sprintf(buf, "X:%3d Y:%3d \n", conv_x, conv_y);

		/* -----------------------------------------------------*/
		struct botao bAtual;
		

		if (touch_event.status == TOUCHED_STATUS){
			if(processa_touch(botoes, &bAtual, Nbotoes, conv_x, conv_y) ){
				printf("touch_counter %d \n\n",touch_counter);
				if (!(bAtual.x == lock.x && bAtual.y == lock.y)){
					bAtual.p_handler();
				}else if((bAtual.x == lock.x && bAtual.y == lock.y)){
					if(touch_counter == 2){
						touch_counter = 0;
						bAtual.p_handler();
					}else{
						touch_counter ++;
					}
				}
			}
		}


		//update_screen(conv_x, conv_y);
		/* -----------------------------------------------------*/

		/* Add the new string to the string buffer */
		strcat(tx_buf, buf);
		i++;

		/* Check if there is still messages in the queue and
		* if we have reached the maximum numbers of events */
	} while ((mxt_is_message_pending(device)) & (i < MAX_ENTRIES));

	/* If there is any entries in the buffer, send them over USART */
	if (i > 0) {
		usart_serial_write_packet(USART_SERIAL_EXAMPLE, (uint8_t *)tx_buf, strlen(tx_buf));
	}
}

// RTT configuration
static void RTT_init(uint16_t pllPreScale, uint32_t IrqNPulses)
{
	uint32_t ul_previous_time;

	/* Configure RTT for a 1 second tick interrupt */
	rtt_sel_source(RTT, false);
	rtt_init(RTT, pllPreScale);

	ul_previous_time = rtt_read_timer_value(RTT);
	while (ul_previous_time == rtt_read_timer_value(RTT));

	rtt_write_alarm_time(RTT, IrqNPulses+ul_previous_time);

	/* Enable RTT interrupt */
	NVIC_DisableIRQ(RTT_IRQn);
	NVIC_ClearPendingIRQ(RTT_IRQn);
	NVIC_SetPriority(RTT_IRQn, 0);
	NVIC_EnableIRQ(RTT_IRQn);
	rtt_enable_interrupt(RTT, RTT_MR_ALMIEN);
}
void RTT_Handler(void)
{
	uint32_t ul_status;

	/* Get RTT status */
	ul_status = rtt_get_status(RTT);


	/* IRQ due to Alarm */
	if ((ul_status & RTT_SR_ALMS) == RTT_SR_ALMS) {
		//pin_toggle(LED_PIO, LED_IDX_MASK);    // AQUI
		f_rtt_alarme = true;                  // flag RTT alarme
	}
}

static float get_time_rtt(){
	uint ul_previous_time = rtt_read_timer_value(RTT);
}

// Door sensor initialization
porta_init(){
	pmc_enable_periph_clk(BUT_PIO_ID);
	// Configura PIO para lidar com o pino do bot?o como entrada
	// com pull-up
	pio_configure(BUT_PIO, PIO_INPUT, BUT_IDX_MASK, PIO_PULLUP);
	// Configura interrup??o no pino referente ao botao e associa
	// fun??o de callback caso uma interrup??o for gerada
	// a fun??o de callback ? a: but_callback()
	pio_set_debounce_filter(BUT_PIO,BUT_IDX_MASK,20);
}

// Start wash function
void inicia_lavagem(void){
	//testa a porta
	if(!pio_get(BUT_PIO,PIO_INPUT,BUT_IDX_MASK)){

		lavando = false;
	
		draw_porta_warning();

		delay_s(2);

		tela_inicial();
		}else{
			ili9488_set_foreground_color(COLOR_CONVERT(COLOR_WHITE));
			ili9488_draw_filled_rectangle(0,0,ILI9488_LCD_WIDTH,ILI9488_LCD_HEIGHT);

			
			ili9488_draw_pixmap(cancela.x,
			cancela.y,
			cancela.image->width,
			cancela.image->height,
			cancela.image->data);

			ili9488_draw_pixmap(lock.x,
			lock.y,
			lock.image->width,
			lock.image->height,
			lock.image->data);

			f_rtt_alarme = true;
			tempo = (p_ciclo->enxagueTempo*p_ciclo->enxagueQnt) +
			p_ciclo->centrifugacaoTempo ;

	}
}


// Initialize first cicle and menu order
t_ciclo *initMenuOrder(){
	c_rapido.previous = &c_centrifuga;
	c_rapido.next = &c_diario;

	c_diario.previous = &c_rapido;
	c_diario.next = &c_pesado;

	c_pesado.previous = &c_diario;
	c_pesado.next = &c_enxague;

	c_enxague.previous = &c_pesado;
	c_enxague.next = &c_centrifuga;

	c_centrifuga.previous = &c_enxague;
	c_centrifuga.next = &c_rapido;

	return(&c_diario);
}

// Main screen initialization
void tela_inicial(){
	
	p_ciclo = initMenuOrder();
	
	ili9488_set_foreground_color(COLOR_CONVERT(COLOR_WHITE));
	ili9488_draw_filled_rectangle(0,0,ILI9488_LCD_WIDTH,ILI9488_LCD_HEIGHT);

	draw_ciclo(p_ciclo,buffer);

	draw_tela_inicial_logos(laundry,rpm,bubble);


	ili9488_draw_pixmap(play.x,
	play.y,
	play.image->width,
	play.image->height,
	play.image->data);


	ili9488_draw_pixmap(prox.x,
	prox.y,
	prox.image->width,
	prox.image->height,
	prox.image->data);

	ili9488_draw_pixmap(ant.x,
	ant.y,
	ant.image->width,
	ant.image->height,
	ant.image->data);

}


int main(void)
{


	// Avaiable buttons struct vector
	struct botao botoes_inicial[] = {lock,ant, prox,play};

	struct botao botoes_lavando[] = {lock,cancela};



	struct mxt_device device; /* Device data container */

	/* Initialize the USART configuration struct */
	const usart_serial_options_t usart_serial_options = {
		.baudrate     = USART_SERIAL_EXAMPLE_BAUDRATE,
		.charlength   = USART_SERIAL_CHAR_LENGTH,
		.paritytype   = USART_SERIAL_PARITY,
		.stopbits     = USART_SERIAL_STOP_BIT
	};

	sysclk_init(); /* Initialize system clocks */
	board_init();  /* Initialize board */
	configure_lcd();

	//draw_button(0);
	/* Initialize the mXT touch device */
	mxt_init(&device);

	/* Initialize stdio on USART */
	stdio_serial_init(USART_SERIAL_EXAMPLE, &usart_serial_options);

	tela_inicial();

	char temp[32];
	sprintf(temp,"%d",0);

	porta_init();


	while (true) {
		if (blocked){
					lock.image = &locked;
				}else{
					lock.image = &unlocked;
				}
					ili9488_draw_pixmap(lock.x,
					lock.y,
					lock.image->width,
					lock.image->height,
					lock.image->data);
		/* Check for any pending messages and run message handler if any
		* message is found in the queue */
		if (mxt_is_message_pending(&device)) {
			if (!lavando && !blocked){
				mxt_handler(&device, botoes_inicial, 4);

			draw_ciclo(p_ciclo,buffer);

			}else if(lavando && !blocked){
				mxt_handler(&device, botoes_lavando, 2);

			} else{
				mxt_handler(&device, botoes_lavando, 1);
			}
		}

		// RTT usage
		if (f_rtt_alarme && lavando){

			ili9488_set_foreground_color(COLOR_CONVERT(COLOR_WHITE));
			ili9488_draw_filled_rectangle(272,260,400,320);
			ili9488_draw_filled_rectangle(100,40,130,70);
			ili9488_draw_filled_rectangle(100,140,170,170);
			ili9488_draw_filled_rectangle(100,260,145,290);

			uint16_t pllPreScale = (int) (((float) 32768) / 1.0);
			uint32_t irqRTTvalue  = 60;

			// reboot RTT to generate new IRQ
			RTT_init(pllPreScale, irqRTTvalue);


			/*
			* CLEAR FLAG
			*/
			f_rtt_alarme = false;


			ili9488_set_foreground_color(COLOR_CONVERT(COLOR_WHITE));

			ili9488_draw_string(230, 120, temp);

			ili9488_set_foreground_color(COLOR_CONVERT(COLOR_BLACK));
			sprintf(temp,"%d",tempo);
			ili9488_draw_string(230, 120, temp);

			uint8_t palavra[256];
			sprintf(palavra,"Faltam :");
			ili9488_draw_string(130, 120, palavra);

			uint8_t min[256];
			sprintf(min,"minutos");
			ili9488_draw_string(280, 120, min);

			tempo-=1;
		}

	}

	return 0;
}
