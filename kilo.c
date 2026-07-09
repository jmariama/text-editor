#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

struct termios orig_termios;

void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}
void enableRawMode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode); // from <stdlib.h> calls disableRAWMode() automatially when program exits.

    struct termios raw = orig_termios;

    raw.c_lflag &= ~(ECHO | ICANON); //ICANON flag disabled.
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main(void) {

    enableRawMode();

    char c;
    while (read(STDIN_FILENO, &c, 1) == 1 && c!= 'q') {
        if (iscntrl(c)) { //checks if character is control character
            printf("%d\n", c);
        } else { //prints if character is a printable character
            printf("%d ('%c')\n", c, c);
        }
        }
    return 0;
}

