#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

int glob_var = 6;

void *t_function(void *data)
{
	int id;
	pthread_t t_id;
	glob_var++;
	id = (*(int *)data);
	t_id = pthread_self();   //현재thread 식별자 확인
	printf("pid=%d, t_id=%lu, id=%d, glob_var=%d\n",getpid(),t_id,id,glob_var);

	return (void*)(id*id); //이중포인터로내보내면 값으로나감..//그냥불러오면주소로나감
}


int main(void)
{
	pthread_t p_thread[2];
	int status;
	int a=1;
	int b=2;
	int err;

	printf("before pthread_create() pid = %d, glob_var=%d\n",getpid(),glob_var);

	//create thread1
	//함수t_function에서 아규먼트a로 실행시키면 생성됨.생성된 thread는 pthread_exit(3)을
	//호출하거나 또는 함수에서 return할경우 제거됨. 함수와 야규먼트는 void*타입이어야함
	if((err = pthread_create(&p_thread[0],NULL,t_function,(void*)&a)) <0)
	{
   	perror("thread create error : ");
		exit(1);
	}
	//create thread2
	if((err = pthread_create(&p_thread[1],NULL,t_function,(void*)&b)) <0)  
	{
		perror("thread create error : ");
		exit(2);
	}


	//특정 pthread가 종료될떄까지 기다리다가 종료시 자원해제시켜줌.
	//두번째인자에는 pthread의 return값,,void ** 로받으니 주의.
	pthread_join(p_thread[0],(void**)&status);
	printf("thread join : %d\n",status);


	pthread_join(p_thread[1],(void**)&status);
	printf("thread join : %d\n",status);

	printf("after pthread_create() glob_var%d\n",glob_var);

	return 0;
}

