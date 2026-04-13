#include <stdbool.h>
#include <stdlib.h>
#include <curses.h> 
#include "text_blob.h"

void handle_cursor_movement(
    int input,
    int max_row,
    int* cursor_row,
    size_t* cursor_row_char_index,
    text_blob** cursor_row_text_object,
    text_blob** top_line,
    char prefix
) {
    if (input == 'H') { // move to top of screen 
        while (*cursor_row != 0 && (*cursor_row_text_object)->previous) {
            *cursor_row_text_object = (*cursor_row_text_object)->previous;
            *cursor_row -= 1;
        }
        *cursor_row_char_index = 0;
    }

    else if (input == 'M') { // move to middle of screen 
        while (*cursor_row < max_row / 2 && (*cursor_row_text_object)->next) {
            *cursor_row_text_object = (*cursor_row_text_object)->next;
            *cursor_row += 1;
        }

        while (*cursor_row > max_row / 2 && (*cursor_row_text_object)->previous) {
            *cursor_row_text_object = (*cursor_row_text_object)->previous;
            *cursor_row -= 1;
        }

        *cursor_row_char_index = 0;
    }

    else if (input == 'L') { // move to bottom of screen 
        while (*cursor_row_text_object && (*cursor_row_text_object)->next) {
            *cursor_row_text_object = (*cursor_row_text_object)->next;
            *cursor_row += 1;
        }
        *cursor_row_char_index = 0;
    }

    else if (input == '0') {
        *cursor_row_char_index = 0;
    }

    else if (input == 'w') {
        int current_char = (*cursor_row_text_object)->text[*cursor_row_char_index];

        while (current_char != ' ' && *cursor_row_char_index < (*cursor_row_text_object)->text_size) {
            (*cursor_row_char_index)++;
            current_char = (*cursor_row_text_object)->text[*cursor_row_char_index];
        }

        while (current_char == ' ' && *cursor_row_char_index < (*cursor_row_text_object)->text_size) {
            (*cursor_row_char_index)++;
            current_char = (*cursor_row_text_object)->text[*cursor_row_char_index];
        }
    }

    else if (input == 'e') {
        int current_char = (*cursor_row_text_object)->text[*cursor_row_char_index];

        bool user_is_in_word = current_char != ' ';
        bool user_is_at_word_end =
            *cursor_row_char_index != (*cursor_row_text_object)->text_size &&
            (*cursor_row_text_object)->text[*cursor_row_char_index + 1] == ' ';

        if (user_is_in_word && !user_is_at_word_end) {
            while (current_char != ' ' && *cursor_row_char_index < (*cursor_row_text_object)->text_size) {
                (*cursor_row_char_index)++;
                current_char = (*cursor_row_text_object)->text[*cursor_row_char_index];
            }

            if (*cursor_row_char_index == (*cursor_row_text_object)->text_size - 1) {
                return;
            }

            (*cursor_row_char_index)--;
            return;
        }

        if (user_is_in_word && user_is_at_word_end) {
            (*cursor_row_char_index)++;
        }

        current_char = (*cursor_row_text_object)->text[*cursor_row_char_index];

        while (current_char == ' ' && *cursor_row_char_index < (*cursor_row_text_object)->text_size) {
            (*cursor_row_char_index)++;
            current_char = (*cursor_row_text_object)->text[*cursor_row_char_index];
        }

        while (current_char != ' ' && *cursor_row_char_index < (*cursor_row_text_object)->text_size) {
            (*cursor_row_char_index)++;
            current_char = (*cursor_row_text_object)->text[*cursor_row_char_index];
        }

        if (*cursor_row_char_index == (*cursor_row_text_object)->text_size - 1) {
            return;
        }

        (*cursor_row_char_index)--;
    }

    else if (input == 'b') {
        int current_char = (*cursor_row_text_object)->text[*cursor_row_char_index];

        bool user_is_in_word = current_char != ' ';
        bool user_is_at_word_start =
            *cursor_row_char_index > 0 &&
            (*cursor_row_text_object)->text[*cursor_row_char_index - 1] == ' ';

        if (user_is_in_word && !user_is_at_word_start) {
            while (current_char != ' ' && *cursor_row_char_index > 0) {
                (*cursor_row_char_index)--;
                current_char = (*cursor_row_text_object)->text[*cursor_row_char_index];
            }

            if (*cursor_row_char_index == 0) {
                return;
            }

            (*cursor_row_char_index)++;
            return;
        }

        if (user_is_in_word && user_is_at_word_start) {
            (*cursor_row_char_index)--;
            current_char = (*cursor_row_text_object)->text[*cursor_row_char_index];
        }

        while (current_char == ' ' && *cursor_row_char_index > 0) {
            (*cursor_row_char_index)--;
            current_char = (*cursor_row_text_object)->text[*cursor_row_char_index];
        }

        while (current_char != ' ' && *cursor_row_char_index > 0) {
            (*cursor_row_char_index)--;
            current_char = (*cursor_row_text_object)->text[*cursor_row_char_index];
        }

        if (*cursor_row_char_index == 0) {
            return;
        }

        (*cursor_row_char_index)++;
    }

    else if (input == 'd' && prefix == 'd') { // delete current line
        text_blob* node = *cursor_row_text_object;

        bool is_first = node->previous == NULL;
        bool is_last  = node->next == NULL;

        if (is_first && is_last) {
            *top_line = NULL;
            *cursor_row_text_object = NULL;
            *cursor_row = 0;
        }
        else if (is_first) {
            *top_line = node->next;
            (*top_line)->previous = NULL;

            *cursor_row_text_object = *top_line;
            *cursor_row = 0;
        }
        else if (is_last) {
            node->previous->next = NULL;
            *cursor_row_text_object = node->previous;
            (*cursor_row)--;
        }
        else {
            node->previous->next = node->next;
            node->next->previous = node->previous;

            *cursor_row_text_object = node->previous;
            (*cursor_row)--;
        }

        prefix = 0;
    }

    else if (input == KEY_UP || input == 'k') {
        if (!(*cursor_row_text_object)->previous) return;

        if (*cursor_row != 0) {
            *cursor_row -= 1;
            *cursor_row_text_object = (*cursor_row_text_object)->previous;

            if (*cursor_row_char_index > (*cursor_row_text_object)->text_size) {
                *cursor_row_char_index = (*cursor_row_text_object)->text_size;
            }
        }
        else if (*top_line && (*top_line)->previous) {
            *top_line = (*top_line)->previous;
            *cursor_row_text_object = (*cursor_row_text_object)->previous;
        }
    }

    else if (input == KEY_DOWN || input == 'j') {
        if (!(*cursor_row_text_object)->next) return;

        if (*cursor_row != max_row) {
            *cursor_row += 1;
            *cursor_row_text_object = (*cursor_row_text_object)->next;

            if (*cursor_row_char_index > (*cursor_row_text_object)->text_size) {
                *cursor_row_char_index = (*cursor_row_text_object)->text_size;
            }
        }
        else if (*top_line && (*top_line)->next) {
            *top_line = (*top_line)->next;
            *cursor_row_text_object = (*cursor_row_text_object)->next;
        }
    }

    else if (input == KEY_LEFT || input == 'h') {
        if (*cursor_row_char_index != 0) {
            (*cursor_row_char_index)--;
        }
    }

    else if (input == KEY_RIGHT || input == 'l') {
        if (*cursor_row_char_index < (*cursor_row_text_object)->text_size) {
            (*cursor_row_char_index)++;
        }
    }
}