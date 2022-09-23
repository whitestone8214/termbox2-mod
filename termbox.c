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


#include "termbox.h"


int tb_init(void) {
    return tb_init_file("/dev/tty");
}

int tb_init_file(const char *path) {
    if (global.initialized) {
        return TB_ERR_INIT_ALREADY;
    }
    int ttyfd = open(path, O_RDWR);
    if (ttyfd < 0) {
        global.last_errno = errno;
        return TB_ERR_INIT_OPEN;
    }
    global.ttyfd_open = 1;
    return tb_init_fd(ttyfd);
}

int tb_init_fd(int ttyfd) {
    return tb_init_rwfd(ttyfd, ttyfd);
}

int tb_init_rwfd(int rfd, int wfd) {
    int rv;

    tb_reset();
    global.ttyfd = rfd == wfd && isatty(rfd) ? rfd : -1;
    global.rfd = rfd;
    global.wfd = wfd;

    do {
        if_err_break(rv, init_term_attrs());
        if_err_break(rv, init_term_caps());
        if_err_break(rv, init_cap_trie());
        if_err_break(rv, init_resize_handler());
        if_err_break(rv, send_init_escape_codes());
        if_err_break(rv, send_clear());
        if_err_break(rv, update_term_size());
        if_err_break(rv, init_cellbuf());
        global.initialized = 1;
    } while (0);

    if (rv != TB_OK) {
        tb_deinit();
    }

    return rv;
}

int tb_shutdown(void) {
    if_not_init_return();
    tb_deinit();
    return TB_OK;
}

int tb_width(void) {
    if_not_init_return();
    return global.width;
}

int tb_height(void) {
    if_not_init_return();
    return global.height;
}

int tb_clear(void) {
    if_not_init_return();
    return cellbuf_clear(&global.back);
}

int tb_set_clear_attrs(uintattr_t fg, uintattr_t bg) {
    if_not_init_return();
    global.fg = fg;
    global.bg = bg;
    return TB_OK;
}

int tb_present(void) {
    if_not_init_return();

    int rv;

    // TODO Assert global.back.(width,height) == global.front.(width,height)

    global.last_x = -1;
    global.last_y = -1;

    int x, y, i;
    for (y = 0; y < global.front.height; y++) {
        for (x = 0; x < global.front.width;) {
            struct tb_cell *back, *front;
            if_err_return(rv, cellbuf_get(&global.back, x, y, &back));
            if_err_return(rv, cellbuf_get(&global.front, x, y, &front));

            int w;
            {
#ifdef TB_OPT_EGC
                if (back->nech > 0)
                    w = wcswidth((wchar_t *)back->ech, back->nech);
                else
#endif
                    /* wcwidth() simply returns -1 on overflow of wchar_t */
                    w = wcwidth((wchar_t)back->ch);
            }
            if (w < 1) {
                w = 1;
            }

            if (cell_cmp(back, front) != 0) {
                cell_copy(front, back);

                send_attr(back->fg, back->bg);
                if (w > 1 && x >= global.front.width - (w - 1)) {
                    for (i = x; i < global.front.width; i++) {
                        send_char(i, y, ' ');
                    }
                } else {
                    {
#ifdef TB_OPT_EGC
                        if (back->nech > 0)
                            send_cluster(x, y, back->ech, back->nech);
                        else
#endif
                            send_char(x, y, back->ch);
                    }
                    for (i = 1; i < w; i++) {
                        struct tb_cell *front_wide;
                        if_err_return(rv,
                            cellbuf_get(&global.front, x + i, y, &front_wide));
                        if_err_return(rv,
                            cell_set(front_wide, 0, 1, back->fg, back->bg));
                    }
                }
            }
            x += w;
        }
    }

    if_err_return(rv, send_cursor_if(global.cursor_x, global.cursor_y));
    if_err_return(rv, bytebuf_flush(&global.out, global.wfd));

    return TB_OK;
}

int tb_set_cursor(int cx, int cy) {
    if_not_init_return();
    int rv;
    if (cx < 0)
        cx = 0;
    if (cy < 0)
        cy = 0;
    if (global.cursor_x == -1) {
        if_err_return(rv,
            bytebuf_puts(&global.out, global.caps[TB_CAP_SHOW_CURSOR]));
    }
    if_err_return(rv, send_cursor_if(cx, cy));
    global.cursor_x = cx;
    global.cursor_y = cy;
    return TB_OK;
}

int tb_hide_cursor(void) {
    if_not_init_return();
    int rv;
    if (global.cursor_x >= 0) {
        if_err_return(rv,
            bytebuf_puts(&global.out, global.caps[TB_CAP_HIDE_CURSOR]));
    }
    global.cursor_x = -1;
    global.cursor_y = -1;
    return TB_OK;
}

int tb_set_cell(int x, int y, uint32_t ch, uintattr_t fg, uintattr_t bg) {
    if_not_init_return();
    return tb_set_cell_ex(x, y, &ch, 1, fg, bg);
}

int tb_set_cell_ex(int x, int y, uint32_t *ch, size_t nch, uintattr_t fg,
    uintattr_t bg) {
    if_not_init_return();
    int rv;
    struct tb_cell *cell;
    if_err_return(rv, cellbuf_get(&global.back, x, y, &cell));
    if_err_return(rv, cell_set(cell, ch, nch, fg, bg));
    return TB_OK;
}

int tb_extend_cell(int x, int y, uint32_t ch) {
    if_not_init_return();
#ifdef TB_OPT_EGC
    int rv;
    struct tb_cell *cell;
    size_t nech;
    if_err_return(rv, cellbuf_get(&global.back, x, y, &cell));
    if (cell->nech > 0) { // append to ech
        nech = cell->nech + 1;
        if_err_return(rv, cell_reserve_ech(cell, nech));
        cell->ech[nech - 1] = ch;
    } else { // make new ech
        nech = 2;
        if_err_return(rv, cell_reserve_ech(cell, nech));
        cell->ech[0] = cell->ch;
        cell->ech[1] = ch;
    }
    cell->ech[nech] = '\0';
    cell->nech = nech;
    return TB_OK;
#else
    (void)x;
    (void)y;
    (void)ch;
    return TB_ERR;
#endif
}

int tb_set_input_mode(int mode) {
    if_not_init_return();
    if (mode == TB_INPUT_CURRENT) {
        return global.input_mode;
    }

    if ((mode & (TB_INPUT_ESC | TB_INPUT_ALT)) == 0) {
        mode |= TB_INPUT_ESC;
    }

    if ((mode & (TB_INPUT_ESC | TB_INPUT_ALT)) == (TB_INPUT_ESC | TB_INPUT_ALT))
    {
        mode &= ~TB_INPUT_ALT;
    }

    if (mode & TB_INPUT_MOUSE) {
        bytebuf_puts(&global.out, TB_HARDCAP_ENTER_MOUSE);
        bytebuf_flush(&global.out, global.wfd);
    } else {
        bytebuf_puts(&global.out, TB_HARDCAP_EXIT_MOUSE);
        bytebuf_flush(&global.out, global.wfd);
    }

    global.input_mode = mode;
    return TB_OK;
}

