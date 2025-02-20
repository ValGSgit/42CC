#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/time.h>
#include <errno.h>

#define MAX_ARGS 10
#define CMD_LEN 200
#define TIMEOUT 10 // seconds
#define OUTPUT_FILE "test_results.txt"

// ANSI color codes for terminal output
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"

// Structure to hold test cases
typedef struct {
    char *input;
    char *expected;
} TestCase;

// List of test cases
TestCase test_cases[] = {
    {"5 800 200 200", "Should run indefinitely with no deaths"},  // Basic case with enough time for philosophers to eat and think
    {"1 800 200 200", "Philosopher should die after 800ms due to no forks"},  // Single philosopher should die as they can't eat
    {"5 600 200 200", "Should run for a while with no deadlock, then exit normally"},  // Ensures deadlock prevention works
    {"5 300 200 100", "At least one philosopher should die due to starvation"},  // Tests starvation scenario
    {"5 800 200 200 3", "Each philosopher eats 3 times, then program exits"},  // Ensures stopping condition works
    {"5 800 0 0 1", "Should stop after each philosopher eats once, despite zero wait times"},  // Edge case for instant eating
    {"200 800 200 200", "Stress test, should not crash or deadlock"},  // Ensures stability with many philosophers
    {"5 10 1 1", "Most philosophers should die quickly due to extreme timing constraints"},  // Tests timing precision and responsiveness
    {"5 -100 200 200", "Should fail with an error due to negative time"},  // Invalid input handling
    {"0 800 200 200", "Should fail with an error due to zero philosophers"},  // Edge case with no philosophers
    {"5 800 200", "Should fail due to incorrect argument count"},  // Too few arguments
    {"5 800 200 200 10 20", "Should fail due to too many arguments"},  // Too many arguments
    {"5 100 50 30", "Should run briefly but exit normally, ensuring eating logic is followed"},  // Short execution time
    {"2 800 200 200", "Both philosophers should eat without deadlocking"},  // Verifies small cases work correctly
    {"10 800 200 200", "Should run indefinitely with no deaths, testing moderate load"},  // Tests medium-scale simulation
    {"5 10000 500 400", "Should allow extended eating periods with no deaths"},  // Ensures prolonged thinking/eating works
    {"3 500 250 200", "Should balance eating and thinking with no starvation"},  // Verifies fairness in odd-numbered cases
    {"5 100 100 100 5", "Should exit after each philosopher eats 5 times"},  // Ensures eating counter is respected
};


// Function to write to both stdout (with color) and file
void dual_write(const char *message, const char *color, FILE *file) {
    fprintf(stdout, "%s%s%s", color, message, RESET);
    fputs(message, file);
}

// Signal handler for alarm
void sigalrm_handler(int sig) {
    (void)sig;  // Silence unused parameter warning
    dual_write("Timeout occurred\n", YELLOW, fopen(OUTPUT_FILE, "a"));
}

// Execute test case with timeout
// void run_test(const char *cmd, const char *expected, FILE *file) {
//     int pid = fork();
//     if (pid == -1) {
//         perror("fork failed");
//         exit(1);
//     } else if (pid == 0) { // Child process
//         char *args[MAX_ARGS];
//         char cmd_copy[CMD_LEN];
//         strcpy(cmd_copy, cmd);
//         char *token = strtok(cmd_copy, " ");
//         int i = 0;
//         while (token != NULL && i < MAX_ARGS - 1) {
//             args[i++] = token;
//             token = strtok(NULL, " ");
//         }
//         args[i] = NULL;
        
//         // Set up signal handler for alarm
//         signal(SIGALRM, sigalrm_handler);
        
//         // Set alarm for timeout
//         alarm(TIMEOUT);
        
//         execvp("./philo", args);
//         perror("execvp failed");
//         exit(1);
//     } else { // Parent process
//         int status;
//         struct timeval start, end;
//         gettimeofday(&start, NULL);
        
//         while (waitpid(pid, &status, WNOHANG) == 0) {
//             gettimeofday(&end, NULL);
//             if ((end.tv_sec - start.tv_sec) > TIMEOUT) {
//                 kill(pid, SIGKILL); // Terminate if timeout 
//                 dual_write("Test with input '", BLUE, file);
//                 dual_write(cmd, BLUE, file);
//                 dual_write("':\n", BLUE, file);
//                 dual_write("  Expected: ", BLUE, file);
//                 dual_write(expected, BLUE, file);
//                 dual_write("\n", BLUE, file);
//                 dual_write("  Timed out after ", RED, file);
//                 dual_write("5", RED, file);
//                 dual_write(" seconds\n", RED, file);
//                 dual_write("--------------------------\n", BLUE, file);
//                 return;
//             }
//             usleep(10000); // Sleep for 100ms before checking again
//         }
        
