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
int main(void) {
    char *lbuf = malloc(256);
    if (!lbuf) return EXIT_FAILURE;

    size_t buffer_size = 256;
    size_t len = 0;

    for (;;) {
        if (!fgets(lbuf + len, buffer_size - len, stdin)) {
            if (len == 0) break;        // true EOF
            // process final partial line
        }

        len += strlen(lbuf + len);

        char *pos = strchr(lbuf, '\n');
        if (pos) {
            *pos = '\0';

            size_t n;
            size_t *nums = numberline(len + 1, lbuf, &n, 0);
            if (nums) {
                int ret = fprintnumbers(stdout, "%#zX", ",\t", n, nums);
                free(nums);
                if (ret < 0) return EXIT_FAILURE;
            }

            len = 0;  // reset for next line
            continue;
        }

        // need more space
        if (len == buffer_size - 1) {
            size_t new_size = buffer_size * 2;
            char *tmp = realloc(lbuf, new_size);
            if (!tmp) return EXIT_FAILURE;

            lbuf = tmp;
            buffer_size = new_size;
        }
    }

    free(lbuf);
    return EXIT_SUCCESS;
}
```