int tb_set_output_mode(int mode) {
    if_not_init_return();
    switch (mode) {
        case TB_OUTPUT_CURRENT:
            return global.output_mode;
        case TB_OUTPUT_NORMAL:
        case TB_OUTPUT_256:
        case TB_OUTPUT_216:
        case TB_OUTPUT_GRAYSCALE:
#ifdef TB_OPT_TRUECOLOR
        case TB_OUTPUT_TRUECOLOR:
#endif
            global.output_mode = mode;
            return TB_OK;
    }
    return TB_ERR;
}

int tb_peek_event(struct tb_event *event, int timeout_ms) {
    if_not_init_return();
    return wait_event(event, timeout_ms);
}

int tb_poll_event(struct tb_event *event) {
    if_not_init_return();
    return wait_event(event, -1);
}

int tb_get_fds(int *ttyfd, int *resizefd) {
    if_not_init_return();

    *ttyfd = global.rfd;
    *resizefd = global.resize_pipefd[0];

    return TB_OK;
}

int tb_print(int x, int y, uintattr_t fg, uintattr_t bg, const char *str) {
    return tb_print_ex(x, y, fg, bg, NULL, str);
}

int tb_print_ex(int x, int y, uintattr_t fg, uintattr_t bg, size_t *out_w,
    const char *str) {
    int rv;
    uint32_t uni;
    int w, ix = x;
    if (out_w) {
        *out_w = 0;
    }
    while (*str) {
        str += tb_utf8_char_to_unicode(&uni, str);
        w = wcwidth((wchar_t)uni);
        if (w < 0) {
            w = 1;
        }
        if (w == 0 && x > ix) {
            if_err_return(rv, tb_extend_cell(x - 1, y, uni));
        } else {
            if_err_return(rv, tb_set_cell(x, y, uni, fg, bg));
        }
        x += w;
        if (out_w) {
            *out_w += w;
        }
    }
    return TB_OK;
}

int tb_printf(int x, int y, uintattr_t fg, uintattr_t bg, const char *fmt,
    ...) {
    int rv;
    va_list vl;
    va_start(vl, fmt);
    rv = tb_printf_inner(x, y, fg, bg, NULL, fmt, vl);
    va_end(vl);
    return rv;
}

int tb_printf_ex(int x, int y, uintattr_t fg, uintattr_t bg, size_t *out_w,
    const char *fmt, ...) {
    int rv;
    va_list vl;
    va_start(vl, fmt);
    rv = tb_printf_inner(x, y, fg, bg, out_w, fmt, vl);
    va_end(vl);
    return rv;
}

int tb_send(const char *buf, size_t nbuf) {
    return bytebuf_nputs(&global.out, buf, nbuf);
}

int tb_sendf(const char *fmt, ...) {
    int rv;
    char buf[TB_OPT_PRINTF_BUF];
    va_list vl;
    va_start(vl, fmt);
    rv = vsnprintf(buf, sizeof(buf), fmt, vl);
    va_end(vl);
    if (rv < 0 || rv >= (int)sizeof(buf)) {
        return TB_ERR;
    }
    return tb_send(buf, (size_t)rv);
}

int tb_set_func(int fn_type, int (*fn)(struct tb_event *, size_t *)) {
    switch (fn_type) {
        case TB_FUNC_EXTRACT_PRE:
            global.fn_extract_esc_pre = fn;
            return TB_OK;
        case TB_FUNC_EXTRACT_POST:
            global.fn_extract_esc_post = fn;
            return TB_OK;
    }
    return TB_ERR;
}

struct tb_cell *tb_cell_buffer(void) {
    if (!global.initialized)
        return NULL;
    return global.back.cells;
}

int tb_utf8_char_length(char c) {
    return utf8_length[(unsigned char)c];
}

int tb_utf8_char_to_unicode(uint32_t *out, const char *c) {
    if (*c == 0) {
        return TB_ERR;
    }

    int i;
    unsigned char len = tb_utf8_char_length(*c);
    unsigned char mask = utf8_mask[len - 1];
    uint32_t result = c[0] & mask;
    for (i = 1; i < len; ++i) {
        result <<= 6;
        result |= c[i] & 0x3f;
    }

    *out = result;
    return (int)len;
}

int tb_utf8_unicode_to_char(char *out, uint32_t c) {
    int len = 0;
    int first;
    int i;

    if (c < 0x80) {
        first = 0;
        len = 1;
    } else if (c < 0x800) {
        first = 0xc0;
        len = 2;
    } else if (c < 0x10000) {
        first = 0xe0;
        len = 3;
    } else if (c < 0x200000) {
        first = 0xf0;
        len = 4;
    } else if (c < 0x4000000) {
        first = 0xf8;
        len = 5;
    } else {
        first = 0xfc;
        len = 6;
    }

    for (i = len - 1; i > 0; --i) {
        out[i] = (c & 0x3f) | 0x80;
        c >>= 6;
    }
    out[0] = c | first;

    return len;
}

int tb_last_errno(void) {
    return global.last_errno;
}

const char *tb_strerror(int err) {
    switch (err) {
        case TB_OK:
            return "Success";
        case TB_ERR_NEED_MORE:
            return "Not enough input";
        case TB_ERR_INIT_ALREADY:
            return "Termbox initialized already";
        case TB_ERR_MEM:
            return "Out of memory";
        case TB_ERR_NO_EVENT:
            return "No event";
        case TB_ERR_NO_TERM:
            return "No TERM in environment";
        case TB_ERR_NOT_INIT:
            return "Termbox not initialized";
        case TB_ERR_OUT_OF_BOUNDS:
            return "Out of bounds";
        case TB_ERR_UNSUPPORTED_TERM:
            return "Unsupported terminal";
        case TB_ERR_CAP_COLLISION:
            return "Termcaps collision";
        case TB_ERR_RESIZE_SSCANF:
            return "Terminal width/height not received by sscanf() after "
                   "resize";
        case TB_ERR:
        case TB_ERR_INIT_OPEN:
        case TB_ERR_READ:
        case TB_ERR_RESIZE_IOCTL:
        case TB_ERR_RESIZE_PIPE:
        case TB_ERR_RESIZE_SIGACTION:
        case TB_ERR_POLL:
        case TB_ERR_TCGETATTR:
        case TB_ERR_TCSETATTR:
        case TB_ERR_RESIZE_WRITE:
        case TB_ERR_RESIZE_POLL:
        case TB_ERR_RESIZE_READ:
        default:
            strerror_r(global.last_errno, global.errbuf, sizeof(global.errbuf));
            return (const char *)global.errbuf;
    }
}

int tb_has_truecolor(void) {
#ifdef TB_OPT_TRUECOLOR
    return 1;
#else
    return 0;
#endif
}

int tb_has_egc(void) {
#ifdef TB_OPT_EGC
    return 1;
#else
    return 0;
#endif
}

const char *tb_version(void) {
    return TB_VERSION_STR;
}

