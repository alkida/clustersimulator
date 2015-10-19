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

#ifndef MACHINECONFIGURATION_H_
#define MACHINECONFIGURATION_H_

#include <map>
#include <utility>
#include <string>
#include "TaskConstraints.h"
#include "Task.h"

using namespace std;

/*
 * machine has an amount of cpu, ram and disk
 * and it's possible to check if a machine matches the constraints of a task
 * */
class MachineAttributes {
public:
    MachineAttributes();
    virtual ~MachineAttributes();
    void addAttribute(long attr, long val);
    bool matchesConstraints(Task *t, int mid);
    void setCapacity(float cpu, float ram,float disk);
    float getCapCpu();
    float getCapRam();
    float getCapDisk();
private:
    map<long,long> attributes;
    float cpu, ram,disk;
};

#endif /* MACHINECONFIGURATION_H_ */
