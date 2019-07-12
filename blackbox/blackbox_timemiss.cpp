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
char  currenttime[256];
//현재시간을 return하는 함수
int currentDateTime()
{
    time_t     now = time(0); //현재 시간을 time_t 타입으로 저장
    struct tm  tstruct; //시간구조체 tm

    tstruct = *localtime(&now);
    strftime(currenttime, sizeof(currenttime), "%Y%m%d_%H", &tstruct); //'년월일_시'로 저장

    return 0;
}
//경로확인해서 디렉토리 만드는 함수
void Makedir(char* path)
{
	currentDateTime();
	char *buffer;
	int tp;
	string in_path;

	in_path.assign(path);
	in_path.append("/");
	in_path.append(currenttime);
	buffer=(char*)in_path.c_str();

	tp = mkdir(buffer, 0777);
	if(tp == -1)
		std::cout<<buffer<<" 디렉토리 생성 오류"<<"\n";
	else
		std::cout<<buffer<<" 디렉토리 생성 성공"<<"\n";
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
	if(checkmemory(path) < 10)
	{
		struct dirent **namelist;
		char tmp[256];
		int count;
		int i;
		DIR *dip;
		struct dirent *dit;

		strcpy(tmp,path);

		if((count = scandir(tmp,&namelist,NULL,alphasort)) == -1)
			std::cout<<"파일 읽기 실패\n";
		// string str;
		// char* buff;
		// str.assign(namelist[3]->d_name);


		if((dip=opendir(namelist[3]->d_name))==NULL)
			std::cout<<namelist[3]->d_name<<"이름의 삭제할디렉토리열기실패\n";
		else 
			std::cout<<namelist[3]->d_name<<"이름의 삭제할디렉토리열기성공\n";

		// dit = readdir(dip);
		// str.append(dit->d_name);

		// buff=(char*)str.c_str();

		char folder_name[256],temp[256];
		strcpy(folder_name, namelist[3]->d_name);
		

		while((dit=readdir(dip))!=NULL)	//폴더안에있는파일삭제
		{
			strcpy(temp, folder_name);
			strcat(temp,"/");
			strcat(temp,dit->d_name);
			int a=remove(temp);
			if(a==-1)
				std::cout<< temp<<"파일삭제실패:"<<strerror(errno)<<"\n";
			else 
				std::cout<< temp<<"파일삭제성공\n";
		
			//std::cout<<dit->d_name<<std::endl;
			sleep(3);
		}
		
		if(rmdir(namelist[3]->d_name) == -1)	//파일다지운폴더지우기
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
int videosave(char *path)
{
	Mat img_color;
    int frameCnt = 0;

	currentDateTime();
    currentfileTime();

	string str;

	str.assign(path);
	str.append("/");
	str.append(currenttime);
	str.append("/");

	string str2;
	str2 = filename;

	std::cout<<str+str2<<".avi"<<std::endl;

	//카메라 영상 캡쳐
	VideoCapture cap(0);
	if(!cap.isOpened()){
		std::cerr << "에러-카메라를 열 수 없음\n";
        return -1;
	}

	//동영상을 저장하기 위한 준비
	Size size = Size((int)cap.get(CV_CAP_PROP_FRAME_WIDTH),(int)cap.get(CV_CAP_PROP_FRAME_HEIGHT));
	double fps = 30.0;

	VideoWriter writer;
	writer.open(str+str2+".avi", VideoWriter::fourcc('D', 'I', 'V', 'X'), fps, size, true);

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

        if (frameCnt == 900){ //fps 30기준으로 프레임갯수가 1800개이면 1분!

            writer.release();
            frameCnt = 0;

            currentfileTime();
            str2 = filename;
            writer.open(str+str2+".avi", VideoWriter::fourcc('D', 'I', 'V', 'X'), fps, size, true);

            printf("1분 동영상 완료!\n");
        }
    }

	return 0;
}
void *t_function1(void *data)//1번 쓰레드, 경로확인해서 동영상저장
{
    char *data_path;
    data_path = ((char *)data);
	char nowpath[256];

	getcwd(nowpath,256);
	std::cout<<"1현재작업중인디렉토리 : "<<nowpath<<"\n";

    while(1)
    {
        Makedir(data_path);//블랙박스디렉토리생성

        videosave(data_path);//비디오1분단위로자르기

		getcwd(nowpath,256);
		std::cout<<"1현재작업중인디렉토리 : "<<nowpath<<"\n";
	
        sleep(1);
    }
}
void *t_function2(void *data)//2번 쓰레드, 용량확인해서디렉토리삭제
{
    char *data_path;
    data_path = ((char *)data);

     while(1)
    {
		char nowpath[256];

		getcwd(nowpath,256);
		
        Deletedir(data_path);//용량확인해서디렉토리삭제
		std::cout<<"2현재작업중인디렉토리 : "<<nowpath<<"\n";
        sleep(1);
    }
}

int main(void)
{
	char blackBox_path[256]="/home/yealim/VScode/blackbox";
	char nowpath[256];

	getcwd(nowpath,256);
	std::cout<<"현재작업중인디렉토리 : "<<nowpath<<"\n";

    pthread_t p_thread[2];
    int thr_id;
    int status;

    // 쓰레드1 생성
    thr_id = pthread_create(&p_thread[0], NULL, t_function1, (void *)blackBox_path);
    if (thr_id < 0)
    {
        perror("thread create error : ");
        exit(0);
    }

    // 쓰레드2 생성
    thr_id = pthread_create(&p_thread[1], NULL, t_function2, (void *)blackBox_path);
    if (thr_id < 0)
    {
        perror("thread create error : ");
        exit(0);
    }

    pthread_join(p_thread[0], (void **)&status);
    pthread_join(p_thread[1], (void **)&status);

	return 0; 
}
