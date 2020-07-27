#ifndef OLEDCTRL_H
#define OLEDCTRL_H

#ifndef OLEDCTRL_H_DUMMY_DEFINE

#    include <stdint.h>
#    include <stdbool.h>

#    ifndef OLEDCTRL_MSG_MAX_LEN
#        define OLEDCTRL_MSG_MAX_LEN 32
#    endif

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

#endif // #ifndef OLEDCTRL_H_DUMMY_DEFINE
#endif // OLEDCTRL_H
