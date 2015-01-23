////////////////////////////////////////////////////////////////////////////////////////////////
// NAME:
//  Scheduler.h
// DESCRIPTION:
//  This is a very simple scheduller which emulates the Arduino scheduler behavior in C
// AUTHOR:
//  Pedro Iniguez Huerta
// COPYRIGHTS:
//	Intel (r) copyrights
// DATE:
//   Sep/07/2014
// HISTORY CHANGES:
// flag Date       Name                 Description
// ---- ---------- -------------------- --------------------------------------------------------
// $A0  09/07/2014 Pedro Iniguez Huerta Created.
////////////////////////////////////////////////////////////////////////////////////////////////

#include <unistd.h>

const int MAX_TASKS = 10;

typedef void (*functiontype)();

struct Task
{
  functiontype task;
  int targetMS;
  int currentMS;
};

class Scheduler
{
private:
  struct Task list[MAX_TASKS];
  int  loopTime;

  void updateTaskInfo(int slot,functiontype task, int targetMS,int currentMS)
  {
    list[slot].task=task;
    list[slot].targetMS=targetMS;
    list[slot].currentMS=currentMS;

    //printf("task=%p, ms=%d\n",task,targetMS);
  }

public:
  Scheduler(int usTime)
  {
    loopTime = usTime;

    for(int i=0;i<MAX_TASKS;i++)
      updateTaskInfo(i,NULL,0,0);
  }
   
  void schedule(functiontype task,int ms)
  {
    for(int i=0;i<MAX_TASKS;i++)
    {
      if(list[i].task == NULL)
      {
        updateTaskInfo(i,task,ms,0);
        return;
      }
    }
  }

  void update()
  {
    // sleep loopTime Micro seconds before attempting a new verification
    usleep(loopTime);

    // check if any tasks needs to be executed
    for(int i=0;i<MAX_TASKS;i++)
    {
      if(list[i].task != NULL)
      {
        // updates ellapsed milliseconds
        list[i].currentMS++;

        // check if the tasks needs to be executed
        if(list[i].currentMS == list[i].targetMS)
        {
          // executes the task
          list[i].task();

          // remove task from the execution queue task list
          updateTaskInfo(i,NULL,0,0);
        }
      }
    }    
  }
};

