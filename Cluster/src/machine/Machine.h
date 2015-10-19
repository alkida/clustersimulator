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


#ifndef MACHINE_H_
#define MACHINE_H_

#include <omnetpp.h>
#include <list>
#include "MachineAttributes.h"
#include "Task.h"
#include "MachineState.h"

class Machine : public cSimpleModule{
public:
    Machine();
    virtual ~Machine();
protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
private:
    void onSendStateTimer();
    void onSendConfigTimer();
    void onNewTask(Task *task);
    void onMachineUp(double newCpu, double newRam, double newDisk);
    void onMachineUpdate(double newCpu, double newRam, double newDisk);
    void onMachineDown();
    void onDeleteTask(Task *task);
    void onEndTask(Task *task);
    //void onEvictTask(Task *task);
    void onKillTask(Task *task);

    void evictTasks(list<Task*> tlist);
    void cancelEndEvent(Task *t);
    void sendState();
    Task* chooseEvict(bool lowprio);
    void checkFreeState();
    int id;
    MachineAttributes *config;
    MachineState *state;
    list<cMessage*> endtimers;
    double cap_ram;
};

Define_Module(Machine);

#endif /* MACHINE_H_ */
