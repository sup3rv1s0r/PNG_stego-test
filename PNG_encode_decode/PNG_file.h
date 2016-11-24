#include "..\libpng-1.2.37-src\png.h"
#include "..\AESStringCrypt\AESStringCrypt.h"

// lib comment to use libpng and zlib library
#if DEBUG
#pragma comment(lib,"..\\LIB_Debug\\libpng.lib")
#pragma comment(lib,"..\\LIB_Debug\\zlib.lib")
#pragma comment(lib,"..\\AESStringCrypt\\Output\\Debug\\AESStringCrypt.lib")
#else
#pragma comment(lib,"..\\libpng-1.2.37-src\\projects\\VS2012\\Win32_LIB_Release\\libpng.lib")
#pragma comment(lib,"..\\zlib-1.2.3\\projects\\VS2012\\Win32_LIB_Release\\zlib.lib")
#pragma comment(lib,"..\\AESStringCrypt\\Output\\Release\\AESStringCrypt.lib")
#endif
/* Class PNG_file
 * Contains the data for a PNG file object
 */
class PNG_file {

public:
	
	//Constructor
	PNG_file(const char *inputFileName);

	//Function for encoding data into the PNG from a file
	void encode(const char *fileToEncodeName, char *passphrase);

	//Function for outputing the newly created PNG to a file
	void outputPNG(const char *outputFileName);

	//Function for outputing the decoded PNG to a file
	void decode(const char *outputFileName, char *passphrase);

private:
	png_bytep* row_pointers;
	png_infop info_ptr;
	png_structp read_ptr;
	png_structp write_ptr;
};