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



#ifndef JOBARRIVAL_H_
#define JOBARRIVAL_H_

#include <omnetpp.h>
#include "Task.h"
#include <vector>
//#include "FreqDistr.h"
#include "Job.h"
#include "TaskConstraintsDistr.h"

class TaskConstrStat{
public:
    string constr;
    vector<pair<pair<int,int>,float> > vals;
};

class TaskInfo{
    public:
    TaskInfo(){
    }
    TaskInfo(long t, long jid, long tid, long sched, long prio, double cpu, double ram, double disk, long duration, long e, double realcpu, double realram, double realdisk, double disktime,bool constr_diff){
        this->t=t;
        this->jid=jid;
        this->tid=tid;
        this->sched=sched;
        this->prio=prio;
        this->cpu=cpu;
        this->ram=ram;
        this->disk=disk;
        this->e = e;
        this->duration = duration;
        this->realcpu = realcpu;
        this->realram = realram;
        this->realdisk = realdisk;
        this->disktime = disktime;
        this->constr_diff = constr_diff;
    }
    long t,jid,tid,sched,prio;
    double cpu,ram,disk;
    long duration,e;
    double realcpu,realram,realdisk,disktime;
    bool constr_diff;
};

class JobArrival : public cSimpleModule{
public:
    JobArrival();
    virtual ~JobArrival();
protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
private:
    void loadTaskInfo();
    std::vector<TaskInfo*> taskinfo;
    int ti_pos;
    cModule *cluster;
    TaskConstraintsDistr *tcd;

    int arrival_real;
    int initialtasks;
    double prob_longrunning;
    double prob_abnormalend;
    int started = 0;

    int curjid = 0;
    int job_arrival();
    int taskperjob();
    double task_cpu();
    double task_ram();
    int task_prio();
    int task_duration_end();
    int task_duration_kill();

    double time_max;

};

Define_Module(JobArrival);

#endif /* JOBARRIVAL_H_ */
