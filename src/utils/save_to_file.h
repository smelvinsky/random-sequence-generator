/**
 * Created by smelvinsky on 18.11.17.
 */

#ifndef SAVE_TO_FILE_H
#define SAVE_TO_FILE_H

FILE *create_and_open_file(char *filename);
void write_to_file(const void *data_ptr, FILE *stream);
void close_file(FILE *stream);

char *generate_file_name(void);

#endif //SAVE_TO_FILE_H
