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

#include "Machine.h"
#include "MachineInitializer.h"
#include "basemsg_m.h"
#include "Net.h"
#include "Job.h"
#include <algorithm>
#include "MachineState.h"

Machine::Machine() {
}

Machine::~Machine() {
}

/*
 * initialize is called by omnet
 * a config is taken and a state is initialized
 * periodically the machine sends a state to the scheduler
 * google machines don't use all ram, but only a percentage, cap_ram
 * */
void Machine::initialize()
{
    cModule *cluster  = simulation.getModuleByPath("Cluster");
    cap_ram = cluster->par("ram_machine");
    id = (int)par("id");
    config = MachineInitializer::getInstance()->configForMachine(id);
    state = new MachineState(id);
    scheduleAt(10,new cMessage("sendstate"));
}

/*
 * Messages can be
 * sendstate : self message
 *          action: send current state to the scheduler
 * newtask : message from the scheduler
 *          action: set the task running on this machine
 * machineup/machineupdate : message from MachineArrival
 *          action: save new cpu and ram specified in the message
 * machinedown : message from MachineArrival
 *          action: evict all tasks and go in maintenance mode
 * delete : message from scheduler
 *          action: a task is restarted (in the trace) and the machine must delete this task and remove its generated end event
 * endtask : self message
 *          action: a task ends, so scheduler must be alerted
 * */
void Machine::handleMessage(cMessage *msg)
{
    if( !strcmp(msg->getName(),"sendstate")){
        onSendStateTimer();
    }
    if( !strcmp(msg->getName(),"newtask") ){
        Task *task = (Task*)msg->par(0).pointerValue();
        onNewTask(task);
    }
    if( !strcmp(msg->getName(),"machineup") ){
        onMachineUp(msg->par(0).doubleValue(),msg->par(1).doubleValue(),msg->par(2).doubleValue());
    }
    if( !strcmp(msg->getName(),"machineupdate") ){
        onMachineUpdate(msg->par(0).doubleValue(),msg->par(1).doubleValue(),msg->par(2).doubleValue());
    }
    if( !strcmp(msg->getName(),"machinedown") ){
        onMachineDown();
    }

    if( !strcmp(msg->getName(),"delete") ){
        BaseMsg *bmsg = (BaseMsg*)msg;
        Task *task = (Task*)bmsg->par(0).pointerValue();
        onDeleteTask(task);
    }

    if(!strcmp(msg->getName(),"endtask")){
        endtimers.remove(msg);
        Task *task = (Task*)msg->par(0).pointerValue();
        onEndTask(task);
    }

    /*
    if( !strcmp(msg->getName(),"evict") ){
        BaseMsg *bmsg = (BaseMsg*)msg;
        Task *task = (Task*)bmsg->par(0).pointerValue();
        onEvictTask(task);
    }*/

    delete msg;
}


void Machine::onSendStateTimer(){
    if( config ){
        checkFreeState();
        sendState();
    }
    scheduleAt(simTime()+10,new cMessage("sendstate"));
}


void Machine::onNewTask(Task *task){
    task->setStartTime(simTime().dbl());
    //add task to running tasks of this machine
    state->addTask(task);
    //if it's instantly killed, end it
    if( task->getEndCause()!=4 && task->startTime + task->getDuration() < simTime().dbl() ){
        onEndTask(task);
        return;
    }
    //self schedule an end event
    cMessage *cmsg = new cMessage("endtask");
    cMsgPar *par = new cMsgPar("task");
    par->setPointerValue(task);
    cmsg->addPar(par);
    //tasks that are killed have a fixed end time
    //tasks that terminates normally have a fixed running time
    scheduleAt((task->getEndCause()!=4?task->startTime:simTime())+task->getDuration(),cmsg);
    endtimers.push_back(cmsg);
    //check if overloaded
    checkFreeState();
}

void Machine::onMachineDown(){
    //in case of maintenance evict all tasks
    BaseMsg *msg = new BaseMsg("maintenance");
    msg->setSrc(id);
    msg->setDst(ID_SCHEDULER);
    send(msg,"gate$o");
    evictTasks(state->tasks);
}

