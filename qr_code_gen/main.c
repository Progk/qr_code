#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include "qr_table.h"
#include "image.h"
#include "code.h"


int main()
{
	int i;
	int j;
	int version;
	char *str_source = "QR codes must include function patterns. These are shapes that must be placed in specific areas of the QR code"; //for now only 14 symbols works properly
	//char *str_source = "hello";
	char *str_source_bin;
	char **blocks;
	int **correction_blocks;
	char **pattern;
	char *data;


	//testing information
	str_source_bin = convert_to_utf8( str_source ); //convert string to utf8 ascii
	printf("source: \n%s\n\n", str_source_bin );
	version = optimal_version( strlen( str_source_bin ) ); //optimal version
	str_source_bin = add_service_inf(str_source_bin, &version); //add service information into string
	blocks = create_blocks( str_source_bin, version ); //create blocks
	correction_blocks = create_correction_block( blocks, version ); //create correction blocks
	printf("\nversion: %d", version);
	printf("\nsize: %d", strlen(str_source_bin));
	printf("\nnumber of blocks: %d", number_of_blocks[version]);
	printf("\n\nbin full: \n%s\n", str_source_bin);
	printf("\nblocks:");
	//pattern = create_canvas_pattern ( blocks, correction_blocks, version );
	for ( i = 0; i < number_of_blocks[version]; i++) //output blocks
			printf ("\nn:%d size:%d\n%s ", i, strlen(blocks[i]), blocks[i]);
	printf("\n\ncorrection blocks:");
	for ( i = 0; i < number_of_blocks[version]; i++) //output corrections blocks
	{
		printf ("\ncor_n:%d size:%d\n", i, correction_blocks[i][0] );
			for ( j = 1; j <= correction_blocks[i][0]; j++)
				printf( "%d ", correction_blocks[i][j] );
	}
	printf("\n\n");
	data = create_data ( blocks, correction_blocks, version );
	pattern = create_canvas_pattern ( data, version );
	create_bmp( pattern, "my", version );
	
	
	getchar();

}