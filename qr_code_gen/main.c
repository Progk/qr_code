#define _CRT_SECURE_NO_WARNINGS
#define NUMBER_OF_SYMBOLS 2048
#define NAME_OF_BMP_FILE 512
#include <stdio.h>
#include "code.h"



int main()
{
	char input[NUMBER_OF_SYMBOLS];
	char output[NAME_OF_BMP_FILE];
	
	printf ( "%s", "QR Code Generator ( ASCII symbols only )" );
	
	printf ( "\n\n\n%s\n", "Enter string: " );
		fgets ( &input[0], NUMBER_OF_SYMBOLS, stdin );
	
	if ( strlen ( input ) != 1 )
		input[strlen( input ) - 1] = '\0';
	else
		return 0;

	printf ( "\n%s\n", "Enter name of BMP file: " );

	fgets ( &output[0], NAME_OF_BMP_FILE, stdin );

	if ( strlen ( output ) != 1 )
		output[strlen( output ) - 1] = '\0';
	else
		return 0;

	qr_code_generation ( &input[0], &output[0] );
	
	printf( "\n%s", "QR Code was generated successful!" );

	getchar();

	return 0;

}