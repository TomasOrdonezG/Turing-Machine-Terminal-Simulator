#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <termios.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

void hideCursor();
void showCursor();
void enableNonBlockingInput();
void resetInputSettings();
void clearConsole();
void type(char *str, char c, int maxLength);
void backspace(char *str);
const char *strshorten(const char *str, int shortenLength);
const char *strclamp(const char *str, int clampLength, char extensionChar);
const char *strPad(const char *str, int n);

#endif