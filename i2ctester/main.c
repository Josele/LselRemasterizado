#include <stdio.h>
#include <signal.h>
#include <unistd.h>

// Xenomai native skin libraries
#include <sys/mman.h>
#include <native/task.h>
#include <native/timer.h>
#include <native/mutex.h>
// WiringPi
#include <wiringPi.h>
//i2c
#include "lsquaredc.h"
#include "i2c1.h"
// RT compilant print library
#include <rtdk.h>

#define I2C_BUSES 2
#define I2C_IR_ADDRESS_0 0x21
#define I2C_IR_ADDRESS_1 0x22
#define I2C_IR_ADDRESS_2 0x23
#define I2C_IR_ADDRESS_3 0x24
#define I2C_RENFE_BYTE 0x2c
#define I2C_DIESEL_BYTE 0x34
#define I2C_PRUEBA_BYTE 0x55



typedef struct{
        int i2chandler;
        RT_MUTEX mutex;
}i2chandler_t;

i2chandler_t* i2chandler[I2C_BUSES];
static int n_i2c_handlers = 0;



void catch_signal () {}
void initializeXenomaiEnv(void) {
	// Catch signals
	signal(SIGTERM, catch_signal);
	signal(SIGINT, catch_signal);

	/* Avoids memory swapping for this program */
	mlockall(MCL_CURRENT | MCL_FUTURE);

	// Initialize rdtk to use rt_printf
	rt_print_auto_init(1);
}
void initializeWiringPi(void) {

	wiringPiSetup();
}
void i2chandler_init(i2chandler_t* this, int n_bus) {
	
	this->i2chandler = i2c_open(n_bus);
	rt_mutex_create(&this->mutex, NULL);

}

i2chandler_t*
i2chandler_new(int n_bus) 
{
	i2chandler_t* this = (i2chandler_t*) malloc(sizeof(i2chandler_t));
	i2chandler_init(this, n_bus);
	i2chandler[n_i2c_handlers++] = this;
	
	return this;
}
void initializei2c(void) {

	int i;
	//i2chandler_new(0);
	for(i = 0 ; i < I2C_BUSES ; i++)
		i2chandler_new(i);	
	
}

uint8_t i2ctest(uint16_t i2c_address){

         uint8_t buff;
	 uint8_t error;

         uint16_t read_IR_comand[]={(i2c_address<<1), 0x00 ,I2C_RESTART ,(i2c_address<<1)|1 ,I2C_READ};

//        if(this->i2c_address == I2C_IR_ADDRESS_0 || this->i2c_address == I2C_IR_ADDRESS_1){
        rt_mutex_acquire(&(i2chandler[0]->mutex), TM_INFINITE);
        error=i2c_send_sequence(i2chandler[0]->i2chandler, read_IR_comand, 5, &buff);
        rt_mutex_release(&(i2chandler[0]->mutex));
        return (error==0xff?0xff:buff);
}
int main(int argc, char* argv[]) {
	uint8_t result;
	int correcto=0;
	int error=0;
	int diesel,renfe=0;
	int i;
	int i2c_address_sensor=I2C_IR_ADDRESS_0;
	initializeWiringPi();
	digitalWrite(19,0);
	digitalWrite(20,0);		
	//Initialize the i2c
	i2c_pins_setup();
	initializei2c();
	FILE *f,*f2;
	
        f = fopen("result.txt", "w");
        f2 = fopen("respuesta.txt", "w");
	while(i2c_address_sensor<(I2C_IR_ADDRESS_3+1)){
 	result=0;
        correcto=0;
        error=0;
        diesel=0;
	renfe=0;
       	i=0;

	fprintf(f, "\n\n\nSensor :%x \n\n ", i2c_address_sensor);
	fprintf(f2, "\n\n\nSensor :%x \n\n ", i2c_address_sensor);

	for(i=0;i<500;i++)
	{
		result=i2ctest(i2c_address_sensor);
		if( result == I2C_RENFE_BYTE ){
       		correcto++;  
		renfe++;
		}else if(result == I2C_DIESEL_BYTE){
         	correcto++;
		diesel++;  
    		}else if(result == 0xc3){
                correcto++;
                }else if(result == 0xff)
		error++;

		else{
	if(i%50==0)
	fprintf(f2, "%i \n ", result);
	else
	fprintf(f2, "%i | ", result);
	}
	delay(20);
	//printf("%i\n",i);
	}
	 //printf("%i",i2c_address_sensor);

	fprintf(f, "\n Tasa de error: %f\n errores: %i\n Tasa de incorrectos del sensor %x :  %f\n Correctos %i\n Incorrectos %i \n Diesel %i Renfe %i ",(double)error/i,error,i2c_address_sensor, ((double)(i-correcto)/i), correcto,i-correcto,diesel,renfe);
 	i2c_address_sensor++;
	}
        fclose(f);
        fclose(f2);
	/** test  de osciloscopio
	for(i=0;i<10000;i++){
		i2ctest(I2C_PRUEBA_BYTE);
		delay(20);
		}
	*/
	i2c_close(i2chandler[0]->i2chandler );
	i2c_close(i2chandler[1]->i2chandler );
	digitalWrite(19,0);
	digitalWrite(20,0);
	}



