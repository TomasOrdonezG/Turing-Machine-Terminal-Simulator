#include "turingMachine.h"

TuringMachine *buildTM(char *filepath)
{
    TuringMachine *tm = malloc(sizeof(TuringMachine));
    tm->tape = NULL;

    // Initialize TM
    tm->transitions = malloc(sizeof(Transition*));
    tm->transitions[0] = malloc(sizeof(Transition));
    tm->transitionSizes = malloc(sizeof(int));
    tm->transitionCapacities = malloc(sizeof(int));
    tm->transitionSizes[0] = 0;
    tm->transitionCapacities[0] = 1;
    tm->size = 1;
    
    FILE *tmFile = fopen(filepath, "r");
    
    // Build TM from file
    
    // Read initial, accept, and reject states
    if (fscanf(tmFile, "%d,%d,%d", &tm->initialState, &tm->acceptState, &tm->rejectState) != 3)
    {
        fprintf(stderr, "Error: TM file must begin with `<initial state (int), accept state (int), reject state (int)>`.\n");
        exit(1);
    }
    
    // Read transitions
    Transition t;
    while (fscanf(tmFile, "%d,%c:%d,%c,%c", &t.currState, &t.read, &t.nextState, &t.write, &t.move) == 5)
    {    
        // Assert valid move
        if (t.move != LEFT && t.move != RIGHT)
        {
            fprintf(stderr, "Error: Transition moves must be either '%c' or '%c'. Found '%c'.\n", LEFT, RIGHT, t.move);
            exit(1);
        }

        int i = t.currState;
        if (i >= tm->size)
        {
            // No transition array yet
            int newSize = i + 1;
            tm->transitions = realloc(tm->transitions, sizeof(Transition*) * newSize);
            tm->transitionSizes = realloc(tm->transitionSizes, sizeof(int) * newSize);
            tm->transitionCapacities = realloc(tm->transitionCapacities, sizeof(int) * newSize);
            for (int j = tm->size; j < newSize; j++)
            {
                tm->transitions[j] = malloc(sizeof(Transition));
                tm->transitionSizes[j] = 0;
                tm->transitionCapacities[j] = 1;
            }

            // Insert transition
            tm->transitions[i][0] = t;
            tm->transitionSizes[i]++;
            tm->size = newSize;
        }
        else
        {
            // Transition array exists
            if (tm->transitionSizes[i] == tm->transitionCapacities[i])
            {
                // Double capacity if size == capacity
                tm->transitionCapacities[i] *= 2;
                tm->transitions[i] = realloc(tm->transitions[i], sizeof(Transition) * tm->transitionCapacities[i]);
            }

            // Insert current transition and increment size
            tm->transitions[i][tm->transitionSizes[i]++] = t;
        }
    }

    fclose(tmFile);

    return tm;
}

void freeTM(TuringMachine *tm)
{
    if (tm->tape != NULL)
        free(tm->tape);
    for (int i = 0; i < tm->size; i++)
        free(tm->transitions[i]);
    free(tm->transitions);
    free(tm->transitionSizes);
    free(tm->transitionCapacities);
    free(tm);
}

void assertTMInitialized(TuringMachine *tm)
{
    if (tm->transitions == NULL)
    {
        fprintf(stderr, "Error: TM has not been initialized yet.\n");
        exit(1);
    }
}

Transition *getTransition(TuringMachine *tm, char c)
{
    assertTMInitialized(tm);
    int currState = tm->currentState;

    if (currState < 0)
    {
        fprintf(stderr, "Error: Current state lower than zero: %d\n", currState);
        exit(1);
    }
    if (currState >= tm->size)
    {
        printf("Warning: Current state `%d` has no outgoing transitions.\n", currState);
        return NULL;
    }

    int tSize = tm->transitionSizes[currState];
    Transition *transitions = tm->transitions[currState];

    // Search for the righ transitions, if not found return NULL
    for (int i = 0; i < tSize; i++)
        if (transitions[i].read == c)
            return transitions + i;
    return NULL;
}

void initializeTape(TuringMachine *tm, char *w)
{
    assertTMInitialized(tm);
    tm->head = 0;
    tm->accept =  0;
    tm->reject =  0;
    tm->currentState = tm->initialState;
    int wLen = strlen(w);

    // Initialize tape size
    tm->tapeSize = wLen + 2;  // To include _ and \0 after w in the tape
    tm->tape = malloc(sizeof(char) * tm->tapeSize);
    strncpy(tm->tape, w, wLen + 1);
    tm->tape[wLen] = '_';
    tm->tape[wLen + 1] = '\0';
    // Tape is now [ w0, w1, ..., wn, _, \0 ]
}

void assertTapeInitialized(TuringMachine *tm)
{
    if (tm->tape == NULL)
    {
        fprintf(stderr, "Error: Tape accessed before initialization.\n");
        exit(1);
    }
}

void moveLeft(TuringMachine *tm)
{
    assertTapeInitialized(tm);
    if (--(tm->head) < 0)
        tm->reject = 1;
}

void moveRight(TuringMachine *tm)
{
    tm->head++;
}

char readTape(TuringMachine *tm)
{
    assertTapeInitialized(tm);
    if (tm->head < 0)
    {
        fprintf(stderr, "Error: Tape head out of bounds to the left when attempting to read.\n");
        exit(1);
    }
    
    if (tm->head < tm->tapeSize)
        return tm->tape[tm->head];

    // When head >= tapeSize always return blank
    return BLANK;
}

void writeTape(TuringMachine *tm, char c)
{
    assertTapeInitialized(tm);

    if (tm->head < 0)
    {
        fprintf(stderr, "Error: Tape head out of bounds to the left when attempting a write.\n");
        exit(1);
    }

    if (tm->head < tm->tapeSize - 2)
    {
        // Simply replace tape character
        tm->tape[tm->head] = c;
        return;
    }

    // No change when writing a blank out of bounds to the right
    if (c == BLANK) return;

    // When attempting to write on or past the rightmost blank:
    // Extend tape such that write head = size - 3 (the last 2 will be blank and \0)
    int oldSize = tm->tapeSize;
    tm->tapeSize = tm->head + 3;
    tm->tape = realloc(tm->tape, sizeof(char) * tm->tapeSize);

    // Insert blanks from position old size - 1 to position new size - 4
    for (int i = oldSize - 1; i <= tm->head - 1; i++)
        tm->tape[i] = BLANK;
    
    // Write to the tape and end with blank, \0
    tm->tape[tm->head] = c;
    tm->tape[tm->head + 1] = BLANK;
    tm->tape[tm->head + 2] = '\0';
}

int didHalt(TuringMachine *tm)
{
    return (tm->accept || tm->reject);
}

void step(TuringMachine *tm)
{
    assertTapeInitialized(tm);

    // Read input
    char c = readTape(tm);

    // Find transition
    Transition *t = getTransition(tm, c);
    
    // Reject if transition does not exist
    if (t == NULL)
    {
        tm->accept = 0;
        tm->reject = 1;
        return;
    }

    // Write input
    writeTape(tm, t->write);

    // Move head
    if (t->move == LEFT)
        moveLeft(tm);
    else if (t->move == RIGHT)
        moveRight(tm);
    else
    {
        fprintf(stderr, "Error: Transition moves must be either '%c' or '%c'. Found '%c'.\n", LEFT, RIGHT, t->move);
        exit(1);
    }

    // Update state
    tm->currentState = t->nextState;

    // Check if the new state is an accept state
    if (tm->currentState == tm->acceptState)
    {
        tm->reject = 0;
        tm->accept = 1;
    }
}
