/*
Copyright 2012-2018 Jun Wako, Jack Humbert, Yiancar

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
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "util.h"
#include "matrix.h"
#include "debounce.h"
#include "quantum.h"
#ifdef SPLIT_KEYBOARD
#    include "split_common/split_util.h"
#    include "split_common/transactions.h"

#    define ROWS_PER_HAND (MATRIX_ROWS / 2)
#else
//#    define ROWS_PER_HAND (MATRIX_ROWS)
#endif

#ifdef DIRECT_PINS_RIGHT
#    define SPLIT_MUTABLE
#else
#    define SPLIT_MUTABLE const
#endif
#ifdef MATRIX_ROW_PINS_RIGHT
#    define SPLIT_MUTABLE_ROW
#else
#    define SPLIT_MUTABLE_ROW const
#endif
#ifdef MATRIX_COL_PINS_RIGHT
#    define SPLIT_MUTABLE_COL
#else
#    define SPLIT_MUTABLE_COL const
#endif

#ifdef DIRECT_PINS
static SPLIT_MUTABLE pin_t direct_pins[MATRIX_ROWS][MATRIX_COLS] = DIRECT_PINS;
#else
static const pin_t row_pins[] = MATRIX_ROW_PINS;
static const pin_t col_pins[] = MATRIX_COL_PINS;

/* Consistency checking of the size of the matrix and the number of pins */
// clang-format off
#    if (DIODE_DIRECTION == BOTHWAYS)
#        define NUM_ROW_PINS (MATRIX_ROWS / 2)
#        define ROWS_PER_HAND (MATRIX_ROWS / 2)
_Static_assert(NUM_ROW_PINS * 2 == MATRIX_ROWS, "Must be exactly divisible");
_Static_assert(NUM_ROW_PINS == sizeof(row_pins)/sizeof(row_pins[0]), \
    "Number of elements in MATRIX_ROW_PINS * 2 must be equal to MATRIX_ROWS");
#    else
#        define NUM_ROW_PINS MATRIX_ROWS
#        define ROWS_PER_HAND (MATRIX_ROWS)
_Static_assert(NUM_ROW_PINS == sizeof(row_pins)/sizeof(row_pins[0]), \
    "Number of elements in MATRIX_ROW_PINS must be equal to MATRIX_ROWS");
#    endif

_Static_assert(MATRIX_COLS == sizeof(col_pins)/sizeof(col_pins[0]), \
    "Number of elements in MATRIX_COL_PINS must be equal to MATRIX_COLS");
// clang-format on
#endif

/* matrix state(1:on, 0:off) */
static matrix_row_t raw_matrix[MATRIX_ROWS];  // raw values
extern matrix_row_t matrix[MATRIX_ROWS];      // debounced values

#ifdef SPLIT_KEYBOARD
// row offsets for each hand
uint8_t thisHand, thatHand;
#endif

// user-defined overridable functions
__attribute__((weak)) void matrix_init_pins(void);
__attribute__((weak)) void matrix_read_cols_on_row(matrix_row_t current_matrix[], uint8_t current_row);
__attribute__((weak)) void matrix_read_rows_on_col(matrix_row_t current_matrix[], uint8_t current_col, matrix_row_t row_shifter);
#ifdef SPLIT_KEYBOARD
__attribute__((weak)) void matrix_slave_scan_kb(void) { matrix_slave_scan_user(); }
__attribute__((weak)) void matrix_slave_scan_user(void) {}
#endif

static inline void setPinOutput_writeLow(pin_t pin) {
    ATOMIC_BLOCK_FORCEON {
        setPinOutput(pin);
        writePinLow(pin);
    }
}

static inline void setPinInputHigh_atomic(pin_t pin) {
    ATOMIC_BLOCK_FORCEON { setPinInputHigh(pin); }
}

static inline uint8_t readMatrixPin(pin_t pin) {
    if (pin != NO_PIN) {
        return readPin(pin);
    } else {
        return 1;
    }
}

// matrix code

#ifdef DIRECT_PINS

__attribute__((weak)) void matrix_init_pins(void) {
    for (int row = 0; row < MATRIX_ROWS; row++) {
        for (int col = 0; col < MATRIX_COLS; col++) {
            pin_t pin = direct_pins[row][col];
            if (pin != NO_PIN) {
                setPinInputHigh(pin);
            }
        }
    }
}

