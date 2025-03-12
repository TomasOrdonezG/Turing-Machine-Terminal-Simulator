#include "main.h"
#include "turingMachine.h"
#include "utils.h"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: <tm filepath>\n");
        exit(1);
    }
    tm = buildTM(argv[1]);

    // Get terminal size
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    hideCursor();
    clearConsole();

    char seq[3] = "--";
    enableNonBlockingInput();
    atexit(resetInputSettings);

    int didTuiStateChange = 1;

    while (1)
    {
        // Wait until a key is pressed
        if (read(STDIN_FILENO, &keyPressed, 1) <= 0 && !didTuiStateChange)
        {
            // Reset keys
            arrowPressed.up = arrowPressed.down = arrowPressed.left = arrowPressed.right = keyPressed = 0;
            usleep(1000);
            continue;
        }

        // Exit on ESC
        if (keyPressed == 27 && read(STDIN_FILENO, &seq[0], 1) <= 0) break;
        
        // Read arrow input
        if (seq[0] == '[' && read(STDIN_FILENO, &seq[1], 1) > 0)
        {
            arrowPressed.up    = (seq[1] == 'A');
            arrowPressed.down  = (seq[1] == 'B');
            arrowPressed.right = (seq[1] == 'C');
            arrowPressed.left  = (seq[1] == 'D');
        }
        
        // Check for backspace
        isBackspace = (keyPressed == 8 || keyPressed == 127);

        // Input string
        static char inputStr[MAX_INPUT_LEN] = "\0";

        if (state == INPUT)
        {
            // User writes the input to the TM
            clearConsole();
            
            // Type/backspace
            if (isprint((unsigned int) keyPressed))
                type(inputStr, keyPressed, MAX_INPUT_LEN);
            else if (isBackspace)
                backspace(inputStr);

            // On enter, switch states
            else if (keyPressed == '\n')
            {
                didTuiStateChange = 1;
                state = EXECUTING;

                // Start executing TM
                initializeTape(tm, inputStr);
                continue;
            }

            // Display text edit
            printf("\n   %s_   \n", strshorten(inputStr, w.ws_col - 6));
        }
        else if (state == EXECUTING)
        {
            if (keyPressed == ' ')
            {
                step(tm);
                if (didHalt(tm))
                {
                    printf("\n   %s\n", tm->accept ? "accept!" : "reject!");
                    break;
                }
            }
            
            clearConsole();
            displayTM();
        }

        didTuiStateChange = 0;
    }
    
    showCursor();
    freeTM(tm);
    return 0;
}

void displayTM()
{
    printf("\n   %s   \n", strclamp(tm->tape, w.ws_col - 6, BLANK));
    printf("   %s\n", strPad("^", tm->head));

    Transition *t = getTransition(tm, readTape(tm));
    char head[4] = { 0 };
    if (t != NULL && t->move == LEFT)
    {
        sprintf(head, "< %c", t->write);
        printf("%s\n", strPad(head, tm->head + 1));
    }
    else if (t != NULL && t->move == RIGHT)
    {
        sprintf(head, "%c >", t->write);
        printf("%s\n", strPad(head, tm->head + 3));
    }
    printf("\n   Curren state: %d\n", tm->currentState);
}
