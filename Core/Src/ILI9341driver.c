/*
 * tftdriver.c
 * driver para
 *  Created on: 20 abr. 2020
 *  Author: Jesús F. García Bruña.
 */

#include "main.h"
#include "ILI9341driver.h"

// ________________________________ Constantes _______________________________
static const int16_t lcd_width = 240;
static const int16_t lcd_height = 320;
static const int16_t lcd_x_max = 239;
static const int16_t lcd_y_max = 319;


// Configuración de las E/S relacionadas con el LCD
//
static GPIO_TypeDef * CS_GPIO_Port = lcd_CS_GPIO_Port;
static GPIO_TypeDef * RS_GPIO_Port = lcd_RS_GPIO_Port;
static GPIO_TypeDef * WR_GPIO_Port = lcd_WR_GPIO_Port;
static GPIO_TypeDef * RD_GPIO_Port = lcd_RD_GPIO_Port;
static GPIO_TypeDef * RST_GPIO_Port = lcd_RST_GPIO_Port;
static GPIO_TypeDef * DATA_GPIO_Port = GPIOA;

static uint16_t	CS_Pin  = lcd_CS_Pin;
static uint16_t	RS_Pin  = lcd_RS_Pin;
static uint16_t	WR_Pin  = lcd_WR_Pin;
static uint16_t	RD_Pin  = lcd_RD_Pin;
static uint16_t	RST_Pin = lcd_RST_Pin;



// __________________ Propiedades privadas de ILI9341_driver ___________________
//
// Área de dibujo activa:
volatile int16_t draw_area_x0 = 0;
volatile int16_t draw_area_y0 = 0;
volatile int16_t draw_area_x1 = 0;
volatile int16_t draw_area_y1 = 0;
volatile uint32_t draw_area_npixels = 0;

volatile static uint16_t background_color = ILI9341_ORANGE;
volatile static uint16_t foreground_color = ILI9341_BLACK;

// Macros
//
#define CS_IDLE	   CS_GPIO_Port->BSRR = CS_Pin;
#define CS_ACTIVE  CS_GPIO_Port->BRR = CS_Pin;

#define RS_IDLE	   RS_GPIO_Port->BSRR = RS_Pin;
#define RS_DATA	   RS_GPIO_Port->BSRR = RS_Pin;
#define CD_DATA    RS_GPIO_Port->BSRR = RS_Pin;
#define RS_ACTIVE  RS_GPIO_Port->BRR = RS_Pin;
#define RS_COMMAND RS_GPIO_Port->BRR = RS_Pin;
#define CD_COMMAND RS_GPIO_Port->BRR = RS_Pin;

#define WR_IDLE	   WR_GPIO_Port->BSRR = WR_Pin;
#define WR_ACTIVE  WR_GPIO_Port->BRR = WR_Pin;
#define WR_STROBE	{WR_ACTIVE; WR_IDLE;}

#define RD_IDLE	   RD_GPIO_Port->BSRR = RD_Pin;
#define RD_ACTIVE  RD_GPIO_Port->BRR = RD_Pin;
#define RD_STROBE  {RD_ACTIVE; RD_IDLE;}

#define RST_IDLE   RST_GPIO_Port->BSRR = RST_Pin;
#define RST_ACTIVE RST_GPIO_Port->BRR = RST_Pin;

// Pone un byte en el Bus de datos del LCD.
#define LCD_PUT_DATA(b) DATA_GPIO_Port->BSRR = (uint32_t) ((uint8_t) b) | ((uint8_t) ~b) << 16;

// Escribe un byte en el LCD, Este se tratará como datoo comando en función
// del estado de la señal RS, alias CD el cual habrá sido establecio
// previamente al valor requerido.
#define INLINE_LCD_WRITE(b)	\
{							\
	WR_ACTIVE;				\
	LCD_PUT_DATA(b);		\
	WR_IDLE;				\
}

// Declaraciones privadas.