__attribute__((weak)) void matrix_read_cols_on_row(matrix_row_t current_matrix[], uint8_t current_row) {
    // Start with a clear matrix row
    matrix_row_t current_row_value = 0;

    matrix_row_t row_shifter = MATRIX_ROW_SHIFTER;
    for (uint8_t col_index = 0; col_index < MATRIX_COLS; col_index++, row_shifter <<= 1) {
        pin_t pin = direct_pins[current_row][col_index];
        if (pin != NO_PIN) {
            current_row_value |= readPin(pin) ? 0 : row_shifter;
        }
    }

    // Update the matrix
    current_matrix[current_row] = current_row_value;
}

#else

#    if (DIODE_DIRECTION == COL2ROW) || (DIODE_DIRECTION == ROW2COL)
#    elif (DIODE_DIRECTION == BOTHWAYS)
#    else
#        error DIODE_DIRECTION must be one of COL2ROW, ROW2COL, or BOTHWAYS!
#    endif

#    if (DIODE_DIRECTION != ROW2COL)
static bool select_row(uint8_t row) {
    pin_t pin = row_pins[row];
    if (pin != NO_PIN) {
        setPinOutput_writeLow(pin);
        return true;
    }
    return false;
}

static void unselect_row(uint8_t row) {
    pin_t pin = row_pins[row];
    if (pin != NO_PIN) {
        setPinInputHigh_atomic(pin);
    }
}

__attribute__((weak)) void matrix_read_cols_on_row(matrix_row_t current_matrix[], uint8_t current_row) {
    // Start with a clear matrix row
    matrix_row_t current_row_value = 0;

    if (!select_row(current_row)) {  // Select row
        return;                      // skip NO_PIN row
    }
    matrix_output_select_delay();

    // For each col...
    matrix_row_t row_shifter = MATRIX_ROW_SHIFTER;
    for (uint8_t col_index = 0; col_index < MATRIX_COLS; col_index++, row_shifter <<= 1) {
        uint8_t pin_state = readMatrixPin(col_pins[col_index]);

        // Populate the matrix row with the state of the col pin
        current_row_value |= pin_state ? 0 : row_shifter;
    }

    // Unselect row
    unselect_row(current_row);
    matrix_output_unselect_delay(current_row, current_row_value != 0);  // wait for all Col signals to go HIGH

    // Update the matrix
    current_matrix[current_row] = current_row_value;
}

#    endif

#    if (DIODE_DIRECTION != COL2ROW)

static bool select_col(uint8_t col) {
    pin_t pin = col_pins[col];
    if (pin != NO_PIN) {
        setPinOutput_writeLow(pin);
        return true;
    }
    return false;
}

static void unselect_col(uint8_t col) {
    pin_t pin = col_pins[col];
    if (pin != NO_PIN) {
        setPinInputHigh_atomic(pin);
    }
}

__attribute__((weak)) void matrix_read_rows_on_col(matrix_row_t current_matrix[], uint8_t current_col, matrix_row_t row_shifter) {
    bool key_pressed = false;

    // Select col
    if (!select_col(current_col)) {  // select col
        return;                      // skip NO_PIN col
    }
    matrix_output_select_delay();

    // For each row...
    for (uint8_t row_index = 0; row_index < ROWS_PER_HAND; row_index++) {
        // Check row pin state
        if (readMatrixPin(row_pins[row_index]) == 0) {
            // Pin LO, set col bit
            current_matrix[row_index] |= row_shifter;
            key_pressed = true;
        } else {
            // Pin HI, clear col bit
            current_matrix[row_index] &= ~row_shifter;
        }
    }

    // Unselect col
    unselect_col(current_col);
    matrix_output_unselect_delay(current_col, key_pressed);  // wait for all Row signals to go HIGH
}
#    endif

static void unselect_rows(void) {
    for (uint8_t x = 0; x < ROWS_PER_HAND; x++) {
        pin_t pin = row_pins[x];
        if (pin != NO_PIN) {
            setPinInputHigh_atomic(pin);
        }
    }
}

static void unselect_cols(void) {
    for (uint8_t x = 0; x < MATRIX_COLS; x++) {
        pin_t pin = col_pins[x];
        if (pin != NO_PIN) {
            setPinInputHigh_atomic(pin);
        }
    }
}

__attribute__((weak)) void matrix_init_pins(void) {
    unselect_rows();
    unselect_cols();
}

#endif

