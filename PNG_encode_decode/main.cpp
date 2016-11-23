#include "PNG_file.h"

void err(char *filename)
{
	fprintf(stderr,"%s [-e <input> -o <output_image> | -d <output>] <image>\n",filename);
	fprintf(stderr,"\t -e\t Conceal the input data\n");
	fprintf(stderr,"\t -d\t Reveal the encoded data\n");
}

int main(int argc, char *argv[])
{
	/*
	 * Command Options
	 * stego.exe [-e <input>] [-d <output>] <image>
	 * Encode : -e
	 * Decode : -d
	 * Output : -o
	 */

	if(argc < 4)
	{
		err(argv[0]);
		return -1;
	}

	/* steganograhpy를 이용해서 메세지를 숨김*/
	if(argv[1][1] == 'e')
	{
		/* 숨기는 경우 프로그램 실행시 argumnet가 총 6개 필요함 */
		if(argc != 6)
		{
			err(argv[0]);
			return -1;
		}

		PNG_file link = PNG_file(argv[5]);
		link.encode(argv[2]);
		link.outputPNG(argv[4]);
	}
	/* steganography를 이용해서 숨겨진 메세지를 복원 */
	else if(argv[1][1] == 'd')
	{
		/* 복원하는 경우 프로그램 실행시 argument가 총 4개 필요함*/
		if(argc != 4)
		{
			err(argv[0]);
			return -1;
		}

		PNG_file link = PNG_file(argv[3]);
		link.decode(argv[2]);
	}

	return 0;
}