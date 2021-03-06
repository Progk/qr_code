#include "code.h"

char* convert_to_bin( unsigned int num, unsigned int bit ) //convert int to binary, bit is number of digit
{
	char *str_bin;

	str_bin = ( char* )calloc( bit + 1, sizeof( char ) ); //bin str 0-7 -  bit
	
	if ( str_bin == NULL )
	{
		fprintf( stderr, "Can't allocate memory\n" );
		exit(1);
	}

	memset( str_bin, ( char )( ( int )'0' ), bit ); //initializing array
	str_bin[bit] = '\0';

	bit--;
	while ( num > 0 )
	{
		str_bin[bit] += num%2;
		bit--;
		num/=2;
	}

	return str_bin;
}


int convert_to_int( char* str )
{
	int sum = 0;
	int i;

	for ( i = 0; i < 8; i++)
		sum+= ( int )( ( str[7-i] - 48 ) * pow( 2., i ) );

	return sum;

}

char* convert_to_utf8( char *str ) //convert int to utf8 binary
{
	int i;
	char *str_bin;
	int length;

	str_bin = ( char* )calloc( strlen( str ) * 8 + 1, sizeof( char ) ); //for bin str

	if ( str_bin == NULL )
	{
		fprintf( stderr, "Can't allocate memory\n" );
		exit(1);
	}

	length = strlen( str );
	for ( i = 0; i < length; i++ )
		memcpy( str_bin + i * 8, convert_to_bin( str[i], 8 ), 8 * sizeof( char ) ); //copy binary code

	str_bin[strlen( str ) * 8] = '\0'; //end of string

	return str_bin;
}


int optimal_version ( int size ) //chosing optimal version
{
	int i;
	int version;
	int buffer = 0;

	buffer = ( int )fabs( ( float )( size - size_of_information[1] ) );
	version = 1;

	for ( i = 2; i <= sizeof( size_of_information ); i++ )
		if (fabs( ( float )( size - size_of_information[i] ) ) < buffer )
		{
			buffer = ( int )fabs( ( float )( size - size_of_information[i] ) );
			version = i;
		}
		else
		{
			break;
		}

		return version;
}


char*  add_service_inf(char *str, int* ver) //service information and addition string to full size
{
	int data = 4; //service data (bit) = coding method(4 bit) + size of data(8 or 16 bit) 
	int zero;
	int extra = 0; //extra byte
	int buffer;
	int length;
	int i;
	char *str_full;
	char *zero_str;

	if ( *ver <= 9 ) //length of data field
		data+=8;
	else 
		data+=16;
	length = strlen( str );
	if ( ( length + data ) > ( size_of_information[*ver] ) ) //if big size
	{
		( *ver )++;
		data = 4;
		if ( *ver <= 9 ) //length of data field again, because version changed
			data+=8;
		else 
			data+=16;
	}

	zero = (strlen( str ) + data )%8; //number of zero until str%8 != 0
	zero_str = ( char* )calloc( zero + 1, sizeof( char ) );
	if ( zero_str == NULL )
	{
		fprintf( stderr, "Can't allocate memory\n" );
		exit(1);
	}
	zero_str[zero] = '\0';
	memset( zero_str, ( char )((int)'0'), zero ); //initializing array

	buffer = ( strlen( str ) + data + zero - size_of_information[*ver] );
	if ( buffer < 0 )
		extra = -buffer/8; //number of extra byte

	str_full = ( char* )calloc( strlen( str ) + data + zero + extra*8 + 1, sizeof(char) ); //new str full
	if ( str_full == NULL )
	{
		fprintf( stderr, "Can't allocate memory\n" );
		exit(1);
	}
	str_full[strlen( str ) + data + zero + extra*8] = '\0';
	strcpy(str_full, "0100"); //add coding method
	strcpy(&str_full[4], convert_to_bin(strlen( str ) / 8, data - 4 )); //add size of data
	strcpy(&str_full[data], str); //add data(bit)
	strcpy(&str_full[data + strlen(str)], zero_str); //add zero

	for (i = 0; i < extra; i++) //add extra byte
	{
		if (i%2 == 0)
			strcpy(&str_full[data + strlen(str) + zero + i*8], "11101100");
		else
			strcpy(&str_full[data + strlen(str) + zero + i*8], "00010001");
	}

	return str_full;
}


