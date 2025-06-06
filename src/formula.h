#ifndef _FORMULA_H_
#define _FORMULA_H_

#define MAXNAMELEN 5
#define MAXEXPRESSIONSIZE 256

typedef struct {
    char name[MAXNAMELEN];
    int name_len;
    char value[MAXEXPRESSIONSIZE];
    int value_len;
} calc_variable;

int infix_parse(char *in, char *out, int len);
double calculate(char *f, int f_len);

calc_variable *parse_var(char *in, int len);

#endif
