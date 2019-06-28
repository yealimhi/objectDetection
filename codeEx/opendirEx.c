#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(void)
{
    DIR *dir_info;
    struct dirent *dir_entry;
	 /*
		 struct dirnet
		 {
		 	long d_ino;   //아이노드
			off_t d_off;  //dienet의 offset
			unsigned short d_reclen; //d_name의 길이
			char d_name[NAME_MAX+1]; //파일이름(없으면 NULL로 종료)
		 }

	*/

    mkdir("makeA",0755);
    mkdir("makeB",0755);

    dir_info=opendir(".");    //현재의파일오픈

    if(NULL!=dir_info)
    {
        while(dir_entry=readdir(dir_info))
        {
            printf("%s\n", dir_entry->d_name); 
        }
        closedir(dir_info);
    }

}
