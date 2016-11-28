/* PNG_file
 * author: Grant Curell
 * Performs IO and encoding and decoding on PNG images
 * Feel free to reuse at your leisure. Cite me if you like, but it's no big deal.
 * Thanks to the random dudes I bummed the code for ipow and filesize from on
 * stackoverflow ;-).
 */

#include <stdio.h>
#include <stdlib.h>
#include "PNG_file.h"

#define PNG_SIG_LENGTH 8 //The signature length for PNG
#define BYTE_SIZE 8 //Size of a byte
#define SIZE_WIDTH 32 //The number of bits used for storing the length of a file
					  //Must be a multiple of 8

/* Integer power function
 * The C++ standard pow function uses doubles and I needed an integer version.
 * This is just a standard implementation using modular exponentiation.
 */
int ipow(int base, int exp) {
	int result = 1;
	while (exp)
	{
		if (exp & 1)
			result *= base;
		exp >>= 1;
		base *= base;
	}

	return result;
}


//Dirty function for calculating the size of a file
unsigned int filesize(const char *filename)
{
	FILE *f = fopen(filename,"rb");  /* open the file in read only */

	unsigned int size = 0;
	if (fseek(f,0,SEEK_END)==0) /* seek was successful */
		size = ftell(f);
	fclose(f);
	return size;
}


/*
*		MIME Base64 Algorithm
*/

/* Base64 Encode Table */
static const char MimeBase64[] = {
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
	'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
	'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
	'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
	'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
	'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
	'w', 'x', 'y', 'z', '0', '1', '2', '3',
	'4', '5', '6', '7', '8', '9', '+', '/'
};

/* Base64 Encode Table */
static int DecodeMimeBase64[256] = {
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 00-0F */
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 10-1F */
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,62,-1,-1,-1,63,  /* 20-2F */
	52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-1,-1,-1,  /* 30-3F */
	-1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,  /* 40-4F */
	15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,  /* 50-5F */
	-1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,  /* 60-6F */
	41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1,  /* 70-7F */
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 80-8F */
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 90-9F */
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* A0-AF */
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* B0-BF */
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* C0-CF */
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* D0-DF */
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* E0-EF */
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1   /* F0-FF */
};

/* Base64 Encode Function */
int base64_encode(char *text, int numBytes, char **encodedText)
{
	unsigned char input[3]  = {0,0,0};
	unsigned char output[4] = {0,0,0,0};
	int   index, i, j, size;
	char *p, *plen;
	plen           = text + numBytes - 1;
	size           = (4 * (numBytes / 3)) + (numBytes % 3? 4 : 0) + 1;
	(*encodedText) = (char*)malloc(size);
	j              = 0;
	for  (i = 0, p = text;p <= plen; i++, p++) {
		index = i % 3;
		input[index] = *p;
		if (index == 2 || p == plen) {
			output[0] = ((input[0] & 0xFC) >> 2);
			output[1] = ((input[0] & 0x3) << 4) | ((input[1] & 0xF0) >> 4);
			output[2] = ((input[1] & 0xF) << 2) | ((input[2] & 0xC0) >> 6);
			output[3] = (input[2] & 0x3F);
			(*encodedText)[j++] = MimeBase64[output[0]];
			(*encodedText)[j++] = MimeBase64[output[1]];
			(*encodedText)[j++] = index == 0? '=' : MimeBase64[output[2]];
			(*encodedText)[j++] = index <  2? '=' : MimeBase64[output[3]];
			input[0] = input[1] = input[2] = 0;
		}
	}
	(*encodedText)[j] = '\0';
	return size;
}

/* Base64 Decode Function */
int base64_decode(char *text, unsigned char *dst, int numBytes)
{
	const char* cp;
	int space_idx = 0, phase;
	int d, prev_d = 0;
	unsigned char c;

	space_idx = 0;
	phase = 0;

	for ( cp = text; *cp != '\0'; ++cp ) {
		d = DecodeMimeBase64[(int) *cp];
		if ( d != -1 ) {
			switch ( phase ) {
			case 0:
				++phase;
				break;
			case 1:
				c = ( ( prev_d << 2 ) | ( ( d & 0x30 ) >> 4 ) );
				if ( space_idx < numBytes )
					dst[space_idx++] = c;
				++phase;
				break;
			case 2:
				c = ( ( ( prev_d & 0xf ) << 4 ) | ( ( d & 0x3c ) >> 2 ) );
				if ( space_idx < numBytes )
					dst[space_idx++] = c;
				++phase;
				break;
			case 3:
				c = ( ( ( prev_d & 0x03 ) << 6 ) | d );
				if ( space_idx < numBytes )
					dst[space_idx++] = c;
				phase = 0;
				break;
			}
			prev_d = d;
		}
	}

	return space_idx;

}


