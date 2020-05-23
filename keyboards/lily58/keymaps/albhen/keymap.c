#include QMK_KEYBOARD_H

#ifdef PROTOCOL_LUFA
  #include "lufa.h"
  #include "split_util.h"
#endif
#ifdef SSD1306OLED
  #include "ssd1306.h"

  #ifdef CONTROLLABLE_OLEDS
    #ifndef RAW_ENABLE
      #error "RAW_ENABLE must be set"
    #endif
    #include "raw_hid.h"
    #include "split_scomm.h"
    #include "via.h"
  #endif
#endif



#ifdef RGBLIGHT_ENABLE
//Following line allows macro to read current RGB settings
extern rgblight_config_t rgblight_config;
#endif

extern uint8_t is_master;

#define _QWERTY 0
#define _LOWER 1
#define _RAISE 2
#define _ADJUST 3

enum custom_keycodes {
  QWERTY = SAFE_RANGE,
  LOWER,
  RAISE,
  ADJUST,
#ifdef CONTROLLABLE_OLEDS
  // VIA-defined keycodes, which can be added to the keymap below, or mapped in VIA.
  SET_MASTER_TAG = USER00,
  INC_MASTER_TAG = USER01,
  DEC_MASTER_TAG = USER02,
  SET_SLAVE_TAG = USER03,
  INC_SLAVE_TAG = USER04,
  DEC_SLAVE_TAG = USER05,
#endif
};


const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

/* QWERTY
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * | ESC  |   1  |   2  |   3  |   4  |   5  |                    |   6  |   7  |   8  |   9  |   0  |  `   |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * | Tab  |   Q  |   W  |   E  |   R  |   T  |                    |   Y  |   U  |   I  |   O  |   P  |  -   |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |LCTRL |   A  |   S  |   D  |   F  |   G  |-------.    ,-------|   H  |   J  |   K  |   L  |   ;  |  '   |
 * |------+------+------+------+------+------|   [   |    |    ]  |------+------+------+------+------+------|
 * |LShift|   Z  |   X  |   C  |   V  |   B  |-------|    |-------|   N  |   M  |   ,  |   .  |   /  |RShift|
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *                   | LAlt | LGUI |LOWER | /Space  /       \Enter \  |RAISE |BackSP| RGUI |
 *                   |      |      |      |/       /         \      \ |      |      |      |
 *                   `----------------------------'           '------''--------------------'
 */

 [_QWERTY] = LAYOUT( \
  KC_ESC,   KC_1,   KC_2,    KC_3,    KC_4,    KC_5,                     KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_GRV, \
  KC_TAB,   KC_Q,   KC_W,    KC_E,    KC_R,    KC_T,                     KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_MINS, \
  KC_LCTRL, KC_A,   KC_S,    KC_D,    KC_F,    KC_G,                     KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT, \
  KC_LSFT,  KC_Z,   KC_X,    KC_C,    KC_V,    KC_B, KC_LBRC,  KC_RBRC,  KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,  KC_RSFT, \
                             KC_LALT, KC_LGUI,LOWER, KC_SPC,   KC_ENT,   RAISE,   KC_BSPC, KC_RGUI \
),
/* LOWER
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * |      |      |      |      |      |      |                    |      |      |      |      |      |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |  F1  |  F2  |  F3  |  F4  |  F5  |  F6  |                    |  F7  |  F8  |  F9  | F10  | F11  | F12  |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |   `  |   !  |   @  |   #  |   $  |   %  |-------.    ,-------|   ^  |   &  |   *  |   (  |   )  |   -  |
 * |------+------+------+------+------+------|   [   |    |    ]  |------+------+------+------+------+------|
 * |      |      |      |      |      |      |-------|    |-------|      |   _  |   +  |   {  |   }  |   |  |
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *                   | LAlt | LGUI |LOWER | /Space  /       \Enter \  |RAISE |BackSP| RGUI |
 *                   |      |      |      |/       /         \      \ |      |      |      |
 *                   `----------------------------'           '------''--------------------'
 */