void matrix_init(void) {
#ifdef SPLIT_KEYBOARD
    split_pre_init();

    // Set pinout for right half if pinout for that half is defined
    if (!isLeftHand) {
#    ifdef DIRECT_PINS_RIGHT
        const pin_t direct_pins_right[MATRIX_ROWS][MATRIX_COLS] = DIRECT_PINS_RIGHT;
        for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
            for (uint8_t j = 0; j < MATRIX_COLS; j++) {
                direct_pins[i][j] = direct_pins_right[i][j];
            }
        }
#    endif
#    ifdef MATRIX_ROW_PINS_RIGHT
        const pin_t row_pins_right[MATRIX_ROWS] = MATRIX_ROW_PINS_RIGHT;
        for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
            row_pins[i] = row_pins_right[i];
        }
#    endif
#    ifdef MATRIX_COL_PINS_RIGHT
        const pin_t col_pins_right[MATRIX_COLS] = MATRIX_COL_PINS_RIGHT;
        for (uint8_t i = 0; i < MATRIX_COLS; i++) {
            col_pins[i] = col_pins_right[i];
        }
#    endif
    }

    thisHand = isLeftHand ? 0 : (ROWS_PER_HAND);
    thatHand = ROWS_PER_HAND - thisHand;
#endif

    // initialize key pins
    matrix_init_pins();

    // initialize matrix state: all keys off
    memset(matrix, 0, sizeof(matrix));
    memset(raw_matrix, 0, sizeof(raw_matrix));

    debounce_init(ROWS_PER_HAND);

    matrix_init_quantum();

#ifdef SPLIT_KEYBOARD
    split_post_init();
#endif
}

#ifdef SPLIT_KEYBOARD
// Fallback implementation for keyboards not using the standard split_util.c
__attribute__((weak)) bool transport_master_if_connected(matrix_row_t master_matrix[], matrix_row_t slave_matrix[]) {
    transport_master(master_matrix, slave_matrix);
    return true;  // Treat the transport as always connected
}

bool matrix_post_scan(void) {
    bool changed = false;
    if (is_keyboard_master()) {
        static bool  last_connected              = false;
        matrix_row_t slave_matrix[ROWS_PER_HAND] = {0};
        if (transport_master_if_connected(matrix + thisHand, slave_matrix)) {
            changed = memcmp(matrix + thatHand, slave_matrix, sizeof(slave_matrix)) != 0;

            last_connected = true;
        } else if (last_connected) {
            // reset other half when disconnected
            memset(slave_matrix, 0, sizeof(slave_matrix));
            changed = true;

            last_connected = false;
        }

        if (changed) memcpy(matrix + thatHand, slave_matrix, sizeof(slave_matrix));

        matrix_scan_quantum();
    } else {
        transport_slave(matrix + thatHand, matrix + thisHand);

        matrix_slave_scan_kb();
    }

    return changed;
}
#endif

uint8_t matrix_scan(void) {
    matrix_row_t curr_matrix[MATRIX_ROWS] = {0};

#if defined(DIRECT_PINS) || (DIODE_DIRECTION == COL2ROW)
    // Set row, read cols
    for (uint8_t current_row = 0; current_row < ROWS_PER_HAND; current_row++) {
        matrix_read_cols_on_row(curr_matrix, current_row);
    }
#elif (DIODE_DIRECTION == ROW2COL)
    // Set col, read rows
    matrix_row_t row_shifter = MATRIX_ROW_SHIFTER;
    for (uint8_t current_col = 0; current_col < MATRIX_COLS; current_col++, row_shifter <<= 1) {
        matrix_read_rows_on_col(curr_matrix, current_col, row_shifter);
    }
#elif (DIODE_DIRECTION == BOTHWAYS)
    for (uint8_t current_row = 0; current_row < MATRIX_ROWS / 2; current_row++) {
        matrix_read_cols_on_row(curr_matrix, current_row);
    }
    matrix_row_t row_shifter = MATRIX_ROW_SHIFTER;
    for (uint8_t current_col = 0; current_col < MATRIX_COLS; current_col++, row_shifter <<= 1) {
        matrix_read_rows_on_col(curr_matrix + MATRIX_ROWS / 2, current_col, row_shifter);
    }
#endif

    bool changed = memcmp(raw_matrix, curr_matrix, sizeof(curr_matrix)) != 0;
    if (changed) memcpy(raw_matrix, curr_matrix, sizeof(curr_matrix));

#ifdef SPLIT_KEYBOARD
    debounce(raw_matrix, matrix + thisHand, ROWS_PER_HAND, changed);
    changed = (changed || matrix_post_scan());
#else
    debounce(raw_matrix, matrix, MATRIX_ROWS, changed);
    matrix_scan_quantum();
#endif
    return (uint8_t)changed;
}
