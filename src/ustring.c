#include <stdint.h>

/* strcmp using uint8_t instead of char
 * needed because not using any std lib (no strcmp)
 */
uint32_t ustrcmp(uint8_t *a, uint8_t *b) {
    while (*a && (*a == *b)) {
        a++;
        b++;
    }
    return *a - *b;
}
