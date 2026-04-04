#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <curses.h> // In linking with curses you need to have -lncurses in your LDFLAGS or on the command line

#define FILEPATH "smaller_text.txt"

// https://invisible-island.net/ncurses/ncurses-intro.html

typedef struct text_blob text_blob;
struct text_blob {
    size_t size;
    char* text;
    text_blob* next;
    text_blob* previous;
};

typedef struct split_text_result split_text_result;
struct split_text_result {
    text_blob first_blob;
    text_blob second_blob;
};

split_text_result split_text(text_blob text_snippet, size_t split_location){
    size_t left_split_size = split_location + 1;
    size_t right_split_size = text_snippet.size - split_location + 1;

    char *first_text = malloc((left_split_size) * sizeof(char));
    char *second_text = malloc((right_split_size) * sizeof(char));

    // copy from [0, location) like splitting does in python
    memcpy(first_text, text_snippet.text, left_split_size); 
    first_text[left_split_size - 1] = '\0';

    memcpy(second_text, text_snippet.text + split_location, right_split_size); 
    second_text[right_split_size - 1] = '\0';

    free(text_snippet.text);

    text_blob left_blob = {
        .size = left_split_size,
        .text = first_text,
        .previous = text_snippet.previous
    };

    text_blob right_blob = {
        .size = right_split_size,
        .text = second_text,
        .next = text_snippet.next
    };

    left_blob.next = &right_blob;
    right_blob.previous = &left_blob;

    split_text_result result = {
        .first_blob = left_blob,
        .second_blob = right_blob
    };

    return result;
}

text_blob join_text(text_blob first_snippet, text_blob second_snippet) {
    char *combined_buffer = malloc((first_snippet.size + second_snippet.size - 1) * sizeof(char));

    memcpy(combined_buffer, first_snippet.text, first_snippet.size - 1); 
    memcpy(combined_buffer + first_snippet.size - 1, second_snippet.text, second_snippet.size); 

    text_blob result = {
        .size = first_snippet.size + second_snippet.size - 1,
        .text = combined_buffer,
        .previous = first_snippet.previous,
        .next = second_snippet.next
    };

    free(first_snippet.text);
    free(second_snippet.text);

    return result;
}

text_blob* parse_file(const char* file_path, size_t num_lines, text_blob result[static 1]){
    size_t current_blob_index = 0;
    FILE * fstream = fopen(file_path, "r");

    if (!fstream) {
        perror("fopen");
        exit(1);
    }

    char * next_line = NULL;
    size_t capacity = 0;
    ssize_t line_length;

    while ((line_length = getline(&next_line, &capacity, fstream)) != -1) {
        char *copy = malloc(line_length + 1);
        memcpy(copy, next_line, line_length + 1);

        result[current_blob_index] = (text_blob){
            .size = line_length,
            .text = copy,
            .next = NULL,
            .previous = (current_blob_index == 0 ? NULL : &result[current_blob_index - 1])
        };

        if (current_blob_index != 0){
            result[current_blob_index - 1].next = &result[current_blob_index];
        }

        current_blob_index++;
    }

    free(next_line);
    fclose(fstream);
    return result;
}

void print_within_screen(text_blob current_text_object[static 1]) {
    initscr();
    noecho(); // don't echo user input
    cbreak(); // read after each character, don't wat for a newline 
    keypad(stdscr, TRUE); // necessary to allow for reading in inputs like arrow keys
    use_default_colors(); // use the default colors that are being used in the current terminal

    int max_row, max_col;
    getmaxyx(stdscr, max_row, max_col);

    text_blob* top_line = current_text_object;
    int cursor_row = 0;

    while (true) {
        erase();

        text_blob* iterator = top_line;
        int current_row = 0;
        while (iterator != NULL && current_row < max_row) {
            mvprintw(current_row, 0, "%s", iterator->text);

            // Ensure cursor stays at the start of the line
            move(current_row, 0);

            iterator = iterator->next;
            current_row++;
        }

        move(cursor_row, 0); // leave the cursor at the top after initial write 
        refresh();

        int user_input = getch();

        if (user_input == KEY_UP) {
            if (cursor_row != 0) {
                cursor_row -= 1;
            }
            else if (top_line != NULL && top_line->previous != NULL) {
                top_line = top_line->previous;
            }
        }

        else if (user_input == KEY_DOWN) {
            if (cursor_row != max_row) {
                cursor_row += 1;
            }
            else if (top_line != NULL && top_line->next != NULL) {
                top_line = top_line->next;
            }
        }
    }
    endwin();
}

int main(){
    FILE * fstream = fopen(FILEPATH, "r");

    if (!fstream) {
        perror("fopen");
        return 1;
    }

    char * next_line = NULL;
    size_t capacity = 0;
    ssize_t line_length;

    size_t num_lines = 0;
    while ((line_length = getline(&next_line, &capacity, fstream)) != -1) {
        num_lines += 1;
    }
    fclose(fstream);
    text_blob result[num_lines];
    parse_file(FILEPATH, num_lines, result);

    print_within_screen(result);
    endwin();
}