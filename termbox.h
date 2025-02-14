/*
MIT License

Copyright (c) 2010-2020 nsf <no.smile.face@gmail.com>
              2015-2022 Adam Saponara <as@php.net>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
/*
	Modified by: Minho Jo <whitestone8214@gmail.com>
*/


#ifndef __TERMBOX_H
#define __TERMBOX_H

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE
#endif

#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE
#endif

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <signal.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
#include <wchar.h>

#ifdef __cplusplus
extern "C" {
#endif

// __ffi_start

#define TB_VERSION_STR "2.1.0-dev"

#if defined(TB_LIB_OPTS) || 0 // __tb_lib_opts
// Ensure consistent compile-time options when using as a library
#undef TB_OPT_TRUECOLOR
#undef TB_OPT_EGC
#undef TB_OPT_PRINTF_BUF
#undef TB_OPT_READ_BUF
#define TB_OPT_TRUECOLOR
#define TB_OPT_EGC
#endif

/* ASCII key constants (tb_event.key) */
#define TB_KEY_CTRL_TILDE       0x00
#define TB_KEY_CTRL_2           0x00 /* clash with 'CTRL_TILDE'     */
#define TB_KEY_CTRL_A           0x01
#define TB_KEY_CTRL_B           0x02
#define TB_KEY_CTRL_C           0x03
#define TB_KEY_CTRL_D           0x04
#define TB_KEY_CTRL_E           0x05
#define TB_KEY_CTRL_F           0x06
#define TB_KEY_CTRL_G           0x07
#define TB_KEY_BACKSPACE        0x08
#define TB_KEY_CTRL_H           0x08 /* clash with 'CTRL_BACKSPACE' */
#define TB_KEY_TAB              0x09
#define TB_KEY_CTRL_I           0x09 /* clash with 'TAB'            */
#define TB_KEY_CTRL_J           0x0a
#define TB_KEY_CTRL_K           0x0b
#define TB_KEY_CTRL_L           0x0c
#define TB_KEY_ENTER            0x0d
#define TB_KEY_CTRL_M           0x0d /* clash with 'ENTER'          */
#define TB_KEY_CTRL_N           0x0e
#define TB_KEY_CTRL_O           0x0f
#define TB_KEY_CTRL_P           0x10
#define TB_KEY_CTRL_Q           0x11
#define TB_KEY_CTRL_R           0x12
#define TB_KEY_CTRL_S           0x13
#define TB_KEY_CTRL_T           0x14
#define TB_KEY_CTRL_U           0x15
#define TB_KEY_CTRL_V           0x16
#define TB_KEY_CTRL_W           0x17
#define TB_KEY_CTRL_X           0x18
#define TB_KEY_CTRL_Y           0x19
#define TB_KEY_CTRL_Z           0x1a
#define TB_KEY_ESC              0x1b
#define TB_KEY_CTRL_LSQ_BRACKET 0x1b /* clash with 'ESC'            */
#define TB_KEY_CTRL_3           0x1b /* clash with 'ESC'            */
#define TB_KEY_CTRL_4           0x1c
#define TB_KEY_CTRL_BACKSLASH   0x1c /* clash with 'CTRL_4'         */
#define TB_KEY_CTRL_5           0x1d
#define TB_KEY_CTRL_RSQ_BRACKET 0x1d /* clash with 'CTRL_5'         */
#define TB_KEY_CTRL_6           0x1e
#define TB_KEY_CTRL_7           0x1f
#define TB_KEY_CTRL_SLASH       0x1f /* clash with 'CTRL_7'         */
#define TB_KEY_CTRL_UNDERSCORE  0x1f /* clash with 'CTRL_7'         */
#define TB_KEY_SPACE            0x20
#define TB_KEY_BACKSPACE2       0x7f
#define TB_KEY_CTRL_8           0x7f /* clash with 'BACKSPACE2'     */

#define tb_key_i(i)             0xffff - (i)
/* Terminal-dependent key constants (tb_event.key) and terminfo capabilities */
/* BEGIN codegen h */
/* Produced by ./codegen.sh on Sun, 19 Sep 2021 01:02:02 +0000 */
#define TB_KEY_F1               (0xffff - 0)
#define TB_KEY_F2               (0xffff - 1)
#define TB_KEY_F3               (0xffff - 2)
#define TB_KEY_F4               (0xffff - 3)
#define TB_KEY_F5               (0xffff - 4)
#define TB_KEY_F6               (0xffff - 5)
#define TB_KEY_F7               (0xffff - 6)
#define TB_KEY_F8               (0xffff - 7)
#define TB_KEY_F9               (0xffff - 8)
#define TB_KEY_F10              (0xffff - 9)
#define TB_KEY_F11              (0xffff - 10)
#define TB_KEY_F12              (0xffff - 11)
#define TB_KEY_INSERT           (0xffff - 12)
#define TB_KEY_DELETE           (0xffff - 13)
#define TB_KEY_HOME             (0xffff - 14)
#define TB_KEY_END              (0xffff - 15)
#define TB_KEY_PGUP             (0xffff - 16)
#define TB_KEY_PGDN             (0xffff - 17)
#define TB_KEY_ARROW_UP         (0xffff - 18)
#define TB_KEY_ARROW_DOWN       (0xffff - 19)
#define TB_KEY_ARROW_LEFT       (0xffff - 20)
#define TB_KEY_ARROW_RIGHT      (0xffff - 21)
#define TB_KEY_BACK_TAB         (0xffff - 22)
#define TB_KEY_MOUSE_LEFT       (0xffff - 23)
#define TB_KEY_MOUSE_RIGHT      (0xffff - 24)
#define TB_KEY_MOUSE_MIDDLE     (0xffff - 25)
#define TB_KEY_MOUSE_RELEASE    (0xffff - 26)
#define TB_KEY_MOUSE_WHEEL_UP   (0xffff - 27)
#define TB_KEY_MOUSE_WHEEL_DOWN (0xffff - 28)

#define TB_CAP_F1               0
#define TB_CAP_F2               1
#define TB_CAP_F3               2
#define TB_CAP_F4               3
#define TB_CAP_F5               4
#define TB_CAP_F6               5
#define TB_CAP_F7               6
#define TB_CAP_F8               7
#define TB_CAP_F9               8
#define TB_CAP_F10              9
#define TB_CAP_F11              10
#define TB_CAP_F12              11
#define TB_CAP_INSERT           12
#define TB_CAP_DELETE           13
#define TB_CAP_HOME             14
#define TB_CAP_END              15
#define TB_CAP_PGUP             16
#define TB_CAP_PGDN             17
#define TB_CAP_ARROW_UP         18
#define TB_CAP_ARROW_DOWN       19
#define TB_CAP_ARROW_LEFT       20
#define TB_CAP_ARROW_RIGHT      21
#define TB_CAP_BACK_TAB         22
#define TB_CAP__COUNT_KEYS      23
#define TB_CAP_ENTER_CA         23
#define TB_CAP_EXIT_CA          24
#define TB_CAP_SHOW_CURSOR      25
#define TB_CAP_HIDE_CURSOR      26
#define TB_CAP_CLEAR_SCREEN     27
#define TB_CAP_SGR0             28
#define TB_CAP_UNDERLINE        29
#define TB_CAP_BOLD             30
#define TB_CAP_BLINK            31
#define TB_CAP_ITALIC           32
#define TB_CAP_REVERSE          33
#define TB_CAP_ENTER_KEYPAD     34
#define TB_CAP_EXIT_KEYPAD      35
#define TB_CAP__COUNT           36
/* END codegen h */

/* Some hard-coded caps */
#define TB_HARDCAP_ENTER_MOUSE  "\x1b[?1000h\x1b[?1002h\x1b[?1015h\x1b[?1006h"
#define TB_HARDCAP_EXIT_MOUSE   "\x1b[?1006l\x1b[?1015l\x1b[?1002l\x1b[?1000l"

/* Colors (numeric) and attributes (bitwise) (tb_cell.fg, tb_cell.bg) */
#define TB_BLACK                0x0001
#define TB_RED                  0x0002
#define TB_GREEN                0x0003
#define TB_YELLOW               0x0004
#define TB_BLUE                 0x0005
#define TB_MAGENTA              0x0006
#define TB_CYAN                 0x0007
#define TB_WHITE                0x0008
#define TB_BOLD                 0x0100
#define TB_UNDERLINE            0x0200
#define TB_REVERSE              0x0400
#define TB_ITALIC               0x0800
#define TB_BLINK                0x1000
#define TB_DEFAULT              0x2000
#ifdef TB_OPT_TRUECOLOR
#define TB_TRUECOLOR_BOLD      0x01000000
#define TB_TRUECOLOR_UNDERLINE 0x02000000
#define TB_TRUECOLOR_REVERSE   0x04000000
#define TB_TRUECOLOR_ITALIC    0x08000000
#define TB_TRUECOLOR_BLINK     0x10000000
#define TB_TRUECOLOR_DEFAULT   0x20000000
#endif

/* Event types (tb_event.type) */
#define TB_EVENT_KEY        1
#define TB_EVENT_RESIZE     2
#define TB_EVENT_MOUSE      3

/* Key modifiers (bitwise) (tb_event.mod) */
#define TB_MOD_ALT          1
#define TB_MOD_CTRL         2
#define TB_MOD_SHIFT        4
#define TB_MOD_MOTION       8

/* Input modes (bitwise) (tb_set_input_mode) */
#define TB_INPUT_CURRENT    0
#define TB_INPUT_ESC        1
#define TB_INPUT_ALT        2
#define TB_INPUT_MOUSE      4

/* Output modes (tb_set_output_mode) */
#define TB_OUTPUT_CURRENT   0
#define TB_OUTPUT_NORMAL    1
#define TB_OUTPUT_256       2
#define TB_OUTPUT_216       3
#define TB_OUTPUT_GRAYSCALE 4
#ifdef TB_OPT_TRUECOLOR
#define TB_OUTPUT_TRUECOLOR 5
#endif

/* Common function return values unless otherwise noted.
 *
 * Library behavior is undefined after receiving TB_ERR_MEM. Callers may
 * attempt reinitializing by freeing memory, invoking tb_shutdown, then
 * tb_init.
 */
#define TB_OK                   0
#define TB_ERR                  -1
#define TB_ERR_NEED_MORE        -2
#define TB_ERR_INIT_ALREADY     -3
#define TB_ERR_INIT_OPEN        -4
#define TB_ERR_MEM              -5
#define TB_ERR_NO_EVENT         -6
#define TB_ERR_NO_TERM          -7
#define TB_ERR_NOT_INIT         -8
#define TB_ERR_OUT_OF_BOUNDS    -9
#define TB_ERR_READ             -10
#define TB_ERR_RESIZE_IOCTL     -11
#define TB_ERR_RESIZE_PIPE      -12
#define TB_ERR_RESIZE_SIGACTION -13
#define TB_ERR_POLL             -14
#define TB_ERR_TCGETATTR        -15
#define TB_ERR_TCSETATTR        -16
#define TB_ERR_UNSUPPORTED_TERM -17
#define TB_ERR_RESIZE_WRITE     -18
#define TB_ERR_RESIZE_POLL      -19
#define TB_ERR_RESIZE_READ      -20
#define TB_ERR_RESIZE_SSCANF    -21
#define TB_ERR_CAP_COLLISION    -22

#define TB_ERR_SELECT           TB_ERR_POLL
#define TB_ERR_RESIZE_SELECT    TB_ERR_RESIZE_POLL

/* Function types to be used with tb_set_func() */
#define TB_FUNC_EXTRACT_PRE     0
#define TB_FUNC_EXTRACT_POST    1

/* Define this to set the size of the buffer used in tb_printf()
 * and tb_sendf()
 */
#ifndef TB_OPT_PRINTF_BUF
#define TB_OPT_PRINTF_BUF 4096
#endif

/* Define this to set the size of the read buffer used when reading
 * from the tty
 */
#ifndef TB_OPT_READ_BUF
#define TB_OPT_READ_BUF 64
#endif

/* Define this for limited back compat with termbox v1 */
#ifdef TB_OPT_V1_COMPAT
#define tb_change_cell          tb_set_cell
#define tb_put_cell(x, y, c)    tb_set_cell((x), (y), (c)->ch, (c)->fg, (c)->bg)
#define tb_set_clear_attributes tb_set_clear_attrs
#define tb_select_input_mode    tb_set_input_mode
#define tb_select_output_mode   tb_set_output_mode
#endif

/* Define these to swap in a different allocator */
#ifndef tb_malloc
#define tb_malloc  malloc
#define tb_realloc realloc
#define tb_free    free
#endif

#ifdef TB_OPT_TRUECOLOR
typedef uint32_t uintattr_t;
#else
typedef uint16_t uintattr_t;
#endif

/* The terminal screen is represented as 2d array of cells. The structure is
 * optimized for dealing with single-width (wcwidth()==1) Unicode code points,
 * however some support for grapheme clusters (e.g., combining diacritical
 * marks) and wide code points (e.g., Hiragana) is provided through ech, nech,
 * cech via tb_set_cell_ex(). ech is only valid when nech>0, otherwise ch is
 * used.
 *
 * For non-single-width code points, given N=wcwidth(ch)/wcswidth(ech):
 *
 *   when N==0: termbox forces a single-width cell. Callers should avoid this
 *              if aiming to render text accurately.
 *
 *    when N>1: termbox zeroes out the following N-1 cells and skips sending
 *              them to the tty. So, e.g., if the caller sets x=0,y=0 to an N==2
 *              code point, the caller's next set should be at x=2,y=0. Anything
 *              set at x=1,y=0 will be ignored. If there are not enough columns
 *              remaining on the line to render N width, spaces are sent
 *              instead.
 *
 * See tb_present() for implementation.
 */
struct tb_cell {
    uint32_t ch;   /* a Unicode character */
    uintattr_t fg; /* bitwise foreground attributes */
    uintattr_t bg; /* bitwise background attributes */
#ifdef TB_OPT_EGC
    uint32_t *ech; /* a grapheme cluster of Unicode code points */
    size_t nech;   /* length in bytes of ech, 0 means use ch instead of ech */
    size_t cech;   /* capacity in bytes of ech */
#endif
};

/* An incoming event from the tty.
 *
 * Given the event type, the following fields are relevant:
 *
 *      when TB_EVENT_KEY: (key XOR ch, one will be zero), mod. Note there is
 *                         overlap between TB_MOD_CTRL and TB_KEY_CTRL_*.
 *                         TB_MOD_CTRL and TB_MOD_SHIFT are only set as
 *                         modifiers to TB_KEY_ARROW_*.
 *
 *   when TB_EVENT_RESIZE: w, h
 *
 *    when TB_EVENT_MOUSE: key (TB_KEY_MOUSE_*), x, y
 */
struct tb_event {
    uint8_t type; /* one of TB_EVENT_* constants */
    uint8_t mod;  /* bitwise TB_MOD_* constants */
    uint16_t key; /* one of TB_KEY_* constants */
    uint32_t ch;  /* a Unicode code point */
    int32_t w;    /* resize width */
    int32_t h;    /* resize height */
    int32_t x;    /* mouse x */
    int32_t y;    /* mouse y */
};

/* Initializes the termbox library. This function should be called before any
 * other functions. tb_init() is equivalent to tb_init_file("/dev/tty"). After
 * successful initialization, the library must be finalized using the
 * tb_shutdown() function.
 */
int tb_init(void);
int tb_init_file(const char *path);
int tb_init_fd(int ttyfd);
int tb_init_rwfd(int rfd, int wfd);
int tb_shutdown(void);

/* Returns the size of the internal back buffer (which is the same as terminal's
 * window size in rows and columns). The internal buffer can be resized after
 * tb_clear() or tb_present() function calls. Both dimensions have an
 * unspecified negative value when called before tb_init() or after
 * tb_shutdown().
 */
int tb_width(void);
int tb_height(void);

/* Clears the internal back buffer using TB_DEFAULT color or the
 * color/attributes set by tb_set_clear_attrs() function.
 */
int tb_clear(void);
int tb_set_clear_attrs(uintattr_t fg, uintattr_t bg);

/* Synchronizes the internal back buffer with the terminal by writing to tty. */
int tb_present(void);

/* Sets the position of the cursor. Upper-left character is (0, 0). */
int tb_set_cursor(int cx, int cy);
int tb_hide_cursor(void);

/* Set cell contents in the internal back buffer at the specified position.
 *
 * Use tb_set_cell_ex() for rendering grapheme clusters (e.g., combining
 * diacritical marks).
 *
 * Function tb_set_cell(x, y, ch, fg, bg) is equivalent to
 * tb_set_cell_ex(x, y, &ch, 1, fg, bg).
 *
 * Function tb_extend_cell() is a shortcut for appending 1 code point to
 * cell->ech.
 */
int tb_set_cell(int x, int y, uint32_t ch, uintattr_t fg, uintattr_t bg);
int tb_set_cell_ex(int x, int y, uint32_t *ch, size_t nch, uintattr_t fg,
    uintattr_t bg);
int tb_extend_cell(int x, int y, uint32_t ch);

/* Sets the input mode. Termbox has two input modes:
 *
 * 1. TB_INPUT_ESC
 *    When escape (\x1b) is in the buffer and there's no match for an escape
 *    sequence, a key event for TB_KEY_ESC is returned.
 *
 * 2. TB_INPUT_ALT
 *    When escape (\x1b) is in the buffer and there's no match for an escape
 *    sequence, the next keyboard event is returned with a TB_MOD_ALT modifier.
 *
 * You can also apply TB_INPUT_MOUSE via bitwise OR operation to either of the
 * modes (e.g., TB_INPUT_ESC | TB_INPUT_MOUSE) to receive TB_EVENT_MOUSE events.
 * If none of the main two modes were set, but the mouse mode was, TB_INPUT_ESC
 * mode is used. If for some reason you've decided to use
 * (TB_INPUT_ESC | TB_INPUT_ALT) combination, it will behave as if only
 * TB_INPUT_ESC was selected.
 *
 * If mode is TB_INPUT_CURRENT, the function returns the current input mode.
 *
 * The default input mode is TB_INPUT_ESC.
 */
int tb_set_input_mode(int mode);

/* Sets the termbox output mode. Termbox has multiple output modes:
 *
 * 1. TB_OUTPUT_NORMAL     => [0..8]
 *
 *    This mode provides 8 different colors:
 *      TB_BLACK, TB_RED, TB_GREEN, TB_YELLOW,
 *      TB_BLUE, TB_MAGENTA, TB_CYAN, TB_WHITE
 *
 *    Plus TB_DEFAULT which skips sending a color code (i.e., uses the
 *    terminal's default color).
 *
 *    Colors (including TB_DEFAULT) may be bitwise OR'd with attributes:
 *      TB_BOLD, TB_UNDERLINE, TB_REVERSE, TB_ITALIC, TB_BLINK
 *
 *    The value 0 is interpreted as TB_DEFAULT.
 *
 *    Some notes: TB_REVERSE can be applied as either fg or bg attributes for
 *    the same effect. TB_BOLD, TB_UNDERLINE, TB_ITALIC, TB_BLINK apply as fg
 *    attributes only, and are ignored as bg attributes.
 *
 *    Example usage:
 *      tb_set_cell(x, y, '@', TB_BLACK | TB_BOLD, TB_RED);
 *
 * 2. TB_OUTPUT_256        => [0..255]
 *
 *    In this mode you get 256 distinct colors:
 *      0x00 - 0x07: the 8 colors as in TB_OUTPUT_NORMAL
 *      0x08 - 0x0f: bright versions of the above
 *      0x10 - 0xe7: 216 different colors
 *      0xe8 - 0xff: 24 different shades of grey
 *
 *    Attributes may be bitwise OR'd as in TB_OUTPUT_NORMAL.
 *
 *    In this mode 0x00 represents TB_BLACK, so TB_DEFAULT must be used for
 *    default colors.
 *
 * 3. TB_OUTPUT_216        => [0..216]
 *
 *    This mode supports the 3rd range of TB_OUTPUT_256 only, but you don't
 *    need to provide an offset.
 *
 *    The value 0 is interpreted as TB_DEFAULT.
 *
 * 4. TB_OUTPUT_GRAYSCALE  => [0..24]
 *
 *    This mode supports the 4th range of TB_OUTPUT_256 only, but you don't
 *    need to provide an offset.
 *
 *    The value 0 is interpreted as TB_DEFAULT.
 *
 * 5. TB_OUTPUT_TRUECOLOR  => [0x000000..0xffffff]
 *
 *    This mode provides 24-bit color on supported terminals. The format is
 *    0xRRGGBB. Colors may be bitwise OR'd with `TB_TRUECOLOR_*` attributes.
 *
 *    In this mode 0x000000 represents black, so TB_TRUECOLOR_DEFAULT must be
 *    used for default colors.
 *
 * If mode is TB_OUTPUT_CURRENT, the function returns the current output mode.
 *
 * The default output mode is TB_OUTPUT_NORMAL.
 *
 * To use the terminal default color (i.e., to not send an escape code), pass
 * TB_DEFAULT (or TB_TRUECOLOR_DEFAULT in TB_OUTPUT_TRUECOLOR mode). For
 * convenience, the value 0 is interpreted as TB_DEFAULT in TB_OUTPUT_NORMAL,
 * TB_OUTPUT_216, and TB_OUTPUT_GRAYSCALE.
 *
 * Note, not all terminals support all output modes, especially beyond
 * TB_OUTPUT_NORMAL. There is also no very reliable way to determine color
 * support dynamically. If portability is desired, callers are recommended to
 * use TB_OUTPUT_NORMAL or make output mode end-user configurable.
 */
int tb_set_output_mode(int mode);

/* Wait for an event up to timeout_ms milliseconds and fill the event structure
 * with it. If no event is available within the timeout period, TB_ERR_NO_EVENT
 * is returned. On a resize event, the underlying select(2) call may be
 * interrupted, yielding a return code of TB_ERR_POLL. In this case, you may
 * check errno via tb_last_errno(). If it's EINTR, you can safely ignore that
 * and call tb_peek_event() again.
 */
int tb_peek_event(struct tb_event *event, int timeout_ms);

/* Same as tb_peek_event except no timeout. */
int tb_poll_event(struct tb_event *event);

/* Internal termbox FDs that can be used with poll() / select(). Must call
 * tb_poll_event() / tb_peek_event() if activity is detected. */
int tb_get_fds(int *ttyfd, int *resizefd);

/* Print and printf functions. Specify param out_w to determine width of printed
 * string.
 */
int tb_print(int x, int y, uintattr_t fg, uintattr_t bg, const char *str);
int tb_printf(int x, int y, uintattr_t fg, uintattr_t bg, const char *fmt, ...);
int tb_print_ex(int x, int y, uintattr_t fg, uintattr_t bg, size_t *out_w,
    const char *str);
int tb_printf_ex(int x, int y, uintattr_t fg, uintattr_t bg, size_t *out_w,
    const char *fmt, ...);

/* Send raw bytes to terminal. */
int tb_send(const char *buf, size_t nbuf);
int tb_sendf(const char *fmt, ...);

/* Set custom functions. fn_type is one of TB_FUNC_* constants, fn is a
 * compatible function pointer, or NULL to clear.
 *
 * TB_FUNC_EXTRACT_PRE:
 *   If specified, invoke this function BEFORE termbox tries to extract any
 *   escape sequences from the input buffer.
 *
 * TB_FUNC_EXTRACT_POST:
 *   If specified, invoke this function AFTER termbox tries (and fails) to
 *   extract any escape sequences from the input buffer.
 */
int tb_set_func(int fn_type, int (*fn)(struct tb_event *, size_t *));

/* Utility functions. */
int tb_utf8_char_length(char c);
int tb_utf8_char_to_unicode(uint32_t *out, const char *c);
int tb_utf8_unicode_to_char(char *out, uint32_t c);
int tb_last_errno(void);
const char *tb_strerror(int err);
struct tb_cell *tb_cell_buffer(void);
int tb_has_truecolor(void);
int tb_has_egc(void);
const char *tb_version(void);

#ifdef __cplusplus
}
#endif

