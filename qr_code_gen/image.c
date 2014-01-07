#include "image.h"

void create_header ( bmp_file_header *bmp, int version )
{
	DWORD size;

	bmp->signature = 0x4D42;
	bmp->size = size_of_information[version]/8 + number_of_correction_byte[version];
	bmp->free1 = 0;
	bmp->free2 = 0;
	bmp->image_data = 54;


	bmp->header_length = 40;
	bmp->image_width = ( ( ( ( version - 1 ) * 4 ) + 21 ) + 8) * PIXEL_PER_MODUL/3; //test
	bmp->image_height = bmp->image_width;
	bmp->image_planes = 1;
	bmp->bit_per_pixel = 1;
	bmp->image_compression = 0;
	bmp->image_data_length = 841; //test
	bmp->hor_res = bmp->image_width;
	bmp->vert_res = bmp->image_width;
	bmp->image_number_colors = 0; //test
	bmp->image_important_colors = 0; //test
}

void create_bmp ( char **pattern, int version )
{
	FILE *f;
	bmp_file_header bmp;
	int size;
	int i;
	int j;
	create_header ( &bmp, version );
	f = fopen( "test.bmp", "wb" );
	fwrite( &bmp, sizeof(bmp), 1, f);
	for ( i = 0; i < number_of_correction_byte[version]; i++ )
	{
		for ( j = 0;j < strlen( pattern[i] ); j++ )
		{
			if ( pattern[i][j] == '0' )
				pattern[i][j] = 255;
			else
				pattern[i][j] = 0;
		}
	}
	for ( i = 0; i < number_of_correction_byte[version]; i++ )
	{
		if ( strlen( pattern[i] )%4 != 0)
			fwrite ( pattern[i], strlen( pattern[i] ) + strlen( pattern[i] )%4, 1, f );
		else
			fwrite ( pattern[i], strlen( pattern[i] ), 1, f );
	}
	fwrite( &bmp, sizeof(bmp), 1, f);
	fclose(f);
}
