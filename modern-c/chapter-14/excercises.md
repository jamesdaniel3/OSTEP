## Excercise 1

#### Improve the `main` of the example such that it is able to cope with arbitrarily long input lines

This is the original main:

```c
int main (void) {
    char lbuf[256];
    for (;;) {
        if (fgetline(sizeof lbuf, lbuf, stdin)) {
            size_t n;
            size_t* nums = numberline(strlen(lbuf) + 1, lbuf, &n, 0);
            if (nums) {
                int ret = fprintnumbers(stdout, "%#zX", ",\t", n, nums);
                if (ret < 0) {
                    return EXIT_FAILURE;
                }
                free(nums);
            }
        }
        else {
            if (!lbuf[0]) {
                break;
            }
            for (;;) {
                int c = getc(stdin);
                if (c == EOF) {
                    return EXIT_FAILURE;
                }
                if (c == '\n') {
                    fprintf(stderr, "line too long: %s\n", lbuf);
                    break;
                }
            }

        }
    }
}
```

Here is an updated version that can take lines of any size:

```c
int main (void) {
    char* lbuf = malloc(256);
    for (;;) {
        if (fgetline(sizeof lbuf, lbuf, stdin)) {
            size_t n;
            size_t* nums = numberline(strlen(lbuf) + 1, lbuf, &n, 0);
            if (nums) {
                int ret = fprintnumbers(stdout, "%#zX", ",\t", n, nums);
                if (ret < 0) {
                    return EXIT_FAILURE;
                }
                free(nums);
            }
        }
        else {
            char* updated_lbuf = realloc(lbuf, sizeof(lbuf) * 2);
            if (!updated_lbuf) {
                return EXIT_FAILURE;
            }
            lbuf = updated_lbuf;

        }
    }
}
```
