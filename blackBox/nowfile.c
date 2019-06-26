#include <time.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

void stprintf_time(time_t,char *);

int main()
{
    time_t the_time;
    char buffer[256];

    time(&the_time);
    stprintf_time(the_time,buffer);
    mkdir(buffer,0755);

    return 0;
}

void stprintf_time(time_t org_time,char *time_str)
{
    struct tm *tm_ptr;
    tm_ptr = localtime(&org_time);

    sprintf(time_str,"%d%d%d_%d",
                tm_ptr->tm_year+1900,tm_ptr->tm_mon+1,tm_ptr->tm_mday,tm_ptr->tm_hour);
}