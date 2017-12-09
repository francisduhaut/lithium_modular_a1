#ifndef TEMPO_H_
#define TEMPO_H_


/* Set to ensure base delay */
#define DELAY_TIMING_US         (0x000B)
#define DELAY_TIMING_S          (0x00840000)

/* Delay routine for LCD or any other devices */
void Delay_us(const uint32_t);
/* Delay routine for LCD or any other devices */
void Delay_s(const uint8_t);


/* End of multiple inclusion prevention macro */
#endif
