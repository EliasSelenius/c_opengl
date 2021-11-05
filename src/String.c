#include "String.h"

String* stringCreate(const char* text) {
    int len = strlen(text);
    String* res = malloc(sizeof(String) + sizeof(char) * len);
    res->length = len;
    for (int i = 0; i < len; i++) { res->characters[i] = text[i]; }

    return res;
}