[_LOWER] = LAYOUT( \
  _______, _______, _______, _______, _______, _______,                   _______, _______, _______,_______, _______, _______,\
  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,                     KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12, \
  KC_GRV, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC,                   KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_TILD, \
  _______, _______, _______, _______, _______, _______, _______, _______, XXXXXXX, KC_UNDS, KC_PLUS, KC_LCBR, KC_RCBR, KC_PIPE, \
                             _______, _______, _______, _______, _______,  _______, _______, _______\
),
/* RAISE
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * |      |      |      |      |      |      |                    |      |      |      |      |      |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |   `  |   1  |   2  |   3  |   4  |   5  |                    |   6  |   7  |   8  |   9  |   0  |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |  F1  |  F2  |  F3  |  F4  |  F5  |  F6  |-------.    ,-------|      | Left | Down |  Up  |Right |      |
 * |------+------+------+------+------+------|   [   |    |    ]  |------+------+------+------+------+------|
 * |  F7  |  F8  |  F9  | F10  | F11  | F12  |-------|    |-------|   +  |   -  |   =  |   [  |   ]  |   \  |
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *                   | LAlt | LGUI |LOWER | /Space  /       \Enter \  |RAISE |BackSP| RGUI |
 *                   |      |      |      |/       /         \      \ |      |      |      |
 *                   `----------------------------'           '------''--------------------'
 */

[_RAISE] = LAYOUT( \
  _______, _______, _______, _______, _______, _______,                     _______, _______, _______, _______, _______, _______, \
  KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                        KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    _______, \
  KC_F1,  KC_F2,    KC_F3,   KC_F4,   KC_F5,   KC_F6,                       XXXXXXX, KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, XXXXXXX, \
  KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,   _______, _______,  KC_PLUS, KC_MINS, KC_EQL,  KC_LBRC, KC_RBRC, KC_BSLS, \
                             _______, _______, _______,  _______, _______,  _______, _______, _______ \
),
/* ADJUST
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * |      |      |      |      |      |      |                    |      |      |      |      |      |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |      |      |      |      |      |                    |      |      |      |      |      |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |      |      |      |      |      |-------.    ,-------|      |      |RGB ON| HUE+ | SAT+ | VAL+ |
 * |------+------+------+------+------+------|       |    |       |------+------+------+------+------+------|
 * |      |      |      |      |      |      |-------|    |-------|      |      | MODE | HUE- | SAT- | VAL- |
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *                   | LAlt | LGUI |LOWER | /Space  /       \Enter \  |RAISE |BackSP| RGUI |
 *                   |      |      |      |/       /         \      \ |      |      |      |
 *                   `----------------------------'           '------''--------------------'
 */
  [_ADJUST] = LAYOUT( \
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                   XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, \
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                   XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, \
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                   XXXXXXX, XXXXXXX, RGB_TOG, RGB_HUI, RGB_SAI, RGB_VAI, \
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, RGB_MOD, RGB_HUD, RGB_SAD, RGB_VAD,\
                             _______, _______, _______, _______, _______,  _______, _______, _______ \
  )
};

int RGB_current_mode;

// Setting ADJUST layer RGB back to default
void update_tri_layer_RGB(uint8_t layer1, uint8_t layer2, uint8_t layer3) {
  if (IS_LAYER_ON(layer1) && IS_LAYER_ON(layer2)) {
    layer_on(layer3);
  } else {
    layer_off(layer3);
  }
}

void matrix_init_user(void) {
    #ifdef RGBLIGHT_ENABLE
      RGB_current_mode = rgblight_config.mode;
    #endif
    //SSD1306 OLED init, make sure to add #define SSD1306OLED in config.h
    #ifdef SSD1306OLED
        iota_gfx_init(false);   // turns on the display
    #endif
}

//SSD1306 OLED update loop, make sure to add #define SSD1306OLED in config.h
#ifdef SSD1306OLED

