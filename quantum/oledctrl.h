/* Copyright 2020 Albert Diserholt
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

/*
 * Add support for controlling the content of the OLED screen(s) from the
 * operating system via raw HID.
 * Add "OLED_CONTROL_ENABLE=yes" to your rules.mk to build the
 * support needed to send commands from the operating system to the firmware.
 * Add "#define OLEDCTRL_SPLIT" to config.h to build in support for controlling
 * the slave side via the master.
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

#include <stdint.h>
#include <stdbool.h>

#ifndef OLEDCTRL_MSG_MAX_LEN
#    define OLEDCTRL_MSG_MAX_LEN 32
#endif

// Result codes for the commands.
// First byte in messages from the firmware.
enum oledctrl_result_id {
    id_success = 0x00,
    id_failure = 0x01
};

// Type of a message sent from the firmware
// Second byte in messages from the firmware.
enum oledctrl_message_id {
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
 *   Clear the entire screen. Depending on how oled_task_user() looks, this might
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
enum oledctrl_command_id {
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
enum oledctrl_event_id {
    id_event_set_tag = 0x00,
    id_event_increment_tag = 0x01,
    id_event_decrement_tag = 0x02
};

// Which OLED screen to control.
// Typically the fourth byte in messages to and from the firmware.
enum oledctrl_screen_id {
    id_master = 0x00,
    id_slave  = 0x01
};

bool oledctrl_has_content(void);
bool oledctrl_draw(void);

void oledctrl_send_event(uint8_t event, uint8_t *args, uint8_t args_len);

#ifdef OLEDCTRL_SPLIT

typedef struct _oledctrl_syncinfo_t {
    uint8_t msg[OLEDCTRL_MSG_MAX_LEN];
} oledctrl_syncinfo_t;

/* for split keyboard master side */
void oledctrl_send_msg(const uint8_t *msg, uint8_t len);
bool oledctrl_is_msg_pending(void);
void oledctrl_clear_msg_pending(void);
void oledctrl_get_syncinfo(oledctrl_syncinfo_t *syncinfo);
/* for split keyboard slave side */
void oledctrl_update_sync(oledctrl_syncinfo_t *syncinfo);
void oledctrl_receive_msg(uint8_t *msg, uint8_t len);

#endif // OLEDCTRL_SPLIT