void lcd_setAddrWindow(int16_t x0, int16_t y0, int16_t x1, int16_t y1);
void lcd_set_draw_area(int16_t x0, int16_t y0, int16_t x1, int16_t y1);
void lcd_flood_area(uint16_t color);

/**
 * Reinicia el LCD por hardware.
 */
void lcd_reset(void)
{
	// Todas las señales de control en reposo:
	RST_IDLE;
	CS_IDLE;
	RS_IDLE;
	WR_IDLE;
	RD_IDLE;
	HAL_Delay(10);

	RST_ACTIVE;
	HAL_Delay(10);
	RST_IDLE;

	// Data transfer sync
	CS_ACTIVE;
	CD_COMMAND;
	LCD_PUT_DATA(0x00);
	for (int i = 0; i != 3; i++)
		WR_STROBE;

	CS_IDLE;

}

/**
 * Reconfigura el Bus de datos para escribir sobre el LCD.
 */
void lcd_set_write_dir()
{

}

/**
 * Reconfigura el Bus de datos para leer desde el LCD.
 */
void lcd_set_read_dir()
{

}


/**
 * Envía un byte al LCD, sin especificar si se trata de un dato o de
 * un comando.
 */
void lcd_write(uint8_t c)
{
	//CS_ACTIVE;
	WR_ACTIVE;
	LCD_PUT_DATA(c);
	WR_IDLE;
}

/**
 * Envía un comando al LCD
 */
void lcd_write_command(uint8_t cmd)
{
	CD_COMMAND;
	INLINE_LCD_WRITE(cmd)
}

/**
 * Envía un dato al LCD
 */
void lcd_write_data(uint8_t data)
{
	CD_DATA;
	INLINE_LCD_WRITE(data)
}


void lcd_startup(void)
{

	// Reset Hardware:
	lcd_reset();
	HAL_Delay(200);

	CS_ACTIVE;
	// writeRegister8(ILI9341_SOFTRESET, 0);
	lcd_write_command(ILI9341_SOFTRESET);
	lcd_write_data(0);
	HAL_Delay(50);

	// writeRegister8(ILI9341_DISPLAYOFF, 0);
	lcd_write_command(ILI9341_DISPLAYOFF);
	lcd_write_data(0);

	//writeRegister8(ILI9341_POWERCONTROL1, 0x23);
	lcd_write_command(ILI9341_POWERCONTROL1);
	lcd_write_data(0x23);

	//writeRegister8(ILI9341_POWERCONTROL2, 0x10);
	lcd_write_command(ILI9341_POWERCONTROL2);
	lcd_write_data(0x10);

	//writeRegister16(ILI9341_VCOMCONTROL1, 0x2B2B);
	lcd_write_command(ILI9341_VCOMCONTROL1);
	lcd_write_data(0x2B);
	lcd_write_data(0x2B);

	//writeRegister8(ILI9341_VCOMCONTROL2, 0xC0);
	lcd_write_command(ILI9341_VCOMCONTROL2);
	lcd_write_data(0xC0);

	//writeRegister8(ILI9341_MEMCONTROL, ILI9341_MADCTL_MY | ILI9341_MADCTL_BGR);
	// Se establece la rotación para situar la coodenada 0,0 en la esquina
	// superior izquierda, con el display en vertical, los iconos impresos en la
	// parte inferior y el alojamiento SD en la zona superior.
	lcd_write_command(ILI9341_MEMCONTROL);
	lcd_write_data( ILI9341_MADCTL_BGR | ILI9341_MADCTL_MX );

	//writeRegister8(ILI9341_PIXELFORMAT, 0x55);
	lcd_write_command(ILI9341_PIXELFORMAT);
	lcd_write_data(0x55);

	//writeRegister16(ILI9341_FRAMECONTROL, 0x001B);
	lcd_write_command(ILI9341_FRAMECONTROL);
	lcd_write_data(0x00);
	lcd_write_data(0x1B);

	//writeRegister8(ILI9341_ENTRYMODE, 0x07);
	lcd_write_command(ILI9341_ENTRYMODE);
	lcd_write_data(0x07);

	// Es necesario invertir los colores para obtener el resultado
	// requerido.
	lcd_write_command(0x21); //inversion.
	lcd_write_data(0x00);

	//writeRegister8(ILI9341_SLEEPOUT, 0);
	lcd_write_command(ILI9341_SLEEPOUT);
	lcd_write_data(0x00);

	HAL_Delay(150);

/*lcd_write_command(ILI9341_DISPLAY_CTRL);
	lcd_write_data(4); */


	//writeRegister8(ILI9341_DISPLAYON, 0);
	lcd_write_command(ILI9341_DISPLAYON);
	lcd_write_data(0x00);
	HAL_Delay(500);

/*lcd_write_command(ILI9341_BRIGHTNESS_SET);
	lcd_write_data(0x10);*/

	//CS_IDLE; Ya lo liberará la siguiente función...

	//setAddrWindow(0, 0, TFTWIDTH - 1, TFTHEIGHT - 1);
	lcd_set_draw_area(0, 0, lcd_x_max, lcd_y_max);
	// Se rellena la pantalla con el color de fondo
	lcd_flood_area(background_color);

	return;

}