char** create_blocks( char *str, int version )
{
	char **mas; //array of blocks
	int i;
	int size; //size of block
	int extra_size; //extra bit
	int number = number_of_blocks[version]; //number of blocks
	int element = 0; //pointer to starting copy of string

	size = ( size_of_information[version] / 8 ) / number; //number byte in block
	extra_size = ( size_of_information[version] / 8 ) % number; //extra byte
	mas = ( char** )calloc( number, sizeof(char*) );
	
	if ( mas == NULL )
	{
		fprintf( stderr, "Can't allocate memory\n" );
		exit(1);
	}

	for ( i = 0; i < number; i++ )
	{
		if ( ( i + extra_size ) != number )
		{
			mas[i] = ( char* )calloc( size * 8 + 1, sizeof(char) ); //without extra size
			if ( mas[i] == NULL )
				{
					fprintf( stderr, "Can't allocate memory\n" );
					exit(1);
				}
			memcpy( mas[i], &str[element], size * 8); 
			mas[i][size * 8] = '\0';
			element+=( size * 8 );
		}
		else
		{
			mas[i] = ( char* )calloc( (size + 1) * 8 + 1, sizeof(char) ); //with extra size
			if ( mas[i] == NULL )
				{
					fprintf( stderr, "Can't allocate memory\n" );
					exit(1);
				}
			memcpy( mas[i], &str[element], (size + 1) * 8); 
			mas[i][(size + 1) * 8] = '\0';
			element+=(size + 1) * 8;
			extra_size--;
		}
	}

	return mas;
}


int** create_correction_block( char **mas, int version ) //fix
{
	int i;
	int j;
	int k;
	int element; //first element array
	int number; //number byte in blocks
	int max;
	int var;
	int** correction_blocks; //corrections blocks
	int correction; //number of correction byte
	char bin[8]; //array of byte

	correction_blocks = ( int** )calloc( number_of_blocks[version], sizeof(int*) );
	if ( correction_blocks == NULL )
	{
		fprintf( stderr, "Can't allocate memory\n" );
		exit(1);
	}
	correction = number_of_correction_byte[version];

	for ( i = 0; i < number_of_blocks[version]; i++ )
	{
		number = strlen( mas[i] ) / 8;
		if ( number > correction)
			max = number;
		else
			max = correction;
		correction_blocks[i] = ( int* )calloc( max + 1, sizeof( int ) );
		if ( correction_blocks[i] == NULL )
		{
			fprintf( stderr, "Can't allocate memory\n" );
			exit(1);
		}
		correction_blocks[i][0] = correction; //element 0 is  correction size
		for ( j = 1; j <= number; j++) // convert each byte to int
		{
			strncpy( &bin[0], mas[i] + (j-1) * 8, 8 );
			correction_blocks[i][j] = convert_to_int( &bin[0] );
		}

		for ( j = 1; j <= number; j++ )
		{
			element = correction_blocks[i][1];
			memcpy( correction_blocks[i] + 1, correction_blocks[i] + 2, (max - 1)*sizeof(int)); //to left
			correction_blocks[i][max] = 0; //last element = 0
			if ( element != 0 )
			{
				for ( k = 1; k <= correction; k++)
				{
					var = inverse_field_values[element];
					var = var + polynomials[correction][k-1];
					if ( var > 254 )
						var = var%255;
					correction_blocks[i][k] = correction_blocks[i][k]^field_values[var];

				}
			}

		}

	}
	return correction_blocks;
}


char* create_data ( char **blocks, int **cor_blocks, int version ) //up
{
	int i;
	int j;
	int var = 0;
	char *data;
	char *buf;
	int size;
	int col = 0;
	int extra_size;
	int number = number_of_blocks[version];

	col+=size_of_information[version];
	col+=number_of_correction_byte[version] * 8 * number;
	data = ( char* )calloc(col + 1, sizeof(char) );
	buf = ( char* )calloc(8 + 1, sizeof(char) );
	if ( ( data == NULL ) || ( buf == NULL ) )
	{
		fprintf( stderr, "Can't allocate memory\n" );
		exit(1);
	}
	data[col] = '\0';
	data[8] = '\0';
	size = (size_of_information[version] / 8) / number; //number byte in block
	extra_size = (size_of_information[version] / 8) % number; //extra byte

	for ( i = 0; i < size; i++) //blocks
	{
		for ( j = 0 ; j < number; j++)
		{
			memcpy ( &data[var], &blocks[j][i*8], 8 );
			var+=8;
		}
	}

	for ( i = 0; i < extra_size; i++ ) //extra blocks
	{
		data[var] = blocks[number - 1][i];
		memcpy ( &data[var], &blocks[number - 1][i*8], 8 );
		var+=8;
	}

	for ( i = 0; i < number_of_correction_byte[version]; i++) //correction
	{
		for ( j = 0 ; j < number; j++)
		{
			buf = convert_to_bin ( cor_blocks[j][i+1], 8 );
			memcpy( &data[var], buf, 8 );
			var+=8;
		}
	}

	return data;
}


