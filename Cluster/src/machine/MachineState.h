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

#include <list>
#include "Task.h"

#ifndef MACHINESTATE_H_
#define MACHINESTATE_H_

/*
 * on a machine is possible to add a task, remove a task, check if a task is running,
 * get ram and cpu used by running tasks
 * check how much ram is possible to free by removing low priority tasks
 * */
class MachineState {
public:
    MachineState(int id);
    virtual ~MachineState();
    void addTask(Task *task);
    float getAvgRamUsage();
    float getAvgCpuUsage();
    float getAvgFreeableUsage();
    float getAvgFreeable2Usage();

    void removeTask(Task *task);
    void clearTasks();
    bool isRunning(Task *t);
    std::list<Task*> tasks;
    int id;
private:
    float cpuusage,ramusage;
    float cpuusagemax, ramusagemax;
};

#endif /* MACHINESTATE_H_ */
