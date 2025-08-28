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

void clear_screen() {
    system(CLEAR_SCREEN);
}

void wait_for_key() {
    printf("Press any key to continue...");
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
        printf("Error: Could not open output file for writing.\n");
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
    printf("=====================================\n");
    printf("     CONTACT INTEREST FORM\n");
    printf("=====================================\n\n");
}

void get_input(const char* prompt, char* buffer, const size_t size) {
    printf("%s", prompt);
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
        printf("No contacts file found yet.\n");
        return;
    }

    int count = 0;
    char line[256];

    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, "Contact Entry")) {
            count++;
        }
    }

    fclose(file);
    printf("Total contacts collected: %d\n", count);
}

int main() {
    char name[MAX_NAME_LENGTH];
    char email[MAX_EMAIL_LENGTH];

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

        printf("Options:\n");
        printf("1. Add new contact\n");
        printf("2. View contacts file location\n");
        printf("3. Exit\n");
        printf("\nEnter your choice (1-3): ");

        char choice = getchar();
        while (getchar() != '\n') {} // Clear input buffer

        switch (choice) {
            case '1':
                clear_screen();
                display_header();
                printf("Please enter your information:\n\n");

                // Get name
                get_input("Full Name: ", name, sizeof(name));

                // Validate name
                if (strlen(name) == 0) {
                    printf("\nError: Name cannot be empty.\n");
                    wait_for_key();
                    continue;
                }

                // Get email
                while (1) {
                    get_input("Email Address: ", email, sizeof(email));

                    if (strlen(email) == 0) {
                        printf("Error: Email cannot be empty.\n");
                        continue;
                    }

                    if (!is_valid_email(email)) {
                        printf("Error: Please enter a valid email address.\n");
                        continue;
                    }

                    break;
                }

                // Confirm information
                printf("\n--- Please confirm your information ---\n");
                printf("Name: %s\n", name);
                printf("Email: %s\n", email);
                printf("\nIs this correct? (y/n): ");

                choice = getchar();
                while (getchar() != '\n') {} // Clear input buffer

                if (choice == 'y' || choice == 'Y') {
                    if (save_contact(name, email)) {
                        printf("\nThank you! Your information has been saved successfully.\n");
                    } else {
                        printf("\nError: Failed to save your information. Please try again.\n");
                    }
                } else {
                    printf("\nInformation not saved. Please try again.\n");
                }

                wait_for_key();
                break;

            case '2':
                clear_screen();
                display_header();
                printf("Contacts are being saved to: %s\n", OUTPUT_FILE);
                printf("This file is located in the same directory as this application.\n\n");
                wait_for_key();
                break;

            case '3':
                printf("\nThank you for using the Contact Interest Form!\n");
                printf("All contacts have been saved to: %s\n", OUTPUT_FILE);
                return 0;

            default:
                printf("\nInvalid choice. Please try again.\n");
                wait_for_key();
                break;
        }
    }
}
