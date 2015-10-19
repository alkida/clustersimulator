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

#include "MachineState.h"
#include <omnetpp.h>
#include <list>
#include <algorithm>

MachineState::MachineState(int id) {
    cpuusage=0;
    ramusage=0;
    cpuusagemax = 0;
    ramusagemax = 0;
    this->id = id;
}

MachineState::~MachineState() {

}

void MachineState::addTask(Task *task){
    tasks.push_back(task);
    cpuusage += task->getCpuUsage();
    ramusage += task->getRamUsage();
    cpuusagemax += task->getRealCpuUsage();
    ramusagemax += task->getRealRamUsage();
}

void MachineState::removeTask(Task *task){
    tasks.remove(task);
    cpuusage -= task->getCpuUsage();
    ramusage -= task->getRamUsage();
    cpuusagemax -= task->getRealCpuUsage();
    ramusagemax -= task->getRealRamUsage();

}

void MachineState::clearTasks(){
    tasks.clear();
    cpuusage = 0;
    ramusage = 0;
    cpuusagemax = 0;
    ramusagemax = 0;

}


bool MachineState::isRunning(Task *t){
    return std::find(tasks.begin(),tasks.end(),t) != tasks.end();
}




float MachineState::getAvgRamUsage(){
    float s = 0;
    for(list<Task*>::iterator it=tasks.begin();it!=tasks.end();it++){
        Task *t = *it;
        s += t->getAvgRam();
    }
    return (ramusage=s);
}

float MachineState::getAvgFreeableUsage(){
    float s = 0;
    for(list<Task*>::iterator it=tasks.begin();it!=tasks.end();it++){
        Task *t = *it;
        if( t->getSchedPrio() < 2 ) s += t->getAvgRam();
    }
    return s;
}
float MachineState::getAvgFreeable2Usage(){
    float s = 0;
    for(list<Task*>::iterator it=tasks.begin();it!=tasks.end();it++){
        Task *t = *it;
        if( t->getSchedPrio() >= 2 && t->getSchedPrio() < 6 ) s += t->getAvgRam();
    }
    return s;
}

float MachineState::getAvgCpuUsage(){
    float s = 0;
    for(list<Task*>::iterator it=tasks.begin();it!=tasks.end();it++){
        Task *t = *it;
        s += t->getAvgCpu();
    }
    return (cpuusage=s);
}