static int tb_reset(void) {
    int ttyfd_open = global.ttyfd_open;
    memset(&global, 0, sizeof(global));
    global.ttyfd = -1;
    global.rfd = -1;
    global.wfd = -1;
    global.ttyfd_open = ttyfd_open;
    global.resize_pipefd[0] = -1;
    global.resize_pipefd[1] = -1;
    global.width = -1;
    global.height = -1;
    global.cursor_x = -1;
    global.cursor_y = -1;
    global.last_x = -1;
    global.last_y = -1;
    global.fg = TB_DEFAULT;
    global.bg = TB_DEFAULT;
    global.last_fg = ~global.fg;
    global.last_bg = ~global.bg;
    global.input_mode = TB_INPUT_ESC;
    global.output_mode = TB_OUTPUT_NORMAL;
    return TB_OK;
}

static int init_term_attrs(void) {
    if (global.ttyfd < 0) {
        return TB_OK;
    }

    if (tcgetattr(global.ttyfd, &global.orig_tios) != 0) {
        global.last_errno = errno;
        return TB_ERR_TCGETATTR;
    }

    struct termios tios;
    memcpy(&tios, &global.orig_tios, sizeof(tios));
    global.has_orig_tios = 1;

    cfmakeraw(&tios);
    tios.c_cc[VMIN] = 1;
    tios.c_cc[VTIME] = 0;

    if (tcsetattr(global.ttyfd, TCSAFLUSH, &tios) != 0) {
        global.last_errno = errno;
        return TB_ERR_TCSETATTR;
    }

    return TB_OK;
}

int tb_printf_inner(int x, int y, uintattr_t fg, uintattr_t bg, size_t *out_w,
    const char *fmt, va_list vl) {
    int rv;
    char buf[TB_OPT_PRINTF_BUF];
    rv = vsnprintf(buf, sizeof(buf), fmt, vl);
    if (rv < 0 || rv >= (int)sizeof(buf)) {
        return TB_ERR;
    }
    return tb_print_ex(x, y, fg, bg, out_w, buf);
}

static int init_term_caps(void) {
    if (load_terminfo() == TB_OK) {
        return parse_terminfo_caps();
    }
    return load_builtin_caps();
}

static int init_cap_trie(void) {
    int rv, i;

    // Add caps from terminfo or built-in
    for (i = 0; i < TB_CAP__COUNT_KEYS; i++) {
        if_err_return(rv, cap_trie_add(global.caps[i], tb_key_i(i), 0));
    }

    // Add built-in mod caps
    for (i = 0; builtin_mod_caps[i].cap != NULL; i++) {
        rv = cap_trie_add(builtin_mod_caps[i].cap, builtin_mod_caps[i].key,
            builtin_mod_caps[i].mod);
        // Collisions are OK. This can happen if global.caps collides with
        // builtin_mod_caps. It is desirable to give precedence to global.caps
        // here.
        if (rv != TB_OK && rv != TB_ERR_CAP_COLLISION) {
            return rv;
        }
    }

    return TB_OK;
}

static int cap_trie_add(const char *cap, uint16_t key, uint8_t mod) {
    struct cap_trie_t *next, *node = &global.cap_trie;
    size_t i, j;
    for (i = 0; cap[i] != '\0'; i++) {
        char c = cap[i];
        next = NULL;

        // Check if c is already a child of node
        for (j = 0; j < node->nchildren; j++) {
            if (node->children[j].c == c) {
                next = &node->children[j];
                break;
            }
        }
        if (!next) {
            // We need to add a new child to node
            node->nchildren += 1;
            node->children =
                tb_realloc(node->children, sizeof(*node) * node->nchildren);
            if (!node->children) {
                return TB_ERR_MEM;
            }
            next = &node->children[node->nchildren - 1];
            memset(next, 0, sizeof(*next));
            next->c = c;
        }

        // Continue
        node = next;
    }

    if (node->is_leaf) {
        // Already a leaf here
        return TB_ERR_CAP_COLLISION;
    }

    node->is_leaf = 1;
    node->key = key;
    node->mod = mod;
    return TB_OK;
}

static int cap_trie_find(const char *buf, size_t nbuf, struct cap_trie_t **last,
    size_t *depth) {
    struct cap_trie_t *next, *node = &global.cap_trie;
    size_t i, j;
    *last = node;
    *depth = 0;
    for (i = 0; i < nbuf; i++) {
        char c = buf[i];
        next = NULL;

        // Find c in node.children
        for (j = 0; j < node->nchildren; j++) {
            if (node->children[j].c == c) {
                next = &node->children[j];
                break;
            }
        }
        if (!next) {
            // Not found
            return TB_OK;
        }
        node = next;
        *last = node;
        *depth += 1;
        if (node->is_leaf && node->nchildren < 1) {
            break;
        }
    }
    return TB_OK;
}

static int cap_trie_deinit(struct cap_trie_t *node) {
    size_t j;
    for (j = 0; j < node->nchildren; j++) {
        cap_trie_deinit(&node->children[j]);
    }
    if (node->children) {
        tb_free(node->children);
    }
    memset(node, 0, sizeof(*node));
    return TB_OK;
}

static int init_resize_handler(void) {
    if (pipe(global.resize_pipefd) != 0) {
        global.last_errno = errno;
        return TB_ERR_RESIZE_PIPE;
    }

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_resize;
    if (sigaction(SIGWINCH, &sa, NULL) != 0) {
        global.last_errno = errno;
        return TB_ERR_RESIZE_SIGACTION;
    }

    return TB_OK;
}

static int send_init_escape_codes(void) {
    int rv;
    if_err_return(rv, bytebuf_puts(&global.out, global.caps[TB_CAP_ENTER_CA]));
    if_err_return(rv,
        bytebuf_puts(&global.out, global.caps[TB_CAP_ENTER_KEYPAD]));
    if_err_return(rv,
        bytebuf_puts(&global.out, global.caps[TB_CAP_HIDE_CURSOR]));
    return TB_OK;
}

static int send_clear(void) {
    int rv;

    if_err_return(rv, send_attr(global.fg, global.bg));
    if_err_return(rv,
        bytebuf_puts(&global.out, global.caps[TB_CAP_CLEAR_SCREEN]));

    if_err_return(rv, send_cursor_if(global.cursor_x, global.cursor_y));
    if_err_return(rv, bytebuf_flush(&global.out, global.wfd));

    global.last_x = -1;
    global.last_y = -1;

    return TB_OK;
}

static int update_term_size(void) {
    int rv, ioctl_errno;

    if (global.ttyfd < 0) {
        return TB_OK;
    }

    struct winsize sz;
    memset(&sz, 0, sizeof(sz));

    // Try ioctl TIOCGWINSZ
    if (ioctl(global.ttyfd, TIOCGWINSZ, &sz) == 0) {
        global.width = sz.ws_col;
        global.height = sz.ws_row;
        return TB_OK;
    }
    ioctl_errno = errno;

    // Try >cursor(9999,9999), >u7, <u6
    if_ok_return(rv, update_term_size_via_esc());

    global.last_errno = ioctl_errno;
    return TB_ERR_RESIZE_IOCTL;
}

