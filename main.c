#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
    #include <windows.h>
    #include <conio.h>
    #define CLEAR_SCREEN "cls"
#else
    #include <unistd.h>
    #define CLEAR_SCREEN "clear"
#endif

#define MAX_NAME_LENGTH 200
#define MAX_EMAIL_LENGTH 150
#define OUTPUT_FILE "contacts.txt"

// Colour codes
#define RESET           "\033[0m"
#define BOLD            "\033[1m"
#define RED             "\033[31m"
#define GREEN           "\033[32m"
#define YELLOW          "\033[33m"
#define BLUE            "\033[34m"
#define MAGENTA         "\033[35m"
#define CYAN            "\033[36m"
#define WHITE           "\033[37m"
#define BRIGHT_GREEN    "\033[92m"
#define BRIGHT_BLUE     "\033[94m"
#define BRIGHT_CYAN     "\033[96m"

// Are colours supported? They better be.
int colours_enabled = 1;

#ifdef _WIN32
void enable_windows_colours() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;

    if (hOut == INVALID_HANDLE_VALUE) {
        colours_enabled = 0;
        return;
    }

    if (!GetConsoleMode(hOut, &dwMode)) {
        colours_enabled = 0;
        return;
    }

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(hOut, dwMode)) {
        colours_enabled = 0;
    }
}
#endif

void clear_screen() {
    system(CLEAR_SCREEN);
}

void wait_for_key() {
    printf("%sPress any key to continue...%s", CYAN, RESET);
    fflush(stdout);

#ifdef _WIN32
    _getch();
#else
    getchar();
#endif
}

int is_allowed_email_char(const char c) {
    // Allowed characters: alphanumeric, @, ., -, _, +
    return ((c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') ||
            c == '@' || c == '.' || c == '-' || c == '_' || c == '+');
}

int is_valid_email(const char* email) {
    int at_count = 0;
    int dot_count = 0;
    int at_pos = -1;
    const int len = strlen(email);

    if (len < 5) return 0;

    for (int i = 0; i < len; i++) {
        if (!is_allowed_email_char(email[i])) {
            return 0;
        }

        if (email[i] == '@') {
            at_count++;
            at_pos = i;
        } else if (email[i] == '.') {
            dot_count++;
        }
    }

    // Must have exactly one @ and at least one dot
    // @ cannot be first or last character
    // Must have at least one character after the last dot
    // Cannot start or end with dot or hyphen
    return (at_count == 1 && dot_count >= 1 &&
            at_pos > 0 && at_pos < len - 1 &&
            strrchr(email, '.') > strchr(email, '@') &&
            email[0] != '.' && email[0] != '-' &&
            email[len-1] != '.' && email[len-1] != '-');
}

void get_timestamp(char* buffer, const size_t size) {
    const time_t now = time(NULL);
    const struct tm* local = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", local);
}

int save_contact(const char* name, const char* email) {
    FILE* file = fopen(OUTPUT_FILE, "a");
    if (!file) {
        printf("%sError: Could not open output file for writing.%s\n", RED, RESET);
        return 0;
    }

    char timestamp[50];
    get_timestamp(timestamp, sizeof(timestamp));

    fprintf(file, "=====================================\n");
    fprintf(file, "Entry\n");
    fprintf(file, "Date/Time: %s\n", timestamp);
    fprintf(file, "Name: %s\n", name);
    fprintf(file, "Email: %s\n", email);
    fprintf(file, "=====================================\n\n");

    fclose(file);
    return 1;
}

void display_header() {
    printf("%s%s=====================================\n", BOLD, BRIGHT_CYAN);
    printf("     CONTACT INTEREST FORM\n");
    printf("=====================================%s\n\n", RESET);
}

void get_input(const char* prompt, char* buffer, const size_t size) {
    printf("%s%s%s", BRIGHT_BLUE, prompt, RESET);
    fflush(stdout);

    if (fgets(buffer, size, stdin) != NULL) {
        // Remove newline if present
        const size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
    }
}

void display_stats() {
    FILE* file = fopen(OUTPUT_FILE, "r");
    if (!file) {
        printf("%sNo contacts file found yet.%s\n", YELLOW, RESET);
        return;
    }

    int count = 0;
    char line[256];

    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, "Entry")) {
            count++;
        }
    }

    fclose(file);
    printf("%sTotal contacts collected: %s%s%d%s\n", WHITE, BOLD, BRIGHT_GREEN, count, RESET);
}