//         dual_write("Test with input '", BLUE, file);
//         dual_write(cmd, BLUE, file);
//         dual_write("':\n", BLUE, file);
//         dual_write("  Expected: ", BLUE, file);
//         dual_write(expected, BLUE, file);
//         dual_write("\n", BLUE, file);
        
//         if (WIFEXITED(status)) {
//             dual_write("  Exited with status ", GREEN, file);
//             char exit_status[10];
//             snprintf(exit_status, sizeof(exit_status), "%d\n", WEXITSTATUS(status));
//             dual_write(exit_status, GREEN, file);
//         } else if (WIFSIGNALED(status)) {
//             dual_write("  Terminated by signal ", RED, file);
//             char signal_num[10];
//             snprintf(signal_num, sizeof(signal_num), "%d\n", WTERMSIG(status));
//             dual_write(signal_num, RED, file);
//         }
//         dual_write("--------------------------\n", BLUE, file);
//     }
// }
void run_test(const char *cmd, const char *expected, FILE *file) {
    int pid = fork();
    if (pid == -1) {
        perror("fork failed");
        exit(1);
    } else if (pid == 0) { // Child process
        char *args[MAX_ARGS];
        char cmd_copy[CMD_LEN];
        strcpy(cmd_copy, cmd);
        
        char *token = strtok(cmd_copy, " ");
        int i = 0;
        while (token != NULL && i < MAX_ARGS - 1) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;
        
        // Set up signal handler for alarm
        signal(SIGALRM, sigalrm_handler);
        
        // Set alarm for timeout
        alarm(TIMEOUT);
        
        execvp("./philo", args);
        perror("execvp failed");
        exit(1);
    } else { // Parent process
        int status;
        struct timeval start, end;
        gettimeofday(&start, NULL);
        
        while (waitpid(pid, &status, WNOHANG) == 0) {
            gettimeofday(&end, NULL);
            if ((end.tv_sec - start.tv_sec) > TIMEOUT) {
                kill(pid, SIGKILL); // Terminate if timeout 
                dual_write("Test with input '", BLUE, file);
                dual_write(cmd, BLUE, file);
                dual_write("':\n", BLUE, file);
                dual_write("  Expected: ", BLUE, file);
                dual_write(expected, BLUE, file);
                dual_write("\n", BLUE, file);
                dual_write("  Timed out after ", RED, file);
                dual_write("5", RED, file);
                dual_write(" seconds\n", RED, file);
                dual_write("--------------------------\n", BLUE, file);
                return;
            }
            usleep(10000); // Sleep for 100ms before checking again
        }
        
        gettimeofday(&end, NULL);
        long elapsed_time = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000;
        
        dual_write("Test with input '", BLUE, file);
        dual_write(cmd, BLUE, file);
        dual_write("':\n", BLUE, file);
        dual_write("  Expected: ", BLUE, file);
        dual_write(expected, BLUE, file);
        dual_write("\n", BLUE, file);
        dual_write("  Time taken: ", GREEN, file);
        char time_str[20];
        snprintf(time_str, sizeof(time_str), "%ld ms\n", elapsed_time);
        dual_write(time_str, GREEN, file);
        
        if (WIFEXITED(status) || status == 14) {
            dual_write("  Exited with status ", GREEN, file);
            char exit_status[10];
            snprintf(exit_status, sizeof(exit_status), "%d\n", WEXITSTATUS(status));
            dual_write(exit_status, GREEN, file);
        } else if (WIFSIGNALED(status)) {
            dual_write("  Terminated by signal ", RED, file);
            char signal_num[10];
            snprintf(signal_num, sizeof(signal_num), "%d\n", WTERMSIG(status));
            dual_write(signal_num, RED, file);
        }
        dual_write("--------------------------\n", BLUE, file);
    }
}

int main() {
    FILE *file = fopen(OUTPUT_FILE, "w");
    if (file == NULL) {
        perror("Could not open file for writing");
        exit(1);
    }

    int num_tests = sizeof(test_cases) / sizeof(TestCase);
    for (int i = 0; i < num_tests; i++) {
        char cmd[CMD_LEN];
        snprintf(cmd, sizeof(cmd), "./philo %s", test_cases[i].input);
        run_test(cmd, test_cases[i].expected, file);
    }
    fclose(file);
    return 0;
}