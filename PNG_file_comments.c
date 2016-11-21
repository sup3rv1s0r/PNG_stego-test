PNG_file.cpp

#include <stdio.h>
#include <stdlib.h>
#include "PNG_file.h"

#define PNG_SIG_LENGTH 8 //The signature length for PNG
#define BYTE_SIZE 8 //Size of a byte
#define SIZE_WIDTH 32 //The number of bits used for storing the length of a file


//모듈 식 지수 연산을 사용한 표준 구현
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


//파일 크기를 계산하기위한 더티 함수
unsigned int filesize(const char *filename)
{
    FILE *f = fopen(filename,"rb");  /* open the file in read only */

    unsigned int size = 0;
    if (fseek(f,0,SEEK_END)==0) /* seek was successful */
        size = ftell(f);
    fclose(f);
    return size;
}

PNG_file::PNG_file(const char *inputFileName) {

    FILE * inputFile;

    unsigned char header[BYTE_SIZE];//크기가 8인 문자형 변수 선언

    inputFile = fopen (inputFileName,"rb");//처음에 입력한 파일 열기

    //파일이 열려있는지 확인
    if(!inputFile)
        exit(1);

    
               //읽기 시작
    fread(header, 1, PNG_SIG_LENGTH, inputFile);

    //png파일인지 확인
    if(png_sig_cmp(header, 0, PNG_SIG_LENGTH))
        exit(1);


    //libPNG 데이터 구조 및 오류 처리 설정
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
    //End데이터 구조 및 오류처리 설정

    //PNG에 대한 IO초기화
    png_init_io(read_ptr, inputFile);

    //우리가 시작할 때 읽은 PNG_SIG_LENGTH인 libPNG에 대해 알림
    png_set_sig_bytes(read_ptr, PNG_SIG_LENGTH);

    //전체 PNG파일을 메모리로 읽어옴
    png_read_png(read_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    row_pointers = png_get_rows(read_ptr, info_ptr);

    //만약 read_ptr에 들어있는 bit_depth가 8이 아닐경우 종료
    if(read_ptr->bit_depth != BYTE_SIZE)
        exit(1);

    fclose(inputFile);
}

void PNG_file::encode(const char *fileToEncodeName) {
    //인코딩 시작

    FILE * fileToEncode;

    unsigned char buffer = 0;

    fileToEncode = fopen (fileToEncodeName,"rb");//대입된 이름에 파일 실행

    //파일이 켜져있는지 확인
    if(!fileToEncode)
        exit(1);

    //파일 크기로 변수 생성
    unsigned long size = filesize(fileToEncodeName);

    //이 코드 섹션은 입력 파일을 그림으로 인코딩
    //입력 파일을 비트 단위로 최소 중요도로 인코딩
    for(int y=0; y < read_ptr->height; y++) {
        int x=0;
        if(y == 0)
            for(x; x < SIZE_WIDTH; x++) {
                if((size & ipow(2,x)))
                    *(row_pointers[y]+x) |= 1;
                else
                    *(row_pointers[y]+x) &= 0xFE;
            }
        for(x; x < read_ptr->width*3; x++) {
            if(x%BYTE_SIZE == 0) {
                if(!fread(&buffer, 1, 1, fileToEncode)) 
                    goto loop_end;
            }    
            if((buffer & ipow(2,x%BYTE_SIZE)))
                *(row_pointers[y]+x) |= 1;
            else
                *(row_pointers[y]+x) &= 0xFE;
        }
        //파일 크기를 너무 크게 사용하지 않았는지 확인
        if(y >= read_ptr->height)
            exit(1);
    }

    //멀티루프를 방지하기 위하여 이곳으로 점프
    loop_end:

    fclose(fileToEncode);

}

void PNG_file::decode(const char *outputFileName) {
    //디코딩 시작

    FILE * outputFile;

    unsigned char buffer = 0;

    outputFile = fopen (outputFileName,"wb");//입력받은 변수에 파일 오픈

    //파일이 열려있는지 확인
    if(!outputFile)
        exit(1);

    unsigned int size = 0;
    for(int y=0; y < read_ptr->height; y++) {
        int x=0;
        if(y == 0)
            for(x; x < SIZE_WIDTH; x++) {
                size |= ((*(row_pointers[0]+x) & 1 ) << x);
            }
        for(x; x < read_ptr->width*3; x++) {
            if((x > SIZE_WIDTH || y > 0) && x%BYTE_SIZE == 0) {
                fwrite(&buffer, 1, 1, outputFile);
                buffer = 0;
            }
            if(((read_ptr->width*y)*3+x) == size*BYTE_SIZE+SIZE_WIDTH)
                goto loop_end;
            buffer |= ((*(row_pointers[y]+x) & 1) << x%BYTE_SIZE);
        }
    }

    //다중 루프를 벗어나기 위해 여기로 이동
    loop_end:

    fclose(outputFile);

}

void PNG_file::outputPNG(const char *outputFileName) {
    //여기서 쓰기 시작

    FILE * outputFile;

    outputFile = fopen (outputFileName,"wb");
    if(!outputFile)
        exit(1);

    //쓰기 위해서 PNG구조를 초기화
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