/* PNG Constructor
 * Constructor for the PNG_file class Simply reads in a PNG file
 */
PNG_file::PNG_file(const char *inputFileName) {

	FILE * inputFile;

	unsigned char header[BYTE_SIZE];

	inputFile = fopen (inputFileName,"rb");

	//Check if the file opened
	if(!inputFile)
		exit(1);

	// START READING HERE

	fread(header, 1, PNG_SIG_LENGTH, inputFile);

	//Check if it is a PNG
	if(png_sig_cmp(header, 0, PNG_SIG_LENGTH))
		exit(1);


	//Set up libPNG data structures and error handling
	read_ptr = png_create_read_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!read_ptr)
		exit(1);

	info_ptr = png_create_info_struct(read_ptr);

	if (!info_ptr) {
		png_destroy_read_struct(&read_ptr,
			(png_infopp)NULL, (png_infopp)NULL);
		exit(1);
	}

	png_infop end_info = png_create_info_struct(read_ptr);

	if (!end_info) {
		png_destroy_read_struct(&read_ptr, &info_ptr,
			(png_infopp)NULL);
		exit(1);
	}
	//End data structure/error handling setup

	//Initialize IO for PNG
	png_init_io(read_ptr, inputFile);

	//Alert libPNG that we read PNG_SIG_LENGTH bytes at the beginning
	png_set_sig_bytes(read_ptr, PNG_SIG_LENGTH);

	//Read the entire PNG image into memory
	png_read_png(read_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

	row_pointers = png_get_rows(read_ptr, info_ptr);

	//TODO ADD A CHECK SO WE ONLY USE COMPATIBLE PNG IMAGES
	if(read_ptr->bit_depth != BYTE_SIZE)
		exit(1);

	fclose(inputFile);
}

void PNG_file::encode(char *szMessage, char *szPassPhrase)
{
	//BEGIN ENCODING HERE

	//FILE * fileToEncode;

	int i = 0;

	unsigned char buffer = NULL;
	//char *szOriginalStr = NULL;
	char *szEncodedStr = NULL;
	char *szEncryptedStr = NULL;
	int nEncryptedLen = 0, nEncodedLen;
	unsigned long size = strlen(szMessage);
	
	/* malloc memory for encoded string*/
	/* in normal case, encoded string size is 1.3 times bigger than original */
	/* so sizeof(szEncodedStr) = (int)(original_size*1.3)+2 */
	/* +2 is for temporary and null-termination(1 byte each) */
	szEncodedStr = (char*)malloc((int)(sizeof(char)*size)*1.3+10);

	/* encode original string with MIME Base64 */
	size = nEncodedLen = base64_encode(szMessage,size,&szEncodedStr);

	/* the encrypted string is up to 68 bytes larger than the plaintext string */
	if(szPassPhrase != NULL)
	{
		/* Allocate memory for encrypted string */
		szEncryptedStr = (char *)malloc(sizeof(char)*strlen(szEncodedStr)+68);
		/* Encrypt the string. Return value is encrypted length */
		nEncryptedLen = AESStringCrypt((unsigned char*)szPassPhrase,strlen(szPassPhrase),(unsigned char*)szEncodedStr,strlen(szEncodedStr),(unsigned char*)szEncryptedStr);
		size = nEncryptedLen;
	}

	//This section of code encodes the input file into the picture
	//It encodes the input file bit by bit into the least significant
	//bits of the original picture file
	for(int y=0; y < read_ptr->height; y++)
	{
		int x=0;
		//Write the file size into the file y==0 ensures that it only happens
		//once
		if(y == 0)
			for(x; x < SIZE_WIDTH; x++)
			{
				if((size & ipow(2,x)))
					*(row_pointers[y]+x) |= 1;
				else
					*(row_pointers[y]+x) &= 0xFE;
			}
			for(x; x < read_ptr->width*3; x++)
			{
				if(x%BYTE_SIZE == 0)
				{
					/* If data has been encrypted */
					if(szEncryptedStr)
					{
						if(i >= nEncryptedLen)
							break;
						buffer = szEncryptedStr[i++];
					}
					/* If data has not been encrypted */
					else
					{
						if(i >= nEncodedLen)
							break;
						buffer = szEncodedStr[i++];
					}
				}
				//png_bytep here = row_pointers[y]+x; for debugging
				if((buffer & ipow(2,x%BYTE_SIZE)))
					*(row_pointers[y]+x) |= 1;
				else
					*(row_pointers[y]+x) &= 0xFE;
			}
			//Make sure that we did not use a file too large that it can't be encoded
			if(y >= read_ptr->height)
				exit(1);
	}

	free(szEncodedStr);

	/* If szEncryptedStr has been allocated */
	if(szEncryptedStr)
		free(szEncryptedStr);
}

