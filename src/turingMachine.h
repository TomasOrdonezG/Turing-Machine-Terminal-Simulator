#ifndef TURING_H
#define TURING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLANK '_'
#define LEFT 'L'
#define RIGHT 'R'

typedef struct {
    int currState;
    char read;
    int nextState;
    char write;
    char move;
} Transition;

typedef struct {
    // Representation
    Transition **transitions;
    int size;
    int *transitionSizes;
    int *transitionCapacities;
    int initialState;
    int acceptState;
    int rejectState;
    int currentState;

    // Execution
    int head;
    char *tape;
    int tapeSize;
    int accept;
    int reject;
} TuringMachine;

TuringMachine *buildTM(char *filepath);
void freeTM(TuringMachine *tm);
void assertTMInitialized(TuringMachine *tm);
Transition *getTransition(TuringMachine *tm, char c);
void initializeTape(TuringMachine *tm, char *w);
void assertTapeInitialized(TuringMachine *tm);
void moveLeft(TuringMachine *tm);
void moveRight(TuringMachine *tm);
char readTape(TuringMachine *tm);
void writeTape(TuringMachine *tm, char c);
int didHalt(TuringMachine *tm);
void step(TuringMachine *tm);


#endif