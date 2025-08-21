#include <stdio.h>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#include <time.h>
struct termios oldt;
#endif


void set_raw_mode() {
#ifndef _WIN32
    struct termios newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
#endif
}

void reset_mode() {
#ifndef _WIN32
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif
}

double get_current_time_seconds() {
#ifdef _WIN32
    return GetTickCount() / 1000.0;
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
#endif
}

int main() {
    set_raw_mode();

    printf("Press the spacebar repetitively to set the tempo. Press 'q' to quit and calculate.\n");

    double intervals[1000];
    int count = 0;
    double prev_time = 0.0;

    while (1) {
        char ch;
#ifdef _WIN32
        ch = getch();
#else
        ch = getchar();
#endif

        if (ch == 'q' || ch == 'Q') {
            break;
        }

        if (ch == ' ') {
            double current_time = get_current_time_seconds();

            if (count > 0) {
                intervals[count - 1] = current_time - prev_time;
            } else {
                // First tap, just set the start time
            }

            prev_time = current_time;
            count++;
        }
    }

    reset_mode();

    if (count > 1) {
        double sum = 0.0;
        for (int i = 0; i < count - 1; i++) {
            sum += intervals[i];
        }
        double avg_time_per_beat = sum / (count - 1);
        double bpm = 60.0 / avg_time_per_beat;
        double frames_per_beat = 24.0 * avg_time_per_beat;

        printf("\nAverage BPM: %.2f\n", bpm);
        printf("Frames per beat at 24 FPS: %.2f\n", frames_per_beat);
    } else {
        printf("\nNot enough taps to calculate tempo.\n");
    }

    printf("Press any key to close...\n");
    set_raw_mode();
#ifdef _WIN32
    getch();
#else
    getchar();
#endif
    reset_mode();

    return 0;
}