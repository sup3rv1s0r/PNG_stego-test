main.cpp

#include "PNG_file.h"
void main() {
    PNG_file link = PNG_file("big.png");//PNG_file.h에 PNG_file변수에 big.png를 대입한 link변수 선언
    link.encode("1.txt");//똑같이 encode함수에 1.txt대입
    link.outputPNG("bigen.png");//같은 방식으로 bigen.png대입
    PNG_file link = PNG_file("bigensimple.png");//여기도~
//    link.decode("2.txt");//~~
}