/**
 * Selecciona la ventana de direcciones de memoria, sobre la que operarán las
 * funciones deescritura y lectura de memoria de lLCD
 *
 */
void lcd_setAddrWindow(int16_t x0, int16_t y0, int16_t x1, int16_t y1) {

	CS_ACTIVE;
	CD_COMMAND;
	INLINE_LCD_WRITE(ILI9341_COLADDRSET);
	CD_DATA
	INLINE_LCD_WRITE((x0 >> 8));
	INLINE_LCD_WRITE(((uint8_t) x0));
	INLINE_LCD_WRITE((x1 >> 8));
	INLINE_LCD_WRITE(ILI9341_COLADDRSET);

	lcd_write_command(ILI9341_COLADDRSET);
	lcd_write_data((x0 >> 8));
	lcd_write_data((x0 & 0xFF));
	lcd_write_data((x1 >> 8));
	lcd_write_data((x1 & 0xFF));

	lcd_write_command(ILI9341_PAGEADDRSET);
	lcd_write_data((y0 >> 8));
	lcd_write_data((y0 & 0xFF));
	lcd_write_data((y1 >> 8));
	lcd_write_data((y1 & 0xFF));

	CS_IDLE;

}


/**
 * Selecciona el área de dibujo activa y calcula su área en pixels
 */
void lcd_set_draw_area(int16_t x0, int16_t y0, int16_t x1, int16_t y1) {

	// Se normalizan los datos de entrada:
	if(x1 > x0){
		draw_area_x0 = x0;
		draw_area_x1 = x1;
	}
	else{
		draw_area_x0 = x1;
		draw_area_x1 = x0;
	}
	if(draw_area_x0 < 0)
		draw_area_x0 = 0;
	if(draw_area_x1 > lcd_x_max)
		draw_area_x1 = lcd_x_max;

	if(y1 > y0){
		draw_area_y0 = y0;
		draw_area_y1 = y1;
	}
	else{
		draw_area_y0 = y1;
		draw_area_y1 = y0;
	}
	if(draw_area_y0 < 0)
		draw_area_y0 = 0;
	if(draw_area_y1 > lcd_y_max)
		draw_area_y1 = lcd_y_max;

	// Si el área seleccionada queda fuera de los límites físicos,
	// no se seleccionará ningún área.
	if(draw_area_x0 > lcd_x_max || draw_area_y0 > lcd_y_max){
		draw_area_x0 = 0;
		draw_area_x1 = 0;
		draw_area_y0 = 0;
		draw_area_y1 = 0;
		draw_area_npixels = 0;
		return;
	}

	// Se calcula el área en pixels
	draw_area_npixels = (1 + draw_area_y1 - draw_area_y0) * lcd_width;
	draw_area_npixels += 1 + draw_area_x1 - draw_area_x0;

	// Se selecciona la ventana de direcciones de memoria correspondiente al
	// área de dibujo activa.
	lcd_setAddrWindow(draw_area_x0, draw_area_y0, draw_area_x1, draw_area_y1);
}

