## Excercise 1

#### Compute the space needed to allocate a vector of doubles where there is 1 value for every living person and determine if allocating such a vector is feasible on your platform

On my system, a double is 8 bytes. There are roughly 8.3 billion living people. 

This array would require: $8 * 8.3B = 66.4B$ bytes or $66.4$ gigabytes. 

This is not feasible. 


## Excercise 2

#### Write implementations of the missing functions

They gave me all of the following, aside for the `circular_append`, `circular_pop`, and `circular_resize` functions. The book says to not write the `circular_resize` function in the text, so I have skipped that one for the moment. I adjust the guard conditions to make the code easier to read and adjusted the line in `circular_element` that I commented on. 

```c
#include <stdlib.h>

typedef struct circular circular;
struct circular {
    size_t start;
    size_t len;
    size_t cap;
    double* tab;
}

circular* circular_append(circular* c, double value) {
    // append a new element to the buffer c, return c if the new element could be appended and null otherwise
    circular * ret = NULL;
    if (!c) {
        return ret;
    }

    if (c->len == c->cap) {
        c = circular_resize(c, c->cap * 2);
    }

    size_t next_index = circular_getpos(c, c->len);
    c->tab[next_index] = value;
    c->len++;

    return c;
}

double circular_pop(circular* c) {
    // remove the oldest element from c and return its value (0.0 if there are no elements)
    double ret = 0.0;

    if (!c || c->len == 0) {
        return ret;
    }

    ret = c->tab[c->start];
    c->start = circular_getpos(c, c->start + 1);
    c->len--;

    return ret;
}

double* circular_element(circular const* c, size_t pos) {
    double* ret = NULL;
    if (!c) {
        return ret;
    }

    if (pos >= c->len) { // this originally compared pos to cap which I think is wrong 
        return ret;
    }

    pos = circular_getpos(c, pos);
    ret = &c->tab[pos];

    return ret;
}

circular* circular_init(circular* c, size_t cap) {
    if (!c) {
        return c;
    }

    if (cap) {
        *c = (circular){
            .cap = cap,
            .tab = malloc(sizeof(double[cap])),
        };

        if (!c->tab) {
            c->cap = 0;
        }
    } 
    else {
        *c = (circular){};
    }

    return c;
}

void circular_destroy(circular* c) {
    if (!c) {
        return;
    }

    free(c->tab);
    circular_init(c, 0);
}

circular* circular_new(size_t len) {
    return circular_init(malloc(sizeof(circular)), len);
}

void circular_delete(circular* c) {
    circular_destroy(c);
    free(c);
}

circular* circular_resize(circular* c, size_t cap) {
    // resize to capacity cap
}

size_t circular_getlength(circular const* c) {
    if (!c) {
        return 0;
    }

    return c->len;
}

static size_t circular_getpos(circular const c[static 1], size_t pos) {
    if (!c->cap) {
        return pos;
    }

    pos += c->start;
    pos %= c->cap;

    return pos;
}


int main(){
    printf("Double %lu\n", sizeof(double[5]));
}
```