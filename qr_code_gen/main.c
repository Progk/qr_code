#define _CRT_SECURE_NO_WARNINGS
#define NUMBER_OF_SYMBOLS 2048
#include <stdio.h>
#include "code.h"



int main()
{
	char input[NUMBER_OF_SYMBOLS];
	char output[NUMBER_OF_SYMBOLS];
	
	printf ( "%s", "QR Code Generator ( ASCII symbols only )" );
	
	printf ( "\n\n\n%s\n", "Enter string: " );
		fgets ( &input[0], 1024, stdin );
	
	if ( strlen ( input ) != 1 )
		input[strlen( input ) - 1] = '\0';
	else
		return 0;

	printf ( "\n%s\n", "Enter name of BMP file: " );

	fgets ( &output[0], 1024, stdin );

	if ( strlen ( output ) != 1 )
		output[strlen( output ) - 1] = '\0';
	else
		return 0;

	qr_code_generation ( &input[0], &output[0] );
	
	printf( "\n%s", "QR Code was generated successful!" );

	getchar();

	//free ( &input[0] );
	//free ( &output[0] );

	return 0;

}