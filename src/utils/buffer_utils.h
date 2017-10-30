//
// Created by smelvinsky on 30.10.17.
//

#ifndef BUFFER_UTILS_H
#define BUFFER_UTILS_H

#include <stddef.h>

typedef struct buffer_t
{
    /** actual buffer pointer */
    void *buff;

    /** buffer size */
    size_t size;

} buffer;

#endif //BUFFER_UTILS_H