static int update_term_size_via_esc(void) {
#ifndef TB_RESIZE_FALLBACK_MS
#define TB_RESIZE_FALLBACK_MS 1000
#endif

    char *move_and_report = "\x1b[9999;9999H\x1b[6n";
    ssize_t write_rv =
        write(global.wfd, move_and_report, strlen(move_and_report));
    if (write_rv != (ssize_t)strlen(move_and_report)) {
        return TB_ERR_RESIZE_WRITE;
    }

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(global.rfd, &fds);

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = TB_RESIZE_FALLBACK_MS * 1000;

    int select_rv = select(global.rfd + 1, &fds, NULL, NULL, &timeout);

    if (select_rv != 1) {
        global.last_errno = errno;
        return TB_ERR_RESIZE_POLL;
    }

    char buf[TB_OPT_READ_BUF];
    ssize_t read_rv = read(global.rfd, buf, sizeof(buf) - 1);
    if (read_rv < 1) {
        global.last_errno = errno;
        return TB_ERR_RESIZE_READ;
    }
    buf[read_rv] = '\0';

    int rw, rh;
    if (sscanf(buf, "\x1b[%d;%dR", &rh, &rw) != 2) {
        return TB_ERR_RESIZE_SSCANF;
    }

    global.width = rw;
    global.height = rh;
    return TB_OK;
}

static int init_cellbuf(void) {
    int rv;
    if_err_return(rv, cellbuf_init(&global.back, global.width, global.height));
    if_err_return(rv, cellbuf_init(&global.front, global.width, global.height));
    if_err_return(rv, cellbuf_clear(&global.back));
    if_err_return(rv, cellbuf_clear(&global.front));
    return TB_OK;
}

static int tb_deinit(void) {
    if (global.caps[0] != NULL && global.wfd >= 0) {
        bytebuf_puts(&global.out, global.caps[TB_CAP_SHOW_CURSOR]);
        bytebuf_puts(&global.out, global.caps[TB_CAP_SGR0]);
        bytebuf_puts(&global.out, global.caps[TB_CAP_CLEAR_SCREEN]);
        bytebuf_puts(&global.out, global.caps[TB_CAP_EXIT_CA]);
        bytebuf_puts(&global.out, global.caps[TB_CAP_EXIT_KEYPAD]);
        bytebuf_puts(&global.out, TB_HARDCAP_EXIT_MOUSE);
        bytebuf_flush(&global.out, global.wfd);
    }
    if (global.ttyfd >= 0) {
        if (global.has_orig_tios) {
            tcsetattr(global.ttyfd, TCSAFLUSH, &global.orig_tios);
        }
        if (global.ttyfd_open) {
            close(global.ttyfd);
            global.ttyfd_open = 0;
        }
    }

    sigaction(SIGWINCH, &(struct sigaction){.sa_handler = SIG_DFL}, NULL);
    if (global.resize_pipefd[0] >= 0)
        close(global.resize_pipefd[0]);
    if (global.resize_pipefd[1] >= 0)
        close(global.resize_pipefd[1]);

    cellbuf_free(&global.back);
    cellbuf_free(&global.front);
    bytebuf_free(&global.in);
    bytebuf_free(&global.out);

    if (global.terminfo)
        tb_free(global.terminfo);

    cap_trie_deinit(&global.cap_trie);

    tb_reset();
    return TB_OK;
}

static int load_terminfo(void) {
    int rv;
    char tmp[PATH_MAX];

    // See terminfo(5) "Fetching Compiled Descriptions" for a description of
    // this behavior. Some of these paths are compile-time ncurses options, so
    // best guesses are used here.
    const char *term = getenv("TERM");
    if (!term) {
        return TB_ERR;
    }

    // If TERMINFO is set, try that directory and stop
    const char *terminfo = getenv("TERMINFO");
    if (terminfo) {
        return load_terminfo_from_path(terminfo, term);
    }

    // Next try ~/.terminfo
    const char *home = getenv("HOME");
    if (home) {
        snprintf_or_return(rv, tmp, sizeof(tmp), "%s/.terminfo", home);
        if_ok_return(rv, load_terminfo_from_path(tmp, term));
    }

    // Next try TERMINFO_DIRS
    //
    // Note, empty entries are supposed to be interpretted as the "compiled-in
    // default", which is of course system-dependent. Previously /etc/terminfo
    // was used here. Let's skip empty entries altogether rather than give
    // precedence to a guess, and check common paths after this loop.
    const char *dirs = getenv("TERMINFO_DIRS");
    if (dirs) {
        snprintf_or_return(rv, tmp, sizeof(tmp), "%s", dirs);
        char *dir = strtok(tmp, ":");
        while (dir) {
            const char *cdir = dir;
            if (*cdir != '\0') {
                if_ok_return(rv, load_terminfo_from_path(cdir, term));
            }
            dir = strtok(NULL, ":");
        }
    }

#ifdef TB_TERMINFO_DIR
    if_ok_return(rv, load_terminfo_from_path(TB_TERMINFO_DIR, term));
#endif
    if_ok_return(rv, load_terminfo_from_path("/usr/local/etc/terminfo", term));
    if_ok_return(rv,
        load_terminfo_from_path("/usr/local/share/terminfo", term));
    if_ok_return(rv, load_terminfo_from_path("/usr/local/lib/terminfo", term));
    if_ok_return(rv, load_terminfo_from_path("/etc/terminfo", term));
    if_ok_return(rv, load_terminfo_from_path("/usr/share/terminfo", term));
    if_ok_return(rv, load_terminfo_from_path("/usr/lib/terminfo", term));
    if_ok_return(rv, load_terminfo_from_path("/usr/share/lib/terminfo", term));
    if_ok_return(rv, load_terminfo_from_path("/lib/terminfo", term));

    return TB_ERR;
}

static int load_terminfo_from_path(const char *path, const char *term) {
    int rv;
    char tmp[PATH_MAX];

    // Look for term at this terminfo location, e.g., <terminfo>/x/xterm
    snprintf_or_return(rv, tmp, sizeof(tmp), "%s/%c/%s", path, term[0], term);
    if_ok_return(rv, read_terminfo_path(tmp));

#ifdef __APPLE__
    // Try the Darwin equivalent path, e.g., <terminfo>/78/xterm
    snprintf_or_return(rv, tmp, sizeof(tmp), "%s/%x/%s", path, term[0], term);
    return read_terminfo_path(tmp);
#endif

    return TB_ERR;
}

static int read_terminfo_path(const char *path) {
    FILE *fp = fopen(path, "rb");
    if (!fp) {
        return TB_ERR;
    }

    struct stat st;
    if (fstat(fileno(fp), &st) != 0) {
        fclose(fp);
        return TB_ERR;
    }

    size_t fsize = st.st_size;
    char *data = tb_malloc(fsize);
    if (!data) {
        fclose(fp);
        return TB_ERR;
    }

    if (fread(data, 1, fsize, fp) != fsize) {
        fclose(fp);
        tb_free(data);
        return TB_ERR;
    }

    global.terminfo = data;
    global.nterminfo = fsize;

    fclose(fp);
    return TB_OK;
}

