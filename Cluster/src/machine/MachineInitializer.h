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

#ifndef MACHINEINITIALIZER_H_
#define MACHINEINITIALIZER_H_

#include "MachineAttributes.h"
#include <vector>
//#include "FreqDistr.h"
#include "MachineAttributesDistr.h"
#include <omnetpp.h>

class MachineAttributeStat{
public:
    string attr;
    vector<pair<int,float> > vals;
};

/*
 * machineinitializer gives a config for each machine requesting it
 * config can be taken from trace or from a synthesized distribution
 * */
class MachineInitializer{
public:
    static MachineInitializer* getInstance();
    virtual ~MachineInitializer();
    MachineAttributes* configForMachine(long mid);
private:
    MachineInitializer();
    static MachineInitializer *single;
    MachineAttributesDistr *mad;

    cModule *cluster;
    int arrival_real;

};


#endif