void add_finder_patterns ( char **pattern, int x, int y )
{
	int i;
	int j;
	int offset_x;
	int offset_y;
	for ( i = 0; i < 7; i++)
	{
		for ( j = 0; j < 7; j++)
		{
			if ( ( i == 0 ) | ( i == 6 ) )
			{
				pattern[x + i][y + j] = '1';
			}
			else if ( ( i == 1 ) | ( i == 5 ) )
			{
				if ( ( j == 0 ) | ( j == 6 ) )
					pattern[x + i][y + j] = '1';
				else
					pattern[x + i][y + j] = '0';
			}

			else
			{
				if ( ( j == 1 ) | ( j == 5 ) )
					pattern[x + i][y + j] = '0';
				else
					pattern[x + i][y + j] = '1';
			}

			if ( j == 6 )
			{
				if (( x == 4 ) & ( y == 4 ))
				{
					offset_x = 7;
					offset_y = 7;
				}
				else if (( x == 4 ) & ( y != 4 ))
				{
					offset_x = 7;
					offset_y = -1;
				}
				else
				{
					offset_x = -1;
					offset_y = 7;
				}
				pattern[x + i][y + offset_y] = '0';
				pattern[x + offset_x][y + i] = '0';

			}
		}
	}
	pattern[x + offset_x][y + offset_y] = '0';

}

int test_alignment ( char **pattern, int x, int y )
{
	int i;
	int j;
	int sign = 1;
	int offset = 4; //begin of dark array

	for ( i = x + offset; i <= x + offset + 5; i++ )
	{
		for ( j = y + offset; j <= y + offset + 5; j++ )
		{
			if ( pattern[i][j] != '+' )
				return 1;
		}
	}

	return 0;
}

void add_alignment ( char **pattern, int x, int y )
{
	int i;
	int j;
	int sign = 1;
	int offset = 4; //begin of dark array

	for ( i = 0; i < 5; i++ )
	{
		for ( j = 0; j < 5; j++ )
		{
			if ( i >= 3 )
				sign = -1;

			if ( ( i == 0 ) || ( i == 4 ) )
			{
				pattern[x - 2 * sign + offset][y - 2 + j + offset] = '1';
			}
			else if ( ( i == 1 ) || ( i == 3 ) )
			{
				if ( ( j == 0 ) || ( j == 4 ) )
					pattern[x - 1 * sign + offset][y - 2 + j + offset] = '1';
				else
					pattern[x- 1 * sign + offset][y - 2 + j + offset] = '0';
			}
			else
			{
				if ( j%2 == 0 )
					pattern[x + offset][y - 2 + j + offset] = '1';
				else
					pattern[x + offset][y - 2 + j + offset] = '0';
			}
		}
	}
}

void add_alignment_patterns ( char **pattern, int version ) //add
{
	int i;
	int j;
	int x = 4; //begin of dark array

	for ( i = 0; i < alignment_patterns[version][0]; i++ )
	{
		for ( j = 0; j < alignment_patterns[version][0]; j++ )
		{
			if ( test_alignment ( pattern, alignment_patterns[version][i + 1], alignment_patterns[version][j + 1] ) == 0 )
				add_alignment( pattern, alignment_patterns[version][i + 1], alignment_patterns[version][j + 1] );
		}
	}

}

void add_sync_line ( char **pattern, int size )
{
	int i;
	for ( i = 12; i < (size-12); i++ )
	{
		if ( pattern[10][i] == '+' )
			if ( i%2 == 0 )
			{
				pattern[10][i] = '1';
				pattern[i][10] = '1';
			}
			else
			{
				pattern[10][i] = '0';
				pattern[i][10] = '0';
			}
	}

}

