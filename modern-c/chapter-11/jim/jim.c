/*

Goal: extend challenge 12 to create a super basic text processor with the following capabilities
- allow users to create or open and edit files with command line arg giving file path
- allow for insert and edit modes like vim, save users changes when the session ends 
- support basic vim motions

*/

#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <curses.h> 
#include "text_blob.h"

#define COMMAND_ARR_SIZE 100

enum {
  NORMAL,
  INSERT
} MODES;

enum {
    EXIT_EDITOR,
    EXIT_NORMAL_MODE,
    INVALID_COMMAND
} COMMAND_RESULTS;

text_blob* parse_file(const char* file_path, text_blob lines_of_text[static 1]){
    size_t current_blob_index = 0;
    FILE * fstream = fopen(file_path, "r");

    if (!fstream) {
        perror("Error opening file");
        exit(1);
    }

    char * next_line = NULL;
    size_t capacity = 0;
    ssize_t line_length;

    while ((line_length = getline(&next_line, &capacity, fstream)) != -1) {
        size_t text_size = line_length + 1;
        char *copy = malloc(text_size * 2);
        memcpy(copy, next_line, text_size);

        lines_of_text[current_blob_index] = (text_blob){
            .buffer_size = text_size * 2,
            .text_size = text_size - 1,
            .text = copy,
            .next = NULL,
            .previous = (current_blob_index == 0 ? NULL : &lines_of_text[current_blob_index - 1])
        };

        if (current_blob_index != 0){
            lines_of_text[current_blob_index - 1].next = &lines_of_text[current_blob_index];
        }

        current_blob_index++;
    }

    free(next_line);
    fclose(fstream);
    return lines_of_text;
}

void output_file(const char* file_path, text_blob current_text_blob[static 1]){
    FILE * fstream = fopen(file_path, "w");

    if (!fstream) {
        perror("Error opening file");
        exit(1);
    }

    while (current_text_blob != NULL) {
        fprintf(fstream, "%s", current_text_blob->text);
        free(current_text_blob->text);
        current_text_blob = current_text_blob->next;
    }

    fclose(fstream);
}

void handle_cursor_movement(int input, int max_row, int* cursor_row, size_t* cursor_row_char_index, text_blob* cursor_row_text_object, text_blob* top_line) {
    if (input == KEY_UP) {
        if (*cursor_row != 0) {
            *cursor_row -= 1;
            cursor_row_text_object = cursor_row_text_object->previous;
        }
        else if (top_line != NULL && top_line->previous != NULL) {
            top_line = top_line->previous;
            cursor_row_text_object = cursor_row_text_object->previous;
        }
    }

    else if (input == KEY_DOWN) {
        if (*cursor_row != max_row) {
            *cursor_row += 1;
            cursor_row_text_object = cursor_row_text_object->next;
        }
        else if (top_line != NULL && top_line->next != NULL) {
            top_line = top_line->next;
            cursor_row_text_object = cursor_row_text_object->next;
        }
    }

    else if (input == KEY_LEFT) {
        if (*cursor_row_char_index != 0) {
            *cursor_row_char_index -= 1;
        }
    }

    else if (input == KEY_RIGHT) {
        if (*cursor_row_char_index != cursor_row_text_object->text_size - 1) {
            *cursor_row_char_index += 1;
        }
    }
}

int evaluate_command(char command[static 1]) {
    if (strcmp(":x", command) == 0) {
        return EXIT_EDITOR;
    }
    if (strcmp(":i", command) == 0) {
        return EXIT_NORMAL_MODE;
    }

    return INVALID_COMMAND;
}

