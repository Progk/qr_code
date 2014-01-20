#ifndef IMAGE_H
#define IMAGE_H

#define _CRT_SECURE_NO_WARNINGS

#define PIXEL_PER_MODUL 100

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include "qr_table.h"

typedef unsigned short WORD; 
typedef unsigned long DWORD; 

//header of bmp file
#pragma pack(push, 2)
typedef struct 
{
	WORD signature; //BM
	DWORD size; 
	WORD free1;
	WORD free2;
	DWORD image_data; 

	DWORD header_length;
	DWORD image_width;
	DWORD image_height;
	WORD image_planes; //color planes
	WORD bit_per_pixel; 
	DWORD image_compression;
	DWORD image_data_length;
	DWORD hor_res;
	DWORD vert_res;
	DWORD image_number_colors;
	DWORD image_important_colors;
}bmp_file_header;
#pragma pack(pop)


void create_header ( bmp_file_header *bmp, int version ); //create hader

void create_bmp ( char **pattern, char *name, int version ); //create bmp file 



#endif