#endif /* __TERMBOX_H */

#ifdef TB_IMPL

#define if_err_return(rv, expr)                                                \
    if (((rv) = (expr)) != TB_OK)                                              \
    return (rv)
#define if_err_break(rv, expr)                                                 \
    if (((rv) = (expr)) != TB_OK)                                              \
    break
#define if_ok_return(rv, expr)                                                 \
    if (((rv) = (expr)) == TB_OK)                                              \
    return (rv)
#define if_ok_or_need_more_return(rv, expr)                                    \
    if (((rv) = (expr)) == TB_OK || (rv) == TB_ERR_NEED_MORE)                  \
    return (rv)

#define send_literal(rv, a)                                                    \
    if_err_return((rv), bytebuf_nputs(&global.out, (a), sizeof(a) - 1))

#define send_num(rv, nbuf, n)                                                  \
    if_err_return((rv),                                                        \
        bytebuf_nputs(&global.out, (nbuf), convert_num((n), (nbuf))))

#define snprintf_or_return(rv, str, sz, fmt, ...)                              \
    do {                                                                       \
        (rv) = snprintf((str), (sz), (fmt), __VA_ARGS__);                      \
        if ((rv) < 0 || (rv) >= (int)(sz))                                     \
            return TB_ERR;                                                     \
    } while (0)

