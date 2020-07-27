#include <string.h>
#include "oledctrl.h"

#ifndef MIN
#    define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

// Message from the master to the slave.
static uint8_t oled_ctrl_msg[OLEDCTRL_MSG_MAX_LEN];
// Whether a message is ready to be sent.
static bool oled_ctrl_pending;

/* for split keyboard master side */
void oledctrl_send_msg(const uint8_t *msg, uint8_t len) {
    memcpy(oled_ctrl_msg, msg, MIN(sizeof(oled_ctrl_msg), len));
    oled_ctrl_pending = true;
}

bool oledctrl_is_msg_pending(void) { return oled_ctrl_pending; }

void oledctrl_clear_msg_pending(void) { oled_ctrl_pending = false; }

void oledctrl_get_syncinfo(oledctrl_syncinfo_t *syncinfo) {
    memcpy(syncinfo->msg, oled_ctrl_msg, OLEDCTRL_MSG_MAX_LEN);
}

/* for split keyboard slave side */
void oledctrl_update_sync(oledctrl_syncinfo_t *syncinfo) {
    oledctrl_receive_msg(syncinfo->msg, OLEDCTRL_MSG_MAX_LEN);
}
