/*
 * queue.c
 *
 *  Created on: 18 abr. 2020
 *  Author: Jesús F. García
 */


#include "queue.h"

/**
 * @brief Inicializa una cola circular de bytes.
 * Requiere un puntero a un array de tipo unsigned char, asignado estáticamente.
 * @arg unsigned char *buf: Puntero al array de bytes que actuará como bufer.
 * @arg unsigned int size: Declara el tamaño del array que se adjunta como bufer.
 *
 */
void CircularQueue_uint8_init(
		CircularQueue_uint8 * queue,
		uint8_t * buf,
		uint16_t size )
{
	queue->buf = buf;
	queue->get = buf;
	queue->put = buf;
	queue->size = size;
}

/**
 * @brief introduce un caracter en la cola salvo que esté llena.
 * @return el número de bytes introducidos en la cola, 1 si tuvo éxito,
 * 0 si la cola estaba llena.
 */
uint16_t CircularQueue_uint8_put(CircularQueue_uint8 *queue, uint8_t databyte)
{
	// Calculamos como quedará el puntero put tras la inserción
	uint8_t * nextput = queue->put + 1;
	if((nextput - queue->buf) == queue->size)
		nextput = queue->buf;

	if(queue->get == nextput)
		return 0;				// El buffer está lleno

	*queue->put = databyte;
	queue->put = nextput;
	return 1;
}

/**
 * @brief Extrae un byte de la cola salvo que esté vacia.
 * El byte extraído se deposita en 'databyte' que a tal efecto se pasa por
 * referencia.
 * @return el número de bytes extraídos:
 * 	 1 si ha tenido éxito,
 * 	 0 si la cola estaba vacía.
 */
uint16_t CircularQueue_uint8_get(CircularQueue_uint8 * queue, uint8_t *databyte)
{
	if(queue->get == queue->put)
		return 0;

	*databyte = *queue->get++;

	if((queue->get - queue->buf == queue->size))
		queue->get = queue->buf;

	return 1;
}






