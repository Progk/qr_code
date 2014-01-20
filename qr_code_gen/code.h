#ifndef CODE_H
#define CODE_H

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include "qr_table.h"
#include "image.h"

char* convert_to_bin( unsigned int num, unsigned int bit ); //convert int to binary, bit is number of digit

int convert_to_int( char* str ); //convert binary code to int

char* convert_to_utf8( char *str ); //convert int to utf8 binary

int optimal_version ( int size ); //chosing optimal version

char*  add_service_inf(char *str, int* ver); //service information and addition string to full size

char** create_blocks( char *str, int version ); //create blocks of data

int** create_correction_block( char **mas, int version ); //create correction blocks

char* create_data ( char **blocks, int **cor_blocks, int version ); //create final data

void add_finder_patterns ( char **pattern, int x, int y ); //add finder patterns

void add_alignment_patterns ( char **pattern, int version ); //add alignment patterns

void add_sync_line ( char **pattern, int size ); //add lines of synchronized

void add_code_version ( char** patterns, int version ); //add version of qr code

void add_mask ( char **pattern, int size ); //add mask

void add_data ( char **pattern, char *data, int version); //add all data to pattern

char** create_canvas_pattern ( char *data, int version ); //pattern for image

void qr_code_generation ( char *input, char *output ); //qr code generator

#endif