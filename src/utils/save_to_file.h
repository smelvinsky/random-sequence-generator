/**
 *  Created by smelvinsky on 18.11.17.
 */

#ifndef SAVE_TO_FILE_H
#define SAVE_TO_FILE_H

FILE *create_and_open_file(char *filename);
void close_file(FILE *stream);

void generate_file_name(char **file_name);

#endif //SAVE_TO_FILE_H
