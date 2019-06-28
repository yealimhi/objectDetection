#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

//const char *path = ".";

int main(int argc, char *argv[])
{
    struct  dirent **namelist;
    int     count;
    int     idx;

    if(argc<2)
    {
		printf("argv[1] is empty\n");
		return 1;
    }

    if((count = scandir(argv[1], &namelist, NULL, alphasort)) == -1) {
        fprintf(stderr, "%s Directory Scan Error: %s\n", argv[1], strerror(errno));
        return 1;
    }

    for(idx = 0; idx < count; idx++) {
        printf("%s\n", namelist[idx]->d_name);
    }

     // 건별 데이터 메모리 해제
    for(idx = 0; idx < count; idx++) {
        free(namelist[idx]);
    }

    // namelist에 대한 메모리 해제
    free(namelist);

    return 0;
}

/*
 int scandir(const char *dir, struct dirent ***namelist, int (*select)(const struct dirent *), int(*compar)(const struct dirent **, const struct dirent **))
 int alphasort(const struct dirent **a, const struct dirent **b); 

 //각 디렉토리에 대해 select()호출하면서 dir 디렉토리를 조사. select()에서0이 아닌 값을 반환한 항목들은 malloc()을 통해 할당된 문자열에 저장,비교 함수compare()를 사용한 qsort()에 의해 정렬되고, malloc()를통해 할당된 namelist 배열에 모아진다. select가 NULL이면 모든항목이 선택
//alphasort()함수는 비교함수 대신으로 사용가능
//그래서 free()함수를 사용함
*/