#define if_not_init_return()                                                   \
    if (!global.initialized)                                                   \
    return TB_ERR_NOT_INIT

struct bytebuf_t {
    char *buf;
    size_t len;
    size_t cap;
};

struct cellbuf_t {
    int width;
    int height;
    struct tb_cell *cells;
};

struct cap_trie_t {
    char c;
    struct cap_trie_t *children;
    size_t nchildren;
    int is_leaf;
    uint16_t key;
    uint8_t mod;
};

struct tb_global_t {
    int ttyfd;
    int rfd;
    int wfd;
    int ttyfd_open;
    int resize_pipefd[2];
    int width;
    int height;
    int cursor_x;
    int cursor_y;
    int last_x;
    int last_y;
    uintattr_t fg;
    uintattr_t bg;
    uintattr_t last_fg;
    uintattr_t last_bg;
    int input_mode;
    int output_mode;
    char *terminfo;
    size_t nterminfo;
    const char *caps[TB_CAP__COUNT];
    struct cap_trie_t cap_trie;
    struct bytebuf_t in;
    struct bytebuf_t out;
    struct cellbuf_t back;
    struct cellbuf_t front;
    struct termios orig_tios;
    int has_orig_tios;
    int last_errno;
    int initialized;
    int (*fn_extract_esc_pre)(struct tb_event *, size_t *);
    int (*fn_extract_esc_post)(struct tb_event *, size_t *);
    char errbuf[1024];
};

static struct tb_global_t global = {0};

/* BEGIN codegen c */
/* Produced by ./codegen.sh on Sun, 19 Sep 2021 01:02:03 +0000 */

static const int16_t terminfo_cap_indexes[] = {
    66,  // kf1 (TB_CAP_F1)
    68,  // kf2 (TB_CAP_F2)
    69,  // kf3 (TB_CAP_F3)
    70,  // kf4 (TB_CAP_F4)
    71,  // kf5 (TB_CAP_F5)
    72,  // kf6 (TB_CAP_F6)
    73,  // kf7 (TB_CAP_F7)
    74,  // kf8 (TB_CAP_F8)
    75,  // kf9 (TB_CAP_F9)
    67,  // kf10 (TB_CAP_F10)
    216, // kf11 (TB_CAP_F11)
    217, // kf12 (TB_CAP_F12)
    77,  // kich1 (TB_CAP_INSERT)
    59,  // kdch1 (TB_CAP_DELETE)
    76,  // khome (TB_CAP_HOME)
    164, // kend (TB_CAP_END)
    82,  // kpp (TB_CAP_PGUP)
    81,  // knp (TB_CAP_PGDN)
    87,  // kcuu1 (TB_CAP_ARROW_UP)
    61,  // kcud1 (TB_CAP_ARROW_DOWN)
    79,  // kcub1 (TB_CAP_ARROW_LEFT)
    83,  // kcuf1 (TB_CAP_ARROW_RIGHT)
    148, // kcbt (TB_CAP_BACK_TAB)
    28,  // smcup (TB_CAP_ENTER_CA)
    40,  // rmcup (TB_CAP_EXIT_CA)
    16,  // cnorm (TB_CAP_SHOW_CURSOR)
    13,  // civis (TB_CAP_HIDE_CURSOR)
    5,   // clear (TB_CAP_CLEAR_SCREEN)
    39,  // sgr0 (TB_CAP_SGR0)
    36,  // smul (TB_CAP_UNDERLINE)
    27,  // bold (TB_CAP_BOLD)
    26,  // blink (TB_CAP_BLINK)
    311, // sitm (TB_CAP_ITALIC)
    34,  // rev (TB_CAP_REVERSE)
    89,  // smkx (TB_CAP_ENTER_KEYPAD)
    88,  // rmkx (TB_CAP_EXIT_KEYPAD)
};

// xterm
static const char *xterm_caps[] = {
    "\033OP",                  // kf1 (TB_CAP_F1)
    "\033OQ",                  // kf2 (TB_CAP_F2)
    "\033OR",                  // kf3 (TB_CAP_F3)
    "\033OS",                  // kf4 (TB_CAP_F4)
    "\033[15~",                // kf5 (TB_CAP_F5)
    "\033[17~",                // kf6 (TB_CAP_F6)
    "\033[18~",                // kf7 (TB_CAP_F7)
    "\033[19~",                // kf8 (TB_CAP_F8)
    "\033[20~",                // kf9 (TB_CAP_F9)
    "\033[21~",                // kf10 (TB_CAP_F10)
    "\033[23~",                // kf11 (TB_CAP_F11)
    "\033[24~",                // kf12 (TB_CAP_F12)
    "\033[2~",                 // kich1 (TB_CAP_INSERT)
    "\033[3~",                 // kdch1 (TB_CAP_DELETE)
    "\033OH",                  // khome (TB_CAP_HOME)
    "\033OF",                  // kend (TB_CAP_END)
    "\033[5~",                 // kpp (TB_CAP_PGUP)
    "\033[6~",                 // knp (TB_CAP_PGDN)
    "\033OA",                  // kcuu1 (TB_CAP_ARROW_UP)
    "\033OB",                  // kcud1 (TB_CAP_ARROW_DOWN)
    "\033OD",                  // kcub1 (TB_CAP_ARROW_LEFT)
    "\033OC",                  // kcuf1 (TB_CAP_ARROW_RIGHT)
    "\033[Z",                  // kcbt (TB_CAP_BACK_TAB)
    "\033[?1049h\033[22;0;0t", // smcup (TB_CAP_ENTER_CA)
    "\033[?1049l\033[23;0;0t", // rmcup (TB_CAP_EXIT_CA)
    "\033[?12l\033[?25h",      // cnorm (TB_CAP_SHOW_CURSOR)
    "\033[?25l",               // civis (TB_CAP_HIDE_CURSOR)
    "\033[H\033[2J",           // clear (TB_CAP_CLEAR_SCREEN)
    "\033(B\033[m",            // sgr0 (TB_CAP_SGR0)
    "\033[4m",                 // smul (TB_CAP_UNDERLINE)
    "\033[1m",                 // bold (TB_CAP_BOLD)
    "\033[5m",                 // blink (TB_CAP_BLINK)
    "\033[3m",                 // sitm (TB_CAP_ITALIC)
    "\033[7m",                 // rev (TB_CAP_REVERSE)
    "\033[?1h\033=",           // smkx (TB_CAP_ENTER_KEYPAD)
    "\033[?1l\033>",           // rmkx (TB_CAP_EXIT_KEYPAD)
};

