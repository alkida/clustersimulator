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
#include <omnetpp.h>
//#include "FreqDistr.h"

#ifndef MACHINEARRIVAL_H_
#define MACHINEARRIVAL_H_

class MachineArrivalInfo{
public:
    long t,mid,e;
    double cpu, ram;
    MachineArrivalInfo(long t,long mid,long e, double cpu, double ram){
        this->t = t;
        this->mid = mid;
        this->e = e;
        this->cpu = cpu;
        this->ram = ram;
    }

};

class MachineArrival : public cSimpleModule{
public:
    MachineArrival();
    virtual ~MachineArrival();
protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
private:
    void loadMachineInfo();
    std::vector<MachineArrivalInfo*> machinearrivalinfo;
    long mai_pos=0;

    cModule *cluster;
    int arrival_real;

    double machine_cpu();
    double machine_ram();
    int machine_down();
    int machine_downtime();
    void startMachines();
};

Define_Module(MachineArrival);

#endif /* MACHINEARRIVAL_H_ */
