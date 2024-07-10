/******************* includes *********************/
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <errno.h>

/******************* data *********************/

struct termios orig_termios;

/******************* terminal *********************/

// Die function that prints error message & exits the program
void die(const char *s)
{
    perror(s);
    exit(1);
}

// Disable Raw Mode at Exit
void disableRawMode()
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
    {
        die("tcsetattr");
    }
}

// Turns off echoing
void enableRawMode()
{
    // Read the current attributes into a struct
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
        die("tcgetattr");
    atexit(disableRawMode);

    // Modify the struct by hand
    struct termios raw = orig_termios;

    // IXON flag suspends Ctrl-S and Ctrl-Q
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);

    // Turns off all output processing features
    raw.c_oflag &= ~(OPOST);

    raw.c_cflag |= (CS8);

    // ICANON flag turns off canonical mode -- reading input byte by byte instead of line by line
    // ISIG flag suspends Ctr-C and Ctr-Z
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;
    // Pass the modified struct to write the new termainl attributes back out
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        die("tcsetattr");
}

/******************* init *********************/
int main()
{
    enableRawMode();
    char c;
    // Read 1 byte until there are no bytes to read
    while (1)
    {
        char c = '\0';
        if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN)
            die("read");
        // Control Character: nonprintable characters that we don't want to print to the screen
        if (iscntrl(c))
        {
            printf("%d\r\n", c);
        }
        else
        {
            printf("%d ('%c')\r\n", c, c);
        }
        if (c == 'q')
            break;
    }
    return 0;
}