// linux
static const char *linux_caps[] = {
    "\033[[A",           // kf1 (TB_CAP_F1)
    "\033[[B",           // kf2 (TB_CAP_F2)
    "\033[[C",           // kf3 (TB_CAP_F3)
    "\033[[D",           // kf4 (TB_CAP_F4)
    "\033[[E",           // kf5 (TB_CAP_F5)
    "\033[17~",          // kf6 (TB_CAP_F6)
    "\033[18~",          // kf7 (TB_CAP_F7)
    "\033[19~",          // kf8 (TB_CAP_F8)
    "\033[20~",          // kf9 (TB_CAP_F9)
    "\033[21~",          // kf10 (TB_CAP_F10)
    "\033[23~",          // kf11 (TB_CAP_F11)
    "\033[24~",          // kf12 (TB_CAP_F12)
    "\033[2~",           // kich1 (TB_CAP_INSERT)
    "\033[3~",           // kdch1 (TB_CAP_DELETE)
    "\033[1~",           // khome (TB_CAP_HOME)
    "\033[4~",           // kend (TB_CAP_END)
    "\033[5~",           // kpp (TB_CAP_PGUP)
    "\033[6~",           // knp (TB_CAP_PGDN)
    "\033[A",            // kcuu1 (TB_CAP_ARROW_UP)
    "\033[B",            // kcud1 (TB_CAP_ARROW_DOWN)
    "\033[D",            // kcub1 (TB_CAP_ARROW_LEFT)
    "\033[C",            // kcuf1 (TB_CAP_ARROW_RIGHT)
    "\033[Z",            // kcbt (TB_CAP_BACK_TAB)
    "",                  // smcup (TB_CAP_ENTER_CA)
    "",                  // rmcup (TB_CAP_EXIT_CA)
    "\033[?25h\033[?0c", // cnorm (TB_CAP_SHOW_CURSOR)
    "\033[?25l\033[?1c", // civis (TB_CAP_HIDE_CURSOR)
    "\033[H\033[J",      // clear (TB_CAP_CLEAR_SCREEN)
    "\033[m\017",        // sgr0 (TB_CAP_SGR0)
    "\033[4m",           // smul (TB_CAP_UNDERLINE)
    "\033[1m",           // bold (TB_CAP_BOLD)
    "\033[5m",           // blink (TB_CAP_BLINK)
    "",                  // sitm (TB_CAP_ITALIC)
    "\033[7m",           // rev (TB_CAP_REVERSE)
    "",                  // smkx (TB_CAP_ENTER_KEYPAD)
    "",                  // rmkx (TB_CAP_EXIT_KEYPAD)
};

// screen
static const char *screen_caps[] = {
    "\033OP",            // kf1 (TB_CAP_F1)
    "\033OQ",            // kf2 (TB_CAP_F2)
    "\033OR",            // kf3 (TB_CAP_F3)
    "\033OS",            // kf4 (TB_CAP_F4)
    "\033[15~",          // kf5 (TB_CAP_F5)
    "\033[17~",          // kf6 (TB_CAP_F6)
    "\033[18~",          // kf7 (TB_CAP_F7)
    "\033[19~",          // kf8 (TB_CAP_F8)
    "\033[20~",          // kf9 (TB_CAP_F9)
    "\033[21~",          // kf10 (TB_CAP_F10)
    "\033[23~",          // kf11 (TB_CAP_F11)
    "\033[24~",          // kf12 (TB_CAP_F12)
    "\033[2~",           // kich1 (TB_CAP_INSERT)
    "\033[3~",           // kdch1 (TB_CAP_DELETE)
    "\033[1~",           // khome (TB_CAP_HOME)
    "\033[4~",           // kend (TB_CAP_END)
    "\033[5~",           // kpp (TB_CAP_PGUP)
    "\033[6~",           // knp (TB_CAP_PGDN)
    "\033OA",            // kcuu1 (TB_CAP_ARROW_UP)
    "\033OB",            // kcud1 (TB_CAP_ARROW_DOWN)
    "\033OD",            // kcub1 (TB_CAP_ARROW_LEFT)
    "\033OC",            // kcuf1 (TB_CAP_ARROW_RIGHT)
    "\033[Z",            // kcbt (TB_CAP_BACK_TAB)
    "\033[?1049h",       // smcup (TB_CAP_ENTER_CA)
    "\033[?1049l",       // rmcup (TB_CAP_EXIT_CA)
    "\033[34h\033[?25h", // cnorm (TB_CAP_SHOW_CURSOR)
    "\033[?25l",         // civis (TB_CAP_HIDE_CURSOR)
    "\033[H\033[J",      // clear (TB_CAP_CLEAR_SCREEN)
    "\033[m\017",        // sgr0 (TB_CAP_SGR0)
    "\033[4m",           // smul (TB_CAP_UNDERLINE)
    "\033[1m",           // bold (TB_CAP_BOLD)
    "\033[5m",           // blink (TB_CAP_BLINK)
    "",                  // sitm (TB_CAP_ITALIC)
    "\033[7m",           // rev (TB_CAP_REVERSE)
    "\033[?1h\033=",     // smkx (TB_CAP_ENTER_KEYPAD)
    "\033[?1l\033>",     // rmkx (TB_CAP_EXIT_KEYPAD)
};

// rxvt-256color
static const char *rxvt_256color_caps[] = {
    "\033[11~",              // kf1 (TB_CAP_F1)
    "\033[12~",              // kf2 (TB_CAP_F2)
    "\033[13~",              // kf3 (TB_CAP_F3)
    "\033[14~",              // kf4 (TB_CAP_F4)
    "\033[15~",              // kf5 (TB_CAP_F5)
    "\033[17~",              // kf6 (TB_CAP_F6)
    "\033[18~",              // kf7 (TB_CAP_F7)
    "\033[19~",              // kf8 (TB_CAP_F8)
    "\033[20~",              // kf9 (TB_CAP_F9)
    "\033[21~",              // kf10 (TB_CAP_F10)
    "\033[23~",              // kf11 (TB_CAP_F11)
    "\033[24~",              // kf12 (TB_CAP_F12)
    "\033[2~",               // kich1 (TB_CAP_INSERT)
    "\033[3~",               // kdch1 (TB_CAP_DELETE)
    "\033[7~",               // khome (TB_CAP_HOME)
    "\033[8~",               // kend (TB_CAP_END)
    "\033[5~",               // kpp (TB_CAP_PGUP)
    "\033[6~",               // knp (TB_CAP_PGDN)
    "\033[A",                // kcuu1 (TB_CAP_ARROW_UP)
    "\033[B",                // kcud1 (TB_CAP_ARROW_DOWN)
    "\033[D",                // kcub1 (TB_CAP_ARROW_LEFT)
    "\033[C",                // kcuf1 (TB_CAP_ARROW_RIGHT)
    "\033[Z",                // kcbt (TB_CAP_BACK_TAB)
    "\0337\033[?47h",        // smcup (TB_CAP_ENTER_CA)
    "\033[2J\033[?47l\0338", // rmcup (TB_CAP_EXIT_CA)
    "\033[?25h",             // cnorm (TB_CAP_SHOW_CURSOR)
    "\033[?25l",             // civis (TB_CAP_HIDE_CURSOR)
    "\033[H\033[2J",         // clear (TB_CAP_CLEAR_SCREEN)
    "\033[m\017",            // sgr0 (TB_CAP_SGR0)
    "\033[4m",               // smul (TB_CAP_UNDERLINE)
    "\033[1m",               // bold (TB_CAP_BOLD)
    "\033[5m",               // blink (TB_CAP_BLINK)
    "",                      // sitm (TB_CAP_ITALIC)
    "\033[7m",               // rev (TB_CAP_REVERSE)
    "\033=",                 // smkx (TB_CAP_ENTER_KEYPAD)
    "\033>",                 // rmkx (TB_CAP_EXIT_KEYPAD)
};

// rxvt-unicode
static const char *rxvt_unicode_caps[] = {
    "\033[11~",           // kf1 (TB_CAP_F1)
    "\033[12~",           // kf2 (TB_CAP_F2)
    "\033[13~",           // kf3 (TB_CAP_F3)
    "\033[14~",           // kf4 (TB_CAP_F4)
    "\033[15~",           // kf5 (TB_CAP_F5)
    "\033[17~",           // kf6 (TB_CAP_F6)
    "\033[18~",           // kf7 (TB_CAP_F7)
    "\033[19~",           // kf8 (TB_CAP_F8)
    "\033[20~",           // kf9 (TB_CAP_F9)
    "\033[21~",           // kf10 (TB_CAP_F10)
    "\033[23~",           // kf11 (TB_CAP_F11)
    "\033[24~",           // kf12 (TB_CAP_F12)
    "\033[2~",            // kich1 (TB_CAP_INSERT)
    "\033[3~",            // kdch1 (TB_CAP_DELETE)
    "\033[7~",            // khome (TB_CAP_HOME)
    "\033[8~",            // kend (TB_CAP_END)
    "\033[5~",            // kpp (TB_CAP_PGUP)
    "\033[6~",            // knp (TB_CAP_PGDN)
    "\033[A",             // kcuu1 (TB_CAP_ARROW_UP)
    "\033[B",             // kcud1 (TB_CAP_ARROW_DOWN)
    "\033[D",             // kcub1 (TB_CAP_ARROW_LEFT)
    "\033[C",             // kcuf1 (TB_CAP_ARROW_RIGHT)
    "\033[Z",             // kcbt (TB_CAP_BACK_TAB)
    "\033[?1049h",        // smcup (TB_CAP_ENTER_CA)
    "\033[r\033[?1049l",  // rmcup (TB_CAP_EXIT_CA)
    "\033[?12l\033[?25h", // cnorm (TB_CAP_SHOW_CURSOR)
    "\033[?25l",          // civis (TB_CAP_HIDE_CURSOR)
    "\033[H\033[2J",      // clear (TB_CAP_CLEAR_SCREEN)
    "\033[m\033(B",       // sgr0 (TB_CAP_SGR0)
    "\033[4m",            // smul (TB_CAP_UNDERLINE)
    "\033[1m",            // bold (TB_CAP_BOLD)
    "\033[5m",            // blink (TB_CAP_BLINK)
    "\033[3m",            // sitm (TB_CAP_ITALIC)
    "\033[7m",            // rev (TB_CAP_REVERSE)
    "\033=",              // smkx (TB_CAP_ENTER_KEYPAD)
    "\033>",              // rmkx (TB_CAP_EXIT_KEYPAD)
};

