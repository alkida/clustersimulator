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

#include "MachineAttributes.h"
#include "MachineInitializer.h"
#include <omnetpp.h>
#include <fstream>
#include "MachineAttributesDistr.h"

MachineInitializer* MachineInitializer::single = 0;


MachineInitializer::MachineInitializer() {
    cluster  = simulation.getModuleByPath("Cluster");
    arrival_real = cluster->par("arrival_real");
    mad = new MachineAttributesDistr(arrival_real==0);
}

MachineInitializer::~MachineInitializer() {

}

MachineInitializer* MachineInitializer::getInstance()
{
    if(! single){
        single = new MachineInitializer();
        return single;
    }
    else{
        return single;
    }
}

MachineAttributes* MachineInitializer::configForMachine(long mid){
    return mad->attributesForMachine(mid);
}
