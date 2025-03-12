#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "turingMachine.h"
#include "utils.h"

#define MAX_INPUT_LEN 2048

TuringMachine *tm;

struct winsize w;

char keyPressed;
int isBackspace;
struct {
    int left, right, up, down;
} arrowPressed;

enum { INPUT, EXECUTING } state = INPUT; 

void displayTM();

#endif