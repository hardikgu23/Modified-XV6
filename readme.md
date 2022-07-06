#MODIFIED XV-6

## OVERVIEW

Modifications have been made into xv6 operating system. 
Following changes have been made :-

1) Strace has been implemented. strace runs the specified command until it exits.It intercepts and records the system calls which are called by a process during its execution.It should take one argument, an integer mask, whose bits specify which system calls to trace. For example, to trace the ith system call, a program calls strace 1 << i, where i is the syscall number.

Various scheduling alogorithm have been implemented:- 

## FCFS
1)FCFS : It selects the process with the lowest creation time. The process will run until it no longer needs CPU time.

- It is a non-preemptive process. 

// Find out the process with lowest creation time. 

```sh
struct proc *first_proc = 0;
    // struct proc *p = 0;
    int counter = 0;

    for (p = proc; p < &proc[NPROC]; p++)
    {
      counter++;
      int flag = 0;
      acquire(&p->lock);
      if (p->state == RUNNABLE)
      {
        // printf("hello\n");
        if (first_proc == 0) // if this is first process to be found , then initialize
        {
          //                  printf("Start Counter: %d\n" , counter);
          first_proc = p;
          flag = 1;
        }

        // compare when both the process came first
        else if (p->ctime < first_proc->ctime)
        {
          //                  printf("Entered Entered Entered Entered Entered Entered\n");
          flag = 1;
          release(&first_proc->lock);
          first_proc = p;
        }
        // printf("p->ctime is %d\n first_proc->ctime is %d\n" , p->ctime , first_proc->ctime);
      }

      if (flag != 1) // safe to release it
      {
        release(&p->lock);
      }
    }
```

- We run this procees. Since preemption is disabled , it runs till completion. 
```sh
 // now first_proc contains the  process that came first
    if (first_proc != 0)
    {
      p = first_proc;

      p->num_run++;

      c->proc = p;

      p->state = RUNNING;

      // printf("Entered in FCFS");

      // context switching
      swtch(&c->context, &p->context);

      // process is done running
      // p->state is updated before returning
      c->proc = 0;
      release(&p->lock);
```

## PBS 
- Assign default priority 60 to each entering process
- Find the process with maximum priority (with minimum pririty number )
```sh
    struct proc *priorityproc = 0;
    c->proc = 0;

    // update niceness of each process

    for (p = proc; p < &proc[NPROC]; p++)
    {
      acquire(&p->lock);
      // update the niceness
      int numerator = p->wtime;
      int denominator = p->wtime + p->run_time;

      if (denominator == 0)
      {
        p->niceness = 5;
      }
      else
      {
        p->niceness = (numerator * 10) / denominator;
      }

      release(&p->lock);
    }

    for (p = proc; p < &proc[NPROC]; p++)
    {
      acquire(&p->lock);
      if (p->state != RUNNABLE)
      {
        release(&p->lock);
        continue;
      }

      // lower calculate dp means it has higher dp

      if (!priorityproc || (calculate_dp(p) < calculate_dp(priorityproc)))
      {
        // printf("Hello\n");
        // printf("Hello d1: %d  d2:   %d\n" ,calculate_dp(p) ,  calculate_dp(priorityproc));
        if (priorityproc)
          release(&priorityproc->lock);

        priorityproc = p;
      }
      // in case if priorities are same
      // break ties according to number of times it has been scheduled
      else if ((calculate_dp(p) == calculate_dp(priorityproc)) && (p->num_turns_priority < priorityproc->num_turns_priority))
      {
        // printf("Hi\n");

        release(&priorityproc->lock);
        priorityproc = p;
      }
      // if even this is same , break ties according to ctime
      else if ((calculate_dp(p) == calculate_dp(priorityproc)) && (p->num_turns_priority == priorityproc->num_turns_priority) && (p->ctime < priorityproc->ctime))
      {
        // printf("Offo\n");
        release(&priorityproc->lock);
        priorityproc = p;
      }
      else
      {
        release(&p->lock);
      }
    }
    ```
    
    Now run this process , this is also preemptive , so This process will go to completion. 
    ```sh
     if (priorityproc)
    {
      // printf("Priority of the process is %d sleep_time: %d  run_time: %d \n" , calculate_dp(priorityproc) , priorityproc->wtime , priorityproc->run_time);
      // printf("Hardik\n");
      p = priorityproc;
      p->num_turns_priority++;
      p->num_run++;
      c->proc = p;

      // make runtime and sleeptime (wtime) of the process to be zero
      p->wtime = 0; // make the waiting time of the process to be zero
      p->run_time = 0;

      p->state = RUNNING;

      // printf("Entered in PBS");

      // context switching
      swtch(&c->context, &p->context);

      // process is done running
      // p->state is updated before returning
      c->proc = 0;
      // printf("Hello\n");
      release(&p->lock);
    }
    ```