/**
 * Inunda el área activa con el color indicado.
 */
void lcd_flood_area(uint16_t color)
{
	if (draw_area_npixels == 0)
		return;						// No hay nada que hacer.

	uint8_t color_msb =  color >> 8;
	uint8_t color_lsb =  color;
	uint32_t count = draw_area_npixels;

	CS_ACTIVE;
	CD_COMMAND;
	INLINE_LCD_WRITE(ILI9341_MEMORYWRITE);

	CD_DATA;
	if(color_msb == color_lsb) {
		LCD_PUT_DATA(color_lsb);
		while(count--) {
			WR_STROBE;
			WR_STROBE;
		}
	}
	else {
		while(count--) {
			INLINE_LCD_WRITE(color_msb);
			INLINE_LCD_WRITE(color_lsb);
		}
	}
	CS_IDLE;

}


// TODO
void lcd_fill_area(uint16_t *pattern, uint16_t pat_size)
{

}


/**
 * Dibuja un pixel del color indicado en la posición especificda.
 */
void lcd_draw_pixel(int16_t x, int16_t y, uint16_t color) {

	if( x < 0 || x > lcd_x_max || y < 0 || y > lcd_y_max)
		return;

	lcd_setAddrWindow(x, y, TFT_WIDTH - 1, TFT_HEIGHT - 1);
	CS_ACTIVE;
	CD_COMMAND;
	INLINE_LCD_WRITE(ILI9341_MEMORYWRITE);

	CD_DATA
	INLINE_LCD_WRITE((color >> 8));
	INLINE_LCD_WRITE((uint8_t) color);
	CS_IDLE;

}


/**
 * Dibuja una línea horizontal desde el punto x0, y0, en la dirección
 * determinada por el signo de length, positivo hacia la derecha, negativo
 * hacia la izquierda.
 * Además, limita la longitud para que la línea no se salga de la pantalla.
 */
void lcd_draw_hline(int16_t x0, int16_t y0, int16_t length, uint16_t color)
{
	length += length < 0 ? 1 : -1;

	lcd_set_draw_area(x0, y0, x0 + length+1, y0);
	lcd_flood_area(color);

}

/**
 * Dibuja una línea vertical desde el punto x0, y0, en la dirección
 * determinada por el signo de length, positivo hacia abajo, negativo
 * hacia arriba.
 * Además, limita la longitud para que la línea no se salga de la pantalla.
 */
void lcd_draw_vline(int16_t x0, int16_t y0, int16_t length, uint16_t color)
{
	length += length < 0 ? 1 : -1;
	lcd_set_draw_area(x0, y0, x0, y0 + length+1);
	lcd_flood_area(color);
}

