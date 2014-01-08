#include "code.h"

char* convert_to_bin( unsigned int num ) //convert int to binary
{
	int i;
	char *str_bin;
	//if (num < 128)
	//{
		i=7;
		str_bin = ( char* )calloc( 8, sizeof( char )); //bin str
		memset( str_bin, (char)((int)'0'), 8 ); //initializing array
	//}
	//else
	//{
	//	i=15;
	//	str_bin = ( char* )calloc( 16, sizeof( char )); //bin str
	//	memset( str_bin, (char)((int)'0'), 16 ); //initializing array
	//}
	while ( num > 0 )
	{
		str_bin[i] += num%2;
		i--;
		num/=2;
	}
	return str_bin;
}


int convert_to_int( char* str )
{
	int sum = 0;
	int i;
	for ( i = 0; i < 8; i++)
		sum+= (int)((str[7-i] - 48) * pow( 2., i ));
	return sum;

}

char* convert_to_utf8( char *str ) //convert int to utf8 binary
{
	int i;
	char *str_bin;
	str_bin = ( char* )calloc( strlen( str ) * 8 + 1, sizeof( char )); //for bin str
	for ( i = 0; i < strlen( str ); i++ )
		memcpy( str_bin + i*8, convert_to_bin( str[i] ), 8 * sizeof( char )); //copy binary code
	str_bin[ strlen( str ) * 8 + 1 ] = '\0'; //add end of str
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
	int extra; //extra byte
	int buffer;
	int i;
	char *str_full;
	char *zero_str;

	if (*ver <= 9) //length of data field
		data+=8;
	else 
		data+=16;

	if ( ( strlen( str ) + data ) > ( size_of_information[*ver] ) ) //if big size
	{
		(*ver)++;
		data = 4;
		if (*ver <= 9) //length of data field again, because version changed
			data+=8;
		else 
			data+=16;
	}


	zero = (strlen( str ) + data )%8; //number of zero until str%8 != 0
	zero_str = (char*)calloc( zero, sizeof(char));
	memset( zero_str, (char)((int)'0'), zero ); //initializing array

	buffer = (strlen( str ) + data + zero - size_of_information[*ver]);
	extra = ( int )ceil(fabs( buffer/8. )); //number of extra byte

	str_full = ( char* )calloc( strlen( str ) + data + zero + extra*8, sizeof(char) ); //new str full
	strcpy(str_full, "0100"); //add coding method
	strcpy(&str_full[4], convert_to_bin(strlen( str )/8)); //add size of data
	strcpy(&str_full[data], str); //add data(bit)
	strcpy(&str_full[data + strlen(str)], zero_str); //add zero

	for (i = 0; i < extra; i++) //add extra byte
	{
		if (i%2 == 0)
			strcpy(&str_full[data + strlen(str) + zero + i*8], "11101100");
		else
			strcpy(&str_full[data + strlen(str) + zero + i*8], "00010001");
	}

	free(zero_str);
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
	size = (size_of_information[version] / 8) / number; //number byte in block
	extra_size = (size_of_information[version] / 8) % number; //extra byte
	mas = ( char** )calloc( number, sizeof(char*) );
	for ( i = 0; i < number; i++ )
	{
		if ( (i + extra_size)!=number )
		{
			mas[i] = ( char* )calloc( size * 8 , sizeof(char) ); //without extra size
			memcpy( mas[i], &str[element], size * 8); 
			mas[i][size * 8] = '\0';
			element+=( size * 8 );
		}
		else
		{
			mas[i] = ( char* )calloc( (size + 1) * 8 , sizeof(char) ); //with extra size
			memcpy( mas[i], &str[element], (size + 1) * 8); 
			mas[i][(size + 1) * 8] = '\0';
			element+=(size + 1) * 8;
			extra_size--;
		}
	}
	
	return mas;
}


