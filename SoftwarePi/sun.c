#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wiringPiI2C.h>
#include "sun.h"
#include "sunparse.h"
#include "tasks.h"
#include "Interpreter/interp.h"
sun_t* sun;

void sun_setup(void) {
	sun = sun_new(DEFAULT_DATE, 0x20, SUN_DEADLINE);
	interp_addcmd("sun", sun_cmd, "Set sun parameters\n");
}

int sun_cmd(char* arg) {
	if (0 == strncmp(arg, "date ", strlen("date "))) {
		char* date = arg + strlen("date ");
		if (strlen(date) != 10) {
			printf("Incorrect date. Use format dd/mm/yyyy\n");
			return 1;
		}
		sun_set_date(sun, date);
		printf("Sunrise: %d:%d:%d\n", sun->sunrise.hours, sun->sunrise.minutes,
				sun->sunrise.seconds);
		printf("Sunset: %d:%d:%d\n", sun->sunset.hours, sun->sunset.minutes,
				sun->sunset.seconds);
		return 0;
	}
	if (0 == strncmp(arg, "current", strlen("current"))) {
		printf("%d\n", sun->current_simulated_time);
		return 0;
	}

	if (0 == strncmp(arg, "test", strlen("test"))) {
		int i;
		int flag = 1;
		int buffer = -1;
		int x;
		for (i = 0; i < 5; i++) {
			x = sun->current_simulated_time;
			if ((x > buffer) && (x >= 0) && (x < 1201)) {
				buffer = x;
			} else {
				flag = 0;
			}
			sleep(2);
		}
		if (flag == 1) {
			printf("Test Reloj Virtual OK\n");
			return 0;
		} else {
			printf("Test Reloj Virtual NOK\n");
			return 1;
		}

		return 0;
	}

	return 1;
}

void sun_task(void* arg) {
	sun_t* this = (sun_t*) arg;
	rt_task_set_periodic(NULL, TM_NOW, SUN_PERIOD);
	char buffer[9];
	while (1) {
		rt_task_wait_period(NULL);
		sun_update_simulated_time(this);
		if (this->current_simulated_time == 1200) {
			sprintf(buffer, "23:59:59");
		} else {
			int seg_tot, h, r, min, seg;
			seg_tot = (this->current_simulated_time * 86400) / 1200;
			h = seg_tot / 3600;
			r = seg_tot % 3600;
			min = r / 60;
			seg = r % 60;
			sprintf(buffer, "%02d:%02d:%02d", h, min, seg);
		}

		FILE *fp;
		if ((fp = fopen("/var/www/HoraVirtual.txt.new", "w+")) < 0) {
			rt_printf("Error en archivo destino");
		}
		fprintf(fp, "%s", buffer);
		fclose(fp);
		system("mv /var/www/HoraVirtual.txt.new /var/www/HoraVirtual.txt");
	}
}

sun_t*
sun_new(sun_date_t date, char i2c_address, int deadline) {
	sun_t* this = (sun_t*) malloc(sizeof(sun_t));
	sun_init(this, date, i2c_address, deadline);
	return this;
}

void sun_init(sun_t* this, sun_date_t date, char i2c_address, int deadline) {
	this->i2c_address = i2c_address;
	this->i2c_fd = wiringPiI2CSetup(i2c_address);
	sun_set_date(this, date);
	rt_mutex_create(&(this->mutex), "sun_mutex");
	task_add("Sun", deadline, sun_task, this);
}

void sun_set_date(sun_t* this, sun_date_t date) {

	rt_mutex_acquire(&(this->mutex), TM_INFINITE);
	this->date = date;
	rt_mutex_release(&(this->mutex));

//if (sun_date_cmp (date, current_date) != 0) {
	char cmd[1024];
	char aux[6];
	char *unparsedDate = strdup(this->date);
	char *day = strtok(unparsedDate, "/");
	char *month = strtok(NULL, "/");
	snprintf(aux, 6, "%s/%s", day, month);
	snprintf(cmd, 1024, "wget -q --output-document=sunrise.xml "
			"http://www.earthtools.org/sun/40.4521/-3.7266/%s/1/0", aux);
	system(cmd);
	sun_parse_data(this);
	wiringPiI2CWrite(this->i2c_fd, 0xFF);
	wiringPiI2CWrite(this->i2c_fd, this->sunrise.hours);
	wiringPiI2CWrite(this->i2c_fd, this->sunrise.minutes);
	wiringPiI2CWrite(this->i2c_fd, this->sunrise.seconds);
	wiringPiI2CWrite(this->i2c_fd, this->sunset.hours);
	wiringPiI2CWrite(this->i2c_fd, this->sunset.minutes);
	wiringPiI2CWrite(this->i2c_fd, this->sunset.seconds);
}

void sun_update_simulated_time(sun_t* this) {
	wiringPiI2CWrite(this->i2c_fd, 0xFE);
	char byte1 = wiringPiI2CRead(this->i2c_fd);
	char byte0 = wiringPiI2CRead(this->i2c_fd);
	rt_mutex_acquire(&(this->mutex), TM_INFINITE);
	this->current_simulated_time = (byte0 << 8) + byte1;
	rt_mutex_release(&(this->mutex));
}

int sun_get_simulated_time(sun_t* this) {
	return this->current_simulated_time;
}
;

void sun_destroy(sun_t* this) {
	free(this);
}
