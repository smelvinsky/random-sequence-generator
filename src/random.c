//
// Created by smelvinsky on 22.10.17.
//

#include <stdlib.h>
#include "random.h"


void generate_uint8_t_random_seq(int seq_length, uint8_t *random_seq)
{
    for (int i = 0; i < seq_length; i++)
    {
        //TODO: replace rand()
        *(random_seq + i) = (uint8_t) (rand() % 256);
    }
}
