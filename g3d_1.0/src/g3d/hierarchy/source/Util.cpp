#include "Util.h"

unsigned int G3DHashString(const char* string, unsigned int hash)
{
    int c;
    while ((c = *string++))
    {
        c = (c < 'A' || c > 'Z') ? c : (c - 'A' + 'a'); // Ignore case!
        hash = ((hash << 5) + hash) + c;                // hash*33 + c
    }
    return hash;
}