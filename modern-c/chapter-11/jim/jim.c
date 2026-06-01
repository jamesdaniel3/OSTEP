/*

Goal: extend challenge 12 to create a super basic text processor with the following capabilities
- allow users to create or open and edit files with command line arg giving file path
- allow for insert and edit modes like vim, save users changes when the session ends 
- support vim motions h, j, k, l, H, M, L, w, e, b, 0, dd

Note: There are a ton of problems with this implementation. 


TODO:
- make it so that users can type UTF-8 chars into the command bar 
- add tests for search functions that were rewritten 
- implement find and replace (ensure it supports UTF-8)
- searching back to back seems broken
- turn the buffer into a struct that keeps track of all the relevant vars 
- check search erroring logic
*/

#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <ctype.h>
#include <locale.h>
#include <wchar.h>

#define NCURSES_WIDECHAR 1
#include <curses.h> 

#include "text_blob.h"
#include "cursor.h"
#include "command.h"
#include "file_operations.h"
#include "mbstrings.h"
#include "regex.h"

#define COMMAND_ARR_SIZE 102
enum {
    ERROR_COLORS = 1,
    HIGHLIGHT_COLORS = 2,
} COLOR_PALLETES;

enum {
  NORMAL,
  INSERT
} MODES;

void insert_character_into_document(text_blob* text_object, size_t* current_location, char new_char){
    memmove(
        text_object->text + *current_location + 1, 
        text_object->text + *current_location, 
        text_object->text_size - (*current_location)
    );
    text_object->text[*current_location] = new_char;
    text_object->text_size++;
    *current_location += 1;
}

void cleanup_ncurses(){
    erase();
    refresh();
    endwin();
}

void print_multibyte_string(
    text_blob* current_line, size_t current_row, 
    bool highlight_matches, char* search_query, size_t query_size
){
    mbstate_t state = { };
    size_t next_cursor_position = 0;
    match_position_info next_search_result = {};

    if (highlight_matches) {
        next_search_result = get_regex_match(current_line->text, current_line->text_size, search_query, query_size);
    }

    for (char* current_char_pointer = current_line->text; *current_char_pointer;){
        size_t const max_possible_bytes = current_line->text_size - (current_char_pointer - current_line->text);
        wchar_t wide_char;
        size_t bytes_in_wide_char = mbrtowc(&wide_char, current_char_pointer, max_possible_bytes, &state);

        if (bytes_in_wide_char == (size_t) - 1) {
            // mbrtowc defines this as the response for an encoding error / unreadable data
            cleanup_ncurses();
            exit(1);
        }

        if (bytes_in_wide_char == (size_t) - 2) {
            // mbrtowc defines this as the response for when the n characters it read contribute to but do not complete a valid multibyte character sequence 
            cleanup_ncurses();
            exit(1);
        }

        cchar_t ch;
        wchar_t wstr[] = { wide_char, L'\0' };

        if(
            highlight_matches && 
            next_search_result.length != 0 && 
            next_cursor_position >= next_search_result.starting_index &&
            next_cursor_position < next_search_result.starting_index + next_search_result.length
        ) {
            attron(COLOR_PAIR(HIGHLIGHT_COLORS));
        }
        setcchar(&ch, wstr, 0, 0, NULL);
        mvadd_wch(current_row, next_cursor_position, &ch);
        next_cursor_position++;
        attroff(COLOR_PAIR(HIGHLIGHT_COLORS));

        if (
            highlight_matches && 
            next_search_result.length != 0 && 
            next_cursor_position >= next_search_result.starting_index + next_search_result.length
        ) {
            next_search_result = get_regex_match(current_line->text + next_cursor_position, current_line->text_size - next_cursor_position, search_query, query_size);
            next_search_result.starting_index += next_cursor_position;
        }

        current_char_pointer += bytes_in_wide_char;
    }
}