// Eterm
static const char *eterm_caps[] = {
    "\033[11~",              // kf1 (TB_CAP_F1)
    "\033[12~",              // kf2 (TB_CAP_F2)
    "\033[13~",              // kf3 (TB_CAP_F3)
    "\033[14~",              // kf4 (TB_CAP_F4)
    "\033[15~",              // kf5 (TB_CAP_F5)
    "\033[17~",              // kf6 (TB_CAP_F6)
    "\033[18~",              // kf7 (TB_CAP_F7)
    "\033[19~",              // kf8 (TB_CAP_F8)
    "\033[20~",              // kf9 (TB_CAP_F9)
    "\033[21~",              // kf10 (TB_CAP_F10)
    "\033[23~",              // kf11 (TB_CAP_F11)
    "\033[24~",              // kf12 (TB_CAP_F12)
    "\033[2~",               // kich1 (TB_CAP_INSERT)
    "\033[3~",               // kdch1 (TB_CAP_DELETE)
    "\033[7~",               // khome (TB_CAP_HOME)
    "\033[8~",               // kend (TB_CAP_END)
    "\033[5~",               // kpp (TB_CAP_PGUP)
    "\033[6~",               // knp (TB_CAP_PGDN)
    "\033[A",                // kcuu1 (TB_CAP_ARROW_UP)
    "\033[B",                // kcud1 (TB_CAP_ARROW_DOWN)
    "\033[D",                // kcub1 (TB_CAP_ARROW_LEFT)
    "\033[C",                // kcuf1 (TB_CAP_ARROW_RIGHT)
    "",                      // kcbt (TB_CAP_BACK_TAB)
    "\0337\033[?47h",        // smcup (TB_CAP_ENTER_CA)
    "\033[2J\033[?47l\0338", // rmcup (TB_CAP_EXIT_CA)
    "\033[?25h",             // cnorm (TB_CAP_SHOW_CURSOR)
    "\033[?25l",             // civis (TB_CAP_HIDE_CURSOR)
    "\033[H\033[2J",         // clear (TB_CAP_CLEAR_SCREEN)
    "\033[m\017",            // sgr0 (TB_CAP_SGR0)
    "\033[4m",               // smul (TB_CAP_UNDERLINE)
    "\033[1m",               // bold (TB_CAP_BOLD)
    "\033[5m",               // blink (TB_CAP_BLINK)
    "",                      // sitm (TB_CAP_ITALIC)
    "\033[7m",               // rev (TB_CAP_REVERSE)
    "",                      // smkx (TB_CAP_ENTER_KEYPAD)
    "",                      // rmkx (TB_CAP_EXIT_KEYPAD)
};

static struct {
    const char *name;
    const char **caps;
    const char *alias;
} builtin_terms[] = {
    {"xterm",         xterm_caps,         ""    },
    {"linux",         linux_caps,         ""    },
    {"screen",        screen_caps,        "tmux"},
    {"rxvt-256color", rxvt_256color_caps, ""    },
    {"rxvt-unicode",  rxvt_unicode_caps,  "rxvt"},
    {"Eterm",         eterm_caps,         ""    },
    {NULL,            NULL,               NULL  },
};

/* END codegen c */

