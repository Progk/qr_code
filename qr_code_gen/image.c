#include "image.h"

void create_header ( bmp_file_header *bmp, int version )
{
	DWORD size;
	int offset = 0;
	size = ( ( ( version - 1 ) * 4 ) + 21 );
	size+=8; //white border
	size*=PIXEL_PER_MODUL;
	size*=3;

	if ( size%4 != 0 )
		offset = size%4;

	bmp->signature = 0x4D42;
	bmp->size = ( size + offset ) * (size/3) + 54;
	bmp->free1 = 0;
	bmp->free2 = 0;
	bmp->image_data = 54;

	bmp->header_length = 40;
	bmp->image_width = ( ( ( ( version - 1 ) * 4 ) + 21 ) + 8) * PIXEL_PER_MODUL;
	bmp->image_height = bmp->image_width;
	bmp->image_planes = 1;
	bmp->bit_per_pixel = 24;
	bmp->image_compression = 0;
	bmp->image_data_length = bmp->size - 54;
	bmp->hor_res = 0;
	bmp->vert_res = 0;
	bmp->image_number_colors = 0;
	bmp->image_important_colors = 0;
}

void create_bmp ( char **pattern, char *name, int version )
{
	FILE *file;
	bmp_file_header bmp;
	char *data;
	char *buffer;
	char *black;
	char *white;
	char *output;
	char end[5] = "";
	int number; //number elements in column
	int size; //size string in bmp image
	int i;
	int j;
	int k;
	int var = 0;
	int var2 = 0;

	number = ( ( ( version - 1 ) * 4 ) + 21 );
	number+=8; //white border

	size = ( ( ( version - 1 ) * 4 ) + 21 );
	size+=8; //white border
	size*=PIXEL_PER_MODUL;
	size*=3;

	if ( size%4 != 0 )
		size += size%4;

	create_header ( &bmp, version ); //create header
	
	black = ( char* )calloc( 3, sizeof(char) );
	white =( char* )calloc( 3, sizeof(char) );
	memset( white, 255, 3 );
	data = ( char* )calloc( bmp.image_data_length, sizeof(char) );
	buffer = ( char* )calloc( size, sizeof(char) );
	
	var2 = bmp.image_data_length;
	var2-=size;

	for ( i = 0; i < number; i++ )
	{
		var = 0;
		for ( j = 0; j < number; j++ )
		{
			if ( pattern[i][j] == '0' )
			{
				for ( k = 0; k < PIXEL_PER_MODUL; k++ )
				{
				memcpy ( &buffer[var], white, 3 );
				var+=3;
				}
			}
			else
			{
				for ( k = 0; k < PIXEL_PER_MODUL; k++ )
				{
				memcpy ( &buffer[var], black, 3 );
				var+=3;
				}
			}
		}
		for ( k = 0; k < PIXEL_PER_MODUL; k++ )
		{
			memcpy ( &data[var2], buffer, size ); //add to end of array
			var2-=size;
		}
	}

	output = name;
	if ( strlen( name ) >= 4 )
		memcpy ( end, &name[strlen( name ) - 4], 4 );
	if ( strcmp( end, ".bmp" ) != 0 )
	{
		output = ( char* )calloc( strlen( name ) + 4, sizeof ( char ) );
		strcat( output, name );
		strcat( output, ".bmp" );
	}
		
	file = fopen( output, "wb" );
	fwrite( &bmp, sizeof(bmp), 1, file );
	fwrite( data, bmp.image_data_length, 1, file );
	fclose(file);

	free ( black );
	free ( white );
	free ( buffer );
	free ( data );
}
