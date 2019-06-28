#include <stdio.h>
#include <stlib.h>

int main()
{
    setenv("MYDIR","/home/yealim",0);
    setenv("KERNEL_SRC","/usr/src/linux",0);
    
    printf("MYDIR = %s\n",getenv("MYDIR"));
    printf("KERNEL_SRC = %s\n",getenv("KERNEL_SRC"));
 
    while(*environ)
		printf("%s\n",*environ++);
    
    return 0;
}
