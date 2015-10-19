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

#include "Scheduler.h"
#include "Net.h"
#include "Machine.h"
#include <string>
#include <iterator>
#include <algorithm>
#include "basemsg_m.h"

Scheduler::Scheduler() {
}

Scheduler::~Scheduler() {
}


/*
 * initialize is called by omnet
 * variables are initialized,
 * a timer is started to print stats periodically
 * a timer is started to try to start tasks from ready queue periodically
 * a timer is started to check periodically if there are killed tasks in the ready queue
 * */
void Scheduler::initialize()
{
    cModule *cluster  = simulation.getModuleByPath("Cluster");
    num_machines = cluster->par("num_machines");
    cap_ram = cluster->par("ram_sched");
    machineconfigs = new MachineAttributes*[num_machines];
    for(int i=0;i<num_machines;i++)machineconfigs[i] = NULL;
    machineusages = new MachineUsage*[num_machines];
    for(int i=0;i<num_machines;i++)machineusages[i] = new MachineUsage();
    scheduleAt(450,new cMessage("stats"));
    scheduleAt(simTime()+400,new cMessage("startready"));
    scheduleAt(simTime()+100,new cMessage("checkkilled"));
    FILE *f = fopen("log.txt","a");
    fprintf(f,"cap_ram: %lf\n",cluster->par("ram_sched").doubleValue());
    fclose(f);
}

/*
 * Messages can be
 * stats : self message
 *          action: print stats
 * startready : self message
 *          action : start tasks in ready queue
 * checkkilled : self message
 *          action : check if there are killed tasks in ready queue
 * state : sent from a machine
 *          action : save new information about current machine usage
 * machineconfig : sent from a machine
 *          action : save new resource information about machines
 * newjob : sent from jobarrival
 *          action : schedule start of each task
 * newtask : self message
 *          action : start arrived task
 * maintenance : sent from a machine
 *          action : stop sending tasks to this machine
 * endtask : sent from a machine
 *          action : a task is terminated, check if it's the last task of the job and in that case remove the job
 *                   start new tasks on the machine
 * evictlist : sent from a machine
 *          action : put evicted tasks in the ready queue
 * */
void Scheduler::handleMessage(cMessage *msg)
{
    if(!strcmp(msg->getName(),"stats")){
        onStatsTimer();
    }
    if(!strcmp(msg->getName(),"startready")){
        startReady(false,0);
        scheduleAt(simTime()+400,new cMessage("startready"));
    }
    if(!strcmp(msg->getName(),"checkkilled")){
        checkKilled();
        scheduleAt(simTime()+100,new cMessage("checkkilled"));
    }
    if(!strcmp(msg->getName(),"state")){
        BaseMsg *bmsg = (BaseMsg*)msg;
        int src = bmsg->getSrc();
        double ram = bmsg->par(0).doubleValue();
        double cpu = bmsg->par(1).doubleValue();
        double freeable = bmsg->par(2).doubleValue();
        double freeable2 = bmsg->par(3).doubleValue();
        onMachineState(src,ram,cpu,freeable,freeable2);
    }
    if(!strcmp(msg->getName(),"machineconfig")){
        BaseMsg *bmsg = (BaseMsg*)msg;
        MachineAttributes *machine = (MachineAttributes*)msg->par(0).pointerValue();
        onMachineConfig(bmsg->getSrc(),machine);
    }
    if(!strcmp(msg->getName(),"newjob")){
        BaseMsg *bmsg = (BaseMsg*)msg;
        Job * j = (Job*)bmsg->par(0).pointerValue();
        onNewJob(j);
    }

    if(!strcmp(msg->getName(),"newtask")){
        Task *task = (Task*)msg->par(0).pointerValue();
        onNewTask(task);
    }

    if(!strcmp(msg->getName(),"maintenance")){
        BaseMsg *bmsg = (BaseMsg*)msg;
        int src = bmsg->getSrc();
        onMachineMaintenance(src);
    }

    if(!strcmp(msg->getName(),"endtask")){
        Task *t = (Task*)msg->par(0).pointerValue();
        if( t->getMachine() != -1 )onTaskEnd(t);
    }

    if( !strcmp(msg->getName(),"evictlist")){
        BaseMsg *bmsg = (BaseMsg*)msg;
        int src = bmsg->getSrc();
        list<Task*> *tasks = (list<Task*> *)msg->par(0).pointerValue();
        onEvict(src,tasks);
    }

    delete msg;
}