void PNG_file::decode(char **szOutputMessage, char *szPassPhrase)
{
	//BEGIN DECODING HERE

	//FILE * outputFile;
	int loopbit = 0;
	int i = 0;
	int nDecodedLen = 0;
	int nDecryptedLen = 0;

	unsigned char buffer = 0;
	unsigned char *szOriginalStr = NULL;
	unsigned char *szDecodedStr = NULL;
	unsigned char *szDecryptedStr = NULL;

	unsigned int size = 0;

	for(int y=0; y < read_ptr->height; y++)
	{
		int x=0;
		//Write the file size into the file y==0 ensures that it only happens
		//once
		if(y == 0)
			for(x; x < SIZE_WIDTH; x++)
			{
				size |= ((*(row_pointers[0]+x) & 1 ) << x);
			}
			szOriginalStr = (unsigned char *)malloc(sizeof(unsigned char)*size);
			for(x; x < read_ptr->width*3; x++)
			{
				if((x > SIZE_WIDTH || y > 0) && x%BYTE_SIZE == 0)
				{
					szOriginalStr[i++] = buffer;
					buffer = 0;
				}
				//png_bytep here = row_pointers[y]+x; for debugging
				if(((read_ptr->width*y)*3+x) == size*BYTE_SIZE+SIZE_WIDTH)
				{
					loopbit = 1;
					break;
				}
				buffer |= ((*(row_pointers[y]+x) & 1) << x%BYTE_SIZE);
			}
			if(loopbit)
				break;
	}

	/* If there is any passphrase input */
	if(szPassPhrase != NULL)
	{
		/* allocate memory for decrypted string */
		szDecryptedStr = (unsigned char *)calloc(size,sizeof(unsigned char));

		/* decrypt the string with given passphrase */
		nDecryptedLen = AESStringDecrypt((unsigned char*)szPassPhrase,strlen((char*)szPassPhrase),szOriginalStr,size,szDecryptedStr);
		
		/* AESStringDecrpyt() returns 0xffffffff if there's any error */
		/* if not, it indicates that given data has been successfully decrypted */
		if(nDecryptedLen != 0xffffffff)
		{
			/* Allocate memory for decoed string */
			szDecodedStr = (unsigned char *)calloc(size/1.3+9,sizeof(char));
			/* Decode with base64. Return value is decoded length */
			nDecodedLen = base64_decode((char *)szDecryptedStr,szDecodedStr,nDecryptedLen);

			/* Allocate memory and copy the output message */
			*szOutputMessage = (char *)calloc(nDecodedLen+1,sizeof(char));
			strncpy(*szOutputMessage,(char*)szDecodedStr,nDecodedLen);
		}
		else
		{
			/* Allocate memory and copy the output message */
			*szOutputMessage = (char *)calloc(strlen("Error : Wrong passphrase!")+1,sizeof(char));
			strncpy(*szOutputMessage,"Error : Wrong passphrase!",strlen("Error : Wrong passphrase!"));
		}
	}
	else
	{
		/* Allocate memory for decoded string */
		szDecodedStr = (unsigned char *)calloc(size/1.3+9,sizeof(unsigned char));
		/* Decode with base64. Return value is decoded length */
		nDecodedLen = base64_decode((char *)szOriginalStr,szDecodedStr,strlen((char*)szOriginalStr)-1);

		/* Allocate memory and copy the output message */
		*szOutputMessage = (char *)calloc(nDecodedLen+1,sizeof(char));
		strncpy(*szOutputMessage,(char*)szDecodedStr,nDecodedLen-1);
		
	}

	free(szOriginalStr);
	free(szDecodedStr);

	if(szDecryptedStr)
		free(szDecryptedStr);
}

void PNG_file::outputPNG(const char *outputFileName) {
	//START WRITING HERE

	FILE * outputFile;

	outputFile = fopen (outputFileName,"wb");

	//Check if the file opened
	if(!outputFile)
		exit(1);

	//Initialize the PNG structure for writing
	write_ptr = png_create_write_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!write_ptr)
		exit(1);

	png_init_io(write_ptr, outputFile);

	//Set the rows in the PNG structure
	png_set_rows(write_ptr, info_ptr, row_pointers);

	//Write the rows to the file
	png_write_png(write_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

	fclose(outputFile);
}