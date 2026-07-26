/* includes */
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

/* defines */
#define CTRL_KEY(k) ((k) & 0x1f)


/* data */
struct editorConfig {
  int screenrows;
  int screencols;
struct termios orig_termios;
};

struct editorConfig E; //global variable that contains editors state and initializes above defined struct


/* terminal */

void die(const char *s) {
  //clears screen on exit
  write(STDOUT_FILENO, "\x1b[2J", 4);
  write(STDOUT_FILENO, "\x1b[H", 3);

    perror(s);
    exit(1);
}
void disableRawMode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios) == -1)
    die("tcsetattr");
}


void enableRawMode() {
   if (tcgetattr(STDIN_FILENO, &E.orig_termios) == -1) die("tcgetattr");
    atexit(disableRawMode); // from <stdlib.h> calls disableRAWMode() automatially when program exits.

    struct termios raw = E.orig_termios;

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

int getWindowSize(int *rows, int *cols) {
  struct winsize ws;
  if (1 || ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
    if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12) return -1;
    editorReadKey();
    return -1;
  } else {
    *cols = ws.ws_col;
    *rows = ws.ws_row;
    return 0;
  }
}


//draw hearts at beginning of each row
void editorDrawRows() {
  int y;
  for (y = 0; y < E.screenrows; y++) { //draws on 24 rows
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

// initializes fields in the E struct.
void initEditor() {
  if (getWindowSize(&E.screenrows, &E.screencols) == 01) die("getWindowSize");
}
int main(void) {

    enableRawMode();
    initEditor();

    while (1) {
    editorProcessKeypress();
    editorRefreshScreen();
  }
    return 0;
}

