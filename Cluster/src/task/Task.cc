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

#include "Task.h"
#include <iostream>
#include <omnetpp.h>

Task::Task(double startTime, long jobid, long taskid, long sched_class, long sched_prio, double cpu, double ram, double disk, double duration,  double realcpu, double realram, double realdisk, double disktime, long endcause,TaskConstraints *taskconstraints, bool diffmachine) {
    this->startTime = startTime;
    this->constraints = taskconstraints;
    this->cpu = cpu;
    this->ram = ram;
    this->disk = disk;
    this->jobid = jobid;
    this->taskid = taskid;
    this->sched_class = sched_class;
    this->sched_prio = sched_prio;
    this->duration = duration;
    this->realram = realram;
    this->realcpu = realcpu;
    this->realdisk = realdisk;
    this->disktime = disktime;
    this->endcause = endcause;
    this->diffmachine = diffmachine;
    machine = -1;
    compatible = NULL;
    deleted = false;
    killed = false;
}

Task::~Task() {
    if(compatible)delete[] compatible;
}

TaskConstraints * Task::getConstraints(){
    return constraints;
}

double Task::getCpuUsage(){
    return cpu;
}
double Task::getRamUsage(){
    return ram;
}

double Task::getDiskUsage(){
    return disk;
}

long Task::getJobId(){
    return jobid;
}
long Task::getId(){
    return taskid;
}

void Task::setMachine(int n){
    if( n != -1 && compatible ){
        delete compatible;
        compatible = NULL;
    }
    machine = n;
}

int Task::getMachine(){
    return machine;
}


long Task::getSchedClass(){
    return sched_class;
}

long Task::getSchedPrio(){
    return sched_prio;
}

double Task::getDuration(){
    return duration;
}

double Task::getRealRamUsage(){
    return realram;
}


double Task::getRealCpuUsage(){
    return realcpu;
}

double Task::getRealDiskUsage(){
    return realdisk;
}
double Task::getDiskTimeUsage(){
    return disktime;
}

long Task::getEndCause(){
    return endcause;
}


void Task::clone(Task *t2){
    this->cpu = t2->cpu;
    this->ram = t2->ram;
    this->duration = t2->duration;
    this->realcpu = t2->realcpu;
    this->realram = t2->realram;
    this->endcause = t2->endcause;
    this->sched_prio = t2->sched_prio;
    this->sched_class = t2->sched_class;
    this->constraints = t2->constraints;
}

void Task::deleteConstraints(){
    constraints = new TaskConstraints();
    for(int i=0;i<13000;i++)compatible[i] = -1;
}

bool Task::getDiffConstraint(){
    return diffmachine;
}


/*
 * ram and cpu used by a task are simulated as a value that changes over time
 * initially, scheduler can't know how much resource a task will use,
 *  so the simulator can't take advantage of the real known value,
 *  and the "real" value can't be used
 *  this is simulated with a threshold time, before that the task reports the use as its requested value
 *          after the threshold time, real resource value is used
 * */

double Task::getAvgRam(){
    double cur = simTime().dbl();
    double st = startedAt;
    double rt = cur - st;
    int tshTime = 300;
    return rt > tshTime ? getRealRamUsage() : getRamUsage();

    int avgTime = 300;
    if( simTime() < avgTime )return getRealRamUsage();
    if( rt > avgTime )rt = avgTime;
    double p1 = (avgTime - rt)/avgTime;
    double p2 = 1-p1;
    return p1*getRamUsage() + p2*getRealRamUsage();
}
double Task::getAvgCpu(){
    double cur = simTime().dbl();
    double st = startedAt;
    double rt = cur - st;
    int tshTime = 300;
    return rt > tshTime ? getRealCpuUsage() : getCpuUsage();

    int avgTime = 300;
    if( simTime() < avgTime )return getRealCpuUsage();
    if( rt > avgTime )rt = avgTime;
    double p1 = (avgTime - rt)/avgTime;
    double p2 = 1-p1;
    return p1*getCpuUsage() + p2*getRealCpuUsage();
}

void Task::setStartTime(double t){
    startedAt = t;
}


