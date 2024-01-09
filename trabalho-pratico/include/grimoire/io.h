// NOTE: Keypresses cannot be captured, only character sequences. Modifier keys are not sequences.
// WARN: Non-exaustive list. Add keys as required.

#ifndef TP_GRIMOIRE_IO_H
#define TP_GRIMOIRE_IO_H

#include <stdint.h>
#include <ctype.h>

#include "grimoire/grimoire_public.h"

typedef uint16_t GM_Key;

// MAX KEY INT: 2047
#define GM_MOD_CLEAR  0x07FF // 0b00011111111111
#define GM_MOD_SHIFT  0x0800 // 0b00100000000000
#define GM_MOD_CTRL   0x1000 // 0b01000000000000
#define GM_MOD_ALT    0x2000 // 0b10000000000000

#define GM_HAS_SHIFT(key) ((key) & GM_MOD_SHIFT)
#define GM_HAS_CTRL(key)  ((key) & GM_MOD_CTRL)
#define GM_HAS_ALT(key)   ((key) & GM_MOD_ALT)
#define GM_CANON_KEY(key) ((key) & GM_MOD_CLEAR) /** @brief Gets the true key, stripped of modifiers. */

// TODO: Add HOME, etc.
#define GM_KEY_NUL 0
#define GM_KEY_ENTER 10
#define GM_KEY_ESCAPE 27
#define GM_KEY_SPACE 32
#define GM_KEY_BACKSPACE 127
#define GM_KEY_DELETE 177

#define GM_KEY_F1  (0xFF + 1) 
#define GM_KEY_F2  (0xFF + 2) 
#define GM_KEY_F3  (0xFF + 3) 
#define GM_KEY_F4  (0xFF + 4) 
#define GM_KEY_F5  (0xFF + 5) 
#define GM_KEY_F6  (0xFF + 6) 
#define GM_KEY_F7  (0xFF + 7) 
#define GM_KEY_F8  (0xFF + 8) 
#define GM_KEY_F9  (0xFF + 9) 
#define GM_KEY_F10 (0xFF + 10)
#define GM_KEY_F11 (0xFF + 11)
#define GM_KEY_F12 (0xFF + 12)

#define GM_KEY_ARROW_UP    0xFF + 13
#define GM_KEY_ARROW_DOWN  0xFF + 14
#define GM_KEY_ARROW_RIGHT 0xFF + 15
#define GM_KEY_ARROW_LEFT  0xFF + 16
#define GM_KEY_CTRL_A 1
#define GM_KEY_CTRL_B 2
#define GM_KEY_CTRL_C 3
#define GM_KEY_CTRL_D 4
#define GM_KEY_CTRL_E 5
#define GM_KEY_CTRL_F 6
#define GM_KEY_CTRL_G 7
#define GM_KEY_CTRL_H 8
#define GM_KEY_CTRL_I 9
#define GM_KEY_CTRL_J 10
#define GM_KEY_CTRL_K 11
#define GM_KEY_CTRL_L 12
#define GM_KEY_CTRL_M 13
#define GM_KEY_CTRL_N 14
#define GM_KEY_CTRL_O 15
#define GM_KEY_CTRL_P 16
#define GM_KEY_CTRL_Q 17
#define GM_KEY_CTRL_R 18
#define GM_KEY_CTRL_S 19
#define GM_KEY_CTRL_T 20
#define GM_KEY_CTRL_U 21
#define GM_KEY_CTRL_V 22
#define GM_KEY_CTRL_W 23
#define GM_KEY_CTRL_X 24
#define GM_KEY_CTRL_Y 25
#define GM_KEY_CTRL_Z 26

#define GM_KEY_IS_ALPHANUMERIC(key) (isalnum(GM_CANON_KEY(key)) || (GM_CANON_KEY(key)) == ' ')
#define GM_KEY_IS_PRINTABLE(key) (isprint(GM_CANON_KEY(key)) || (GM_CANON_KEY(key)) == ' ')
#define GM_KEY_IS_FUNCTION(key) ((int)(GM_CANON_KEY(key)) >= GM_KEY_F1 && (int)(GM_CANON_KEY(key)) <= GM_KEY_F12)
#define GM_KEY_IS_ARROW(key) ((int)(GM_CANON_KEY(key)) >= GM_KEY_ARROW_UP && (int)(GM_CANON_KEY(key)) <= GM_KEY_ARROW_LEFT)
#define GM_IS_CTRL(key) ((int)(GM_CANON_KEY(key)) >= GM_KEY_CTRL_A && (int)(GM_CANON_KEY(key)) <= GM_KEY_CTRL_Z)

/**
 * @brief Checks whenever input is available on then input stream. 
 */
int gm_kbhit();

/**
 * @brief Fetches a single key from the input stream.
 */
char gm_getch();

/**
 * @brief Fetches the entire input stream and reads.
 * 
 * WARN: If @ref gm_kbhit is not constantly called and the key is handled, it might attempt to read multiple keys.
 * It currently is undefined behavior.
 */
GM_Key gm_get_key(GM_Term term);

/**
 * @brief Returns the true key from @ref gm_get_key. Modifiers are attempted to be resolved, otherwise they are stripped.
 */
GM_Key gm_get_canonical_key(GM_Key key);

/**
 * @brief Hides the cursor on the terminal.
 * TODO: Move to somewhere else better.
 */
void gm_hide_cursor(GM_Term term);

/**
 * @brief Shows the cursor on the terminal.
 * TODO: Move to somewhere else better.
 */
void gm_show_cursor(GM_Term term);

/**
 * @brief Clears the screen.
 * 
 * @param flush If enabled, stdout will be flushed.
 */
void gm_clear(int flush);

/**
 * @brief Sets the cursor at it's corresponding X and Y position on the terminal.
 * 
 * @param x The column of the terminal.
 * @param y The row of the terminal.
 * @param flush If enabled, stdout will be flushed.
 */
void gm_gotoxy(int x, int y, int flush);

/**
 * @brief Resets all current ANSI attributes.
 * 
 * @param flush If enabled, stdout will be flushed.
 */
void gm_reset_attr(int flush);

#endif