int run_editor(text_blob current_text_object[static 1], size_t mode) {
    initscr();
    noecho(); // don't echo user input
    cbreak(); // read after each character, don't wat for a newline 
    keypad(stdscr, TRUE); // necessary to allow for reading in inputs like arrow keys
    use_default_colors(); // use the default colors that are being used in the current terminal
    start_color(); // allow for color output
    init_pair(1, COLOR_WHITE, COLOR_RED);

    // need to keep track of where user is and edit both ncurses and the appropriate part of our data structure :/

    bool command_started = false;
    bool command_errored = false;
    char bad_command[125] = "Invalid Command: ";

    int max_row, max_col;
    getmaxyx(stdscr, max_row, max_col);

    text_blob* top_line = current_text_object;
    int cursor_row = 0;
    text_blob* cursor_row_text_object;
    size_t cursor_row_char_index = 0;
    char command[COMMAND_ARR_SIZE] = {0}; 
    size_t current_command_index = 0;
    size_t current_command_size = 0;

    while (true) {
        erase();

        if (mode == INSERT) {
            mvprintw(max_row - 1, 0, "%s", "-- INSERT --");
        }

        text_blob* iterator = top_line;
        int current_row = 0;
        while (iterator != NULL && current_row < max_row - 1) {
            mvprintw(current_row, 0, "%s", iterator->text);

            // Ensure cursor stays at the start of the line
            move(current_row, cursor_row_char_index);

            cursor_row_text_object = iterator;
            iterator = iterator->next;
            current_row++;
        }

        move(cursor_row, cursor_row_char_index); 

        if (command_started) {
            mvprintw(max_row - 1, 0, "%s", command);
            move(max_row - 1, current_command_index); 
        }

        if (command_errored) {
            attron(COLOR_PAIR(1));
            mvprintw(max_row - 1, 0, "%s", bad_command);
            attroff(COLOR_PAIR(1));
        }

        refresh();

        int user_input = getch();
        bool user_entered_backspace = user_input == KEY_DC || user_input == KEY_BACKSPACE || user_input == 127 || user_input == '\b';
        // the many different values of backspace 
        // Note: Delete and backspace are different things, but my macbook only has a Delete key so I am choosing to treat them as the same 

        if (mode == NORMAL){
            if (user_input == ':') {
                command_started = true;
                command_errored = false;
            }

            if (command_started) {
                if (user_input == '\n') {
                    int result = evaluate_command(command);
                    switch (result) {
                        case EXIT_EDITOR:
                            return 0;
                        case EXIT_NORMAL_MODE:
                            mode = INSERT;
                            command_started = false;
                            current_command_index = 0;
                            command[current_command_index] = '\0';
                            break;
                        default:
                            memcpy(bad_command + sizeof("Invalid Command: ") - 1, command + 1, COMMAND_ARR_SIZE - 1);
                            current_command_index = 0;
                            memset(command, '\0', COMMAND_ARR_SIZE);
                            command_started = false;
                            command_errored = true;
                            break;
                    }
                }

                else if (user_entered_backspace && current_command_index <= 1) {
                    // can't remove the colon and can't go out of bounds
                    continue;
                }

                else if (user_entered_backspace){
                    current_command_index--;
                    memmove(command + current_command_index, command + current_command_index + 1, COMMAND_ARR_SIZE - (current_command_index + 1));
                    continue;
                }

                else if (user_input == KEY_LEFT) {
                    if (current_command_index > 1) {
                        // can't get past the colon
                        current_command_index -= 1;
                    }
                    continue;
                }

                else if (user_input == KEY_RIGHT ) {
                    if (current_command_index != current_command_size) {
                        current_command_index += 1;
                    }
                    continue;
                }

                else {
                    command[current_command_index] = user_input;
                    current_command_index++;
                }
            }
            else {
                handle_cursor_movement(user_input, max_row, &cursor_row, &cursor_row_char_index, cursor_row_text_object, top_line);
            }
        }

        else if (mode == INSERT) {
            if (user_input == 27) { // esc char
                mode = NORMAL;
                continue;
            }

            // get the text and the size, double the buffer size if needed, update the text 
            if (cursor_row_text_object->text_size >= .8 * cursor_row_text_object->buffer_size) {
                char *copy = malloc(cursor_row_text_object->buffer_size * 2);
                memcpy(copy, current_text_object->text, cursor_row_text_object->text_size);
                free(cursor_row_text_object);
                cursor_row_text_object->buffer_size *= 2;
                cursor_row_text_object->text = copy;   // is it problematic that I am referencing a variable that will get cleaned up?
            }

            char *copy = malloc(cursor_row_text_object->buffer_size);
            memcpy(copy, current_text_object->text, cursor_row_char_index);
            copy[cursor_row_char_index] = user_input;
            memcpy(
                copy + cursor_row_char_index + 1, 
                current_text_object->text + cursor_row_char_index + 1, 
                cursor_row_text_object->text_size - cursor_row_char_index
            );
            free(current_text_object->text);
            current_text_object->text = copy;
            current_text_object->text_size++;
            cursor_row_char_index++;
        }
    }
    endwin();
}

int main(int argc, char* argv[argc]){
    if (argc != 2) {
        printf("Wrong number of arguments. Please only pass the file path.\n");
        exit(1);
    }

    set_escdelay(25); 

    bool should_create_file = false;
    FILE * fstream = fopen(argv[1], "r");

    if (!fstream) {
        if (errno == ENOENT ) {
            should_create_file = true;
        }
        else {
            perror("Error opening file");
        }
    }

    text_blob *lines_of_text; 
    if (!should_create_file) {
        char * next_line = NULL;
        size_t capacity = 0;
        ssize_t line_length;

        size_t num_lines = 0;
        while ((line_length = getline(&next_line, &capacity, fstream)) != -1) {
            num_lines += 1;
        }
        fclose(fstream);

        // allocate an array twice the size of the original file, may need resizing later 
        lines_of_text = malloc((num_lines * 2) * sizeof(text_blob)); 

        parse_file(argv[1], lines_of_text);
    }
    else {
        fclose(fstream);
        // we can create the file at the cleanup stage, when we would write out to a new file anyway
        lines_of_text = malloc(100 * sizeof(text_blob)); 

        char * starter_string = malloc(1 * sizeof(char));
        starter_string[0] = '\0';

        lines_of_text[0] = (text_blob){
            .text_size = 1,
            .buffer_size = 100,
            .text = starter_string,
            .next = NULL,
            .previous = NULL
        };
    }

    int exit_code = run_editor(lines_of_text, should_create_file ? INSERT : NORMAL);

    if (exit_code == 0) {
        output_file(argv[1], lines_of_text);
    }
}