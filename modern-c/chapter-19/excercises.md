## Excercise 1

#### Change `descend` so that it receives an `unsigned` depth instead of a pointer

Here is the original code for the `descend` function: 

```c
static char const* descend(
    char const* act,
    unsigned dp[restrict static 1],
    size_t len, char buffer[static len],
    jmp_buf jmpTarget) {

    if (dp[0] + 3 > sizeof(head)) {
        longjmp(jmpTarget, tooDeep);
    }
    ++dp[0];

NEW_LINE: 
    while (!act || !act[0]) {
        if (interrupt) {
            longjmp(jmpTarget, interrupted);
        }
        act = skipspace(fgets(buffer, len, stdin));

        if (!act) {
            if (dp[0] != 1) {
                longjmp(jmpTarget, plusL);
            }
            else {
                goto ASCEND;
            }
        }
    }

    fputs(&head[sizeof(head) - (dp[0] + 2)], stdout);

    for (; act && act[0]; ++act) {
        switch (act[0]){
        case left:
            act = end_line(act + 1, jmpTarget);
            act = descend(act, dp, len, buffer, jmpTarget);
            act = end_line(act + 1, jmpTarget);
            goto NEW_LINE;
        case right:
            if (dp[0] == 1) {
                longjmp(jmpTarget, plusR);
            }
            else {
                goto ASCEND;
            }
        default:
            putchar(act[0]);
        }
    }
    goto NEW_LINE;
ASCEND:
    --dp[0];
    return act;
}
```

Here is the updated code:

```c
static char const* descend(
    char const* act,
    unsigned dp,
    size_t len, char buffer[static len],
    jmp_buf jmpTarget) {

    if (dp + 3 > sizeof(head)) {
        longjmp(jmpTarget, tooDeep);
    }
    ++dp;

NEW_LINE: 
    while (!act || !act[0]) {
        if (interrupt) {
            longjmp(jmpTarget, interrupted);
        }
        act = skipspace(fgets(buffer, len, stdin));

        if (!act) {
            if (dp != 1) {
                longjmp(jmpTarget, plusL);
            }
            else {
                goto ASCEND;
            }
        }
    }

    fputs(&head[sizeof(head) - (dp + 2)], stdout);

    for (; act && act[0]; ++act) {
        switch (act[0]){
        case left:
            act = end_line(act + 1, jmpTarget);
            act = descend(act, dp, len, buffer, jmpTarget);
            act = end_line(act + 1, jmpTarget);
            goto NEW_LINE;
        case right:
            if (dp == 1) {
                longjmp(jmpTarget, plusR);
            }
            else {
                goto ASCEND;
            }
        default:
            putchar(act[0]);
        }
    }
    goto NEW_LINE;
ASCEND:
    --dp;
    return act;
}
```

## Excercise 2

#### Compare the assembler output of the inital version against your version without `dp` pointer

This can't be done at the point that I found it. This code does not compile. I will try to return to this activity once I have more context and the code hopefully compiles 

## Excercise 3

#### Your version of `descend` that passes `depth` as a value might not propogate the depth correctly if it encounters the `plusL` condition. Ensure that it copies that value to an object that can be used by the `fprintf` call in `basic_blocks`

Here is `basic_blocks`:

```c
void basic_blocks(void) {
    char buffer[maxline];
    unsigned depth = 0;
    char const* format = "All matching %0.0d’%c’ ’%c’ pairs have been closed correctly\n";
    jmp_buf jmpTarget;
    switch (setjmp(jmpTarget)) {
        case 0:
            descend(nullptr, &depth, maxline, buffer, jmpTarget);
            break;
        case plusL:
            format = "Warning: %d ’%c’ have not been closed properly (expected ’%c’)\n";
            break;
        case plusR:
            format ="Error: closing too many (%d) ’%c’ parenthesis with additional ’%c’\n";
            break;
        case tooDeep:
            format ="Error: nesting (%d) of ’%c’ ’%c’ constructs is too deep\n";
            break;
        case eofOut:
            format ="Error: EOF for stdout at %d open ’%c’, expecting same amount of ’%c’\n";
            break;
        case interrupted:
            format =
            "Interrupted at level %d of ’%c’ ’%c’ nesting\n";
            break;
        default:;
            format = "Error: unknown error within (%d) ’%c’ ’%c’ constructs\n";
            fflush(stdout);
            fprintf(stderr, format, depth, left, right);
    }
    if (interrupt) {
        SH_PRINT(stderr, interrupt, "is somebody trying to kill us?");
        raise(interrupt);
    }
}
```

The chapter made the following points:
- The descend function always resets the depth variable to 0 before it returns, so the compiler can look at that and always treat outside references to depth as 0 (or its original value)
- This is because the compiler does not recognize the fact that we could hit a longjmp condition and leave the function wihout resetting it 
- We need to make the value volatile. Colatile objects are reloaded from memory each time they are accessed and stored each time they are modified

Here is the modified function using a volatile depth:

```c
static char const* descend(
    char const* act,
    unsigned volatile dp[restrict static 1],
    size_t len, char buffer[static len],
    jmp_buf jmpTarget
){

    if (dp[0] + 3 > sizeof(head)) {
        longjmp(jmpTarget, tooDeep);
    }
    ++dp[0];

NEW_LINE: 
    while (!act || !act[0]) {
        if (interrupt) {
            longjmp(jmpTarget, interrupted);
        }
        act = skipspace(fgets(buffer, len, stdin));

        if (!act) {
            if (dp[0] != 1) {
                longjmp(jmpTarget, plusL);
            }
            else {
                goto ASCEND;
            }
        }
    }

    fputs(&head[sizeof(head) - (dp[0] + 2)], stdout);

    for (; act && act[0]; ++act) {
        switch (act[0]){
        case left:
            act = end_line(act + 1, jmpTarget);
            act = descend(act, dp, len, buffer, jmpTarget);
            act = end_line(act + 1, jmpTarget);
            goto NEW_LINE;
        case right:
            if (dp[0] == 1) {
                longjmp(jmpTarget, plusR);
            }
            else {
                goto ASCEND;
            }
        default:
            putchar(act[0]);
        }
    }
    goto NEW_LINE;
ASCEND:
    --dp[0];
    return act;
}
```

## Excercise 4

#### Use the C23 `typeof` feature for a `typedef` of `jump_buf_base`

Some context as to what `jump_buf_base` is supposed to be:

> The `typedef` for `jump_buf` hides an array type. Because it as an opaque type, we don't know anything about the base type of the array, say `jump_buf_base`:
> - An object of type `jump_buf` cannot be assigned to
> - A  `jump_buf` function parameter is reqritten to a pointer to `jump_buf_base`
> - Such a function always refers to the orgiinal object and not to a copy

I think the solution this is looking for is something like the following:

```c
jmp_buf jmpTarget;
typedef typeof(jmpTarget[0]) jump_buf_base;
```