- setpriority has been implemented as a part of this. It  is provided with new peiority and pid od the process , and it resets the priority to new_priority, niceness to 5. 
- It returns old priority of the process. 
- Also , if the new dynamic priority of the process is less than the old dynamic priority , then rescheduling is done by yield(). 
    
    ```sh
    int setpriority(int new_priority, int pid)
  {
    // printf("hello from setpriority\n");
    if (new_priority < 0 || new_priority > 100)
      return -1;

    for (struct proc *p = proc; p < &proc[NPROC]; p++)
    {
      acquire(&p->lock);
      if (p->pid == pid)
      {
        int old_priority = p->priority;

        int old_dp = calculate_dp(p);

        p->priority = new_priority;
        p->niceness = 5;

        int new_dp = calculate_dp(p);

        release(&p->lock);

        if (new_dp < old_dp)
        {
          yield(); // give up the cpu and reschedule
        }

        return old_priority;
      }
      else
      {
        release(&p->lock);
      }
    }

    return -1; // if no process is found with given pid
  }
  ```
  
## MLFQ
- For each process , we maintain in its struct proc , it queue number(p->queue) and the time it has spend  in the current queue (p->curr_tick). 

- Initially , all the process created have 0 priority.

- For scheduling , we select the queue with greated priority . and run it 
```sh
 for (int q_no = 0; q_no < 5; q_no++) // q_no is the queue number
    {
      for (p = proc; p < &proc[NPROC]; p++)
      {
        acquire(&p->lock);

        if ((p->state != RUNNABLE) || (p->queue != q_no))
        {
          release(&p->lock);
          continue;
        }

        // run this process on the cpu
        c->proc = p;
        p->num_run++;
        p->state = RUNNING;

        // printf("Entered in MLFQ");

        swtch(&c->context, &p->context);

        c->proc = 0;

        release(&p->lock);
        continue;
      }
    }
```

- We have an flag change_q which tells if we have exceeded the max_time the process in the current queue. If set , we move the process to the lower queue. 
```sh
  // check if we have to queue of any process?
    for (p = proc; p < &proc[NPROC]; p++)
    {
      acquire(&p->lock);

      if (p->state != RUNNABLE)
      {
        release(&p->lock);
        continue;
      }

      if (p->change_q == 1)
      {
        // make the process go to a lower queue
        if (p->queue < 4)
        {
          p->queue_wait = 0;
          p->change_q = 0;
          p->queue++;
          p->cur_ticks = 0; // time run by the process in new queue is 0
        }
      }

      release(&p->lock);
    }
```

- The change_q flag is updated in usertrap and kernel trap defined in trap.c
```sh
#ifdef MLFQ
  if (which_dev == 2 && myproc() != 0 && myproc()->state == RUNNING && myproc()->cur_ticks >= (1 << myproc()->queue))
  {
    myproc()->change_q=1;
    yield();
  }
#endif
```
- We also implement aging by maintaining a time of last execution for each process and promoting processes which haven't been executed in a while.

```sh
 // checking for startvation
    // if  a process has age > 300 , then move it to a upper priority queue
    for (p = proc; p < &proc[NPROC]; p++)
    {
      acquire(&p->lock);

      if (p->state != RUNNABLE)
      {
        release(&p->lock);
        continue;
      }

      int age = ticks - p->exec_last;

      if ((age > 300) && (p->queue > 0)) // process is starving
      {
        p->queue_wait = 0;
        p->queue--;
        p->cur_ticks = 0;
      }
      release(&p->lock);
    }
```

## Q: Explain in the README how could this be exploited by  a process
-> This can be exploited as a process may give up cpu just before time slice is complete .Inn this way it will not be demoted the lower queue and will get a fresh time slice in the same queue(higher priority queue). One example of such behaviour can be when a process has small i/o bursts whose frequency is less than time slice. 

## SCHEDULER TEST RESULTS
rtime = 13 wtime =  111  rr
rtime = 32  wtime = 38 fcfs
rtime 16,  wtime 106   pbs
rtime 15,  wtime 140   mlfq

