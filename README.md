# Formula Calculator

A calculator that can evaluate complex expressions using the shunting yard algorithm.

## Dependencies
This project depends on the `ncurses` library for text formatting. (https://linux.die.net/man/3/ncurses)

## Usage
### Evaluation Mode
- Type an equation into the prompt. Press 'return' to evaluate it.
- Type `:x` to exit the program.
- Type `:d` to switch to definition mode.

### Definition Mode
- You can define variables that can be used in equations in this mode. Variable definitions are written in the form `<name> = <expression>`. When used in execution mode the variable's name will be substituted for its associated expression.
- Type `:x` to exit the program.
- Type `:e` to return to evaluation mode.
