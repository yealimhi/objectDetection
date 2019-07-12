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
#include <opencv2/opencv.hpp>//opencv
#include <pthread.h> //pthread_create,pthread_join

std::string get_tegra_pipeline(int width, int height, int fps) {
    return "nvarguscamerasrc ! video/x-raw(memory:NVMM), width=(int)" + std::to_string(width) + ", height=(int)" +
            std::to_string(height) + ", format=(string)NV12, framerate=(fraction)" + std::to_string(fps) +
            "/1 ! nvvidconv ! video/x-raw, format=(string)BGRx ! videoconvert ! video/x-raw, format=(string)BGR ! appsink";
}

using namespace cv;
using std::string;

char  currenttime[100];//폴더이름
char filename[100];//파일이름
char filename_ms[100];//파일이름이 00분 00초일 때 확인

//현재시간을 return하는 함수
int currentDateTime()
{
    time_t     now = time(0); //현재 시간을 time_t 타입으로 저장
    struct tm  tstruct; //시간구조체 tm

    tstruct = *localtime(&now);
    strftime(currenttime, sizeof(currenttime), "%Y%m%d_%H", &tstruct); //'년월일_시'로 저장
	strftime(filename, sizeof(filename), "%Y%m%d_%H%M%S", &tstruct); //'년월일_시분초'로 저장
	strftime(filename_ms,sizeof(filename_ms),"%M%S",&tstruct);//'분초'로 저장

    return 0;
}
//경로확인해서 디렉토리 만드는 함수
void Makedir(char* path)
{
	currentDateTime();
	std::cout<<"폴더이름을만드는데이름초기화"<<filename<<std::endl;
	char *buffer;
	int tp;
	string in_path;

	in_path.assign(path);
	in_path.append("/");
	in_path.append(currenttime);
	buffer=(char*)in_path.c_str();

	tp = mkdir(buffer, 0777);

	/*if(tp == -1)
		std::cout<<buffer<<" 디렉토리 생성 오류"<<"\n";
	else
		std::cout<<buffer<<" 디렉토리 생성 성공"<<"\n";
	*/
}
//용량 확인하는 함수
int checkmemory(char *path) 
{
	FILE *fp;
	struct statfs lstatfs; //마운트된 파일 시스템에대한 정보
	statfs(path, &lstatfs);
	
	if(!(fp=fopen(path,"r")))
		std::cout<<"파일열지못함\n";
  
  	std::cout<<"사용가능한용량 : "<<(int)((double)lstatfs.f_bavail/(double)lstatfs.f_blocks*100)<<"% \n";

	fclose(fp);

	return (int)((double)lstatfs.f_bavail/(double)lstatfs.f_blocks*100);
}
//가장 오래된 폴더부터 삭제
int Deletedir(char *path)
{
	if(checkmemory(path) < 40)
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

		if((dip=opendir(namelist[2]->d_name))==NULL)
			std::cout<<namelist[2]->d_name<<"이름의 삭제할디렉토리열기실패\n";
		else 
			std::cout<<namelist[2]->d_name<<"이름의 삭제할디렉토리열기성공\n";

		char folder_name[256],temp[256];
		strcpy(folder_name, namelist[2]->d_name);

		while((dit=readdir(dip))!=NULL)	  //폴더안에있는파일삭제
		{
			strcpy(temp, folder_name);
			strcat(temp,"/");
			strcat(temp,dit->d_name);
			int a=remove(temp);
			if(a==-1)
				std::cout<< temp<<"파일삭제실패\n";
			else 
				std::cout<< temp<<"파일삭제성공\n";
		}
		
		if(rmdir(namelist[2]->d_name) == -1)   //파일다지운폴더지우기
			std::cout<<"폴더삭제실패\n";
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
	int inkey;
	int ret;

	currentDateTime();
	std::cout<<"처음으로동영상만드는데이름초기화"<<filename<<std::endl;
  
	int  WIDTH = 640;
   	int  HEIGHT = 480; 
	double fps = 30.0;

    std::string pipeline = get_tegra_pipeline(WIDTH, HEIGHT, fps);
	
	string str;
	str.assign(path);
	str.append("/");
	str.append(currenttime);
	str.append("/");

	string str2;
	str2 = filename;

	//카메라 영상 캡쳐
	//VideoCapture cap(0);
	VideoCapture cap(pipeline, cv::CAP_GSTREAMER);
	if(!cap.isOpened()){
		std::cerr << "에러-카메라를 열 수 없음\n";
        return -1;
	}

	//동영상을 저장하기 위한 준비
	Size size = Size(WIDTH,HEIGHT);

	VideoWriter writer;
	writer.open(str+str2+".avi", VideoWriter::fourcc('D', 'I', 'V', 'X'), fps, size, true);

	if(!writer.isOpened()){
		std::cout<<"동영상을 저장하기 위한 초기화 작업 중 에러 발생\n";
	}

	int time_flag= 1;

	while (1)
	{
        cap.read(img_color);

		imshow("Display window",img_color);
		
		inkey = waitKey(1);
		if(inkey == 27){	//ESC누르면 강종
            break;
        } 

		if(img_color.empty()){
			std::cerr<<"빈 영상 입니다.\n";
			break;
		}

        writer.write(img_color);
        frameCnt++;

        if (frameCnt == 1800 || (ret=strcmp(filename_ms,"0000"))==0 && time_flag==1){ //fps 30기준으로 프레임갯수가 1800개이면 1분!

			time_flag = 0;

            writer.release();
            frameCnt = 0;

			currentDateTime();	//경로와 파일이름 재설정
			std::cout<<"30초단위로자르고이름초기화"<<filename<<std::endl;

			str.assign(path);
			str.append("/");
			str.append(currenttime);
			str.append("/");
            str2 = filename;

            writer.open(str+str2+".avi", VideoWriter::fourcc('D', 'I', 'V', 'X'), fps, size, true);

            printf("동영상 저장 완료!\n");
        }
   	}

	return 0;
}
void *t_function1(void *data)//1번 쓰레드, 디렉토리 확인
{
    char *data_path;
    data_path = ((char *)data);

    while(1)
    {
        Makedir(data_path);//블랙박스디렉토리생성

		//std::cout<<"check Path!!!"<<"\n";
	
        sleep(1);
    }
}
void *t_function2(void *data)//2번 쓰레드, 동영상저장
{
    char *data_path;
    data_path = ((char *)data);

     while(1)
    {
        videosave(data_path);//비디오1분단위로자르기

        sleep(1);
    }
}
void *t_function3(void *data)//3번 쓰레드, 용량확인해서디렉토리삭제
{
    char *data_path;
    data_path = ((char *)data);

     while(1)
    {
        Deletedir(data_path);//용량확인해서디렉토리삭제
                std::cout<<"checkMemory!!"<<"\n";
        sleep(1);
    }
}

int main(void)
{
	char blackBox_path[256]="/home/ububtu208/yealim/objectDetection/blackbox";
	
	/*char nowpath[256];
	getcwd(nowpath,256);
	std::cout<<"현재작업중인디렉토리 : "<<nowpath<<"\n";
	*/
    pthread_t p_thread[3];
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
    // 쓰레드3 생성
    thr_id = thr_id = pthread_create(&p_thread[2], NULL, t_function3, (void *)blackBox_path);
    if (thr_id < 0)
    {
        perror("thread create error : ");
        exit(0);
    }

    pthread_join(p_thread[0], (void **)&status);
    pthread_join(p_thread[1], (void **)&status);
    pthread_join(p_thread[2], (void **)&status);

	return 0; 
}
