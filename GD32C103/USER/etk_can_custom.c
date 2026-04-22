#include "etk_can_custom.h"
/* ... lots of signal mapping logic ... */
void custom_timer(void) {
    /* diagnostic message 2000ms logic */
    static uint32_t diag_cnt = 0;
    if(++diag_cnt >= 2000) {
        diag_cnt = 0;
        /* send 0x700 */
    }
}
