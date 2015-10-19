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

#include "MachineAttributesDistr.h"
#include "MachineAttributes.h"
#include <omnetpp.h>
#include <fstream>

/*
 * Machine Attributes can be synthesized or taken from google traces
 * */
MachineAttributesDistr::MachineAttributesDistr(bool synth) {
    this->synth = synth;
    if( synth ){
        return;
    }
    std::ifstream f(simulation.getModuleByPath("Cluster")->par("tracemachineattributes").stdstringValue().c_str(),ifstream::in);
    int n=0;
    do{
        long mid;
        long name,val;
        f >> mid; f.ignore();
        f >> name; f.ignore();
        f >> val;
        if( !f.fail() ){
            n++;
            MachineAttributes *ma = attributes[mid];
            if( ma == NULL ){
                ma = new MachineAttributes();
                attributes[mid] = ma;
            }
            ma->addAttribute(name,val);
        }
    }while(!f.fail());
    cout << "Machine attributes: "<<n<<endl;
}

MachineAttributesDistr::~MachineAttributesDistr() {
}

/*
 * with real traces, search attributes taken from the trace
 * else, generate a random attribute for each machine
 * */
MachineAttributes* MachineAttributesDistr::attributesForMachine(long mid){
    if( synth ){
        MachineAttributes *ma = new MachineAttributes();
        ma->addAttribute(1,intuniform(0,999));
        return ma;
    }
    return attributes[mid];
}
