#include <string.h>
#include "command.h"

int evaluate_command(char command[static 1]) {
    if (strcmp(":x", command) == 0) {
        return EXIT_EDITOR;
    }
    if (strcmp(":q", command) == 0) {
        return EXIT_EDITOR_WITHOUT_SAVE;
    }
    if (strcmp(":i", command) == 0) {
        return EXIT_NORMAL_MODE;
    }

    return INVALID_COMMAND;
}

void insert_character_into_command(char command[static 1], size_t* current_location, int new_char){
    command[*current_location] = new_char;
    *current_location += 1;
}