static struct {
    const char *cap;
    const uint16_t key;
    const uint8_t mod;
} builtin_mod_caps[] = {
  // xterm arrows
    {"\x1b[1;2A",    TB_KEY_ARROW_UP,    TB_MOD_SHIFT                           },
    {"\x1b[1;3A",    TB_KEY_ARROW_UP,    TB_MOD_ALT                             },
    {"\x1b[1;4A",    TB_KEY_ARROW_UP,    TB_MOD_ALT | TB_MOD_SHIFT              },
    {"\x1b[1;5A",    TB_KEY_ARROW_UP,    TB_MOD_CTRL                            },
    {"\x1b[1;6A",    TB_KEY_ARROW_UP,    TB_MOD_CTRL | TB_MOD_SHIFT             },
    {"\x1b[1;7A",    TB_KEY_ARROW_UP,    TB_MOD_CTRL | TB_MOD_ALT               },
    {"\x1b[1;8A",    TB_KEY_ARROW_UP,    TB_MOD_CTRL | TB_MOD_ALT | TB_MOD_SHIFT},

    {"\x1b[1;2B",    TB_KEY_ARROW_DOWN,  TB_MOD_SHIFT                           },
    {"\x1b[1;3B",    TB_KEY_ARROW_DOWN,  TB_MOD_ALT                             },
    {"\x1b[1;4B",    TB_KEY_ARROW_DOWN,  TB_MOD_ALT | TB_MOD_SHIFT              },
    {"\x1b[1;5B",    TB_KEY_ARROW_DOWN,  TB_MOD_CTRL                            },
    {"\x1b[1;6B",    TB_KEY_ARROW_DOWN,  TB_MOD_CTRL | TB_MOD_SHIFT             },
    {"\x1b[1;7B",    TB_KEY_ARROW_DOWN,  TB_MOD_CTRL | TB_MOD_ALT               },
    {"\x1b[1;8B",    TB_KEY_ARROW_DOWN,  TB_MOD_CTRL | TB_MOD_ALT | TB_MOD_SHIFT},

    {"\x1b[1;2C",    TB_KEY_ARROW_RIGHT, TB_MOD_SHIFT                           },
    {"\x1b[1;3C",    TB_KEY_ARROW_RIGHT, TB_MOD_ALT                             },
    {"\x1b[1;4C",    TB_KEY_ARROW_RIGHT, TB_MOD_ALT | TB_MOD_SHIFT              },
    {"\x1b[1;5C",    TB_KEY_ARROW_RIGHT, TB_MOD_CTRL                            },
    {"\x1b[1;6C",    TB_KEY_ARROW_RIGHT, TB_MOD_CTRL | TB_MOD_SHIFT             },
    {"\x1b[1;7C",    TB_KEY_ARROW_RIGHT, TB_MOD_CTRL | TB_MOD_ALT               },
    {"\x1b[1;8C",    TB_KEY_ARROW_RIGHT, TB_MOD_CTRL | TB_MOD_ALT | TB_MOD_SHIFT},

    {"\x1b[1;2D",    TB_KEY_ARROW_LEFT,  TB_MOD_SHIFT                           },
    {"\x1b[1;3D",    TB_KEY_ARROW_LEFT,  TB_MOD_ALT                             },
    {"\x1b[1;4D",    TB_KEY_ARROW_LEFT,  TB_MOD_ALT | TB_MOD_SHIFT              },
    {"\x1b[1;5D",    TB_KEY_ARROW_LEFT,  TB_MOD_CTRL                            },
    {"\x1b[1;6D",    TB_KEY_ARROW_LEFT,  TB_MOD_CTRL | TB_MOD_SHIFT             },
    {"\x1b[1;7D",    TB_KEY_ARROW_LEFT,  TB_MOD_CTRL | TB_MOD_ALT               },
    {"\x1b[1;8D",    TB_KEY_ARROW_LEFT,  TB_MOD_CTRL | TB_MOD_ALT | TB_MOD_SHIFT},

 // xterm keys
    {"\x1b[1;2H",    TB_KEY_HOME,        TB_MOD_SHIFT                           },
    {"\x1b[1;3H",    TB_KEY_HOME,        TB_MOD_ALT                             },
    {"\x1b[1;4H",    TB_KEY_HOME,        TB_MOD_ALT | TB_MOD_SHIFT              },
    {"\x1b[1;5H",    TB_KEY_HOME,        TB_MOD_CTRL                            },
    {"\x1b[1;6H",    TB_KEY_HOME,        TB_MOD_CTRL | TB_MOD_SHIFT             },
    {"\x1b[1;7H",    TB_KEY_HOME,        TB_MOD_CTRL | TB_MOD_ALT               },
    {"\x1b[1;8H",    TB_KEY_HOME,        TB_MOD_CTRL | TB_MOD_ALT | TB_MOD_SHIFT},

    {"\x1b[1;2F",    TB_KEY_END,         TB_MOD_SHIFT                           },
    {"\x1b[1;3F",    TB_KEY_END,         TB_MOD_ALT                             },
    {"\x1b[1;4F",    TB_KEY_END,         TB_MOD_ALT | TB_MOD_SHIFT              },
    {"\x1b[1;5F",    TB_KEY_END,         TB_MOD_CTRL                            },
    {"\x1b[1;6F",    TB_KEY_END,         TB_MOD_CTRL | TB_MOD_SHIFT             },
    {"\x1b[1;7F",    TB_KEY_END,         TB_MOD_CTRL | TB_MOD_ALT               },
    {"\x1b[1;8F",    TB_KEY_END,         TB_MOD_CTRL | TB_MOD_ALT | TB_MOD_SHIFT},

    {"\x1b[2;2~",    TB_KEY_INSERT,      TB_MOD_SHIFT                           },
    {"\x1b[2;3~",    TB_KEY_INSERT,      TB_MOD_ALT                             },
    {"\x1b[2;4~",    TB_KEY_INSERT,      TB_MOD_ALT | TB_MOD_SHIFT              },
    {"\x1b[2;5~",    TB_KEY_INSERT,      TB_MOD_CTRL                            },
    {"\x1b[2;6~",    TB_KEY_INSERT,      TB_MOD_CTRL | TB_MOD_SHIFT             },
    {"\x1b[2;7~",    TB_KEY_INSERT,      TB_MOD_CTRL | TB_MOD_ALT               },
    {"\x1b[2;8~",    TB_KEY_INSERT,      TB_MOD_CTRL | TB_MOD_ALT | TB_MOD_SHIFT},

    {"\x1b[3;2~",    TB_KEY_DELETE,      TB_MOD_SHIFT                           },
    {"\x1b[3;3~",    TB_KEY_DELETE,      TB_MOD_ALT                             },
    {"\x1b[3;4~",    TB_KEY_DELETE,      TB_MOD_ALT | TB_MOD_SHIFT              },
    {"\x1b[3;5~",    TB_KEY_DELETE,      TB_MOD_CTRL                            },
    {"\x1b[3;6~",    TB_KEY_DELETE,      TB_MOD_CTRL | TB_MOD_SHIFT             },
    {"\x1b[3;7~",    TB_KEY_DELETE,      TB_MOD_CTRL | TB_MOD_ALT               },
    {"\x1b[3;8~",    TB_KEY_DELETE,      TB_MOD_CTRL | TB_MOD_ALT | TB_MOD_SHIFT},

    {"\x1b[5;2~",    TB_KEY_PGUP,        TB_MOD_SHIFT                           },
    {"\x1b[5;3~",    TB_KEY_PGUP,        TB_MOD_ALT                             },
    {"\x1b[5;4~",    TB_KEY_PGUP,        TB_MOD_ALT | TB_MOD_SHIFT              },
    {"\x1b[5;5~",    TB_KEY_PGUP,        TB_MOD_CTRL                            },
    {"\x1b[5;6~",    TB_KEY_PGUP,        TB_MOD_CTRL | TB_MOD_SHIFT             },
    {"\x1b[5;7~",    TB_KEY_PGUP,        TB_MOD_CTRL | TB_MOD_ALT               },
    {"\x1b[5;8~",    TB_KEY_PGUP,        TB_MOD_CTRL | TB_MOD_ALT | TB_MOD_SHIFT},

    {"\x1b[6;2~",    TB_KEY_PGDN,        TB_MOD_SHIFT                           },
    {"\x1b[6;3~",    TB_KEY_PGDN,        TB_MOD_ALT                             },
    {"\x1b[6;4~",    TB_KEY_PGDN,        TB_MOD_ALT | TB_MOD_SHIFT              },
    {"\x1b[6;5~",    TB_KEY_PGDN,        TB_MOD_CTRL                            },
    {"\x1b[6;6~",    TB_KEY_PGDN,        TB_MOD_CTRL | TB_MOD_SHIFT             },
    {"\x1b[6;7~",    TB_KEY_PGDN,        TB_MOD_CTRL | TB_MOD_ALT               },
    {"\x1b[6;8~",    TB_KEY_PGDN,        TB_MOD_CTRL | TB_MOD_ALT | TB_MOD_SHIFT},

    {"\x1b[1;2P",    TB_KEY_F1,          TB_MOD_SHIFT                           },
    {"\x1b[1;3P",    TB_KEY_F1,          TB_MOD_ALT                             },
    {"\x1b[1;4P",    TB_KEY_F1,          TB_MOD_ALT | TB_MOD_SHIFT              },
    {"\x1b[1;5P",    TB_KEY_F1,          TB_MOD_CTRL                            },
    {"\x1b[1;6P",    TB_KEY_F1,          TB_MOD_CTRL | TB_MOD_SHIFT             },
    {"\x1b[1;7P",    TB_KEY_F1,          TB_MOD_CTRL | TB_MOD_ALT               },
    {"\x1b[1;8P",    TB_KEY_F1,          TB_MOD_CTRL | TB_MOD_ALT | TB_MOD_SHIFT},

    {"\x1b[1;2Q",    TB_KEY_F2,          TB_MOD_SHIFT                           },
    {"\x1b[1;3Q",    TB_KEY_F2,          TB_MOD_ALT                             },
    {"\x1b[1;4Q",    TB_KEY_F2,          TB_MOD_ALT | TB_MOD_SHIFT              },
    {"\x1b[1;5Q",    TB_KEY_F2,          TB_MOD_CTRL                            },
    {"\x1b[1;6Q",    TB_KEY_F2,          TB_MOD_CTRL | TB_MOD_SHIFT             },
    {"\x1b[1;7Q",    TB_KEY_F2,          TB_MOD_CTRL | TB_MOD_ALT               },
    {"\x1b[1;8Q",    TB_KEY_F2,          TB_MOD_CTRL | TB_MOD_ALT | TB_MOD_SHIFT},

    {"\x1b[1;2R",    TB_KEY_F3,          TB_MOD_SHIFT                           },
    {"\x1b[1;3R",    TB_KEY_F3,          TB_MOD_ALT                             },
    {"\x1b[1;4R",    TB_KEY_F3,          TB_MOD_ALT | TB_MOD_SHIFT              },
    {"\x1b[1;5R",    TB_KEY_F3,          TB_MOD_CTRL                            },
    {"\x1b[1;6R",    TB_KEY_F3,          TB_MOD_CTRL | TB_MOD_SHIFT             },
    {"\x1b[1;7R",    TB_KEY_F3,          TB_MOD_CTRL | TB_MOD_ALT               },
    {"\x1b[1;8R",    TB_KEY_F3,          TB_MOD_CTRL | TB_MOD_ALT | TB_MOD_SHIFT},

    {"\x1b[1;2S",    TB_KEY_F4,          TB_MOD_SHIFT                           },
    {"\x1b[1;3S",    TB_KEY_F4,          TB_MOD_ALT                             },
    {"\x1b[1;4S",    TB_KEY_F4,          TB_MOD_ALT | TB_MOD_SHIFT              },
    {"\x1b[1;5S",    TB_KEY_F4,          TB_MOD_CTRL                            },
    {"\x1b[1;6S",    TB_KEY_F4,          TB_MOD_CTRL | TB_MOD_SHIFT             },
    {"\x1b[1;7S",    TB_KEY_F4,          TB_MOD_CTRL | TB_MOD_ALT               },
    {"\x1b[1;8S",    TB_KEY_F4,          TB_MOD_CTRL | TB_MOD_ALT | TB_MOD_SHIFT},

    {"\x1b[15;2~",   TB_KEY_F5,          TB_MOD_SHIFT                           },
    {"\x1b[15;3~",   TB_KEY_F5,          TB_MOD_ALT                             },
    {"\x1b[15;4~",   TB_KEY_F5,          TB_MOD_ALT | TB_MOD_SHIFT              },
    {"\x1b[15;5~",   TB_KEY_F5,          TB_MOD_CTRL                            },
    {"\x1b[15;6~",   TB_KEY_F5,          TB_MOD_CTRL | TB_MOD_SHIFT             },
    {"\x1b[15;7~",   TB_KEY_F5,          TB_MOD_CTRL | TB_MOD_ALT               },
    {"\x1b[15;8~",   TB_KEY_F5,          TB_MOD_CTRL | TB_MOD_ALT | TB_MOD_SHIFT},

    {"\x1b[17;2~",   TB_KEY_F6,          TB_MOD_SHIFT                           },
    {"\x1b[17;3~",   TB_KEY_F6,          TB_MOD_ALT                             },
    {"\x1b[17;4~",   TB_KEY_F6,          TB_MOD_ALT | TB_MOD_SHIFT              },
    {"\x1b[17;5~",   TB_KEY_F6,          TB_MOD_CTRL                            },
    {"\x1b[17;6~",   TB_KEY_F6,          TB_MOD_CTRL | TB_MOD_SHIFT             },
    {"\x1b[17;7~",   TB_KEY_F6,          TB_MOD_CTRL | TB_MOD_ALT               },
    {"\x1b[17;8~",   TB_KEY_F6,          TB_MOD_CTRL | TB_MOD_ALT | TB_MOD_SHIFT},

    {"\x1b[18;2~",   TB_KEY_F7,          TB_MOD_SHIFT                           },
    {"\x1b[18;3~",   TB_KEY_F7,          TB_MOD_ALT                             },
    {"\x1b[18;4~",   TB_KEY_F7,          TB_MOD_ALT | TB_MOD_SHIFT              },
    {"\x1b[18;5~",   TB_KEY_F7,          TB_MOD_CTRL                            },
    {"\x1b[18;6~",   TB_KEY_F7,          TB_MOD_CTRL | TB_MOD_SHIFT             },
    {"\x1b[18;7~",   TB_KEY_F7,          TB_MOD_CTRL | TB_MOD_ALT               },
    {"\x1b[18;8~",   TB_KEY_F7,          TB_MOD_CTRL | TB_MOD_ALT | TB_MOD_SHIFT},

    {"\x1b[19;2~",   TB_KEY_F8,          TB_MOD_SHIFT                           },
    {"\x1b[19;3~",   TB_KEY_F8,          TB_MOD_ALT                             },
    {"\x1b[19;4~",   TB_KEY_F8,          TB_MOD_ALT | TB_MOD_SHIFT              },
    {"\x1b[19;5~",   TB_KEY_F8,          TB_MOD_CTRL                            },
    {"\x1b[19;6~",   TB_KEY_F8,          TB_MOD_CTRL | TB_MOD_SHIFT             },
    {"\x1b[19;7~",   TB_KEY_F8,          TB_MOD_CTRL | TB_MOD_ALT               },
    {"\x1b[19;8~",   TB_KEY_F8,          TB_MOD_CTRL | TB_MOD_ALT | TB_MOD_SHIFT},

    {"\x1b[20;2~",   TB_KEY_F9,          TB_MOD_SHIFT                           },
    {"\x1b[20;3~",   TB_KEY_F9,          TB_MOD_ALT                             },
    {"\x1b[20;4~",   TB_KEY_F9,          TB_MOD_ALT | TB_MOD_SHIFT              },
    {"\x1b[20;5~",   TB_KEY_F9,          TB_MOD_CTRL                            },
    {"\x1b[20;6~",   TB_KEY_F9,          TB_MOD_CTRL | TB_MOD_SHIFT             },
    {"\x1b[20;7~",   TB_KEY_F9,          TB_MOD_CTRL | TB_MOD_ALT               },
    {"\x1b[20;8~",   TB_KEY_F9,          TB_MOD_CTRL | TB_MOD_ALT | TB_MOD_SHIFT},

    {"\x1b[21;2~",   TB_KEY_F10,         TB_MOD_SHIFT                           },
    {"\x1b[21;3~",   TB_KEY_F10,         TB_MOD_ALT                             },
    {"\x1b[21;4~",   TB_KEY_F10,         TB_MOD_ALT | TB_MOD_SHIFT              },
    {"\x1b[21;5~",   TB_KEY_F10,         TB_MOD_CTRL                            },
    {"\x1b[21;6~",   TB_KEY_F10,         TB_MOD_CTRL | TB_MOD_SHIFT             },
    {"\x1b[21;7~",   TB_KEY_F10,         TB_MOD_CTRL | TB_MOD_ALT               },
    {"\x1b[21;8~",   TB_KEY_F10,         TB_MOD_CTRL | TB_MOD_ALT | TB_MOD_SHIFT},

    {"\x1b[23;2~",   TB_KEY_F11,         TB_MOD_SHIFT                           },
    {"\x1b[23;3~",   TB_KEY_F11,         TB_MOD_ALT                             },
    {"\x1b[23;4~",   TB_KEY_F11,         TB_MOD_ALT | TB_MOD_SHIFT              },
    {"\x1b[23;5~",   TB_KEY_F11,         TB_MOD_CTRL                            },
    {"\x1b[23;6~",   TB_KEY_F11,         TB_MOD_CTRL | TB_MOD_SHIFT             },
    {"\x1b[23;7~",   TB_KEY_F11,         TB_MOD_CTRL | TB_MOD_ALT               },
    {"\x1b[23;8~",   TB_KEY_F11,         TB_MOD_CTRL | TB_MOD_ALT | TB_MOD_SHIFT},

    {"\x1b[24;2~",   TB_KEY_F12,         TB_MOD_SHIFT                           },
    {"\x1b[24;3~",   TB_KEY_F12,         TB_MOD_ALT                             },
    {"\x1b[24;4~",   TB_KEY_F12,         TB_MOD_ALT | TB_MOD_SHIFT              },
    {"\x1b[24;5~",   TB_KEY_F12,         TB_MOD_CTRL                            },
    {"\x1b[24;6~",   TB_KEY_F12,         TB_MOD_CTRL | TB_MOD_SHIFT             },
    {"\x1b[24;7~",   TB_KEY_F12,         TB_MOD_CTRL | TB_MOD_ALT               },
    {"\x1b[24;8~",   TB_KEY_F12,         TB_MOD_CTRL | TB_MOD_ALT | TB_MOD_SHIFT},

 // rxvt arrows
    {"\x1b[a",       TB_KEY_ARROW_UP,    TB_MOD_SHIFT                           },
    {"\x1b\x1b[A",   TB_KEY_ARROW_UP,    TB_MOD_ALT                             },
    {"\x1b\x1b[a",   TB_KEY_ARROW_UP,    TB_MOD_ALT | TB_MOD_SHIFT              },
    {"\x1bOa",       TB_KEY_ARROW_UP,    TB_MOD_CTRL                            },
    {"\x1b\x1bOa",   TB_KEY_ARROW_UP,    TB_MOD_CTRL | TB_MOD_ALT               },

    {"\x1b[b",       TB_KEY_ARROW_DOWN,  TB_MOD_SHIFT                           },
    {"\x1b\x1b[B",   TB_KEY_ARROW_DOWN,  TB_MOD_ALT                             },
    {"\x1b\x1b[b",   TB_KEY_ARROW_DOWN,  TB_MOD_ALT | TB_MOD_SHIFT              },
    {"\x1bOb",       TB_KEY_ARROW_DOWN,  TB_MOD_CTRL                            },
    {"\x1b\x1bOb",   TB_KEY_ARROW_DOWN,  TB_MOD_CTRL | TB_MOD_ALT               },

    {"\x1b[c",       TB_KEY_ARROW_RIGHT, TB_MOD_SHIFT                           },
    {"\x1b\x1b[C",   TB_KEY_ARROW_RIGHT, TB_MOD_ALT                             },
    {"\x1b\x1b[c",   TB_KEY_ARROW_RIGHT, TB_MOD_ALT | TB_MOD_SHIFT              },
    {"\x1bOc",       TB_KEY_ARROW_RIGHT, TB_MOD_CTRL                            },
    {"\x1b\x1bOc",   TB_KEY_ARROW_RIGHT, TB_MOD_CTRL | TB_MOD_ALT               },

    {"\x1b[d",       TB_KEY_ARROW_LEFT,  TB_MOD_SHIFT                           },
    {"\x1b\x1b[D",   TB_KEY_ARROW_LEFT,  TB_MOD_ALT                             },
    {"\x1b\x1b[d",   TB_KEY_ARROW_LEFT,  TB_MOD_ALT | TB_MOD_SHIFT              },
    {"\x1bOd",       TB_KEY_ARROW_LEFT,  TB_MOD_CTRL                            },
    {"\x1b\x1bOd",   TB_KEY_ARROW_LEFT,  TB_MOD_CTRL | TB_MOD_ALT               },

 // rxvt keys
    {"\x1b[7$",      TB_KEY_HOME,        TB_MOD_SHIFT                           },
    {"\x1b\x1b[7~",  TB_KEY_HOME,        TB_MOD_ALT                             },
    {"\x1b\x1b[7$",  TB_KEY_HOME,        TB_MOD_ALT | TB_MOD_SHIFT              },
    {"\x1b[7^",      TB_KEY_HOME,        TB_MOD_CTRL                            },
    {"\x1b[7@",      TB_KEY_HOME,        TB_MOD_CTRL | TB_MOD_SHIFT             },
    {"\x1b\x1b[7^",  TB_KEY_HOME,        TB_MOD_CTRL | TB_MOD_ALT               },
    {"\x1b\x1b[7@",  TB_KEY_HOME,        TB_MOD_CTRL | TB_MOD_ALT | TB_MOD_SHIFT},

    {"\x1b\x1b[8~",  TB_KEY_END,         TB_MOD_ALT                             },
    {"\x1b\x1b[8$",  TB_KEY_END,         TB_MOD_ALT | TB_MOD_SHIFT              },
    {"\x1b[8^",      TB_KEY_END,         TB_MOD_CTRL                            },
    {"\x1b\x1b[8^",  TB_KEY_END,         TB_MOD_CTRL | TB_MOD_ALT               },
    {"\x1b\x1b[8@",  TB_KEY_END,         TB_MOD_CTRL | TB_MOD_ALT | TB_MOD_SHIFT},
    {"\x1b[8@",      TB_KEY_END,         TB_MOD_CTRL | TB_MOD_SHIFT             },
    {"\x1b[8$",      TB_KEY_END,         TB_MOD_SHIFT                           },

    {"\x1b\x1b[2~",  TB_KEY_INSERT,      TB_MOD_ALT                             },
    {"\x1b\x1b[2$",  TB_KEY_INSERT,      TB_MOD_ALT | TB_MOD_SHIFT              },
    {"\x1b[2^",      TB_KEY_INSERT,      TB_MOD_CTRL                            },
    {"\x1b\x1b[2^",  TB_KEY_INSERT,      TB_MOD_CTRL | TB_MOD_ALT               },
    {"\x1b\x1b[2@",  TB_KEY_INSERT,      TB_MOD_CTRL | TB_MOD_ALT | TB_MOD_SHIFT},
    {"\x1b[2@",      TB_KEY_INSERT,      TB_MOD_CTRL | TB_MOD_SHIFT             },
    {"\x1b[2$",      TB_KEY_INSERT,      TB_MOD_SHIFT                           },

    {"\x1b\x1b[3~",  TB_KEY_DELETE,      TB_MOD_ALT                             },
    {"\x1b\x1b[3$",  TB_KEY_DELETE,      TB_MOD_ALT | TB_MOD_SHIFT              },
    {"\x1b[3^",      TB_KEY_DELETE,      TB_MOD_CTRL                            },
    {"\x1b\x1b[3^",  TB_KEY_DELETE,      TB_MOD_CTRL | TB_MOD_ALT               },
    {"\x1b\x1b[3@",  TB_KEY_DELETE,      TB_MOD_CTRL | TB_MOD_ALT | TB_MOD_SHIFT},
    {"\x1b[3@",      TB_KEY_DELETE,      TB_MOD_CTRL | TB_MOD_SHIFT             },
    {"\x1b[3$",      TB_KEY_DELETE,      TB_MOD_SHIFT                           },

    {"\x1b\x1b[5~",  TB_KEY_PGUP,        TB_MOD_ALT                             },
    {"\x1b\x1b[5$",  TB_KEY_PGUP,        TB_MOD_ALT | TB_MOD_SHIFT              },
    {"\x1b[5^",      TB_KEY_PGUP,        TB_MOD_CTRL                            },
    {"\x1b\x1b[5^",  TB_KEY_PGUP,        TB_MOD_CTRL | TB_MOD_ALT               },
    {"\x1b\x1b[5@",  TB_KEY_PGUP,        TB_MOD_CTRL | TB_MOD_ALT | TB_MOD_SHIFT},
    {"\x1b[5@",      TB_KEY_PGUP,        TB_MOD_CTRL | TB_MOD_SHIFT             },
    {"\x1b[5$",      TB_KEY_PGUP,        TB_MOD_SHIFT                           },

    {"\x1b\x1b[6~",  TB_KEY_PGDN,        TB_MOD_ALT                             },
    {"\x1b\x1b[6$",  TB_KEY_PGDN,        TB_MOD_ALT | TB_MOD_SHIFT              },
    {"\x1b[6^",      TB_KEY_PGDN,        TB_MOD_CTRL                            },
    {"\x1b\x1b[6^",  TB_KEY_PGDN,        TB_MOD_CTRL | TB_MOD_ALT               },
    {"\x1b\x1b[6@",  TB_KEY_PGDN,        TB_MOD_CTRL | TB_MOD_ALT | TB_MOD_SHIFT},
    {"\x1b[6@",      TB_KEY_PGDN,        TB_MOD_CTRL | TB_MOD_SHIFT             },
    {"\x1b[6$",      TB_KEY_PGDN,        TB_MOD_SHIFT                           },

    {"\x1b\x1b[11~", TB_KEY_F1,          TB_MOD_ALT                             },
    {"\x1b\x1b[23~", TB_KEY_F1,          TB_MOD_ALT | TB_MOD_SHIFT              },
    {"\x1b[11^",     TB_KEY_F1,          TB_MOD_CTRL                            },
    {"\x1b\x1b[11^", TB_KEY_F1,          TB_MOD_CTRL | TB_MOD_ALT               },
    {"\x1b\x1b[23^", TB_KEY_F1,          TB_MOD_CTRL | TB_MOD_ALT | TB_MOD_SHIFT},
    {"\x1b[23^",     TB_KEY_F1,          TB_MOD_CTRL | TB_MOD_SHIFT             },
    {"\x1b[23~",     TB_KEY_F1,          TB_MOD_SHIFT                           },

    {"\x1b\x1b[12~", TB_KEY_F2,          TB_MOD_ALT                             },
    {"\x1b\x1b[24~", TB_KEY_F2,          TB_MOD_ALT | TB_MOD_SHIFT              },
    {"\x1b[12^",     TB_KEY_F2,          TB_MOD_CTRL                            },
    {"\x1b\x1b[12^", TB_KEY_F2,          TB_MOD_CTRL | TB_MOD_ALT               },
    {"\x1b\x1b[24^", TB_KEY_F2,          TB_MOD_CTRL | TB_MOD_ALT | TB_MOD_SHIFT},
    {"\x1b[24^",     TB_KEY_F2,          TB_MOD_CTRL | TB_MOD_SHIFT             },
    {"\x1b[24~",     TB_KEY_F2,          TB_MOD_SHIFT                           },

    {"\x1b\x1b[13~", TB_KEY_F3,          TB_MOD_ALT                             },
    {"\x1b\x1b[25~", TB_KEY_F3,          TB_MOD_ALT | TB_MOD_SHIFT              },
    {"\x1b[13^",     TB_KEY_F3,          TB_MOD_CTRL                            },
    {"\x1b\x1b[13^", TB_KEY_F3,          TB_MOD_CTRL | TB_MOD_ALT               },
    {"\x1b\x1b[25^", TB_KEY_F3,          TB_MOD_CTRL | TB_MOD_ALT | TB_MOD_SHIFT},
    {"\x1b[25^",     TB_KEY_F3,          TB_MOD_CTRL | TB_MOD_SHIFT             },
    {"\x1b[25~",     TB_KEY_F3,          TB_MOD_SHIFT                           },

    {"\x1b\x1b[14~", TB_KEY_F4,          TB_MOD_ALT                             },
    {"\x1b\x1b[26~", TB_KEY_F4,          TB_MOD_ALT | TB_MOD_SHIFT              },
    {"\x1b[14^",     TB_KEY_F4,          TB_MOD_CTRL                            },
    {"\x1b\x1b[14^", TB_KEY_F4,          TB_MOD_CTRL | TB_MOD_ALT               },
    {"\x1b\x1b[26^", TB_KEY_F4,          TB_MOD_CTRL | TB_MOD_ALT | TB_MOD_SHIFT},
    {"\x1b[26^",     TB_KEY_F4,          TB_MOD_CTRL | TB_MOD_SHIFT             },
    {"\x1b[26~",     TB_KEY_F4,          TB_MOD_SHIFT                           },

    {"\x1b\x1b[15~", TB_KEY_F5,          TB_MOD_ALT                             },
    {"\x1b\x1b[28~", TB_KEY_F5,          TB_MOD_ALT | TB_MOD_SHIFT              },
    {"\x1b[15^",     TB_KEY_F5,          TB_MOD_CTRL                            },
    {"\x1b\x1b[15^", TB_KEY_F5,          TB_MOD_CTRL | TB_MOD_ALT               },
    {"\x1b\x1b[28^", TB_KEY_F5,          TB_MOD_CTRL | TB_MOD_ALT | TB_MOD_SHIFT},
    {"\x1b[28^",     TB_KEY_F5,          TB_MOD_CTRL | TB_MOD_SHIFT             },
    {"\x1b[28~",     TB_KEY_F5,          TB_MOD_SHIFT                           },

    {"\x1b\x1b[17~", TB_KEY_F6,          TB_MOD_ALT                             },
    {"\x1b\x1b[29~", TB_KEY_F6,          TB_MOD_ALT | TB_MOD_SHIFT              },
    {"\x1b[17^",     TB_KEY_F6,          TB_MOD_CTRL                            },
    {"\x1b\x1b[17^", TB_KEY_F6,          TB_MOD_CTRL | TB_MOD_ALT               },
    {"\x1b\x1b[29^", TB_KEY_F6,          TB_MOD_CTRL | TB_MOD_ALT | TB_MOD_SHIFT},
    {"\x1b[29^",     TB_KEY_F6,          TB_MOD_CTRL | TB_MOD_SHIFT             },
    {"\x1b[29~",     TB_KEY_F6,          TB_MOD_SHIFT                           },

    {"\x1b\x1b[18~", TB_KEY_F7,          TB_MOD_ALT                             },
    {"\x1b\x1b[31~", TB_KEY_F7,          TB_MOD_ALT | TB_MOD_SHIFT              },
    {"\x1b[18^",     TB_KEY_F7,          TB_MOD_CTRL                            },
    {"\x1b\x1b[18^", TB_KEY_F7,          TB_MOD_CTRL | TB_MOD_ALT               },
    {"\x1b\x1b[31^", TB_KEY_F7,          TB_MOD_CTRL | TB_MOD_ALT | TB_MOD_SHIFT},
    {"\x1b[31^",     TB_KEY_F7,          TB_MOD_CTRL | TB_MOD_SHIFT             },
    {"\x1b[31~",     TB_KEY_F7,          TB_MOD_SHIFT                           },

    {"\x1b\x1b[19~", TB_KEY_F8,          TB_MOD_ALT                             },
    {"\x1b\x1b[32~", TB_KEY_F8,          TB_MOD_ALT | TB_MOD_SHIFT              },
    {"\x1b[19^",     TB_KEY_F8,          TB_MOD_CTRL                            },
    {"\x1b\x1b[19^", TB_KEY_F8,          TB_MOD_CTRL | TB_MOD_ALT               },
    {"\x1b\x1b[32^", TB_KEY_F8,          TB_MOD_CTRL | TB_MOD_ALT | TB_MOD_SHIFT},
    {"\x1b[32^",     TB_KEY_F8,          TB_MOD_CTRL | TB_MOD_SHIFT             },
    {"\x1b[32~",     TB_KEY_F8,          TB_MOD_SHIFT                           },

    {"\x1b\x1b[20~", TB_KEY_F9,          TB_MOD_ALT                             },
    {"\x1b\x1b[33~", TB_KEY_F9,          TB_MOD_ALT | TB_MOD_SHIFT              },
    {"\x1b[20^",     TB_KEY_F9,          TB_MOD_CTRL                            },
    {"\x1b\x1b[20^", TB_KEY_F9,          TB_MOD_CTRL | TB_MOD_ALT               },
    {"\x1b\x1b[33^", TB_KEY_F9,          TB_MOD_CTRL | TB_MOD_ALT | TB_MOD_SHIFT},
    {"\x1b[33^",     TB_KEY_F9,          TB_MOD_CTRL | TB_MOD_SHIFT             },
    {"\x1b[33~",     TB_KEY_F9,          TB_MOD_SHIFT                           },

    {"\x1b\x1b[21~", TB_KEY_F10,         TB_MOD_ALT                             },
    {"\x1b\x1b[34~", TB_KEY_F10,         TB_MOD_ALT | TB_MOD_SHIFT              },
    {"\x1b[21^",     TB_KEY_F10,         TB_MOD_CTRL                            },
    {"\x1b\x1b[21^", TB_KEY_F10,         TB_MOD_CTRL | TB_MOD_ALT               },
    {"\x1b\x1b[34^", TB_KEY_F10,         TB_MOD_CTRL | TB_MOD_ALT | TB_MOD_SHIFT},
    {"\x1b[34^",     TB_KEY_F10,         TB_MOD_CTRL | TB_MOD_SHIFT             },
    {"\x1b[34~",     TB_KEY_F10,         TB_MOD_SHIFT                           },

    {"\x1b\x1b[23~", TB_KEY_F11,         TB_MOD_ALT                             },
    {"\x1b\x1b[23$", TB_KEY_F11,         TB_MOD_ALT | TB_MOD_SHIFT              },
    {"\x1b[23^",     TB_KEY_F11,         TB_MOD_CTRL                            },
    {"\x1b\x1b[23^", TB_KEY_F11,         TB_MOD_CTRL | TB_MOD_ALT               },
    {"\x1b\x1b[23@", TB_KEY_F11,         TB_MOD_CTRL | TB_MOD_ALT | TB_MOD_SHIFT},
    {"\x1b[23@",     TB_KEY_F11,         TB_MOD_CTRL | TB_MOD_SHIFT             },
    {"\x1b[23$",     TB_KEY_F11,         TB_MOD_SHIFT                           },

    {"\x1b\x1b[24~", TB_KEY_F12,         TB_MOD_ALT                             },
    {"\x1b\x1b[24$", TB_KEY_F12,         TB_MOD_ALT | TB_MOD_SHIFT              },
    {"\x1b[24^",     TB_KEY_F12,         TB_MOD_CTRL                            },
    {"\x1b\x1b[24^", TB_KEY_F12,         TB_MOD_CTRL | TB_MOD_ALT               },
    {"\x1b\x1b[24@", TB_KEY_F12,         TB_MOD_CTRL | TB_MOD_ALT | TB_MOD_SHIFT},
    {"\x1b[24@",     TB_KEY_F12,         TB_MOD_CTRL | TB_MOD_SHIFT             },
    {"\x1b[24$",     TB_KEY_F12,         TB_MOD_SHIFT                           },

 // linux console/putty arrows
    {"\x1b[A",       TB_KEY_ARROW_UP,    TB_MOD_SHIFT                           },
    {"\x1b[B",       TB_KEY_ARROW_DOWN,  TB_MOD_SHIFT                           },
    {"\x1b[C",       TB_KEY_ARROW_RIGHT, TB_MOD_SHIFT                           },
    {"\x1b[D",       TB_KEY_ARROW_LEFT,  TB_MOD_SHIFT                           },

 // more putty arrows
    {"\x1bOA",       TB_KEY_ARROW_UP,    TB_MOD_CTRL                            },
    {"\x1b\x1bOA",   TB_KEY_ARROW_UP,    TB_MOD_CTRL | TB_MOD_ALT               },
    {"\x1bOB",       TB_KEY_ARROW_DOWN,  TB_MOD_CTRL                            },
    {"\x1b\x1bOB",   TB_KEY_ARROW_DOWN,  TB_MOD_CTRL | TB_MOD_ALT               },
    {"\x1bOC",       TB_KEY_ARROW_RIGHT, TB_MOD_CTRL                            },
    {"\x1b\x1bOC",   TB_KEY_ARROW_RIGHT, TB_MOD_CTRL | TB_MOD_ALT               },
    {"\x1bOD",       TB_KEY_ARROW_LEFT,  TB_MOD_CTRL                            },
    {"\x1b\x1bOD",   TB_KEY_ARROW_LEFT,  TB_MOD_CTRL | TB_MOD_ALT               },

    {NULL,           0,                  0                                      },
};

