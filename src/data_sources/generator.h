//
// Created by smelvinsky on 30.10.17.
//

#ifndef GENERATOR_H
#define GENERATOR_H

#include "../utils/buffer_utils.h"

/** initializes hardware of all generator sources */
void gen_hw_init(buffer *buff_1, buffer *buff_2, buffer *buff_3, buffer *buff_4);

/** deinitializes hardware of all generator sources */
void gen_hw_close(buffer *buff_1, buffer *buff_2, buffer *buff_3, buffer *buff_4);

/** reads from source 1 */
int gen_read_from_source1(buffer *buff_1);

/** reads from source 2 */
int gen_read_from_source2(buffer *buff_2);

#endif //GENERATOR_H
