/*
 * queue.h
 *
 *  Created on: 18 abr. 2020
 *      Author: Jesús F. García Bruña
 */

#ifndef INC_QUEUE_H_
#define INC_QUEUE_H_

#include <stdint.h>


/**
 * Envolura para cola circular de bytes.
 * Antes de usarla, es necesario inicializar la estructura:
 * Asignar un array estático de bytes a 'buf'
 * Inicializar 'get' y 'put' para que apunten al comienzo de este array.
 * Inicializar 'size' con el tamaño del array asignado
 */
typedef struct CircularQueue_uint8{
	uint8_t * 	buf;
	uint8_t * 	get;
	uint8_t * 	put;
	uint16_t	size;

}CircularQueue_uint8;

/**
 * @brief Inicializa una cola circular de bytes.
 * Requiere un puntero a un array de tipo unsigned char, asignado estáticamente.
 * @param buf: Puntero al array de bytes que actuará como bufer.
 * @param size: Declara el tamaño del array que se adjunta como bufer.
 *
 */
void CircularQueue_uint8_init(
		CircularQueue_uint8 * queue,
		uint8_t * buf,
		uint16_t size
		);

/**
 * @brief introduce un caracter en la cola salvo que esté llena.
 * @return el número de bytes introducidos en la cola, 1 si tuvo éxito,
 * 0 si la cola estaba llena.
 */

uint16_t CircularQueue_uint8_put(
		CircularQueue_uint8 *queue,
		uint8_t databyte
		);

uint16_t CircularQueue_uint8_get(
		CircularQueue_uint8 * queue,
		uint8_t *databyte
		);


#endif /* INC_QUEUE_H_ */
