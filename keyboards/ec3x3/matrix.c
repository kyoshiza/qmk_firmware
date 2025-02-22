/*
Copyright 2012 Jun Wako <wakojun@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
 * scan matrix
 */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "print.h"
#include "debug.h"
#include "util.h"
#include "matrix.h"
#include "quantum.h"

#ifndef DEBOUNCE
#  define DEBOUNCE 5
#endif

#define ERROR_DISCONNECT_COUNT 5

#ifndef CHARGE_PIN
#define CHARGE_PIN B2
#endif
#ifndef ADC_PIN
#define ADC_PIN B6
#endif

#define KEY_NUM (MATRIX_COLS*MATRIX_ROWS)
#define KEY2CR(K,C,R) {(C)=(K)%MATRIX_COLS;(R)=(K)/MATRIX_COLS;}
#define CR2KEY(C,R,K) {(K) = ((C)%MATRIX_COLS)+(((R)%MATRIX_ROWS)*MATRIX_COLS);}
#define VAL2THRESHOLD(VAL) (((VAL)>=1024) ? 255 : (VAL/4))

#define PI ((float)3.1415926535)
#define CALIBRATE_BUF 5
#define CALIBRATE_INI 20
#define CALIBRATE_FREQ_MS 10000
#define DEFAULT_THRESHOLD_DOWN 90
#define DEFAULT_THRESHOLD_UP 25
#define DEFAULT_FILTER_Q (1.0/sqrt(2.0))
#define DEFAULT_SAMPLE_HZ 150.0
#define DEFAULT_LOWPASS_HZ 1.0
#define _barrier() asm volatile ("" ::: "memory")

static const uint8_t row_pins[] = MATRIX_ROW_PINS;
static const uint8_t col_pins[] = MATRIX_COL_PINS;

/* matrix state(1:on, 0:off) */
static matrix_row_t matrix[MATRIX_ROWS];
static uint8_t debounce[KEY_NUM];
uint32_t timer_1ms;

__attribute__ ((weak))
void matrix_init_kb(void) {
    matrix_init_user();
}

__attribute__ ((weak))
void matrix_scan_kb(void) {
    matrix_scan_user();
}

__attribute__ ((weak))
void matrix_init_user(void) {
}

__attribute__ ((weak))
void matrix_scan_user(void) {
}

inline
uint8_t matrix_rows(void)
{
    return MATRIX_ROWS;
}

inline
uint8_t matrix_cols(void)
{
    return MATRIX_COLS;
}

typedef struct{
    uint8_t threshold;
    uint8_t buf[CALIBRATE_BUF];
    uint8_t idx;
}calibrate_info;

uint8_t key_val[KEY_NUM];

calibrate_info calibrate[KEY_NUM];
uint32_t last_timer;

/* Filter */
typedef struct{
    float a; // Feed forward coefficient
    float b; // Feedback coefficient

    float input;
    float output;
    uint8_t first;
}filter_info;

filter_info filter[KEY_NUM];

void finter_config(filter_info *f, float fc, float q, float fs){

    //fc Digital LPF cutoff frequency
    //q  Quality factor (1 / sqrt (2) default)
    //fs Sampling frequency

    float t = 1.0/fs;
    float c = 1.0*PI*fc;
    f->b = tan(c*t/2.0)/(1.0+tan(c*t/2.0));
    f->a = (1.0-tan(c*t/2.0))/(1.0+tan(c*t/2.0));
    f->first = 1;
}

float filter_process(filter_info *f, float val){
    float result;
    if(f->first){
        f->input = val;
        f->output = val;
        f->first = 0;
    }

    result = f->b*val + f->b*f->input + f->a*f->output;
    f->input = val;
    f->output = result;
    return result;
}

/* Spin WAIT */
void spin_wait(volatile uint32_t time){
    while(time>0){
        time--;
    }
}

