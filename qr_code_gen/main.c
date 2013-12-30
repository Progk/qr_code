#include <stdio.h>
#include <string.h>
#include <malloc.h>

char* convert_to_bin( int num ) //convert int to binary
{
	int i = 7;
	char *str_bin;
	str_bin = ( char* )calloc( 8, sizeof( char )); //bin str
	memset( str_bin, (char)((int)'0'), 8 ); //initializing array
	while ( num > 0 )
	{
		str_bin[i] += num%2;
		i--;
		num/=2;
	}
	return str_bin;
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

int main()
{
	char c;
	char *str_source = "hello";
	char *str_source_bin;
	str_source_bin = convert_to_utf8( str_source );
	printf("%s", str_source_bin);
	getchar();

}