typedef struct text_blob text_blob;
struct text_blob {
    size_t text_size;
    size_t buffer_size;
    char* text;
    text_blob* next;
    text_blob* previous;
};

typedef struct split_text_result split_text_result;
struct split_text_result {
    text_blob* first_blob;
    text_blob* second_blob;
};

split_text_result split_text(text_blob* text_snippet, size_t split_location);
text_blob join_text(text_blob first_snippet, text_blob second_snippet);