// KEY SCAN PROCESS
uint16_t get_one_key_val(uint16_t k){
    uint8_t c,r;
    KEY2CR(k,c,r);

    //Interrupt Disable
    cli();

    // LINE SELECT
    for(uint8_t pin=0;pin<sizeof(col_pins)/sizeof(col_pins[0]);pin++){
        if(c & (1<<pin)){
            PORTx_ADDRESS(col_pins[pin]) |= _BV((col_pins[pin])&0xF);
        }else{
            PORTx_ADDRESS(col_pins[pin]) &= ~_BV((col_pins[pin])&0xF);
        }
    }
    _barrier();

    // DISCHARGE PIN = OFF
    PORTx_ADDRESS(CHARGE_PIN) &= ~_BV((CHARGE_PIN)&0xF);
    _barrier();

    // SPIN WAIT
    spin_wait(20);

    // CHARGE
    PORTx_ADDRESS(row_pins[r]) |= _BV((row_pins[r])&0xF);
    _barrier();

    // SPIN WAIT
    //spin_wait(1);

    // ADC Start
    ADCSRA |= (_BV(ADIF)|_BV(ADSC));
    // WAIT UNTIL ADC COMPLETE
    loop_until_bit_is_set(ADCSRA, ADIF);

    // DisCharge PIN = ON
    PORTx_ADDRESS(CHARGE_PIN) |= _BV((CHARGE_PIN)&0xF);
    // DISCHARGE
    PORTx_ADDRESS(row_pins[r]) &= ~_BV((row_pins[r])&0xF);
    _barrier();

    //Interrupt Enable
    sei();

    // ADC RESULT SET
    return (uint16_t)ADC;
}

void init_calibrate_info(void){
    static uint16_t buf[KEY_NUM] = {0};
    volatile uint32_t tmp = 0;
    (void)tmp;

    // Ramp-Up
    for(uint16_t k=0;k<KEY_NUM;k++){
        tmp = get_one_key_val(k);
        // SPIN WAIT(While DISCHARGE COMPLETE)
        spin_wait(10);
    }

    // Calibration
    for(uint16_t i=0;i<CALIBRATE_INI;i++){
        for(uint16_t k=0;k<KEY_NUM;k++){
            buf[k] += get_one_key_val(k);
            // SPIN WAIT(While DISCHARGE COMPLETE)
            spin_wait(10);
        }
    }

    for(uint16_t k=0;k<KEY_NUM;k++){
        // Initialize
        calibrate[k].threshold = VAL2THRESHOLD(buf[k]/CALIBRATE_INI);
        calibrate[k].idx = 0;
        // Average
        for(uint16_t i=0;i<CALIBRATE_BUF;i++){
            calibrate[k].buf[i] = calibrate[k].threshold;
        }
    }
}

// matrix code
static void init_pins(void) {
    uint8_t pin;
    // Output Pin
    for(pin=0;pin<sizeof(row_pins)/sizeof(row_pins[0]);pin++){
        DDRx_ADDRESS(row_pins[pin]) |= _BV((row_pins[pin])&0xF);
    }
    for(pin=0;pin<sizeof(col_pins)/sizeof(col_pins[0]);pin++){
        DDRx_ADDRESS(col_pins[pin]) |= _BV((col_pins[pin])&0xF);
    }
    DDRx_ADDRESS(CHARGE_PIN) |= _BV((CHARGE_PIN)&0xF);

    // InputPort = ADC13(PINB6)
    #if ADC_PIN==B6
    ADMUX  = 0b01000101;
    ADCSRB = 0b00100000;
    ADCSRA = 0b10000110;
    DIDR2  = 0b00100000;
    #else
    ERROR
    #endif
}

