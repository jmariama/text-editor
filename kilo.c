/* includes */
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

/* defines */
#define CTRL_KEY(k) ((k) & 0x1f)


/* data */
struct termios orig_termios;

/* terminal */

void die(const char *s) {
  //clears screen on exit
  write(STDOUT_FILENO, "\x1b[2J", 4);
  write(STDOUT_FILENO, "\x1b[H", 3);

    perror(s);
    exit(1);
}
void disableRawMode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
    die("tcsetattr");
}


void enableRawMode() {
   if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("tcgetattr");
    atexit(disableRawMode); // from <stdlib.h> calls disableRAWMode() automatially when program exits.

    struct termios raw = orig_termios;

    raw.c_iflag &= ~(BRKINT |ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG); //ICANON flag disabled, ~ISIG disables ctrlc & ctrlc signals
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;
    
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");

    
}
// WAITS FOR ONE KEYPRESS AND RETURNS IT
char editorReadKey() {
  int nread;
  char c;
while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
    if (nread ==-1 && errno != EAGAIN) die("read");

  }
  return c;
}

//draw hearts at beginning of each row
void editorDrawRows() {
  int y;
  for (y = 0; y < 24; y++) { //draws on 24 rows
    write(STDOUT_FILENO, "\xE2\x99\xA5\r\n", 5); //utf-8 encoding
  }
}

/*** output ***/
void editorRefreshScreen() {
  //clears screen
  write(STDOUT_FILENO, "\x1b[2J", 4);
  write(STDOUT_FILENO, "\x1b[H", 3); // repositions cursor
  //draws heart on each row
  editorDrawRows();
  //repositions cursor
  write(STDOUT_FILENO, "\x1b[H", 3);
}




// WAITS FOR KEYPRESS AND PROCESSES IT
  //*inputs*//
  void editorProcessKeypress() {
  char c = editorReadKey();
  switch (c) {
    case CTRL_KEY('q'):
      //clears screen when ctrl+q is hit
      write(STDOUT_FILENO, "\x1b[2J", 4);
      write(STDOUT_FILENO, "\x1b[H", 3);
      exit(0);
      break;
  }
}





/* initialization */
int main(void) {

    enableRawMode();

    while (1) {
    editorProcessKeypress();
    editorRefreshScreen();
  }
    return 0;
}

