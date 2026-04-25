typedef struct circular circular;
struct circular {
    size_t start;
    size_t len;
    size_t cap;
    double* tab;
};

static size_t circular_getpos(circular const c[static 1], size_t pos);

circular* circular_resize(circular* c, size_t nlen);

circular* circular_append(circular* c, double value);

double circular_pop(circular* c);

double* circular_element(circular const* c, size_t pos);

circular* circular_new(size_t len);

void circular_delete(circular* c);

size_t circular_getlength(circular const* c);

void print_buffer_info(circular* c);