uint8_t matrix_scan(void) {
    uint8_t changed = 0;
	uint8_t changed_all = 0;
    uint16_t result;
    uint16_t buf;
    uint8_t c,r;

    // For each key...
    for (uint8_t k = 0; k < KEY_NUM; k++) {

		if(debounce[k]>0){
			continue;
		}

        KEY2CR(k,c,r);
        result = get_one_key_val(k);
        key_val[k] = VAL2THRESHOLD(result);
		changed = 0;

        // KEY DOWN?
        if(!(matrix[r] & (MATRIX_ROW_SHIFTER << c))){
            if(VAL2THRESHOLD(result) > (calibrate[k].threshold + DEFAULT_THRESHOLD_DOWN )){
                matrix[r] |= (MATRIX_ROW_SHIFTER << c);
                changed = 1;
                #ifdef CONSOLE_ENABLE
                uprintf("col=%d row=%d \n", c, r);
                uprintf("key%d = down %d %d\n", k, VAL2THRESHOLD(result), calibrate[k].threshold);
                #endif
            }
            else{
                filter_process(&filter[k], (float)result);
            }
        }
        // KEY UP?
        else{
            if(VAL2THRESHOLD(result) < (calibrate[k].threshold + DEFAULT_THRESHOLD_UP)){
                matrix[r] &= ~(MATRIX_ROW_SHIFTER << c);
                changed = 1;
                #ifdef CONSOLE_ENABLE
                uprintf("key%d = up %d %d\n", k, VAL2THRESHOLD(result), calibrate[k].threshold);
                #endif
            }
        }

		if(changed){
			debounce[k] = DEBOUNCE;
			changed_all = 1;
		}
    }

    // Re Calibration
    uint32_t timer_now = timer_read32();

	// Calibration Timer
    if (TIMER_DIFF_32(timer_now, last_timer) > CALIBRATE_FREQ_MS) {
        // Timer Update
        last_timer = timer_now;
        for(uint16_t k=0;k<KEY_NUM;k++){
            // Calibrate Update
            calibrate[k].buf[calibrate[k].idx] = VAL2THRESHOLD((uint16_t)filter[k].output);
            calibrate[k].idx = (calibrate[k].idx+1)%CALIBRATE_BUF;

            // Average
            buf = 0;
            for(uint16_t i=0;i<CALIBRATE_BUF;i++){
                buf += calibrate[k].buf[i];
            }
            calibrate[k].threshold = buf/CALIBRATE_BUF;
        }
    }

	// 1ms Timer
	if (TIMER_DIFF_32(timer_now, timer_1ms) > 1) {
		timer_1ms = timer_now;
		for(uint16_t k=0;k<KEY_NUM;k++){
			if(debounce[k]>0){
				debounce[k]--;
			}
		}
	}

    matrix_scan_quantum();

    return changed_all;
}

void get_matrix_key_val(uint8_t col, uint8_t row, uint8_t* val, uint8_t *threshold){
    uint8_t key;
    CR2KEY(col,row,key);
    *val = key_val[key];
    *threshold = calibrate[key].threshold;
}

void matrix_init(void)
{
    // initialize key pins
    init_pins();

    // initialize matrix state: all keys off
    for (uint8_t i=0; i < MATRIX_ROWS; i++) {
        matrix[i] = 0;
    }
    init_calibrate_info();
    last_timer = timer_read32();
	timer_1ms = last_timer;

    // Filter config
    for(uint8_t k=0;k<KEY_NUM;k++){
        // Low Pass Filter Init
        finter_config(&filter[k], DEFAULT_LOWPASS_HZ, DEFAULT_FILTER_Q, DEFAULT_SAMPLE_HZ);
		debounce[k]=0;
    }

    matrix_init_quantum();
}

bool matrix_is_modified(void)
{
    return true;
}

inline
bool matrix_is_on(uint8_t row, uint8_t col)
{
    return (matrix[row] & ((matrix_row_t)1<<col));
}

inline
matrix_row_t matrix_get_row(uint8_t row)
{
    return matrix[row];
}

void matrix_print(void)
{
    print("\nr/c 0123456789ABCDEF\n");
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        phex(row); print(": ");
        pbin_reverse16(matrix_get_row(row));
        print("\n");
    }
}

uint8_t matrix_key_count(void)
{
    uint8_t count = 0;
    for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
        count += bitpop16(matrix[i]);
    }
    return count;
}
