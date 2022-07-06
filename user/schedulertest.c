// #include "kernel/types.h"
// #include "kernel/stat.h"
// #include "user/user.h"
// #include "kernel/fcntl.h"

// #define NFORK 10
// #define IO 5

// int main()
// {
//     int n, pid;
//     int wtime, rtime;
//     int twtime = 0, trtime = 0;
//     for (n = 0; n < NFORK; n++)
//     {
//         pid = fork();
//         if (pid < 0)
//             break;
//         if (pid == 0)
//         {
// // #ifndef FCFS 
// //             if (n < IO)
// //             {
// //                 sleep(200); // IO bound processes
// //             }
// //             else
// //             {
// // #endif
// #if SCHEDULER==SCHED_RR 
//             if (n < IO)
//             {
//                 sleep(200); // IO bound processes
//             }
//             else
//             {
// #endif
// #if SCHEDULER==SCHED_PBS
//             if (n < IO)
//             {
//                 sleep(200); // IO bound processes
//             }
//             else
//             {
// #endif
// #if SCHEDULER==SCHED_MLFQ
//             if (n < IO)
//             {
//                 sleep(200); // IO bound processes
//             }
//             else
//             {
// #endif
//                 for (volatile int i = 0; i < 1000000000; i++)
//                 {
//                 } // CPU bound process
// // #ifndef FCFS
// //             }
// // #endif
// #if SCHEDULER==SCHED_RR
//             }
// #endif 
// #if SCHEDULER==SCHED_PBS
//             }
// #endif
// #if SCHEDULER==SCHED_MLFQ
//             }
// #endif

//             printf("Process %d finished\n", n);
//             exit(0);
//         }
//         else
//         {
// #if SCHEDULER==SCHED_PBS
//             setpriority(80, pid); // Will only matter for PBS, set lower priority for IO bound processes
// #endif
//         }
//     }
//     for (; n > 0; n--)
//     {
//         if (waitx(0, &wtime, &rtime) >= 0)
//         {
//             trtime += rtime;
//             twtime += wtime;
//         }
//     }
//     printf("Average rtime %d,  wtime %d\n", trtime / NFORK, twtime / NFORK);
//     exit(0);
// }

// // #include "kernel/types.h"
// // #include "kernel/stat.h"
// // #include "user/user.h"
// // #include "kernel/fcntl.h"


// // #define NFORK 10
// // #define IO 5

// // int main() {
// //   int n, pid;
// //   int wtime, rtime;
// //   int twtime=0, trtime=0;
// //   for(n=0; n < NFORK;n++) {
// //       pid = fork();
// //       if (pid < 0)
// //           break;
// //       if (pid == 0) {
// // #if SCHEDULER==SCHED_RR
// //           printf("lmao1\n");
// //           sleep(20000);
// //           if (n < IO) {
// //             sleep(200); // IO bound processes
// //           } else {
// // #else 
// // #if SCHEDULER==SCHED_PBS
// //           printf("lmao2\n");
// //           sleep(20000);
// //           if (n < IO) {
// //             sleep(200); // IO bound processes
// //           } else {
// // #else
// // #if SCHEDULER==SCHED_MLFQ
// //           printf("lmao3\n");
// //           sleep(20000);
// //           if (n < IO) {
// //             sleep(200); // IO bound processes
// //           } else {
// // #endif
// // #endif
// // #endif

// //             for (volatile int i = 0; i < 1000000000; i++) {} // CPU bound process 
// // #if SCHEDULER == SCHED_RR
// //           }
// // #endif
// // #if SCHEDULER==SCHED_PBS
// //           }
// // #endif
// // #if SCHEDULER==SCHED_MLFQ
// //           }
// // #endif
// //           //printf("Process %d finished", n);
// //           exit(0);
// //       } else {
// // #if SCHEDULER !=  SCHED_PBS
// //         setpriority(80, pid); // Will only matter for PBS, set lower priority for IO bound processes 
// // #endif
// //       }
// //   }
// //   for(;n > 0; n--) {
// //       if(waitx(0,&wtime,&rtime) >= 0) {
// //           trtime += rtime;
// //           twtime += wtime;
// //       } 
// //   }
// //   printf("Average rtime %d,  wtime %d\n", trtime / NFORK, twtime / NFORK);
// //   exit(0);
// // }

// // MLFQ = Average rtime 113,  wtime 11
// // PBS = Average rtime 115,  wtime 11
// // RR =  Average rtime 114,  wtime 12
// // FCFS = Average rtime 113,  wtime 13


#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"


#define NFORK 10
#define IO 5

int main() {
  int n, pid;
  int wtime, rtime;
  int twtime=0, trtime=0;
  for(n=0; n < NFORK;n++) {
      pid = fork();
      if (pid < 0)
          break;
      if (pid == 0) {
#ifndef FCFS
          if (n < IO) {
            sleep(200); // IO bound processes
          } else {
#endif
            for (volatile int i = 0; i < 1000000000; i++) {} // CPU bound process 
#ifndef FCFS
          }
#endif
          printf("Process %d finished", n);
          exit(0);
      } else {
#ifdef PBS
        setpriority(80, pid); // Will only matter for PBS, set lower priority for IO bound processes 
#endif
      }
  }
  for(;n > 0; n--) {
      if(waitx(0,&rtime,&wtime) >= 0) {
          trtime += rtime;
          twtime += wtime;
      } 
  }
  printf("Average rtime %d,  wtime %d\n", trtime / NFORK, twtime / NFORK);
  exit(0);
}