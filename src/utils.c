#include "utils.h"

void hideCursor()
{
    printf("\033[?25l"); // Hide cursor
    fflush(stdout);
}

void showCursor()
{
    printf("\033[?25h"); // Show cursor
    fflush(stdout);
}

void enableNonBlockingInput()
{
    struct termios settings;

    // Get current terminal settings
    tcgetattr(STDIN_FILENO, &settings);

    // Disable canonical mode and echoing
    settings.c_lflag &= ~(ICANON | ECHO);

    // Apply settings immediately
    tcsetattr(STDIN_FILENO, TCSANOW, &settings);

    // Set the file descriptor to non-blocking mode
    fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK);
}

void resetInputSettings()
{
    struct termios settings;

    // Get current terminal settings
    tcgetattr(STDIN_FILENO, &settings);

    // Re-enable canonical mode and echoing
    settings.c_lflag |= (ICANON | ECHO);

    // Apply settings immediately
    tcsetattr(STDIN_FILENO, TCSANOW, &settings);
}

void clearConsole()
{
    printf("\033[2J\033[H");
    fflush(stdout);
}


/**
 * Appends a character to the end of a string
 */
void type(char *str, char c, int maxLength)
{
    int len = strlen(str);
    if (len < maxLength - 2)
    str[len] = c;
}


/**
 * Removes the last character of a string.
 */
void backspace(char *str)
{
    int len = strlen(str);
    if (len > 0)
        str[--len] = 0;
}

/**
 * Shortens a string to length of `length` if it is longer than that. Adds "..." if it does get shorter.
 */
const char *strshorten(const char *str, int shortenLength)
{
    static char buf[256];  // Ensure sufficient space
    int len = strlen(str);
    
    if (len > shortenLength)
    {
        strncpy(buf, str, shortenLength - 3);
        buf[shortenLength - 3] = '\0';
        strcat(buf, "...");
        return buf;
    }
    
    return str;
}


/**
 * Like strshorten but extends the string with `extensionChar`paces if its size is shorter than `clampLength`
 */
const char *strclamp(const char *str, int clampLength, char extensionChar)
{
    static char buf[256];  // Ensure sufficient space
    int len = strlen(str);

    if (len > clampLength) return strshorten(str, clampLength);

    strcpy(buf, str);
    memset(buf + len, extensionChar, clampLength - len);
    buf[clampLength] = '\0';

    return buf;
}

/**
 * Will add n spaces before w
 */
const char *strPad(const char *str, int n)
{
    static char buf[256];
    memset(buf, ' ', n);
    buf[n] = '\0';
    return strcat(buf, str);
}
 