static const unsigned char utf8_length[256] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 1, 1};

static const unsigned char utf8_mask[6] = {0x7f, 0x1f, 0x0f, 0x07, 0x03, 0x01};

static int tb_reset(void);
static int tb_printf_inner(int x, int y, uintattr_t fg, uintattr_t bg,
    size_t *out_w, const char *fmt, va_list vl);
static int init_term_attrs(void);
static int init_term_caps(void);
static int init_cap_trie(void);
static int cap_trie_add(const char *cap, uint16_t key, uint8_t mod);
static int cap_trie_find(const char *buf, size_t nbuf, struct cap_trie_t **last,
    size_t *depth);
static int cap_trie_deinit(struct cap_trie_t *node);
static int init_resize_handler(void);
static int send_init_escape_codes(void);
static int send_clear(void);
static int update_term_size(void);
static int update_term_size_via_esc(void);
static int init_cellbuf(void);
static int tb_deinit(void);
static int load_terminfo(void);
static int load_terminfo_from_path(const char *path, const char *term);
static int read_terminfo_path(const char *path);
static int parse_terminfo_caps(void);
static int load_builtin_caps(void);
static const char *get_terminfo_string(int16_t str_offsets_pos,
    int16_t str_table_pos, int16_t str_table_len, int16_t str_index);
