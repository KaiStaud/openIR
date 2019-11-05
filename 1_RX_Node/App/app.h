#include "stdint.h"

/* Initialize module functions */
void app_init(void);

/* Reset system */
void app_reset(void);

/* Operation Modes */
void op_normal(void);
void op_stop(void);
void op_sleep(void);
void op_hil(void);
void op_force(void);

/* "Print Functions" */
void debug(int8_t ambient_temp, uint32_t *batteries,char *health,uint8_t distance);
void send_package(uint32_t *batteries, uint8_t distance);
