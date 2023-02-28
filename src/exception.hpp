#pragma once
#define RAISE_ERROR(x)                                 \
    {                                                  \
        printf("%s\n", x);                             \
        printf("%s:line %d: x\n", __FILE__, __LINE__); \
    }

#define EXCEPTION_TAIL