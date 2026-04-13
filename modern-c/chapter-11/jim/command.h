enum {
    EXIT_EDITOR,
    EXIT_EDITOR_WITHOUT_SAVE,
    EXIT_NORMAL_MODE,
    INVALID_COMMAND
} COMMAND_RESULTS;

int evaluate_command(char command[static 1]);
void insert_character_into_command(char command[static 1], size_t* current_location, int new_char);