static int wait_event(struct tb_event *event, int timeout);
static int extract_event(struct tb_event *event);
static int extract_esc(struct tb_event *event);
static int extract_esc_user(struct tb_event *event, int is_post);
static int extract_esc_cap(struct tb_event *event);
static int extract_esc_mouse(struct tb_event *event);
static int resize_cellbufs(void);
static void handle_resize(int sig);
static int send_attr(uintattr_t fg, uintattr_t bg);
static int send_sgr(uintattr_t fg, uintattr_t bg, uintattr_t fg_is_default,
    uintattr_t bg_is_default);
static int send_cursor_if(int x, int y);
static int send_char(int x, int y, uint32_t ch);
static int send_cluster(int x, int y, uint32_t *ch, size_t nch);
static int convert_num(uint32_t num, char *buf);
static int cell_cmp(struct tb_cell *a, struct tb_cell *b);
static int cell_copy(struct tb_cell *dst, struct tb_cell *src);
static int cell_set(struct tb_cell *cell, uint32_t *ch, size_t nch,
    uintattr_t fg, uintattr_t bg);
static int cell_reserve_ech(struct tb_cell *cell, size_t n);
static int cell_free(struct tb_cell *cell);
static int cellbuf_init(struct cellbuf_t *c, int w, int h);
static int cellbuf_free(struct cellbuf_t *c);
static int cellbuf_clear(struct cellbuf_t *c);
static int cellbuf_get(struct cellbuf_t *c, int x, int y, struct tb_cell **out);
static int cellbuf_resize(struct cellbuf_t *c, int w, int h);
static int bytebuf_puts(struct bytebuf_t *b, const char *str);
static int bytebuf_nputs(struct bytebuf_t *b, const char *str, size_t nstr);
static int bytebuf_shift(struct bytebuf_t *b, size_t n);
static int bytebuf_flush(struct bytebuf_t *b, int fd);
static int bytebuf_reserve(struct bytebuf_t *b, size_t sz);
static int bytebuf_free(struct bytebuf_t *b);

#endif /* TB_IMPL */