void Scheduler::onStatsTimer(){
    /*for(int i=0;i<num_machines;i++){
        if( !machineconfigs[i])continue;
        int ram = machineusages[i]->getRam() * 10000;
        int cpu = machineusages[i]->getCpu() * 10000;
    }*/

    //count running tasks
    int running=0;
    for(map<long,Job*>::iterator it=runningJobs.begin();it!=runningJobs.end();it++){
        Job *job = (*it).second;
        for(list<Task*>::iterator it2=job->getTasks()->begin();it2!=job->getTasks()->end();it2++){
            Task *t =  *it2;
            if( t->getMachine() != -1 )running++;
        }
    }
    //count total tasks
    int total = 0;
    for(map<long,Job*>::iterator it=runningJobs.begin();it!=runningJobs.end();it++){
        Job *job = (*it).second;
        total += it->second->getTasks()->size();
    }
    printf("Time/Ready/N.Evict/Ended/Running: %ld %ld %d %d %d\n",(long)simTime().dbl(),ready.size(),evicted,ended,running);
    FILE *f = fopen("log.txt","a");
    //log current time, size of ready queue, evicted tasks, ended tasks, running tasks, total tasks, killed tasks
    fprintf(f,"%ld %ld %d %d %d %d %d\n",(long)simTime().dbl(),ready.size(),evicted,ended,running,total,killed);
    fclose(f);
    //print wait time in case of different priority and constraints requested
    f = fopen("waits.txt","a");
    fprintf(f,"Time: %lf\n",simTime().dbl());
    if( waitconstrn[0] && waitconstrn[1] )fprintf(f,"Wait non constr: %lf\nWait constr: %lf\n",waitconstr[0]/waitconstrn[0],waitconstr[1]/waitconstrn[1]);
    fclose(f);
    for(int i=0;i<13;i++)evicted2[i]=0;
    for(int i=0;i<13;i++)new2[i]=0;
    ended=killed=evicted=0;
    waitTime=waitTimeN=0;
    if(tc1+tc2)cout << (1.0*tc1/(tc1+tc2))<<endl;
    cout << endl;
    scheduleAt(simTime()+450,new cMessage("stats"));
}

void Scheduler::onMachineState(int mid, double newram,double newcpu, double newfreeable, double newfreeable2){
    machineusages[mid]->setUsage(newram,newcpu,newfreeable,newfreeable2);
    //startReady(true,mid);
}

void Scheduler::onMachineConfig(int mid, MachineAttributes *ma){
    machineconfigs[mid] = ma;
    cout << ma->getCapCpu()<<" " <<ma->getCapRam()<<endl;
    startReady(true,mid);
}

void Scheduler::onNewJob(Job *j){
    EV << "New job "<<j->getId()<<endl;
    startJob(j);
}

void Scheduler::onNewTask(Task *task){
    new2[task->getSchedPrio()]++;
    task->waitingnewtask = false;
    if( task->getConstraints() )tc1++;
    else tc2++;

    started++;
    if(started%1000==0)cout <<"Started: "<<started<<endl;

    task->submitTime = simTime().dbl();
    //avoid unsatisfable constraints, should never happen if constraints are well generated
    if( ! goodConstraints(task) ){
        task->deleteConstraints();
    }
    int n = chooseMachineForTask(task,false,0,1);
    if( n != -1 ){
        startTaskOnMachine(task,n,NULL);
    }else{
        newReady(task);
    }
}