// When add source files to SRC in rules.mk, you can use functions.
const char *read_layer_state(void);
const char *read_logo(void);
//void set_keylog(uint16_t keycode, keyrecord_t *record);
//const char *read_keylog(void);
//const char *read_keylogs(void);

// const char *read_mode_icon(bool swap);
// const char *read_host_led_state(void);
// void set_timelog(void);
// const char *read_timelog(void);

#ifdef CONTROLLABLE_OLEDS

/*
 * Add support for controlling the content of the OLED screen(s) from the
 * operating system via raw HID.
 * Add "EXTRAFLAGS += -DCONTROLLABLE_OLEDS" to your rules.mk to build the
 * support needed to send commands from the operating system to the firmware.
 *
 * Data sent to and from the firmware contains a four-byte header, necessary to
 * bypass VIA if it is enabled, and to handle different commands and screens.
 * This header is necessary and needs to be correct for the commands to be
 * recognised and properly processed. The first two bytes should be 0x02 and
 * 0x00 respectively, used to bypass VIA.  The third byte is a command found in
 * oled_command_id, and the fourth byte is one of the screens in
 * oled_screen_id.
 * For example: 0x02 0x00 0x01 0x00 means that the master screen should be
 * cleared.
 *
 * Commands (sent to firmware):
 * - Byte 1: 0x02
 * - Byte 2: 0x00
 * - Byte 3: oled_command_id
 * - Byte 4: oled_screen_id
 * - Byte ...: Depends on command; see below.
 *
 * Commands should be sent with a short delay in between, e.g. 10 milliseconds,
 * especially when controlling the slave screen, or some commands might get
 * lost.
 *
 * For messages sent by the firmware, the first byte contains a result code in
 * oled_result_id, the second byte whether the message is a response to a
 * command or an event triggered by a keycode as oled_message_id, the third
 * byte is the command the response is for (oled_command_id), or the event
 * (oled_event_id). The following bytes depend on the type of the response or
 * event. For responses, generally the same thing is sent back as was passed
 * in. See the documentation below.
 *
 * Response (sent from firmware):
 * - Byte 1: oled_result_id
 * - Byte 2: 0x00
 * - Byte 3: oled_command_id
 * - Byte 4: oled_screen_id
 * - Byte ...: Depends on response; see below.
 *
 * Event (sent from firmware):
 * - Byte 1: 0x00
 * - Byte 2: 0x01
 * - Byte 3: oled_event_id
 * - Byte 4: oled_screen_id
 * - Byte ...: Depends on event; see below.
 *
 * Note: The raw HID protocol can only support frames of 32 bytes, and minus
 * the necessary header, it means that 28 characters are currently the maximum
 * number of characters that can be written per line. The SSD1306 OLED supports
 * 21 characters per line.
 */

// Result codes for the commands.
// First byte in messages from the firmware.
enum oled_result_id {
  id_success = 0x00,
  id_failure = 0x01
};

// Type of a message sent from the firmware
// Second byte in messages from the firmware.
enum oled_message_id {
  id_response = 0x00,
  id_event = 0x01
};

/*
 * The commands that are recognised to control the OLEDs.
 * Third byte in messages to and from the firmware.
 * id_set_up:
 *   This is equivalent to clear, but also returns the number of characters
 *   that can be written to the screen in the fifth and sixth byte of the
 *   response (columns and rows, respectively).
 *
 * id_clear:
 *   Clear the entire screen. Depending on how matrix_update() looks, this might
 *   revert the screen to its "default" (uncontrolled) state.
 *
 * id_set_line:
 *   Set the content of a line/row of the screen. The fifth byte should be which
 *   line to modify, and the sixth byte and onwards should be content that is
 *   desired on that line. Will replace everything on that line, and will not
 *   wrap to the next one.
 *
 * id_present:
 *   Show the changes to the screen. This has to be called after the desired
 *   number of id_set_line commands have been issued. Think of it as double
 *   buffering, where this command will switch the front and back buffer.
 */
enum oled_command_id {
  id_set_up = 0x00,
  id_clear = 0x01,
  id_set_line = 0x02,
  id_present = 0x03
};

