//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "TaskConstraints.h"

#ifndef TASK_H_
#define TASK_H_


/*
 * task has many attributes, extracted from google traces or synthesized
 * startTime : time when the task starts
 * jobid
 * taskis : unique value inside the same job
 * sched_class : unused value (used by real google scheduler in some way)
 * sched_prio : priority of the task
 * cpu,ram,dusk : cpu,ram,disk requested by the task
 * duration : duration of the task, scheduler must not use this value to take decisions
 * realcpu,realram,realdisk : real cpu,ram,disk used by the task
 * disktime : percentage of time used in i/o
 * endcause : numeric value that represents the cause of the end of the task as google represents it (2=evict, 3=fail, 4=end, 5=kill)
 * taskconstraints : constraints of the task
 * diffmachine : antiaffinity constraint
 * */
class Task {
public:
    Task(double startTime, long jobid, long taskid, long sched_class, long sched_prio, double cpu, double ram, double disk, double duration,  double realcpu, double realram, double realdisk, double disktime, long endcause,TaskConstraints *taskconstraints, bool diffmachine);
    virtual ~Task();
    TaskConstraints *getConstraints();
    double getCpuUsage();
    double getRamUsage();
    double getDiskUsage();
    long getJobId();
    long getId();
    void setMachine(int n);
    void setStartTime(double t);
    int getMachine();
    long getSchedClass();
    long getSchedPrio();
    double getDuration();
    double getRealRamUsage();
    double getRealCpuUsage();
    double getRealDiskUsage();
    double getDiskTimeUsage();
    double getAvgRam();
    double getAvgCpu();
    bool getDiffConstraint();
    long getEndCause();
    void clone(Task *t2);
    void deleteConstraints();
    char *compatible;
    double submitTime;
    double startedAt;
    bool deleted;
    bool waitingnewtask;
    double startTime;
    bool killed;
private:
    long jobid, taskid;
    TaskConstraints *constraints;
    double cpu, ram, disk;
    double realram,realcpu,realdisk,disktime;
    int machine;
    long sched_class;
    long sched_prio;
    double duration;
    long endcause;
    bool diffmachine;

};

#endif /* TASK_H_ */
