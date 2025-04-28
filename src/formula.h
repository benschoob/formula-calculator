#ifndef _FORMULA_H_
#define _FORMULA_H_

#define MAXNAMELEN 5

typedef struct {
    char name[MAXNAMELEN];
    double value;
} calc_variable;

int infix_parse(char *in, char *out, int len);
double calculate(char *f, int f_len);

calc_variable define_var(char *name, double value);

#endif