int** create_correction_block( char **mas, int version )
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
	correction = number_of_correction_byte[version];

	for ( i = 0; i < number_of_blocks[version]; i++ )
	{
		number = strlen( mas[i] ) / 8;
		if ( number > correction)
			max = number;
		else
			max = correction;
		correction_blocks[i] = ( int* )calloc( max + 1, sizeof( int ) );
		correction_blocks[i][0] = correction; //element 0 is  correction
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
				for ( k = 1; k <= (max); k++)
				{
					var = inverse_field_values[element];
					var = ( var + polynomials[correction][k-1] )%255;
					correction_blocks[i][k] = correction_blocks[i][k]^field_values[var];

				}
			}
			
		}
		
	}
	return correction_blocks;
}


char* create_data ( char **blocks, int **cor_blocks, int version )
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
	col+=number_of_correction_byte[version]*8;
	data = (char*)calloc(col, sizeof(char));
	buf = (char*)calloc(8, sizeof(char));
	size = (size_of_information[version] / 8) / number; //number byte in block
	extra_size = (size_of_information[version] / 8) % number; //extra byte

	for ( i = 0; i < size*8; i++) //blocks
	{
		for ( j = 0 ; j < number; j++)
		{
			data[var] = blocks[j][i];
			var++;
		}
	}

	for ( i = 0; i < extra_size*8; i++ ) //extra blocks
	{
		data[var] = blocks[number][i];
		var++;
	}

	for ( i = 0; i < number_of_correction_byte[version]; i++) //correction
	{
		for ( j = 0 ; j < number; j++)
		{
			buf = convert_to_bin (cor_blocks[j][i+1]);
			buf[8]='\0';
			strcpy( &data[var], buf );
			var+=8;
		}
	}

	data[col] = '\0';
	printf("%s", data );
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

void add_aligment_patterns ( char **pattern, int version ) //add
{

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

void add_code_version ( char** patterns, int version ) //add
{

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
	//pattern[4][4] = 'm';
	//pattern[y][y] = 'm';
	for ( i = 4; i <= ( size + 4 ); i++ ) //counting empty elements
		for ( j = 4; j <= ( size + 4 ); j++ )
			if ( pattern[i][j] == '+' )
				number++;

	for ( i = 0; i < (size-1); i+=2 )
	{
		if ( ( x - i ) == 10 )
			x--;
		for ( j = 0; j < size; j++ )
		{
			if ( i%2 == 0 )
			{
				if ( pattern[off_y_up - j][x - i] == '+' )
				{
					pattern[off_y_up - j][x - i] = data[var];
					var++;
					number--;
				}
				if ( pattern[off_y_up - j][x - i - 1] == '+' )
				{
					pattern[off_y_up - j][x - i - 1] = data[var];
					var++;
					number--;
				}
				
			}
			else
			{
				if ( pattern[off_y_down + j][x - i*2] == '+' )
				{
					pattern[off_y_down + j][x - i*2] = data[var];
					var++;
					number--;
				}
				if ( pattern[off_y_down + j][x - i*2 - 1] == '+' )
				{
					pattern[off_y_down + j][x - i*2 - 1] = data[var];
					var++;
					number--;
				}
			}


			/*for ( k = 0; k < size+8; k++) //output pattern. for test only
				{
				for ( kk = 0; kk < size+8; kk++)
					printf("%c ", pattern[k][kk]);
				printf("\n");
				}
			*/

		}
	}



	printf ( "empty: %d\n", number );

}

char** create_canvas_pattern ( char *data, int version ) //pattern for image
{
	// + is empty
	// 0 is white
	// 1 is black
	
	int i;
	int j;
	int x = 4; //begins of dark array
	int size; //size of canvas(module)
	char **pattern; //canvas

	size = ( ( ( version - 1 ) * 4 ) + 21 );
	size+=8; //white border
	pattern = (char**)calloc( size, sizeof(char*));
	
	for ( i = 0; i < size; i++)
	{
		pattern[i] = (char*)calloc(size, sizeof(char));
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
	
	add_aligment_patterns ( pattern, version ); //add aligment
	
	add_sync_line ( pattern, size ); //add sync line

	add_code_version ( pattern, version ); //add code version
	
	add_mask ( pattern, size ); //add mask

	add_data ( pattern, data, version); //add data

	for ( i = 0; i < size; i++) //output pattern. for test only
	{
		for ( j = 0; j < size; j++)
			printf("%c ", pattern[i][j]);
		printf("\n");
	}

	return pattern;
}