static int parse_terminfo_caps(void) {
    // See term(5) "LEGACY STORAGE FORMAT" and "EXTENDED STORAGE FORMAT" for a
    // description of this behavior.

    // Ensure there's at least a header's worth of data
    if (global.nterminfo < 6) {
        return TB_ERR;
    }

    int16_t *header = (int16_t *)global.terminfo;
    // header[0] the magic number (octal 0432 or 01036)
    // header[1] the size, in bytes, of the names section
    // header[2] the number of bytes in the boolean section
    // header[3] the number of short integers in the numbers section
    // header[4] the number of offsets (short integers) in the strings section
    // header[5] the size, in bytes, of the string table

    // Legacy ints are 16-bit, extended ints are 32-bit
    const int bytes_per_int = header[0] == 01036 ? 4  // 32-bit
                                                 : 2; // 16-bit

    // > Between the boolean section and the number section, a null byte will be
    // > inserted, if necessary, to ensure that the number section begins on an
    // > even byte
    const int align_offset = (header[1] + header[2]) % 2 != 0 ? 1 : 0;

    const int pos_str_offsets =
        (6 * sizeof(int16_t)) // header (12 bytes)
        + header[1]           // length of names section
        + header[2]           // length of boolean section
        + align_offset +
        (header[3] * bytes_per_int); // length of numbers section

    const int pos_str_table =
        pos_str_offsets +
        (header[4] * sizeof(int16_t)); // length of string offsets table

    // Load caps
    int i;
    for (i = 0; i < TB_CAP__COUNT; i++) {
        const char *cap = get_terminfo_string(pos_str_offsets, pos_str_table,
            header[5], terminfo_cap_indexes[i]);
        if (!cap) {
            // Something is not right
            return TB_ERR;
        }
        global.caps[i] = cap;
    }

    return TB_OK;
}

static int load_builtin_caps(void) {
    int i, j;
    const char *term = getenv("TERM");

    if (!term) {
        return TB_ERR_NO_TERM;
    }

    // Check for exact TERM match
    for (i = 0; builtin_terms[i].name != NULL; i++) {
        if (strcmp(term, builtin_terms[i].name) == 0) {
            for (j = 0; j < TB_CAP__COUNT; j++) {
                global.caps[j] = builtin_terms[i].caps[j];
            }
            return TB_OK;
        }
    }

    // Check for partial TERM or alias match
    for (i = 0; builtin_terms[i].name != NULL; i++) {
        if (strstr(term, builtin_terms[i].name) != NULL ||
            (*(builtin_terms[i].alias) != '\0' &&
                strstr(term, builtin_terms[i].alias) != NULL))
        {
            for (j = 0; j < TB_CAP__COUNT; j++) {
                global.caps[j] = builtin_terms[i].caps[j];
            }
            return TB_OK;
        }
    }

    return TB_ERR_UNSUPPORTED_TERM;
}

static const char *get_terminfo_string(int16_t str_offsets_pos,
    int16_t str_table_pos, int16_t str_table_len, int16_t str_index) {
    const int16_t *str_offset =
        (int16_t *)(global.terminfo + (int)str_offsets_pos +
                    ((int)str_index * (int)sizeof(int16_t)));
    if (*str_offset >= str_table_len) {
        // Invalid string offset
        return NULL;
    }
    if (((size_t)((int)str_table_pos + (int)*str_offset)) >= global.nterminfo) {
        // Truncated/corrupt terminfo?
        return NULL;
    }
    return (
        const char *)(global.terminfo + (int)str_table_pos + (int)*str_offset);
}

static int wait_event(struct tb_event *event, int timeout) {
    int rv;
    char buf[TB_OPT_READ_BUF];

    memset(event, 0, sizeof(*event));
    if_ok_return(rv, extract_event(event));

    fd_set fds;
    struct timeval tv;
    tv.tv_sec = timeout / 1000;
    tv.tv_usec = (timeout - (tv.tv_sec * 1000)) * 1000;

    do {
        FD_ZERO(&fds);
        FD_SET(global.rfd, &fds);
        FD_SET(global.resize_pipefd[0], &fds);

        int maxfd = global.resize_pipefd[0] > global.rfd
                        ? global.resize_pipefd[0]
                        : global.rfd;

        int select_rv =
            select(maxfd + 1, &fds, NULL, NULL, (timeout < 0) ? NULL : &tv);

        if (select_rv < 0) {
            // Let EINTR/EAGAIN bubble up
            global.last_errno = errno;
            return TB_ERR_POLL;
        } else if (select_rv == 0) {
            return TB_ERR_NO_EVENT;
        }

        int tty_has_events = (FD_ISSET(global.rfd, &fds));
        int resize_has_events = (FD_ISSET(global.resize_pipefd[0], &fds));

        if (tty_has_events) {
            ssize_t read_rv = read(global.rfd, buf, sizeof(buf));
            if (read_rv < 0) {
                global.last_errno = errno;
                return TB_ERR_READ;
            } else if (read_rv > 0) {
                bytebuf_nputs(&global.in, buf, read_rv);
            }
        }

        if (resize_has_events) {
            int ignore = 0;
            read(global.resize_pipefd[0], &ignore, sizeof(ignore));
            // TODO Harden against errors encountered mid-resize
            if_err_return(rv, update_term_size());
            if_err_return(rv, resize_cellbufs());
            event->type = TB_EVENT_RESIZE;
            event->w = global.width;
            event->h = global.height;
            return TB_OK;
        }

        memset(event, 0, sizeof(*event));
        if_ok_return(rv, extract_event(event));
    } while (timeout == -1);

    return rv;
}

static int extract_event(struct tb_event *event) {
    int rv;
    struct bytebuf_t *in = &global.in;

    if (in->len == 0) {
        return TB_ERR;
    }

    if (in->buf[0] == '\x1b') {
        // Escape sequence?
        // In TB_INPUT_ESC, skip if the buffer is a single escape char
        if (!((global.input_mode & TB_INPUT_ESC) && in->len == 1)) {
            if_ok_or_need_more_return(rv, extract_esc(event));
        }

        // Escape key?
        if (global.input_mode & TB_INPUT_ESC) {
            event->type = TB_EVENT_KEY;
            event->ch = 0;
            event->key = TB_KEY_ESC;
            event->mod = 0;
            bytebuf_shift(in, 1);
            return TB_OK;
        }

        // Recurse for alt key
        event->mod |= TB_MOD_ALT;
        bytebuf_shift(in, 1);
        return extract_event(event);
    }

    // ASCII control key?
    if ((uint16_t)in->buf[0] < TB_KEY_SPACE || in->buf[0] == TB_KEY_BACKSPACE2)
    {
        event->type = TB_EVENT_KEY;
        event->ch = 0;
        event->key = (uint16_t)in->buf[0];
        event->mod |= TB_MOD_CTRL;
        bytebuf_shift(in, 1);
        return TB_OK;
    }

    // UTF-8?
    if (in->len >= (size_t)tb_utf8_char_length(in->buf[0])) {
        event->type = TB_EVENT_KEY;
        tb_utf8_char_to_unicode(&event->ch, in->buf);
        event->key = 0;
        bytebuf_shift(in, tb_utf8_char_length(in->buf[0]));
        return TB_OK;
    }

    // Need more input
    return TB_ERR;
}

static int extract_esc(struct tb_event *event) {
    int rv;
    if_ok_or_need_more_return(rv, extract_esc_user(event, 0));
    if_ok_or_need_more_return(rv, extract_esc_cap(event));
    if_ok_or_need_more_return(rv, extract_esc_mouse(event));
    if_ok_or_need_more_return(rv, extract_esc_user(event, 1));
    return TB_ERR;
}