int run_editor(text_blob current_text_object[static 1], size_t mode) {
    setlocale(LC_ALL, "");
    initscr();
    noecho(); // don't echo user input
    cbreak(); // read after each character, don't wat for a newline 
    keypad(stdscr, TRUE); // necessary to allow for reading in inputs like arrow keys
    use_default_colors(); // use the default colors that are being used in the current terminal
    start_color(); // allow for color output
    init_pair(ERROR_COLORS, COLOR_WHITE, COLOR_RED);
    init_pair(HIGHLIGHT_COLORS, COLOR_MAGENTA, COLOR_GREEN);

    bool command_started = false;
    bool command_errored = false;
    char command[COMMAND_ARR_SIZE] = {0}; 
    size_t current_command_index = 0;
    size_t current_command_size = 0;
    char command_error_message[125] = "Invalid Command: ";

    bool search_started = false;
    bool search_errorred = false;
    bool show_search_results = false;
    char search_error_message[125] = "Invalid Expression: ";

    int max_row, max_col;
    getmaxyx(stdscr, max_row, max_col);

    text_blob* top_line = current_text_object;
    int cursor_row = 0;
    text_blob* cursor_row_text_object;
    size_t cursor_row_char_index = 0;

    char normal_mode_prefix = 0; 

    while (true) {
        erase();

        if (mode == INSERT) {
            mvprintw(max_row - 1, 0, "%s", "-- INSERT --");
        }

        text_blob* iterator = top_line;
        int current_row = 0;

        while (iterator != NULL && current_row < max_row - 1) {
            print_multibyte_string(iterator, current_row, show_search_results, command + 1, current_command_size - 1);
            if (current_row == cursor_row) {
                cursor_row_text_object = iterator;
            }

            iterator = iterator->next;
            current_row++;
        }

        move(cursor_row, cursor_row_char_index); 

        if (command_started || search_started) {
            mvprintw(max_row - 1, 0, "%s", command);
            move(max_row - 1, current_command_index); 
        }

        if (command_errored) {
            attron(COLOR_PAIR(ERROR_COLORS));
            mvprintw(max_row - 1, 0, "%s", command_error_message);
            attroff(COLOR_PAIR(ERROR_COLORS));
        }
        else if (search_errorred) {
            attron(COLOR_PAIR(ERROR_COLORS));
            mvprintw(max_row - 1, 0, "%s", search_error_message);
            attroff(COLOR_PAIR(ERROR_COLORS));
        }

        refresh();

        int user_input = getch();

        // the many different values of backspace 
        bool user_entered_backspace = user_input == KEY_DC || user_input == KEY_BACKSPACE || user_input == 127 || user_input == '\b';
        // Note: Delete and backspace are different things, but my macbook only has a Delete key so I am choosing to treat them as the same 

        if (mode == NORMAL){
            if (show_search_results) {
                search_started = false;
                show_search_results = false;
                current_command_index = 0;
                memset(command, 0, COMMAND_ARR_SIZE);
                normal_mode_prefix = 0;
            }

            if (user_input == ':') {
                command_started = true;
                command_errored = false;
                insert_character_into_command(command, &current_command_index, &current_command_size, ':');
                continue;
            }

            if (user_input == '/') {
                search_started = true;
                insert_character_into_command(command, &current_command_index, &current_command_size, '/');
            }

            if (command_started || search_started) {
                if (user_input == '\n') {
                    if (search_started) {
                        if (!is_valid_regex(&command[1], current_command_size - 1)) {
                            memcpy(search_error_message + sizeof("Invalid Expression: ") - 1, command + 1, COMMAND_ARR_SIZE - 1);
                            current_command_index = 0;
                            memset(command, 0, COMMAND_ARR_SIZE);
                            search_started = false;
                            search_errorred = true;
                        }
                        else {
                            show_search_results = true;
                        }
                    }

                    else {
                        int result = evaluate_command(command);
                        switch (result) {
                            case EXIT_EDITOR:
                                cleanup_ncurses();
                                return 0;
                            case EXIT_EDITOR_WITHOUT_SAVE:
                                cleanup_ncurses();
                                return 1;
                            case EXIT_NORMAL_MODE:
                                mode = INSERT;
                                command_started = false;
                                current_command_index = 0;
                                command[current_command_index] = '\0';
                                normal_mode_prefix = 0;
                                break;
                            default:
                                memcpy(command_error_message + sizeof("Invalid Command: ") - 1, command + 1, COMMAND_ARR_SIZE - 1);
                                current_command_index = 0;
                                memset(command, 0, COMMAND_ARR_SIZE);
                                command_started = false;
                                command_errored = true;
                                break;
                        }
                        continue;
                    }
                }

                else if (user_entered_backspace && current_command_index <= 1) {
                    // can't remove the colon and can't go out of bounds
                    continue;
                }

                else if (user_entered_backspace){
                    current_command_index--;
                    memmove(
                        command + current_command_index, 
                        command + current_command_index + 1, 
                        COMMAND_ARR_SIZE - (current_command_index + 1)
                    );
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

                if (current_command_index >= COMMAND_ARR_SIZE - 1) {
                    const char * command_too_long_message = "Input cannot exceed 100 characters in length.";
                    size_t command_too_long_message_size = sizeof("Input cannot exceed 100 characters in length.");
                    memcpy(command_error_message + sizeof("Invalid Input: ") - 1, command_too_long_message, command_too_long_message_size);
                    current_command_index = 0;
                    memset(command, '\0', COMMAND_ARR_SIZE);
                    command_started = false;
                    command_errored = true;
                    continue;
                }

                if(!isalnum(user_input)){
                    // don't let the user type anything that is not explicity handled and is not a leter or digit
                    // we could restrict to just letters for the moment but maybe we will want numbers in the future 
                    
                    // can I make this throw an alert like vim does?
                    continue;
                }

                insert_character_into_command(command, &current_command_index, &current_command_size, user_input);

            }
            else {
                handle_cursor_movement(user_input, max_row, &cursor_row, &cursor_row_char_index, &cursor_row_text_object, &top_line, normal_mode_prefix);
            }
            normal_mode_prefix = user_input; 
        }

        else if (mode == INSERT) {
            if (user_input == 27) { // esc char
                mode = NORMAL;
                continue;
            }

            if (user_input == KEY_DOWN || user_input == KEY_UP || user_input == KEY_LEFT || user_input == KEY_RIGHT) {
                handle_cursor_movement(user_input, max_row, &cursor_row, &cursor_row_char_index, &cursor_row_text_object, &top_line, 0);
                continue;
            }

            if (user_input == '\n') {
                split_text_result new_pair = split_text(cursor_row_text_object, cursor_row_char_index);

                cursor_row_text_object = new_pair.second_blob;
                cursor_row_char_index = 0;
                cursor_row++;
                continue;
            }

            if (user_entered_backspace && cursor_row_char_index < 1 && cursor_row_text_object->previous == NULL) {
                continue;
            }

            if (user_entered_backspace && cursor_row_char_index < 1 && cursor_row_text_object->previous != NULL) {
                cursor_row_text_object = cursor_row_text_object->previous;
                cursor_row--;
                cursor_row_char_index = cursor_row_text_object->text_size;
                join_text(cursor_row_text_object, cursor_row_text_object->next);
                continue;
            }

            if (user_entered_backspace) {
                memmove(
                    cursor_row_text_object->text + cursor_row_char_index - 1, 
                    cursor_row_text_object->text + cursor_row_char_index, 
                    cursor_row_text_object->buffer_size - (cursor_row_char_index)
                );
                cursor_row_text_object->text[cursor_row_text_object->buffer_size - 1] = '\0';
                cursor_row_char_index--;
                cursor_row_text_object->text_size--;
                continue;
            }

            if (user_input == '\t') {
                // treating tabs as two spaces
                insert_character_into_document(cursor_row_text_object, &cursor_row_char_index, ' ');
                insert_character_into_document(cursor_row_text_object, &cursor_row_char_index, ' ');
                continue;
            }

            if (
                cursor_row_text_object->text_size >= .8 * cursor_row_text_object->buffer_size || 
                cursor_row_text_object->buffer_size - cursor_row_text_object->text_size <= 5 // resize early for small arrays 
            ) {
                size_t new_buffer_size = cursor_row_text_object->buffer_size * 2;
                char *copy = malloc(new_buffer_size);

                memcpy(copy, cursor_row_text_object->text, cursor_row_text_object->buffer_size);
                memset(copy + cursor_row_text_object->text_size, '\0', new_buffer_size - cursor_row_text_object->text_size);
                free(cursor_row_text_object->text);

                cursor_row_text_object->buffer_size = new_buffer_size;
                cursor_row_text_object->text = copy; 
            }

            insert_character_into_document(cursor_row_text_object, &cursor_row_char_index, user_input);
        }
    }
}

int main(int argc, char* argv[argc]){
    if (argc != 2) {
        printf("Wrong number of arguments. Please only pass the file path.\n");
        exit(1);
    }

    set_escdelay(25); // how long ncurses has to wait for more input to distinguish the esc char from esc sequences

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

        char* starter_string = malloc(10 * sizeof(char));
        memset(starter_string, '\0', 10);

        lines_of_text[0] = (text_blob){
            .text_size = 0, // newline not included in text size 
            .buffer_size = 10,
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