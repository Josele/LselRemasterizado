/*
 * sensorIR.h
 *
 *  Created on: 02/04/2014
 *      Author: Javier
 */

#ifndef SENSORIR_H
#define SENSORIR_H

#include <native/mutex.h>

#include "observer.h"
#include "tasks.h"
#include "interp.h"

#define MAXSENSORS 4

#define IR_DEADLINE 20000000
#define IR_PERIOD 100000000
#define MAXTRAINS 8

typedef struct {
	char flag;
	int passingTrain;
} event_t;

typedef struct {

	observable_t observable;
	int id;
	//int GPIOlines[MAXTRAINS];
	uint16_t i2c_address;
//	int last_reading;
	event_t* event;
	RT_MUTEX mutex;

} sensorIR_t;

extern sensorIR_t* sensors[MAXSENSORS];
extern int nsensors;

//sensorIR_t* sensorIR_new(int id);
//void sensorIR_init(sensorIR_t* this, int id, event_t* event);
sensorIR_t* sensorIR_new(int id, uint16_t i2c_address);
void sensorIR_init(sensorIR_t* this, int id, event_t* event, uint16_t i2c_address);
void sensorIR_destroy(sensorIR_t* this);
void IRsensors_setup(void);
//int sensorIR_readLine(sensorIR_t* this, int trainLine);
void sensorIR_readLine(sensorIR_t* this, uint8_t* buff);
void sensorIR_trainPassing(sensorIR_t* this);

event_t* sensorIR_get_event(sensorIR_t* this);

#endif
