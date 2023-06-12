#include <ctype.h>

int isalnum(int c) {
    return isalpha(c) || isdigit(c);
}

int isalpha(int c) {
    return ((static_cast<unsigned>(c) | 32) - 'a') < 26;
}

int isascii(int c) {
    return !(c & ~0x7F);
}

int isblank(int c) {
    return (c == ' ' || c == '\t');
}

int iscntrl(int c) {
    return (static_cast<unsigned>(c) < 0x20) || (c == 0x7F);
}

int isdigit(int c) {
    return static_cast<unsigned>(c) - '0' < 10;
}

int isgraph(int c) {
    return static_cast<unsigned>(c) - 0x21 < 0x5E;
}

int islower(int c) {
    return static_cast<unsigned>(c) - 'a' < 26;
}

int isprint(int c) {
    return static_cast<unsigned>(c) - 0x20 < 0x5F;
}

int ispunct(int c) {
    return isgraph(c) && !isalnum(c);
}

int isspace(int c) {
    return (c == ' ') || (static_cast<unsigned>(c) - '\t' < 5);
}

int isupper(int c) {
    return static_cast<unsigned>(c) - 'A' < 26;
}

int isxdigit(int c) {
    return isdigit(c) || ((static_cast<unsigned>(c) | 32) - 'a' < 6);
}

int toascii(int c) {
    return c & 0x7F;
}

int tolower(int c) {
    if (isupper(c)) {
        return c | 32;
    }

    return c;
}

int toupper(int c) {
    if (islower(c)) {
        return c & 0x5F;
    }

    return c;
}