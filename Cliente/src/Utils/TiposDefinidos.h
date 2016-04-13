/*
 * tiposDefinidos.h
 *
 *  Created on: Mar 23, 2016
 *      Author: gonzalo
 */

#ifndef UTILS_TIPOSDEFINIDOS_H_
#define UTILS_TIPOSDEFINIDOS_H_

#define MESSAGE_BUFFER_SIZE 256
#define MESSAGE_ID_BYTES_LIMIT 25
#define MESSAGE_LENGTH_BYTES 2
#define MESSAGE_CODE_BYTES 3
#define MESSAGE_DATA_SIZE (MESSAGE_BUFFER_SIZE - MESSAGE_LENGTH_BYTES - MESSAGE_CODE_BYTES)
#define MESSAGE_VALUE_SIZE (MESSAGE_DATA_SIZE - MESSAGE_ID_BYTES_LIMIT - 1)

#include <string>

// TIPOS RELACIONADOS A ARCHIVOS XML

struct LoggerInfo
{
	bool debugAvailable;
	bool warningAvailable;
	bool errorAvailable;
};

struct Servidor{
	int cantMaximaClientes;
	int puerto;
};

struct Conexion{
	std::string ip;
	int puerto;
};

struct Mensaje{
	std::string id;
	std::string tipo;
	std::string valor;
};

//TIPOS DE DATOS RELACIONADOS CON MENSAJES DE RED

struct NetworkMessage
{
	unsigned short msg_Length;
	char msg_Code[MESSAGE_CODE_BYTES];
	char msg_Data[MESSAGE_DATA_SIZE];
};

struct DataMessage
{
	char msg_status;
	char msg_ID[MESSAGE_ID_BYTES_LIMIT];
	char msg_value[MESSAGE_VALUE_SIZE];
};

/*struct intMessage
{
	char msg_ID[MESSAGE_ID_BYTES_LIMIT];
	int msg_value;
};
struct charMessage
{
	char msg_ID[MESSAGE_ID_BYTES_LIMIT];
	char msg_value;
};
struct doubleMessage
{
	char msg_ID[MESSAGE_ID_BYTES_LIMIT];
	double msg_value;
};
struct stringMessage
{
	char msg_ID[MESSAGE_ID_BYTES_LIMIT];
	std::string msg_value;
};*/

#endif /* UTILS_TIPOSDEFINIDOS_H_ */
