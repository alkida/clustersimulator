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


#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <omnetpp.h>
#include "Job.h"
#include "MachineAttributes.h"
#include "MachineUsage.h"

class Scheduler : public cSimpleModule{
public:
    Scheduler();
    virtual ~Scheduler();
protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    void startJob(Job *job);
private:
    void onStatsTimer();
    void onMachineState(int mid, double newram, double newcpu, double freeable, double freeable2);
    void onMachineConfig(int mid, MachineAttributes *ma);
    void onNewJob(Job *j);
    void onNewTask(Task *task);
    void onMachineMaintenance(int mid);
    void onTaskEnd(Task *t);
    void onEvict(int mid, list<Task*>*tasks);

    void checkKilled();
    int num_machines;
    int chooseMachineForTask(Task *task,bool ended, int endedon, float p1);
    void startReady(bool ended,int endedon);
    void startTaskOnMachine(Task*t,int n,bool *remstarted);
    void deleteTask(Task *t,Task *tnew);
    Task *chooseEvict(int machine);
    void newReady(Task *t);
    bool goodConstraints(Task *t);
    bool noTasksOfJob(long jid, int mid);

    //configs that machines send to the scheduler
    MachineAttributes **machineconfigs;
    //information (used resources) that scheduler believes about each machine
    MachineUsage **machineusages;
    //jobs running
    map<long,Job*> runningJobs;
    //ready queue
    list<Task*> ready;
    double cap_ram;

    //values used for logging and statistics
    int started = 0;
    double minreadyrequest = 0;
    int ended=0, killed=0, evicted=0;
    int evicted2[13] = {0};
    int new2[13] = {0};
    double waitconstr[2] = {0};
    double waitconstrn[2] = {0};

    double waitTime=0;
    long waitTimeN=0;
    long tc1=0,tc2=0;
    double lastStartReady[13000] = {0.0};
};

Define_Module(Scheduler);

#endif /* SCHEDULER_H_ */