void add_code_version ( char** pattern, int version ) //add
{
	int i;
	int x = 4; //beginning of dark array

	for ( i = 0; i < 6; i++ )
	{
		pattern[x + 4 * version + 6][x+i] = code_version[version][i]; //top
		pattern[x + 4 * version + 7][x+i] = code_version[version][6 + i]; //middle
		pattern[x + 4 * version + 8][x+i] = code_version[version][12 + i]; //end
		pattern[x+i][x + 4 * version + 6] = code_version[version][i]; //top
		pattern[x+i][x + 4 * version + 7] = code_version[version][6 + i]; //middle
		pattern[x+i][x + 4 * version + 8]= code_version[version][12 + i]; //end
	}


}

void add_mask ( char **pattern, int size )
{
	// mask is (X+Y)%2 for Medium 101010000010010
	int i;
	int var = 0;
	int var2 = 0;
	char mask[] = "101010000010010";

	for ( i = 4; i < 12; i++ )
	{
		if (i != 10 ) //reserved module
		{
			pattern[12][i] = mask[var];
			pattern[i][12] = mask[strlen( &mask[0] ) - 1 - var];
			var++;
		}

		if ( pattern[size - i - 1][12] != '1' )
		{
			pattern[size - i - 1][12] = mask[var2];
			pattern[12][size - i - 1] = mask[strlen( &mask[0] ) - 1 - var2];
			var2++;
		}
		else
		{
			pattern[12][size - i - 1] = mask[strlen( &mask[0] ) - 1 - var2];
			var2++;
		}
	}

	pattern[12][12] = mask[7];

}

