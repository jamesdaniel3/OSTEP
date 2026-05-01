typedef struct match_position_info match_position_info;
struct match_position_info {
    size_t length;
    size_t starting_index;
};

typedef struct regex_match regex_match;
struct regex_match {
    size_t length;
    char* text;
};

bool is_valid_regex(char const* regex, size_t regex_size);

// assumes it has been given a valid regex expression
match_position_info get_regex_match(
    char const* text_to_search, size_t text_size, 
    char const* regex, size_t regex_size,
    size_t current_result_start, size_t current_text_index
);


typedef struct regex_char_range regex_char_range;
struct regex_char_range {
    char min_accetable_char;
    char max_accetable_char;
    bool is_alphabetical;
    bool is_case_sensative;
};

/*
    This function could be moved to an internal only header because they really should be private but since this isn't
    real prod code I am juse going to include them here. 
*/
regex_char_range get_next_acceptable_chars(char const* regex);