#ifndef SERVER_H
#define SERVER_H
#include "LibSock/fsm.h"
#include <native/mutex.h>

typedef struct {

   	int status;
        RT_MUTEX mutex;

} flag_enviar;



void Des_Ser_init();
fsm_t* serv_init();
int serv_state_trans(fsm_t* serv_fsm);
char* get_buff();
void set_res(char* resp,int length);
#endif