/*
 * The events that the firmware might send, triggered by certain keycodes
 * (see custom_keycodes above).
 * They are the third byte in messages sent from the firmware.
 * id_event_set_tag:
 *   A specific tag is requested to be shown on a specific screen. The fourth
 *   byte is the screen (oled_screen_id below), and the fifth is the tag ID.
 *   Note that KC_1 => tag ID 1.
 *
 * id_event_increment_tag:
 *   Go to the next tag for a specific screen. The fourth byte is the screen
 *   (oled_screen_id below).
 *
 * id_event_decrement_tag:
 *   Go to the previous tag for a specific screen. The fourth byte is the
 *   screen (oled_screen_id below).
 */
enum oled_event_id {
  id_event_set_tag = 0x00,
  id_event_increment_tag = 0x01,
  id_event_decrement_tag = 0x02
};

// Which OLED screen to control.
// Typically the fourth byte in messages to and from the firmware.
enum oled_screen_id {
  id_master = 0x00,
  id_slave  = 0x01
};

// The size of the screen buffer.
#define SCREEN_BUFFER_LENGTH (MatrixCols * MatrixRows)

// The current text shown on the screen (front buffer).
static char current_screen[SCREEN_BUFFER_LENGTH + 1] = {0};
// The back buffer containing the text to show when presenting.
static char screen_buffer[SCREEN_BUFFER_LENGTH + 1] = {0};
// Whether the screen buffer contains variables.
static bool screen_has_variable = false;