void Scheduler::onMachineMaintenance(int mid){
    machineconfigs[mid] = NULL;
}

void Scheduler::onTaskEnd(Task *t){
    if(t->getEndCause() == 4)ended++;
    else killed++;

    long jid = t->getJobId();
    if( runningJobs.find(jid) != runningJobs.end() ){
        Job *j = runningJobs[jid];
        j->getTasks()->remove(t);
        if( j->getTasks()->size() == 0 ){
            runningJobs.erase(jid);
            delete j;
        }
        int machine = t->getMachine();
        if( machine != -1 )startReady(true,machine);
    }

    delete t;
}


void Scheduler::onEvict(int mid, list<Task*>*tasks){
    for(list<Task*>::iterator it = tasks->begin();it!=tasks->end();it++){
        Task *t = *it;
        evicted++;
        evicted2[t->getSchedPrio()]++;
        new2[t->getSchedPrio()]++;
        newReady(t);
    }
    delete tasks;
}


void Scheduler::deleteTask(Task *t,Task *tnew){
    int machine = t->getMachine();
    if( machine != -1 ){
        BaseMsg *bmsg = new BaseMsg("delete");
        bmsg->setSrc(ID_SCHEDULER);
        bmsg->setDst(machine);
        cMsgPar *par = new cMsgPar("task");
        par->setPointerValue(t);
        bmsg->addPar(par);
        send(bmsg,"gate$o");
        t->clone(tnew);
        delete tnew;
    }
}


void Scheduler::startJob(Job *job){
    list<Task*> *tasks = job->getTasks();

    if( runningJobs[job->getId()] == NULL ){
        //first time this job is seed
        runningJobs[job->getId()] = job;
    }else{
        job = runningJobs[job->getId()];
        map<long,Task*> running;
        //save running tasks of this job
        for(list<Task*>::iterator it=job->getTasks()->begin();it!=job->getTasks()->end();it++){
            Task *t = *it;
            running[t->getId()] = t;
        }
        //chech if new tasks of this job are running with the same id
        for(list<Task*>::iterator it=tasks->begin();it!=tasks->end();){
            Task *t = *it;
            Task *t2 = running[t->getId()];
            if( t2 ){
                //in this case, remove the old task
                deleteTask(t2,t);
                ready.remove(t2);
                if( !t2->waitingnewtask )newReady(t2);
                it = tasks->erase(it);
            }else it++;
        }
        //add new tasks to the job
        for(list<Task*>::iterator it=tasks->begin();it!=tasks->end();it++){
            Task *t = *it;
            job->getTasks()->push_back(t);
        }
    }
    int i=0;
    //start new tasks
    for(list<Task*>::iterator it=tasks->begin();it!=tasks->end();it++){
        Task *t = *it;
        t->waitingnewtask = true;
        t->setMachine(-1);
        cMessage *msg = new cMessage("newtask");
        cMsgPar *par = new cMsgPar("task");
        par->setPointerValue(t);
        msg->addPar(par);
        scheduleAt(simTime()+(i+1)*0.001,msg);
        i++;
    }
}

void Scheduler::startTaskOnMachine(Task*task,int n,bool *remstarted){
    if( simTime() > 3600*5 ){
        waitconstr[task->getConstraints()?1:0] += simTime().dbl() - task->submitTime;
        waitconstrn[task->getConstraints()?1:0]++;
    }

    //set new machine state
    machineusages[n]->setUsage(
            machineusages[n]->getRam()+task->getRamUsage(),
            machineusages[n]->getCpu()+task->getCpuUsage(),
            machineusages[n]->getRamFreeable()+(task->getSchedPrio()>=2?0:task->getRamUsage()),
            machineusages[n]->getRamFreeable2()+(task->getSchedPrio()>=2&&task->getSchedPrio()<6?task->getRamUsage():0)
    );
    task->setMachine(n);
    BaseMsg *bmsg = new BaseMsg("newtask");
    bmsg->setSrc(ID_SCHEDULER);
    bmsg->setDst(n);
    cMsgPar *par = new cMsgPar("task");
    par->setPointerValue(task);
    bmsg->addPar(par);
    send(bmsg,"gate$o");
}