static int extract_esc_user(struct tb_event *event, int is_post) {
    int rv;
    size_t consumed = 0;
    struct bytebuf_t *in = &global.in;
    int (*fn)(struct tb_event *, size_t *);

    fn = is_post ? global.fn_extract_esc_post : global.fn_extract_esc_pre;

    if (!fn) {
        return TB_ERR;
    }

    rv = fn(event, &consumed);
    if (rv == TB_OK) {
        bytebuf_shift(in, consumed);
    }

    if_ok_or_need_more_return(rv, rv);
    return TB_ERR;
}

static int extract_esc_cap(struct tb_event *event) {
    int rv;
    struct bytebuf_t *in = &global.in;
    struct cap_trie_t *node;
    size_t depth;

    if_err_return(rv, cap_trie_find(in->buf, in->len, &node, &depth));
    if (node->is_leaf) {
        // Found a leaf node
        event->type = TB_EVENT_KEY;
        event->ch = 0;
        event->key = node->key;
        event->mod = node->mod;
        bytebuf_shift(in, depth);
        return TB_OK;
    } else if (node->nchildren > 0 && in->len <= depth) {
        // Found a branch node (not enough input)
        return TB_ERR_NEED_MORE;
    }

    return TB_ERR;
}

static int extract_esc_mouse(struct tb_event *event) {
    struct bytebuf_t *in = &global.in;

    enum type { TYPE_VT200 = 0, TYPE_1006, TYPE_1015, TYPE_MAX };

    char *cmp[TYPE_MAX] = {//
        // X10 mouse encoding, the simplest one
        // \x1b [ M Cb Cx Cy
        [TYPE_VT200] = "\x1b[M",
        // xterm 1006 extended mode or urxvt 1015 extended mode
        // xterm: \x1b [ < Cb ; Cx ; Cy (M or m)
        [TYPE_1006] = "\x1b[<",
        // urxvt: \x1b [ Cb ; Cx ; Cy M
        [TYPE_1015] = "\x1b["};

    enum type type = 0;
    int ret = TB_ERR;

    // Unrolled at compile-time (probably)
    for (; type < TYPE_MAX; type++) {
        size_t size = strlen(cmp[type]);

        if (in->len >= size && (strncmp(cmp[type], in->buf, size)) == 0) {
            break;
        }
    }

    if (type == TYPE_MAX) {
        ret = TB_ERR; // No match
        return ret;
    }

    size_t buf_shift = 0;

    switch (type) {
        case TYPE_VT200:
            if (in->len >= 6) {
                int b = in->buf[3] - 0x20;
                int fail = 0;

                switch (b & 3) {
                    case 0:
                        event->key = ((b & 64) != 0) ? TB_KEY_MOUSE_WHEEL_UP
                                                     : TB_KEY_MOUSE_LEFT;
                        break;
                    case 1:
                        event->key = ((b & 64) != 0) ? TB_KEY_MOUSE_WHEEL_DOWN
                                                     : TB_KEY_MOUSE_MIDDLE;
                        break;
                    case 2:
                        event->key = TB_KEY_MOUSE_RIGHT;
                        break;
                    case 3:
                        event->key = TB_KEY_MOUSE_RELEASE;
                        break;
                    default:
                        ret = TB_ERR;
                        fail = 1;
                        break;
                }

                if (!fail) {
                    if ((b & 32) != 0) {
                        event->mod |= TB_MOD_MOTION;
                    }

                    // the coord is 1,1 for upper left
                    event->x = ((uint8_t)in->buf[4]) - 0x21;
                    event->y = ((uint8_t)in->buf[5]) - 0x21;

                    ret = TB_OK;
                }

                buf_shift = 6;
            }
            break;
        case TYPE_1006:
            // fallthrough
        case TYPE_1015: {
            size_t index_fail = (size_t)-1;

            enum {
                FIRST_M = 0,
                FIRST_SEMICOLON,
                LAST_SEMICOLON,
                FIRST_LAST_MAX
            };

            size_t indices[FIRST_LAST_MAX] = {index_fail, index_fail,
                index_fail};
            int m_is_capital = 0;

            for (size_t i = 0; i < in->len; i++) {
                if (in->buf[i] == ';') {
                    if (indices[FIRST_SEMICOLON] == index_fail) {
                        indices[FIRST_SEMICOLON] = i;
                    } else {
                        indices[LAST_SEMICOLON] = i;
                    }
                } else if (indices[FIRST_M] == index_fail) {
                    if (in->buf[i] == 'm' || in->buf[i] == 'M') {
                        m_is_capital = (in->buf[i] == 'M');
                        indices[FIRST_M] = i;
                    }
                }
            }

            if (indices[FIRST_M] == index_fail ||
                indices[FIRST_SEMICOLON] == index_fail ||
                indices[LAST_SEMICOLON] == index_fail)
            {
                ret = TB_ERR;
            } else {
                int start = (type == TYPE_1015 ? 2 : 3);

                unsigned n1 = strtoul(&in->buf[start], NULL, 10);
                unsigned n2 =
                    strtoul(&in->buf[indices[FIRST_SEMICOLON] + 1], NULL, 10);
                unsigned n3 =
                    strtoul(&in->buf[indices[LAST_SEMICOLON] + 1], NULL, 10);

                if (type == TYPE_1015) {
                    n1 -= 0x20;
                }

                int fail = 0;

                switch (n1 & 3) {
                    case 0:
                        event->key = ((n1 & 64) != 0) ? TB_KEY_MOUSE_WHEEL_UP
                                                      : TB_KEY_MOUSE_LEFT;
                        break;
                    case 1:
                        event->key = ((n1 & 64) != 0) ? TB_KEY_MOUSE_WHEEL_DOWN
                                                      : TB_KEY_MOUSE_MIDDLE;
                        break;
                    case 2:
                        event->key = TB_KEY_MOUSE_RIGHT;
                        break;
                    case 3:
                        event->key = TB_KEY_MOUSE_RELEASE;
                        break;
                    default:
                        ret = TB_ERR;
                        fail = 1;
                        break;
                }

                buf_shift = in->len;

                if (!fail) {
                    if (!m_is_capital) {
                        // on xterm mouse release is signaled by lowercase m
                        event->key = TB_KEY_MOUSE_RELEASE;
                    }

                    if ((n1 & 32) != 0) {
                        event->mod |= TB_MOD_MOTION;
                    }

                    event->x = ((uint8_t)n2) - 1;
                    event->y = ((uint8_t)n3) - 1;

                    ret = TB_OK;
                }
            }
        } break;
        case TYPE_MAX:
            ret = TB_ERR;
    }

    if (buf_shift > 0) {
        bytebuf_shift(in, buf_shift);
    }

    if (ret == TB_OK) {
        event->type = TB_EVENT_MOUSE;
    }

    return ret;
}

static int resize_cellbufs(void) {
    int rv;
    if_err_return(rv,
        cellbuf_resize(&global.back, global.width, global.height));
    if_err_return(rv,
        cellbuf_resize(&global.front, global.width, global.height));
    if_err_return(rv, cellbuf_clear(&global.front));
    if_err_return(rv, send_clear());
    return TB_OK;
}