void display_menu() {
    printf("%sOptions:%s\n", BOLD, RESET);
    printf("%s1.%s Add new contact\n", BRIGHT_GREEN, RESET);
    printf("%s2.%s View contacts file location\n", BRIGHT_BLUE, RESET);
    printf("%s3.%s Exit\n", RED, RESET);
    printf("\n%sEnter your choice (1-3): %s", YELLOW, RESET);
}

void display_error(const char* message) {
    printf("%s%sError: %s%s\n", BOLD, RED, message, RESET);
}

void display_success(const char* message) {
    printf("%s%s✓ %s%s\n", BOLD, BRIGHT_GREEN, message, RESET);
}

void display_info(const char* message) {
    printf("%s%s%s%s\n", BOLD, BRIGHT_BLUE, message, RESET);
}

int main() {
    char name[MAX_NAME_LENGTH];
    char email[MAX_EMAIL_LENGTH];

#ifdef _WIN32
    enable_windows_colours();
#endif

    // Create output file with header if it doesn't exist
    FILE* check_file = fopen(OUTPUT_FILE, "r");
    if (!check_file) {
        FILE* new_file = fopen(OUTPUT_FILE, "w");
        if (new_file) {
            fprintf(new_file, "CONTACT INTEREST FORM SUBMISSIONS\n");
            fprintf(new_file, "Generated on: ");
            char timestamp[50];
            get_timestamp(timestamp, sizeof(timestamp));
            fprintf(new_file, "%s\n\n", timestamp);
            fclose(new_file);
        }
    } else {
        fclose(check_file);
    }

    while (1) {
        clear_screen();
        display_header();
        display_stats();
        printf("\n");
        display_menu();

        char choice = getchar();
        while (getchar() != '\n') {} // Clear input buffer

        switch (choice) {
            case '1':
                clear_screen();
                display_header();
                printf("%sPlease enter your information:%s\n\n", BOLD, RESET);

                // Get name
                get_input("Full Name: ", name, sizeof(name));

                // Validate name
                if (strlen(name) == 0) {
                    printf("\n");
                    display_error("Name cannot be empty.");
                    wait_for_key();
                    continue;
                }

                // Get email
                while (1) {
                    get_input("Email Address: ", email, sizeof(email));

                    if (strlen(email) == 0) {
                        display_error("Email cannot be empty.");
                        continue;
                    }

                    if (!is_valid_email(email)) {
                        display_error("Invalid email format.");
                        printf("%sEmail can only contain letters, numbers, @, ., -, _, and +%s\n", YELLOW, RESET);
                        printf("%sNo spaces or special characters like *, !, etc. are allowed.%s\n", YELLOW, RESET);
                        continue;
                    }

                    break;
                }

                // Confirm information
                printf("\n%s%s--- Please confirm your information ---%s\n", BOLD, MAGENTA, RESET);
                printf("%sName:%s %s\n", CYAN, RESET, name);
                printf("%sEmail:%s %s\n", CYAN, RESET, email);
                printf("\n%sIs this correct? (y/n): %s", YELLOW, RESET);

                choice = getchar();
                while (getchar() != '\n') {} // Clear input buffer

                if (choice == 'y' || choice == 'Y') {
                    if (save_contact(name, email)) {
                        printf("\n");
                        display_success("Your information has been saved successfully!");
                        printf("%sThank you! We will contact you soon.%s\n", BRIGHT_CYAN, RESET);
                    } else {
                        printf("\n");
                        display_error("Failed to save your information. Please try again.");
                    }
                } else {
                    printf("\n");
                    display_info("Information not saved. Please try again.");
                }

                wait_for_key();
                break;

            case '2':
                clear_screen();
                display_header();
                printf("%sContacts are being saved to:%s %s%s%s\n", CYAN, RESET, BOLD, OUTPUT_FILE, RESET);
                printf("%sThis file is located in the same directory as this application.%s\n\n", WHITE, RESET);
                wait_for_key();
                break;

            case '3':
                printf("\n%s%sThank you for using the Contact Interest Form!%s\n", BOLD, BRIGHT_GREEN, RESET);
                printf("%sAll contacts have been saved to:%s %s%s%s\n", CYAN, RESET, BOLD, OUTPUT_FILE, RESET);
                return 0;

            default:
                printf("\n");
                display_error("Invalid choice. Please try again.");
                wait_for_key();
                break;
        }
    }
}