void Scheduler::newReady(Task *t){
    t->setMachine(-1);
    t->submitTime = simTime().dbl();
    if( minreadyrequest > t->getRamUsage() )minreadyrequest = t->getRamUsage();
    ready.push_back(t);
}

void Scheduler::startReady(bool ended,int endedon){

    if( ended ){
        if( simTime() - lastStartReady[endedon]  < 25 )return;
        lastStartReady[endedon] = simTime().dbl();
    }else{
        for(int i=0;i<13000;i++)lastStartReady[i] = simTime().dbl();
    }
    float p1 = 1;
    float p2 = 1;

    for(list<Task*>::iterator it=ready.begin();it!=ready.end();){
        Task *task = *it;
        int n = uniform(0,1)<(task->getSchedPrio()<2?0.1:p2) ? chooseMachineForTask(task,ended,endedon,p1) : -1;
        if( n != -1 ){
            it=ready.erase(it);
            bool t = false;
            startTaskOnMachine(task,n,&t);
        }else{
            it++;
        }
    }

}

void Scheduler::checkKilled(){
    for(list<Task*>::iterator it=ready.begin();it!=ready.end();){
        Task *task = *it;
        if( task->startTime + task->getDuration() <= simTime().dbl() ){
            onTaskEnd(task);
            it=ready.erase(it);
        }else it++;
    }
}

bool Scheduler::goodConstraints(Task *t){
    for(int i=0;i<num_machines;i++){
        if( machineconfigs[i] && machineconfigs[i]->matchesConstraints(t,i) ){
            return true;
        }
    }
    return false;
}

bool Scheduler::noTasksOfJob(long jid, int mid){
    Job *job = runningJobs[jid];
    for(list<Task*>::iterator it=job->getTasks()->begin();it!=job->getTasks()->end();it++){
        Task *t =  *it;
        if( t->getMachine() == mid )return false;
    }
    return true;
}

int Scheduler::chooseMachineForTask(Task *task, bool ended, int endedon, float p1){
    int n = -1;
    float freeram = -1;
    float lowestprio = -1;
    int nlowestprio = -1;
    if(!ended){
        //try a random machine
        int test = intuniform(0,num_machines-1);
        if( machineconfigs[test]
                && machineusages[test]->getRam() -
                (task->getSchedPrio() <=1 ? 0 : machineusages[test]->getRamFreeable() )*0.66 -
                (task->getSchedPrio() < 6 ? 0 : machineusages[test]->getRamFreeable2())*0.66
                + task->getRamUsage()/p1 < machineconfigs[test]->getCapRam()*cap_ram
                && machineusages[test]->getCpu() + task->getCpuUsage()/p1 < machineconfigs[test]->getCapCpu()
                && machineconfigs[test]->matchesConstraints(task,test)
                && (!task->getDiffConstraint() || noTasksOfJob(task->getJobId(),test) )
                ){
            return test;
        }
    }
    //look all machines
    for(int j=ended?endedon:0;j<num_machines;j++){
        int i = ended?j: intuniform(0,num_machines-1);
        if( machineconfigs[i]
                && machineusages[i]->getRam() -
                (task->getSchedPrio() <=1 ? 0 : machineusages[i]->getRamFreeable() )*0.66 -
                (task->getSchedPrio() < 6 ? 0 : machineusages[i]->getRamFreeable2())*0.66
                + task->getRamUsage()/p1 < machineconfigs[i]->getCapRam()*cap_ram
                && machineusages[i]->getCpu() + task->getCpuUsage()/p1 < machineconfigs[i]->getCapCpu()
                && machineconfigs[i]->matchesConstraints(task,i)
                && (!task->getDiffConstraint() || noTasksOfJob(task->getJobId(),i) )
                ){
            return i;
        }
        if( ended )break;
    }
    return n;
}



