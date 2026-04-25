#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "circular_buffer.h"

static size_t circular_getpos(circular const c[static 1], size_t pos) {
    if (!c->cap) {
        return pos;
    }

    pos += c->start;
    pos %= c->cap;

    return pos;
}

circular* circular_resize(circular* c, size_t nlen) {
    if (!c) {
        return NULL;
    }

    size_t olen = c->len;
    size_t ocap = c->cap;

    if(olen > nlen) {
        // we never have to get rid of elements when shrinking the array
        return NULL;
    }

    if (nlen == ocap) {
        return c;
    }

    size_t ostart = circular_getpos(c, 0);
    size_t nstart = ostart;
    //double* otab = c->tab;
    double* ntab = NULL;

    if (nlen > ocap) { 
        // there is a gap here
        ntab = realloc(c->tab, sizeof(double[nlen]));
        if (!ntab) {
            return NULL;
        }

        // non-empty, and there were already seperate chunks
        if (ostart + olen > ocap) {
            size_t upper_length = ocap - ostart;
            size_t lower_length = olen - upper_length;

            if (lower_length <= (nlen - ocap)) {
                // copy the lower elements after the end of the upper elements 
                memcpy(ntab + ocap, ntab, lower_length*sizeof(double));
            }
            else {
                // move the upper elements to the end of the list 
                nstart = nlen - upper_length;
                memmove(ntab + nstart, ntab + ostart, upper_length * (sizeof(double)));
            }
        }
    }

    else {
        // values just fit in new array 
        if (ostart + olen < nlen) {
            ntab = realloc(c->tab, sizeof(double[nlen]));
            if (!ntab) {
                return NULL;
            }
        }
        // there will be wrapping
        else {
            size_t final_upper_index = (ostart + olen >= ocap) ? ocap - 1 : ostart + olen;
            size_t indicies_to_shift = (final_upper_index + 1) - nlen;
            size_t upper_length = ocap - ostart;
            size_t lower_length = olen - upper_length;

            ntab = malloc(sizeof(double[nlen]));
            if (!ntab) {
                return NULL;
            }

            if (indicies_to_shift >= upper_length) {
                indicies_to_shift = upper_length; 
                nstart = 0;
            }

            memcpy(ntab, c->tab, nlen * sizeof(double));
            memcpy(ntab + indicies_to_shift, c->tab, (lower_length) * sizeof(double)); 
            memcpy(ntab, c->tab + (ocap - indicies_to_shift), (indicies_to_shift) * sizeof(double)); // this might be off by one
            free(c->tab); 
        }
    }

    *c = (circular){
        .cap = nlen,
        .start = nstart,
        .len = olen, 
        .tab = ntab
    };

    return c;
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

    if (pos >= c->len) {
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

size_t circular_getlength(circular const* c) {
    if (!c) {
        return 0;
    }

    return c->len;
}

void print_buffer_string_rep(circular* c) {
    printf("Buffer: ");

    if (c->start == 0) {
        for(size_t i = 0; i < c->len; i++){
            printf("%.1f ", c->tab[i]);
        }

        for(size_t i = c->len; i < c->cap; i++){
            printf("__ ");
        }
        printf("\n");
        return;
    }

    if (c->start + c->len < c->cap) {
        // there is no wrap
        for(size_t i = 0; i < c->start; i++){
            printf("__ ");
        }

        for(size_t i = c->start; i < c->start + c->len; i++){
            printf("%.1f ", c->tab[i]);
        }

        printf("\n");
        return;
    }

    // there is a wrap 
    size_t upper_length = c->cap - c->start;
    size_t lower_length = c->len - upper_length;

    for(size_t i = 0; i < lower_length; i++){
        printf("%.1f ", c->tab[i]);
    }

    for(size_t i = 0; i < c->cap - c->len; i++){
        printf("__ ");
    }

    for(size_t i = c->start; i < c->cap; i++){
        printf("%.1f ", c->tab[i]);
    }

    printf("\n");
    return;
}

void print_buffer_info(circular* c){
    printf("Circle Start: %zu\n", c->start);
    printf("Circle Len: %zu\n", c->len);
    printf("Circle Cap: %zu\n", c->cap);
    print_buffer_string_rep(c);
}