#ifndef CODE_H
#define CODE_H

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include "qr_table.h"
#include "image.h"

char* convert_to_bin( unsigned int num );

int convert_to_int( char* str );

char* convert_to_utf8( char *str ); //convert int to utf8 binary

int optimal_version ( int size ); //chosing optimal version

char*  add_service_inf(char *str, int* ver); //service information and addition string to full size

char** create_blocks( char *str, int version );

int** create_correction_block( char **mas, int version );

char* create_data ( char **blocks, int **cor_blocks, int version );

void add_finder_patterns ( char **pattern, int x, int y );

void add_alignment_patterns ( char **pattern, int version ); //add

void add_sync_line ( char **pattern, int size );

void add_code_version ( char** patterns, int version ); //add

void add_mask ( char **pattern, int size );

void add_data ( char **pattern, char *data, int version);

char** create_canvas_pattern ( char *data, int version ); //pattern for image

void qr_code_generation ( char *input, char *output ); //qr code generator

#endif