static void handle_resize(int sig) {
    int errno_copy = errno;
    write(global.resize_pipefd[1], &sig, sizeof(sig));
    errno = errno_copy;
}

static int send_attr(uintattr_t fg, uintattr_t bg) {
    int rv;

    if (fg == global.last_fg && bg == global.last_bg) {
        return TB_OK;
    }

    if_err_return(rv, bytebuf_puts(&global.out, global.caps[TB_CAP_SGR0]));

    uintattr_t cfg, cbg;
    switch (global.output_mode) {
        default:
        case TB_OUTPUT_NORMAL:
            cfg = fg & 0x0f;
            cbg = bg & 0x0f;
            break;

        case TB_OUTPUT_256:
            cfg = fg & 0xff;
            cbg = bg & 0xff;
            break;

        case TB_OUTPUT_216:
            cfg = fg & 0xff;
            cbg = bg & 0xff;
            if (cfg > 216)
                cfg = 216;
            if (cbg > 216)
                cbg = 216;
            cfg += 0x0f;
            cbg += 0x0f;
            break;

        case TB_OUTPUT_GRAYSCALE:
            cfg = fg & 0xff;
            cbg = bg & 0xff;
            if (cfg > 24)
                cfg = 24;
            if (cbg > 24)
                cbg = 24;
            cfg += 0xe7;
            cbg += 0xe7;
            break;

#ifdef TB_OPT_TRUECOLOR
        case TB_OUTPUT_TRUECOLOR:
            cfg = fg & 0xffffff;
            cbg = bg & 0xffffff;
            break;
#endif
    }

    uintattr_t attr_bold, attr_blink, attr_italic, attr_underline, attr_reverse,
        attr_default;
#ifdef TB_OPT_TRUECOLOR
    if (global.output_mode == TB_OUTPUT_TRUECOLOR) {
        attr_bold = TB_TRUECOLOR_BOLD;
        attr_blink = TB_TRUECOLOR_BLINK;
        attr_italic = TB_TRUECOLOR_ITALIC;
        attr_underline = TB_TRUECOLOR_UNDERLINE;
        attr_reverse = TB_TRUECOLOR_REVERSE;
        attr_default = TB_TRUECOLOR_DEFAULT;
    } else
#endif
    {
        attr_bold = TB_BOLD;
        attr_blink = TB_BLINK;
        attr_italic = TB_ITALIC;
        attr_underline = TB_UNDERLINE;
        attr_reverse = TB_REVERSE;
        attr_default = TB_DEFAULT;
    }

    /* For convenience (and some back compat), interpret 0 as default in some
     * modes */
    if (global.output_mode == TB_OUTPUT_NORMAL ||
        global.output_mode == TB_OUTPUT_216 ||
        global.output_mode == TB_OUTPUT_GRAYSCALE)
    {
        if ((fg & 0xff) == 0)
            fg |= attr_default;
        if ((bg & 0xff) == 0)
            bg |= attr_default;
    }

    if (fg & attr_bold)
        if_err_return(rv, bytebuf_puts(&global.out, global.caps[TB_CAP_BOLD]));

    if (fg & attr_blink)
        if_err_return(rv, bytebuf_puts(&global.out, global.caps[TB_CAP_BLINK]));

    if (fg & attr_underline)
        if_err_return(rv,
            bytebuf_puts(&global.out, global.caps[TB_CAP_UNDERLINE]));

    if (fg & attr_italic)
        if_err_return(rv,
            bytebuf_puts(&global.out, global.caps[TB_CAP_ITALIC]));

    if ((fg & attr_reverse) || (bg & attr_reverse))
        if_err_return(rv,
            bytebuf_puts(&global.out, global.caps[TB_CAP_REVERSE]));

    if_err_return(rv, send_sgr(cfg, cbg, fg & attr_default, bg & attr_default));

    global.last_fg = fg;
    global.last_bg = bg;

    return TB_OK;
}

static int send_sgr(uintattr_t cfg, uintattr_t cbg, uintattr_t fg_is_default,
    uintattr_t bg_is_default) {
    int rv;
    char nbuf[32];

    if (fg_is_default && bg_is_default) {
        return TB_OK;
    }

    switch (global.output_mode) {
        default:
        case TB_OUTPUT_NORMAL:
            send_literal(rv, "\x1b[");
            if (!fg_is_default) {
                send_literal(rv, "3");
                send_num(rv, nbuf, cfg - 1);
                if (!bg_is_default) {
                    send_literal(rv, ";");
                }
            }
            if (!bg_is_default) {
                send_literal(rv, "4");
                send_num(rv, nbuf, cbg - 1);
            }
            send_literal(rv, "m");
            break;

        case TB_OUTPUT_256:
        case TB_OUTPUT_216:
        case TB_OUTPUT_GRAYSCALE:
            send_literal(rv, "\x1b[");
            if (!fg_is_default) {
                send_literal(rv, "38;5;");
                send_num(rv, nbuf, cfg);
                if (!bg_is_default) {
                    send_literal(rv, ";");
                }
            }
            if (!bg_is_default) {
                send_literal(rv, "48;5;");
                send_num(rv, nbuf, cbg);
            }
            send_literal(rv, "m");
            break;

#ifdef TB_OPT_TRUECOLOR
        case TB_OUTPUT_TRUECOLOR:
            send_literal(rv, "\x1b[");
            if (!fg_is_default) {
                send_literal(rv, "38;2;");
                send_num(rv, nbuf, (cfg >> 16) & 0xff);
                send_literal(rv, ";");
                send_num(rv, nbuf, (cfg >> 8) & 0xff);
                send_literal(rv, ";");
                send_num(rv, nbuf, cfg & 0xff);
                if (!bg_is_default) {
                    send_literal(rv, ";");
                }
            }
            if (!bg_is_default) {
                send_literal(rv, "48;2;");
                send_num(rv, nbuf, (cbg >> 16) & 0xff);
                send_literal(rv, ";");
                send_num(rv, nbuf, (cbg >> 8) & 0xff);
                send_literal(rv, ";");
                send_num(rv, nbuf, cbg & 0xff);
            }
            send_literal(rv, "m");
            break;
#endif
    }
    return TB_OK;
}

static int send_cursor_if(int x, int y) {
    int rv;
    char nbuf[32];
    if (x < 0 || y < 0) {
        return TB_OK;
    }
    send_literal(rv, "\x1b[");
    send_num(rv, nbuf, y + 1);
    send_literal(rv, ";");
    send_num(rv, nbuf, x + 1);
    send_literal(rv, "H");
    return TB_OK;
}

static int send_char(int x, int y, uint32_t ch) {
    return send_cluster(x, y, &ch, 1);
}

static int send_cluster(int x, int y, uint32_t *ch, size_t nch) {
    int rv;
    char abuf[8];

    if (global.last_x != x - 1 || global.last_y != y) {
        if_err_return(rv, send_cursor_if(x, y));
    }
    global.last_x = x;
    global.last_y = y;

    int i;
    for (i = 0; i < (int)nch; i++) {
        uint32_t ach = *(ch + i);
        int aw = tb_utf8_unicode_to_char(abuf, ach);
        if (!ach) {
            abuf[0] = ' ';
        }
        if_err_return(rv, bytebuf_nputs(&global.out, abuf, (size_t)aw));
    }

    return TB_OK;
}