// Handle raw HID messages.
// The signature looks a bit different depending on whether VIA is enabled.
#ifdef VIA_ENABLE
void raw_hid_receive_kb(uint8_t *data, uint8_t length) {
#else
void raw_hid_receive(uint8_t *data, uint8_t length) {
#endif
  if(!data || length < 2)
    return;

  // First two bytes in the header are sent for VIA compatibility, and used
  // as the result code and message type for the response.
  enum oled_result_id *result = &data[0];
  *result = id_failure;
  enum oled_message_id *message = &data[1];
  *message = id_response;

  if (length > 3) {
    // The next two bytes are the command and screen, followed by any data.
    uint8_t command_id = data[2];
    enum oled_screen_id screen = data[3];
    uint8_t *command_data = &data[4];
    uint8_t command_length = length - 5;

    if (screen == id_slave && is_master) {
      // Command for the slave, send it over the serial.
      memcpy(hid_message, data, MIN(sizeof(hid_message), length));
      hid_message_pending = true;
      *result = id_success;
    }
    else if (screen == id_master || (screen == id_slave && !is_master)) {
      switch (command_id) {
        case id_set_up: // Return the size of the screen.
          command_data[0] = MatrixCols;
          command_data[1] = MatrixRows;
          // Fall through
        case id_clear: // Clear the buffers
          memset(current_screen, 0, sizeof(current_screen));
          memset(screen_buffer, ' ', sizeof(screen_buffer));
          *result = id_success;
          break;
        case id_set_line: { // Set a line of text.
          uint8_t line = command_data[0];
          char *text = (char*)&command_data[1];
          if (line >= MatrixRows) // Line out of bounds
            break;

          uint8_t len = MIN(strnlen(text, command_length), MatrixCols);
          memcpy(&screen_buffer[line * MatrixCols], text, len);
          if (len < MatrixCols) {
            // Erase everything else until the end of the line.
            memset(&screen_buffer[line * MatrixCols + len], ' ', MatrixCols - len);
          }
          *result = id_success;
          break;
        }
        case id_present: // Copy the content of the back buffer to the front buffer.
          memcpy(current_screen, screen_buffer, SCREEN_BUFFER_LENGTH);
          current_screen[sizeof(current_screen) - 1] = 0;
          *result = id_success;
          screen_has_variable = strchr(current_screen, '%');
          break;
      }
    }
  }

  if (!is_master)
    return; // Slave cannot answer to HID messages

#ifndef VIA_ENABLE
  // VIA module will always do this, so might as well replicate it here.
  raw_hid_send(data, length);
#endif
}

// Byte to keep track of two-step tag selection on a certain screen.
static uint8_t processing_screen_command = 0xFF;
// Buffer used when sending HID events.
static uint8_t hid_event_buffer[32] = {0};
// Send an event over HID raw.
static void send_hid_event(uint8_t event, uint8_t *args, uint8_t len) {
  hid_event_buffer[0] = id_success;
  hid_event_buffer[1] = id_event;
  hid_event_buffer[2] = event;
  memcpy(&hid_event_buffer[3], args, MIN(len, sizeof(hid_event_buffer) - 4));
  raw_hid_send(hid_event_buffer, sizeof(hid_event_buffer));
}

// Buffer for substituting variables with text.
static char var_buffer[MatrixCols];
/**
 * Write the front buffer to the OLED screen.
 * This method supports some simple substitutions of variables inside the
 * lines. For example, "My current layer: %l" will replace "%l" with the name
 * of the current layer. Note that if the line becomes too long for it to fit
 * on the screen, text will be cut off at the end.
 *
 * Current handled variables:
 *   %l  -  The layer name
 */
static void write_screen(struct CharacterMatrix *matrix) {
  if (!screen_has_variable) {
    matrix_write(matrix, current_screen);
  } else {
    // A bit convoluted, but it works.
    for (int row = 0; row < MatrixRows; ++row) {
      for (int col = 0, i = row * MatrixCols; col < MatrixCols; ++col, ++i) {
        // Don't write the final null character, or the first line will disappear.
        if (current_screen[i] == 0) {
          break;
        } else if (current_screen[i] == '%' && i + 1 < sizeof(current_screen)) {
          int len = 0;
          switch(current_screen[i+1]) {
            case 'l': {
              switch (get_highest_layer(layer_state)) {
                case _QWERTY:
                  len = snprintf(var_buffer, sizeof(var_buffer), "Default");
                  break;
                case _LOWER:
                  len = snprintf(var_buffer, sizeof(var_buffer), "Lower");
                  break;
                case _RAISE:
                  len = snprintf(var_buffer, sizeof(var_buffer), "Raise");
                  break;
                case _ADJUST:
                  len = snprintf(var_buffer, sizeof(var_buffer), "Adjust");
                  break;
                default:
                  len = snprintf(var_buffer, sizeof(var_buffer), "Undef-%ld", layer_state);
                  break;
              }
              break;
            }
            /* Add other substitutions here. */
          }

          if (len > 0) {
            // A substitution occurred. The content of the var_buffer will
            // either be printed in full, or until the end of the line.
            len = MIN(MIN(len, MatrixCols - col), sizeof(var_buffer) - 1);
            var_buffer[len] = 0; // Chop if off if necessary.
            matrix_write(matrix, var_buffer);
            col += len - 1; // Will be incremented once more by the loop.
            ++i; // Skip the variable character.
            continue;
          }
        }

        // Write the character normally to the screen.
        matrix_write_char(matrix, current_screen[i]);
      }
    }
  }
}

#endif // CONTROLLABLE_OLEDS

void matrix_scan_user(void) {
   iota_gfx_task();
}

void matrix_render_user(struct CharacterMatrix *matrix) {
  if (is_master) {
#ifdef CONTROLLABLE_OLEDS
    // If the screen has been filled by the operating system, then use that,
    // otherwise revert to the default.
    if (current_screen[0] != 0) {
      write_screen(matrix);
      return;
    }
#endif // CONTROLLABLE_OLEDS
    // If you want to change the display of OLED, you need to change here
    matrix_write_ln(matrix, read_layer_state());
    //matrix_write_ln(matrix, read_keylog());
    //matrix_write_ln(matrix, read_keylogs());
    //matrix_write_ln(matrix, read_mode_icon(keymap_config.swap_lalt_lgui));
    //matrix_write_ln(matrix, read_host_led_state());
    //matrix_write_ln(matrix, read_timelog());
  } else {
#ifdef CONTROLLABLE_OLEDS
    // Process any message from the master.
    if (hid_message[0] != 0) {
#ifdef VIA_ENABLE
      raw_hid_receive_kb(hid_message, sizeof(hid_message));
#else
      raw_hid_receive(hid_message, sizeof(hid_message));
#endif
      memset(hid_message, 0, sizeof(hid_message));
    }
    // If the screen has been filled by the operating system, then use that,
    // otherwise revert to the default.
    if (current_screen[0] != 0) {
      write_screen(matrix);
      return;
    }
#endif
    matrix_write(matrix, read_logo());
  }
}

void matrix_update(struct CharacterMatrix *dest, const struct CharacterMatrix *source) {
  if (memcmp(dest->display, source->display, sizeof(dest->display))) {
    memcpy(dest->display, source->display, sizeof(dest->display));
    dest->dirty = true;
  }
}

void iota_gfx_task_user(void) {
  struct CharacterMatrix matrix;
  matrix_clear(&matrix);
  matrix_render_user(&matrix);
  matrix_update(&display, &matrix);
}
#endif//SSD1306OLED

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  if (record->event.pressed) {
#ifdef SSD1306OLED
    //set_keylog(keycode, record);
#endif
    // set_timelog();
  }

  switch (keycode) {
    case QWERTY:
      if (record->event.pressed) {
        set_single_persistent_default_layer(_QWERTY);
      }
      return false;
      break;
    case LOWER:
      if (record->event.pressed) {
        layer_on(_LOWER);
        update_tri_layer_RGB(_LOWER, _RAISE, _ADJUST);
      } else {
        layer_off(_LOWER);
        update_tri_layer_RGB(_LOWER, _RAISE, _ADJUST);
      }
      return false;
      break;
    case RAISE:
      if (record->event.pressed) {
        layer_on(_RAISE);
        update_tri_layer_RGB(_LOWER, _RAISE, _ADJUST);
      } else {
        layer_off(_RAISE);
        update_tri_layer_RGB(_LOWER, _RAISE, _ADJUST);
      }
      return false;
      break;
    case ADJUST:
        if (record->event.pressed) {
          layer_on(_ADJUST);
        } else {
          layer_off(_ADJUST);
        }
        return false;
        break;
#ifdef CONTROLLABLE_OLEDS
    // Two-step tag selection. First part is to invoke the selection keycode.
    case SET_MASTER_TAG:
    case SET_SLAVE_TAG:
        processing_screen_command = (keycode == SET_MASTER_TAG) ? id_master : id_slave;
        return false;
        break;
    // Keys to increase or decrease the tag on a certain screen.
    case INC_MASTER_TAG:
    case DEC_MASTER_TAG:
    case INC_SLAVE_TAG:
    case DEC_SLAVE_TAG:
        if (record->event.pressed) {
          uint8_t screen = (keycode == INC_MASTER_TAG || keycode == DEC_MASTER_TAG) ? id_master : id_slave;
          uint8_t event = (keycode == INC_MASTER_TAG || keycode == INC_SLAVE_TAG) ? id_event_increment_tag : id_event_decrement_tag;
          send_hid_event(event, &screen, 1);
        }
        return false;
        break;
#endif
  }

#ifdef CONTROLLABLE_OLEDS
  // Two-step tag selection. Second part is to get a numerical button press.
  // The number will not be printed out. If a non-numerical button is pressed,
  // it will be printed out and the tag selection will stop.
  if (processing_screen_command != 0xFF && record->event.pressed) {
    uint8_t screen = processing_screen_command;
    uint8_t tag = 0xFF;
    processing_screen_command = 0xFF;

    switch (keycode) {
      case KC_1 ... KC_9:
        tag = (keycode - KC_1) + 1;
        break;
      case KC_0:
        tag = 0;
        break;
    }
    if (tag != 0xFF) {
      uint8_t args[] = { screen, tag };
      send_hid_event(id_event_set_tag, args, sizeof(args));
      return false;
    }
  }
#endif

  return true;
}

// vim: et:ts=2:sw=2:sts=2
