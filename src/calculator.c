#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ncurses.h>
#include <locale.h>

#include "formula.h"

#define MAXEXPRESSIONSIZE 256

#define MODE_EVAL 0
#define MODE_DEF 1

#define GREEN_PAIR 1

int main(void) {
    char in[MAXEXPRESSIONSIZE];
    char out[MAXEXPRESSIONSIZE * 2];

    int row = 0;
    int col = 0;

    int i;

    int state = MODE_EVAL;

    /* Initialize ncurses */
    setlocale(LC_ALL, "");
    initscr();

    if (has_colors() == FALSE) {
        endwin();
        printf("Your terminal does not support color\n");
        exit(1);
    }
    start_color();

    /* Initialize colors */
    init_pair(GREEN_PAIR, COLOR_WHITE, COLOR_GREEN);

    for (;;) {
        getmaxyx(stdscr, row, col);

        /* Evaluation mode */
        if (state == MODE_EVAL) {
            /* Status Bar */
            attron(COLOR_PAIR(GREEN_PAIR));
            mvaddstr(row - 2, 0, "EVALUATION MODE");
            for (i = strlen("EVALUATION MODE"); i < col; i++) {
                addstr(" ");
            }
            attroff(COLOR_PAIR(GREEN_PAIR));

            /* Get input */
            mvgetnstr(row - 1, 0, in, MAXEXPRESSIONSIZE);
            infix_parse(in, out, MAXEXPRESSIONSIZE * 2);
            snprintf(out, MAXEXPRESSIONSIZE * 2, "%f\n", calculate(out, MAXEXPRESSIONSIZE * 2));

            mvaddstr(row - 3, 0, out);

            move(row - 1, 0);
            clrtoeol();
        }
        
        /* Definition mode */
        else if (state == MODE_DEF) {
            
        }

        refresh();
    }

    endwin();

}
