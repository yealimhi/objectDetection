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
#include <pthread.h> //pthread_create,pthread_join

using namespace cv;
using std::string;

char *strerror(int errnum);

char blackBox_path[256]="/home/yealim/VScode/blackBox_file";

//현재시간을 return하는 함수
char* currentDateTime()
{
     time_t     now = time(0); //현재 시간을 time_t 타입으로 저장
     struct tm  tstruct; //시간구조체 tm
	  static char  buf[256];

     tstruct = *localtime(&now);
     strftime(buf, sizeof(buf), "%Y%m%d_%H", &tstruct); //'년월일_시'로 저장

     return buf;
}
//경로확인해서 디렉토리 만드는 함수-경로안을 일일히 확인해서 디렉토리 생성
//멀티쓰레드로 생성시 오류발생ㅠㅡㅠ
void Makedir(char* path)
{
	char temp[256], *sp, *buffer;
	int tp;
	char ptr[256];
	char tmp[256]="\0";
	string in_path;

	in_path.assign(path);
	in_path.append("/");
	in_path.append(currentDateTime());
	buffer=(char*)in_path.c_str();

	strcpy(temp, buffer); 
	sp=temp;
	chdir("/");

	while((sp = strchr(sp, '/')) != NULL) 
	{
		sp++;

		strcpy(ptr,sp);
		strtok(ptr,"/");

		//std::cout<<"저장할디렉토리이름:"<<ptr<<"\n";
		
		strcat(tmp,"/");
		strcat(tmp,ptr);
		
		//std::cout<<"이동할 디렉토리:"<<tmp<<"\n";

		tp = mkdir(ptr, 0777);
		if(tp == -1)
			std::cout<<"디렉토리생성오류"<<"\n";
		else
			std::cout<<"디렉토리생성성공"<<"\n";
	
		chdir(tmp);
	}
}
char filename[100];//파일이름
//현재시간을(초까지) return하는 함수
int currentfileTime()
{
	time_t     now = time(0); //현재 시간을 time_t 타입으로 저장
    struct tm  tstruct; //시간구조체 tm
	int tp;

	tstruct = *localtime(&now);
	strftime(filename, sizeof(filename), "%Y%m%d_%H%M%S", &tstruct); //'년월일_시:분:초'로 저장

    return 0;
}
//파일 용량 확인하는데 필요한 구도체
struct f_size
{
	long blocks;	//파일전체용량
	long avail;		//파일사용가능한용량
	long avail_per;    //파일사용가능한용량(%)
};
//용량 확인하는 함수
long checkmemory(char *path) 
{
	FILE *fp;
	struct statfs lstatfs;
	struct f_size size;

	if(!(fp=fopen(path,"r")))
		std::cout<<"파일열지못함\n";
  
	// 파일시스템의 총 할당된 크기와 사용량을 구한다.
	statfs(path, &lstatfs);
	
	size.blocks = lstatfs.f_blocks * (lstatfs.f_bsize/1024);
	size.avail  = lstatfs.f_bavail * (lstatfs.f_bsize/1024);
	size.avail_per = (double)size.avail/(double)size.blocks*100;	

  //std::cout<<" 전쳬용량:"<<size.blocks<<", 사용가능한용량:"<<size.avail<< ", %:"<<size.avail_per<<"\n";

	fclose(fp);

	return size.avail_per;
}
//가장 오래된 폴더부터 삭제
int Deletedir(char *path)
{
	if(checkmemory(path) < 15)
	{
		struct dirent **namelist;
		int count;
		int i;
		DIR *dip;
		struct dirent *dit;

		chdir(path);

		if((count = scandir(path,&namelist,NULL,alphasort)) == -1)
			std::cout<<"파일 읽기 실패\n";

		if((dip=opendir(namelist[2]->d_name))==NULL)
			std::cout<<namelist[2]->d_name<<"이름의 삭제할디렉토리열기실패\n";
		else 
			std::cout<<namelist[2]->d_name<<"이름의 삭제할디렉토리열기성공\n";

		while((dit=readdir(dip))!=NULL)	//폴더안에있는파일삭제
		{	
			chdir(namelist[2]->d_name);
			int a=unlink(dit->d_name);
			if(a==-1)
				std::cout<<"파일삭제실패:"<<strerror(errno)<<"\n";
			else 
				std::cout<<"파일삭제성공\n";
		}
		
		chdir(path);
		if(rmdir(namelist[2]->d_name) == -1)	//파일다지운폴더지우기
			std::cout<<"폴더삭제실패:"<<strerror(errno)<<"\n";
		else
			std::cout<<"폴더삭제성공\n";

		closedir(dip);

		for(i=0;i<count;i++)
		{
			free(namelist[i]);
		}

		free(namelist);
		return 0;
	}
	else
		return 0;

}
//동영상을 저장하는 함수
int videosave()
{
	Mat img_color;
    int frameCnt = 0;

    currentfileTime();
    string str;
    str = filename;

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

	writer.open(str+".avi", VideoWriter::fourcc('D', 'I', 'V', 'X'), fps, size, true);

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
            str = filename;
            writer.open(str+".avi", VideoWriter::fourcc('D', 'I', 'V', 'X'), fps, size, true);

            printf("1분 동영상 완료!\n");
        }
    }

	return 0;
}



int main(void)
{
	char blackBox_path[256]="/home/yealim/VScode/blackBox_file";
	char nowpath[256];

  //getcwd(nowpath,256);
  //std::cout<<"현재작업중인디렉토리 : "<<nowpath<<"\n";
	
	std::cout<<"현재사용가능한용량(%): "<<checkmemory(blackBox_path)<<"\n";
	
	Deletedir(blackBox_path);//용량확인해서디렉토리삭제

	Makedir(blackBox_path);//블랙박스디렉토리생성

	getcwd(nowpath,256);
	std::cout<<"현재작업중인디렉토리 : "<<nowpath<<"\n";

	videosave();//비디오1분단위로자르기

	return 0; 
}