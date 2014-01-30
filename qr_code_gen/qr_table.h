#ifndef QR_TABLE_H
#define QR_TABLE_H

int size_of_information[41];

int number_of_blocks[41];

int number_of_correction_byte[41];

int polynomials[31][30];

int field_values[256];

int inverse_field_values[256];

int alignment_patterns[41][8];

char *code_version[41];

#endif