static int convert_num(uint32_t num, char *buf) {
    int i, l = 0;
    char ch;
    do {
        /* '0' = 48; 48 + num%10 < 58 < MAX_8bitCHAR */
        buf[l++] = (char)('0' + (num % 10));
        num /= 10;
    } while (num);
    for (i = 0; i < l / 2; i++) {
        ch = buf[i];
        buf[i] = buf[l - 1 - i];
        buf[l - 1 - i] = ch;
    }
    return l;
}

static int cell_cmp(struct tb_cell *a, struct tb_cell *b) {
    if (a->ch != b->ch || a->fg != b->fg || a->bg != b->bg) {
        return 1;
    }
#ifdef TB_OPT_EGC
    if (a->nech != b->nech) {
        return 1;
    } else if (a->nech > 0) { // a->nech == b->nech
        return memcmp(a->ech, b->ech, a->nech);
    }
#endif
    return 0;
}

static int cell_copy(struct tb_cell *dst, struct tb_cell *src) {
#ifdef TB_OPT_EGC
    if (src->nech > 0) {
        return cell_set(dst, src->ech, src->nech, src->fg, src->bg);
    }
#endif
    return cell_set(dst, &src->ch, 1, src->fg, src->bg);
}

static int cell_set(struct tb_cell *cell, uint32_t *ch, size_t nch,
    uintattr_t fg, uintattr_t bg) {
    cell->ch = ch ? *ch : 0;
    cell->fg = fg;
    cell->bg = bg;
#ifdef TB_OPT_EGC
    if (nch <= 1) {
        cell->nech = 0;
    } else {
        int rv;
        if_err_return(rv, cell_reserve_ech(cell, nch + 1));
        memcpy(cell->ech, ch, nch);
        cell->ech[nch] = '\0';
        cell->nech = nch;
    }
#else
    (void)nch;
    (void)cell_reserve_ech;
#endif
    return TB_OK;
}

static int cell_reserve_ech(struct tb_cell *cell, size_t n) {
#ifdef TB_OPT_EGC
    if (cell->cech >= n) {
        return TB_OK;
    }
    if (!(cell->ech = tb_realloc(cell->ech, n * sizeof(cell->ch)))) {
        return TB_ERR_MEM;
    }
    cell->cech = n;
    return TB_OK;
#else
    (void)cell;
    (void)n;
    return TB_ERR;
#endif
}

static int cell_free(struct tb_cell *cell) {
#ifdef TB_OPT_EGC
    if (cell->ech) {
        tb_free(cell->ech);
    }
#endif
    memset(cell, 0, sizeof(*cell));
    return TB_OK;
}

static int cellbuf_init(struct cellbuf_t *c, int w, int h) {
    c->cells = tb_malloc(sizeof(struct tb_cell) * w * h);
    if (!c->cells) {
        return TB_ERR_MEM;
    }
    memset(c->cells, 0, sizeof(struct tb_cell) * w * h);
    c->width = w;
    c->height = h;
    return TB_OK;
}

static int cellbuf_free(struct cellbuf_t *c) {
    if (c->cells) {
        int i;
        for (i = 0; i < c->width * c->height; i++) {
            cell_free(&c->cells[i]);
        }
        tb_free(c->cells);
    }
    memset(c, 0, sizeof(*c));
    return TB_OK;
}

static int cellbuf_clear(struct cellbuf_t *c) {
    int rv, i;
    uint32_t space = (uint32_t)' ';
    for (i = 0; i < c->width * c->height; i++) {
        if_err_return(rv,
            cell_set(&c->cells[i], &space, 1, global.fg, global.bg));
    }
    return TB_OK;
}

static int cellbuf_get(struct cellbuf_t *c, int x, int y,
    struct tb_cell **out) {
    if (x < 0 || x >= c->width || y < 0 || y >= c->height) {
        *out = NULL;
        return TB_ERR_OUT_OF_BOUNDS;
    }
    *out = &c->cells[(y * c->width) + x];
    return TB_OK;
}

static int cellbuf_resize(struct cellbuf_t *c, int w, int h) {
    int rv;

    int ow = c->width;
    int oh = c->height;

    if (ow == w && oh == h) {
        return TB_OK;
    }

    w = w < 1 ? 1 : w;
    h = h < 1 ? 1 : h;

    int minw = (w < ow) ? w : ow;
    int minh = (h < oh) ? h : oh;

    struct tb_cell *prev = c->cells;

    if_err_return(rv, cellbuf_init(c, w, h));
    if_err_return(rv, cellbuf_clear(c));

    int x, y;
    for (x = 0; x < minw; x++) {
        for (y = 0; y < minh; y++) {
            struct tb_cell *src, *dst;
            src = &prev[(y * ow) + x];
            if_err_return(rv, cellbuf_get(c, x, y, &dst));
            if_err_return(rv, cell_copy(dst, src));
        }
    }

    tb_free(prev);

    return TB_OK;
}

static int bytebuf_puts(struct bytebuf_t *b, const char *str) {
    return bytebuf_nputs(b, str, (size_t)strlen(str));
}

static int bytebuf_nputs(struct bytebuf_t *b, const char *str, size_t nstr) {
    int rv;
    if_err_return(rv, bytebuf_reserve(b, b->len + nstr + 1));
    memcpy(b->buf + b->len, str, nstr);
    b->len += nstr;
    b->buf[b->len] = '\0';
    return TB_OK;
}

static int bytebuf_shift(struct bytebuf_t *b, size_t n) {
    if (n > b->len) {
        n = b->len;
    }
    size_t nmove = b->len - n;
    memmove(b->buf, b->buf + n, nmove);
    b->len -= n;
    return TB_OK;
}

static int bytebuf_flush(struct bytebuf_t *b, int fd) {
    if (b->len <= 0) {
        return TB_OK;
    }
    ssize_t write_rv = write(fd, b->buf, b->len);
    if (write_rv < 0 || (size_t)write_rv != b->len) {
        // Note, errno will be 0 on partial write
        global.last_errno = errno;
        return TB_ERR;
    }
    b->len = 0;
    return TB_OK;
}

static int bytebuf_reserve(struct bytebuf_t *b, size_t sz) {
    if (b->cap >= sz) {
        return TB_OK;
    }
    size_t newcap = b->cap > 0 ? b->cap : 1;
    while (newcap < sz) {
        newcap *= 2;
    }
    char *newbuf;
    if (b->buf) {
        newbuf = tb_realloc(b->buf, newcap);
    } else {
        newbuf = tb_malloc(newcap);
    }
    if (!newbuf) {
        return TB_ERR_MEM;
    }
    b->buf = newbuf;
    b->cap = newcap;
    return TB_OK;
}

static int bytebuf_free(struct bytebuf_t *b) {
    if (b->buf) {
        tb_free(b->buf);
    }
    memset(b, 0, sizeof(*b));
    return TB_OK;
}