void Machine::onMachineUpdate(double newCpu, double newRam, double newDisk){
    //in case of update set new capacity and check if overloaded
    onMachineUp(newCpu,newRam,newDisk);
    checkFreeState();
}

void Machine::onMachineUp(double newCpu, double newRam, double newDisk){
    //in case of wake up, set capacity and alert the scheduler of the machine presence
    BaseMsg *msg = new BaseMsg("machineconfig");
    cMsgPar *par = new cMsgPar("machine");
    config->setCapacity(newCpu,newRam,newDisk);
    par->setPointerValue(config);
    msg->addPar(par);
    msg->setSrc(id);
    msg->setDst(ID_SCHEDULER);
    send(msg,"gate$o");
}


void Machine::onDeleteTask(Task *task){
    state->removeTask(task);
    cancelEndEvent(task);
}

void Machine::onEndTask(Task *task){
    //if a task terminates scheduler must be alerted
    if( state->isRunning(task) ){
        state->removeTask(task);
        BaseMsg *bmsg = new BaseMsg("endtask");
        cMsgPar *par = new cMsgPar("task");
        par->setPointerValue(task);
        bmsg->addPar(par);
        bmsg->setSrc(id);
        bmsg->setDst(ID_SCHEDULER);
        send(bmsg,"gate$o");
    }
}
/*
void Machine::onEvictTask(Task *task){
    state->removeTask(task);
    cancelEndEvent(task);
}*/



void Machine::evictTasks(list<Task*> tlist){
    //delete end events of evicted tasks
    //and send the list of evicted tasks to the scheduler
    list<Task*> *temp = new list<Task*>();
    for(list<Task*>::iterator it = tlist.begin();it!=tlist.end();it++){
        cancelEndEvent(*it);
        if( state->isRunning(*it)){
            temp->push_back(*it);
            state->removeTask(*it);
        }
    }
    BaseMsg *bmsg = new BaseMsg("evictlist");
    cMsgPar *par = new cMsgPar("task");
    par->setPointerValue(temp);
    bmsg->addPar(par);
    bmsg->setSrc(id);
    bmsg->setDst(ID_SCHEDULER);
    send(bmsg,"gate$o");
    sendState();
}

//evict tasks with lowest priority
//in case of equal priority evict newest tasks
Task* Machine::chooseEvict(bool lowprio){
    float max = -1;
    Task *tkill = NULL;
    for(list<Task*>::iterator it=state->tasks.begin();it!=state->tasks.end();it++){
        Task *t = *it;
        float temp = 12-t->getSchedPrio();
        if( temp >= max && (tkill == NULL || tkill->startedAt < t->startedAt)){
            max = temp;
            tkill = t;
        }
    }
    return tkill;
}

void Machine::cancelEndEvent(Task *t){
    for(list<cMessage*>::iterator it=endtimers.begin();it!=endtimers.end();){
        cMessage *cur = *it;
        if( cur->par(0).pointerValue() == t ){
            cancelEvent(cur);
            delete cur;
            it=endtimers.erase(it);
        }else it++;
    }
}


void Machine::checkFreeState(){
    //check if overloaded, in that case evict some tasks
    while( state->getAvgRamUsage() > config->getCapRam()*cap_ram || state->getAvgCpuUsage() > config->getCapCpu() ){
        list<Task*> t;
        t.push_back(chooseEvict(false));
        evictTasks(t);
    }
}

void Machine::sendState(){
    //send periodically to the scheduler cpu usage, ram usage,
    //ram usage by lowest priority group, ram usage by second lowest priority group
    BaseMsg *msg = new BaseMsg("state");
    cMsgPar *par = new cMsgPar("ram");
    par->setDoubleValue(state->getAvgRamUsage());
    msg->addPar(par);
    par = new cMsgPar("cpu");
    par->setDoubleValue(state->getAvgCpuUsage());
    msg->addPar(par);
    par = new cMsgPar("freeable");
    par->setDoubleValue(state->getAvgFreeableUsage());
    msg->addPar(par);
    par = new cMsgPar("freeable2");
    par->setDoubleValue(state->getAvgFreeable2Usage());
    msg->addPar(par);

    msg->setSrc(id);
    msg->setDst(ID_SCHEDULER);
    send(msg,"gate$o");
}
