#include "grimoire/grimoire.h"
#include <stdio.h>
#include <unistd.h>
#include "debug.h"

void listener1(GM_Term term, GM_TERM_SIZE ts) {
    IGNORE_ARG(term);
    printf("TERMINAL UPDATE 1 - ROWS: %u | COLS: %u\n", ts.rows, ts.cols);
}

// void listener2(GM_Term term, GM_TERM_SIZE ts) {
//     IGNORE_ARG(term);
//     printf("TERMINAL UPDATE 2 - ROWS: %u | COLS: %u\n", ts.rows, ts.cols);
// }

// void listener3(GM_Term term, GM_TERM_SIZE ts) {
//     IGNORE_ARG(term);
//     printf("TERMINAL UPDATE 3 - ROWS: %u | COLS: %u\n", ts.rows, ts.cols);
// }

#pragma GCC push_options
#pragma GCC optimize ("O0")
void interactive() {
    // GM_TERM_SIZE ts = gm_get_term_size();

    // printf("ROWS: %u | COLS: %u\n", ts.rows, ts.cols);

    // gm_term_add_tui_resize_listener(listener1);

    // gm_setup_tui_events();

    GM_TERM_SIZE ts = gm_get_tui_size();
    printf("ROWS: %u | COLS: %u\n", ts.rows, ts.cols);

    GM_Term term = gm_term_init();
    gm_term_add_tui_resize_listener(term, listener1);
    // gm_term_add_tui_resize_listener(term, listener2);
    // gm_term_add_tui_resize_listener(term, listener3);

    gm_attron(term, GM_BOLD | GM_ITALIC);

    gm_printf(term, 0, 0, "abc");
    int aaa = NULL;

    gm_attroff(term, GM_ITALIC);

    gm_printf(term, 1, 1, "def");

    gm_refresh(term);

    gm_attroff(term, GM_BOLD);

    while(1) {
        pause();
        // gm_term_remove_tui_resize_listener(term, listener1);
    }
}
#pragma GCC pop_options
