#include "user.h"

int main(int argc, char **argv)
{
    if(argc < 3){
        printf("Invalid number of arguments\n");
        exit(0);
    }
    int priority = atoi(argv[1]);
    int pid = atoi(argv[2]);
    if (setpriority(priority, pid) < 0)
    {
        printf("Error\n");
    }
    exit(0);
}