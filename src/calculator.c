#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ncurses.h>
#include <locale.h>

#include "formula.h"

#define MODE_EVAL 0
#define MODE_DEF 1

#define GREEN_PAIR 1
#define BLUE_PAIR 2

#define MAXVARS 10

calc_variable *vars[MAXVARS];
int var_index = 0;

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
    init_pair(BLUE_PAIR, COLOR_WHITE, COLOR_BLUE);

    for (;;) {
        getmaxyx(stdscr, row, col);

        /* Print variables */
        for (int i = 0; i < var_index; i++) {
            snprintf(out, MAXEXPRESSIONSIZE * 2, "%s=%s", vars[i]->name, vars[i]->value);
            mvaddstr(i, 0, out);
        }

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

            /* Check for special commands */
            /* Exit the program */
            if (strcmp(in, ":x") == 0) {
                endwin();
                exit(0);
            }
            /* Switch to definition mode */
            else if (strcmp(in, ":d") == 0) {
                state = MODE_DEF;
            }
            else {
                /* Parse the expression into infix form and evaluate it */
                infix_parse(in, out, MAXEXPRESSIONSIZE * 2);
                snprintf(out, MAXEXPRESSIONSIZE * 2, "%s = %f\n", in, calculate(out, MAXEXPRESSIONSIZE * 2));

                mvaddstr(row - 3, 0, out);
            }
            move(row - 1, 0);
            clrtoeol();
        }
        
        /* Definition mode */
        else if (state == MODE_DEF) {
            /* Status Bar */
            attron(COLOR_PAIR(BLUE_PAIR));
            mvaddstr(row - 2, 0, "DEFINITION MODE");
            for (i = strlen("DEFINITION MODE"); i < col; i++) {
                addstr(" ");
            }
            attroff(COLOR_PAIR(BLUE_PAIR));

            /* Get input */
            mvgetnstr(row - 1, 0, in, MAXEXPRESSIONSIZE);

            /* Check for special commands */
            /* Exit the program */
            if (strcmp(in, ":x") == 0) {
                endwin();
                exit(0);
            }
            /* Switch to evaluation mode */
            else if (strcmp(in, ":e") == 0) {
                state = MODE_EVAL;
            }
            /* Clear all variables */
            else if (strcmp(in, ":c") == 0) {
                var_index = 0; /* Array is statically allocated so we don't need to free the data, just reset the stack pointer */
                move(0, 0);
                clrtobot();
            }
            else {
                /* Define variable */
                calc_variable *var = parse_var(in, strlen(in));
                if (var == NULL) {
                    /* TODO: print an error message */
                }
                else {
                    vars[var_index] = var;
                    var_index++;
                }
            }
            move(row - 1, 0);
            clrtoeol();
        }

        refresh();
    }

    endwin();
}
