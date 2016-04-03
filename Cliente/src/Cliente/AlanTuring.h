/*
 * CodificadorMensajes.h
 *
 *  Created on: Apr 2, 2016
 *      Author: gonzalo
 */

#ifndef SRC_SERVER_ALANTURING_H_
#define SRC_SERVER_ALANTURING_H_

#include "../Utils/Logger.h"
#include "../Utils/TiposDefinidos.h"
#include <string>
#include <cstring>
#include <stdio.h>

/*
 * Message Codes
 *
 *int: envia nun entero
 *chr: envia un char
 *dbl: envia un double
 *str: envia un string
 *
 */
/*
 * Message Status
 *
 * - : no procesado
 * I : mensaje procesado e Invalido
 * V : mensaje procesado y Valido
 */

class AlanTuring
{
public:
	//devuelve un buffer de 256 bytes con el mensaje codificado en binario
	int encodeXMLMessage(Mensaje mensaje, char* bufferSalida);
	int encodeNetworkMessage(NetworkMessage netMsg, char* bufferSalida);
	//los decode reciben buffers de tamaño 256
	NetworkMessage decode (char* codigoEnigma);
	unsigned short decodeLength(char* codigoEnigma);
	DataMessage decodeMessage (char* datosCodigoEnigma);

	void setNetworkMessageStatus(NetworkMessage* networkMessage, char statusCode);

	/*intMessage decodeIntMessage(char* datosCodigoEnigma);
	charMessage decodeCharMessage(char* datosCodigoEnigma);
	doubleMessage decodeDoubleMessage(char* datosCodigoEnigma);
	stringMessage decodeStringMessage(char* datosCodigoEnigma);*/

	AlanTuring();
	~AlanTuring();

private:
	//llena los campos de netMsg con la información del mensaje
	void fillMsgData(NetworkMessage* netMsg, const Mensaje mensaje);

	void encodeMessage(NetworkMessage* netMsg, const Mensaje mensaje);

	/*void encodeIntMessage(NetworkMessage* netMsg, const Mensaje mensaje);
	void encodeCharMessage(NetworkMessage* netMsg, const Mensaje mensaje);
	void encodeDoubleMessage(NetworkMessage* netMsg, const Mensaje mensaje);
	void encodeStringMessage(NetworkMessage* netMsg, const Mensaje mensaje);*/
};

#endif /* SRC_SERVER_ALANTURING_H_ */
