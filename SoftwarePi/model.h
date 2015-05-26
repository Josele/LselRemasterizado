#ifndef MODEL_H
#define MODEL_H

#include "Model/observer.h"

void model_init(void);
observable_t* model_get_observable(const char* name);
int model_add(const char* name, observable_t* obs);

#endif