/**
 * Dibuja una línea en con cualquier orientación.
 * Es una implementación del algoritmo de Breserham
 *
 * La línea se dibuja pixel a pixel en un bucle que recorre el eje que resulte mas favorable.
 * Para garantizar que todos los pixeles de la línea sean contiguos, interesa tomar como variable
 * independiente el eje que coincida con el de mayor recorrido de la línea a dibujar y calcular las
 * coordenadas del otro eje en función de este. Dicho de otro modo, interesa que la pendiente de
 * la recta a dibujar esté acotada en el intervalo [-1, 1]. Observa que si la pendiente de la recta
 * y = f(x), está fuera de este intervalo, entonces la pendiente de la recta x = f(y) estará
 * necesariamente en el intervalo requerido.
 * Para simplificar el resto de la explicación se expone la implementación para una recta y = f(x).
 * El primer punto de la recta (x,y) coincide exactamente con (x0, y0), el siguiente punto siempre
 * estará en xn+1 = xn + 1, la cuestión fundamental consisite en determinar como incrementar 'y'.
 * Si no modificamos y, entonces la recta incrementará su error en dE = dy/dx, es decir:
 * la pendiente de la recta por el número de pixels que se avanzan, esto es 1.
 * El error total representa la diferencia entre la cota de la recta real y la cota del pixel
 * utilizado.
 *
 * Si el error supera 1/2 pixel es decir si |error| >= 0.5, entonces se corrige la trayectoria de
 * la recta incrementando o decrementando la coordenada 'y' en el sentido de la pendiente,
 * y restamos o sumamos 1 al error total según hayamos incrementado o decrementado 'y'.
 *
 * Por ejemplo: sea que la pendiente de la recta es mayor que 0 y menor o igual a 1,
 * por tanto, cada vez que que se pinta un pixel en la misma línea que el precedente se añade error
 *
 *		dE = dy/dx
 *
 * 		error += dE --> error += dy/dx
 *
 * 		_si_ error > 0,5 _entonces:_
 * 			y += 1
 * 			error -= 1
 *
 * 	Para evitar divisiones y permanecer en el dominio de los enteros, multiplicamos
 * 	la expresión      (dE = dy/dx)·2dx --> 2dx·dE = 2dy
 *
 * 		dE = dy/dx -->  2dy = 2dx·dE y por tanto:
 *                      ------------
 * 		error += dE --> 2dx·error += 2dx·dE -->  2dx·error += 2dy;
 *                                               -----------------
 * 		_si_ 2dx·error > dx _entonces_
 * 			y += 1;
 * 			2dx·error -= 2dx;
 */
void lcd_draw_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
	// Coordenadas del punto de la recta en cada iteración.
	int16_t x = x0;
	int16_t y = y0;

	int16_t dx = x1 - x0;					// Recorrido en x
	int16_t dy = y1 - y0;					// Recorrido en y

	int16_t signo_dx = dx >= 0 ? 1 : -1;
	int16_t abs_dx = dx * signo_dx;

	int16_t signo_dy = dy >= 0 ? 1 : -1;
	int16_t abs_dy = dy * signo_dy;

	int16_t count = 1;						// Número de pixels de que consta la línea

	int16_t	acu_err = 0;					// Error acumulado en la variable dependiente
	int16_t inc_err;						// Error por cada iteración "horizontal" (respecto de la variable independiente)
	int16_t dec_err;						// Error por cada incremento de variable dependiente

	if(abs_dx >= abs_dy){					// y =f(x)
		inc_err = abs_dy + abs_dy;
		dec_err = abs_dx + abs_dx;
		count += abs_dx;
		do {
			// La linea estará formada al menos por el punto inicial
			lcd_draw_pixel(x, y, color);
			//x++;
			x += signo_dx;
			acu_err += inc_err;
			if(acu_err > abs_dx){
				acu_err -= dec_err;
				y += signo_dy;
			}
		} while(--count);
	}
	else {									// x = f(y)
		inc_err = abs_dx + abs_dx;
		dec_err = abs_dy + abs_dy;
		count += abs_dy;
		do {
			// La linea estará formada al menos por el punto inicial
			lcd_draw_pixel(x, y, color);
			y += signo_dy;
			acu_err += inc_err;
			if(acu_err > abs_dx){
				acu_err -= dec_err;
				x += signo_dx;
			}
		} while(--count);
	}
}


// TODO implementación lcd_draw_line() que recorre todo el rectángulo determinado por los puntos inicial y final
// modificando sóo los puntos que pertenecen a la línea, y mezclando los puntos que están parcialmente en la línea
// cuando el punto de la línea se encuentra en medio de dos puntos, dandole a cada punto la intensidad del color de
// primer plano proporcional a lo cuanto está en la línea...
//



/**
 * Rellna un área rectángular.
 */
void lcd_draw_fill_rect(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t color )
{
	lcd_set_draw_area(x0, y0, x1, y1);
	lcd_flood_area(color);

}

