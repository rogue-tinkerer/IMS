#include <stdio.h>
#include <unistd.h>

#define BAR_LENGTH 20
#define DELAY 200000

void clearLine() {
    printf("\r"); //
    for (int i = 0; i < BAR_LENGTH + 15; ++i) {
        printf(" ");
    }
    printf("\r");
    fflush(stdout);
}

void displayProgressBar(int progress) {
    printf("\t Progress: [");
    int i;
    for (i = 0; i < BAR_LENGTH; i++) {
        if (i < progress * BAR_LENGTH / 100) {
            printf("#");
        } else {
            printf(" ");
        }
    }
    if (progress <= 100) {
        printf("] %d%%", progress);
    }
}

int mainP() {
    int i;
    for (i = 0; i <= 100; i += 5) {
        clearLine();
        displayProgressBar(i);
        usleep(DELAY);
    }

    clearLine();
    system("cls");
    return 0;
}