void add_data ( char **pattern, char *data, int version)
{
	int i;
	int j;
	int number = 0; //number of empty blocks
	int size = ( ( ( version - 1 ) * 4 ) + 21 ); //size of canvas
	int var = 0;
	int x = size + 3; //next X coord
	int off_y_up = size + 3; //offset y up
	int off_y_down = 4; //offset Y down


	for ( i = 4; i <= ( size + 4 ); i++ ) //counting empty elements
		for ( j = 4; j <= ( size + 4 ); j++ )
			if ( pattern[i][j] == '+' )
				number++;

	for ( i = 0; i < (size - 1) / 2; i++ )
	{
		if ( ( x - i * 2 ) == 10 )
			x--;

		for ( j = 0; j < size; j++ )
		{
			if ( i % 2 == 0 )
			{
				if ( pattern[off_y_up - j][x - i * 2] == '+' )
				{
					if ( number <= 0 )
					{
						if ( ( off_y_up - j + x - i * 2 ) % 2 == 0 )
							pattern[off_y_up - j][x - i * 2] = '1';
						else
							pattern[off_y_up - j][x - i * 2] = '0';
					}
					else if ( ( off_y_up - j + x - i * 2 ) % 2 == 0 )
					{
						if ( data[var] == '1' )
							pattern[off_y_up - j][x - i * 2] = '0';
						else
							pattern[off_y_up - j][x - i * 2] = '1';
					}
					else
					{
						pattern[off_y_up - j][x - i * 2] = data[var];
					}
					var++;
					number--;
				}

				if ( pattern[off_y_up - j][x - i * 2 - 1] == '+' )
				{
					if ( number <= 0 )
					{
						if ( ( off_y_up - j + x - i * 2 - 1 ) % 2 == 0 )
							pattern[off_y_up - j][x - i * 2 - 1] = '1';
						else
							pattern[off_y_up - j][x - i * 2 - 1] = '0';
					}
					else if ( ( off_y_up - j + x - i * 2 - 1 ) % 2 == 0 )
					{
						if ( data[var] == '1' )
							pattern[off_y_up - j][x - i * 2 - 1] = '0';
						else
							pattern[off_y_up - j][x - i * 2 - 1] = '1';
					}
					else
					{
						pattern[off_y_up - j][x - i * 2 - 1] = data[var];
					}
					var++;
					number--;
				}		
			}
			else
			{
				if ( pattern[off_y_down + j][x - i * 2 - 1] == '+' )
				{
					if ( number <= 0 )
					{
						if ( (off_y_down + j + x - i * 2) % 2 == 0 )
							pattern[off_y_down + j][x - i * 2] = '1';
						else
							pattern[off_y_down + j][x - i * 2] = '0';
					}
					else if ( pattern[off_y_down + j][x - i * 2] == '+' )
					{

						if ( (off_y_down + j + x - i * 2) % 2 == 0 )
						{
							if ( data[var] == '1' )
								pattern[off_y_down + j][x - i * 2] = '0';
							else
								pattern[off_y_down + j][x - i * 2] = '1';
						}
						else
						{
							pattern[off_y_down + j][x - i * 2] = data[var];
						}
						var++;
						number--;
					}
				}

					if ( pattern[off_y_down + j][x - i * 2 - 1] == '+' )
					{
						if ( number <= 0 )
						{
							if ( (off_y_down + j + x - i * 2 - 1) % 2 == 0 )
								pattern[off_y_down + j][x - i * 2 - 1] = '1';
							else
								pattern[off_y_down + j][x - i * 2 - 1] = '0';
						}
						else if ( (off_y_down + j + x - i * 2 - 1) % 2 == 0 )
						{
							if ( data[var] == '1' )
								pattern[off_y_down + j][x - i * 2 - 1] = '0';
							else
								pattern[off_y_down + j][x - i * 2 - 1] = '1';
						}
						else
						{
							pattern[off_y_down + j][x - i * 2 - 1] = data[var];
						}
						var++;
						number--;
					}
				}
			}
		}
	}

	char** create_canvas_pattern ( char *data, int version ) //pattern for image
	{
		// + is empty
		// 0 is white
		// 1 is black

		int i;
		int j;
		int x = 4; //begin of dark array
		int size; //size of canvas(module)
		char **pattern; //canvas

		size = ( ( ( version - 1 ) * 4 ) + 21 );
		size+=8; //white border
		pattern = ( char** )calloc( size, sizeof(char*) );

		if ( pattern == NULL )
		{
		fprintf( stderr, "Can't allocate memory\n" );
		exit(1);
		}

		for ( i = 0; i < size; i++)
		{
			pattern[i] = (char*)calloc(size, sizeof(char));
			if ( pattern[i] == NULL )
			{
				fprintf( stderr, "Can't allocate memory\n" );
				exit(1);
			}
			memset( pattern[i], (char)((int)'+'), size ); //initializing array
		}

		for ( i = 0; i < size; i++) //white border
		{
			for ( j = 0; j < size; j++)
			{
				if ( i < 4 )
					pattern[i][j] = '0';
				else if ( (j < 4) | (j > size - 5) | (i > size - 5)  )
					pattern[i][j] = '0';
			}
		}

		pattern[x + 4 * version + 9][x + 8] = '1'; //black module

		add_finder_patterns ( pattern, x, x); //finder pattern - left top

		add_finder_patterns ( pattern, x, size - x - 7); //finder pattern - rigth top

		add_finder_patterns ( pattern, size - x - 7, x); //finder pattern - left bottom

		if ( version >= 2 )
			add_alignment_patterns ( pattern, version ); //add aligment

		add_sync_line ( pattern, size ); //add sync line

		if ( version >= 7 )
			add_code_version ( pattern, version ); //add code version

		add_mask ( pattern, size ); //add mask

		add_data ( pattern, data, version); //add data

		return pattern;
	}


void qr_code_generation ( char *input, char *output )
{
	int i;
	int version;
	int size;
	char *str_source_bin;
	char *data;
	char **blocks;
	char **pattern;
	int  **correction_blocks;

	str_source_bin = convert_to_utf8( input ); //convert string to utf8 ascii

	version = optimal_version( strlen( str_source_bin ) ); //optimal version

	str_source_bin = add_service_inf(str_source_bin, &version); //add service information into string

	blocks = create_blocks( str_source_bin, version ); //create blocks
	
	correction_blocks = create_correction_block( blocks, version ); //create correction blocks
	
	data = create_data ( blocks, correction_blocks, version ); //create data for qr code
	
	pattern = create_canvas_pattern ( data, version ); //create pattern for bmp
	
	create_bmp( pattern, output, version ); //create bmp

	//free str_source_bin
	free ( str_source_bin );

	//free blocks
	//free correction_blocks
	for ( i = 0; i < number_of_blocks[version]; i++ )
	{
			free( blocks[i] );
			free( correction_blocks[i] );
	}
	free( blocks );
	free( correction_blocks ); 

	//free data
	free( data );

	//free pattern
	size = ( ( ( version - 1 ) * 4 ) + 21 );
	size+=8; //white border
	for ( i = 0; i < size; i++ )
		free( pattern[i] );
	free( pattern );

}