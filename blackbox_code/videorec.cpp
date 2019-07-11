//동영상을 저장하는 함수
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <cstring>
#include <string>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/vfs.h>
#include <libgen.h>
#include <stdlib.h>
#include <dirent.h> //opendir
#include <errno.h> //errno
#include <opencv2/opencv.hpp>//opencv

using namespace cv;
using std::string;

char buf[100];

//현재시간을(초까지) return하는 함수
int currentfileTime()
{
	time_t     now = time(0); //현재 시간을 time_t 타입으로 저장
    struct tm  tstruct; //시간구조체 tm
	int tp;

	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%Y%m%d_%H%M%S", &tstruct); //'년월일_시:분:초'로 저장

    return 0;
}

int main()
{
	Mat img_color;
    int frameCnt = 0;

    currentfileTime();
    string str;
    str = buf;

	//카메라 영상 캡쳐
	VideoCapture cap("road.mp4");
	if(!cap.isOpened()){
		std::cerr << "에러-카메라를 열 수 없음\n";
        return -1;
	}

	//동영상을 저장하기 위한 준비
	Size size = Size((int)cap.get(CV_CAP_PROP_FRAME_WIDTH),(int)cap.get(CV_CAP_PROP_FRAME_HEIGHT));
	VideoWriter writer;
	double fps = 30.0;

	writer.open(str+".avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), fps, size, true);

	if(!writer.isOpened()){
		std::cout<<"동영상을 저장하기 위한 초기화 작업 중 에러 발생\n";
	}

	while (1)
	{
        cap.read(img_color);

		if(img_color.empty()){
			std::cerr<<"빈 영상 입니다.\n";
			break;
		}

        writer.write(img_color);
        frameCnt++;

        if (frameCnt == 1800){ //fps 30기준으로 프레임갯수가 1800개이면 1분!

            writer.release();
            frameCnt = 0;

            currentfileTime();
            str = buf;
            writer.open(str+".avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), fps, size, true);

            printf("1분 동영상 완료!\n");
        }
    }

	return 0;
}