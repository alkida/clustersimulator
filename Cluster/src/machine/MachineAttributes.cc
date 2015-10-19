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
#include "TaskConstraints.h"
#include <stdio.h>
#include <iostream>

MachineAttributes::MachineAttributes() {

}

MachineAttributes::~MachineAttributes() {
}

void MachineAttributes::addAttribute(long attr, long val){
    attributes[attr] = val;
}

bool MachineAttributes::matchesConstraints(Task *t, int mid){
    if( t->compatible == NULL){
        //magic number, max number of machines
        //array used for caching reasons
        t->compatible = new char[13000];
        for(int i=0;i<13000;i++)t->compatible[i] = -1;
    }
    //if this check has been done previously, return cached result
    if( t->compatible[mid] != -1 )return t->compatible[mid] != 0;

    TaskConstraints *tc = t->getConstraints();
    if( tc == NULL )return true;
    for(list<pair<long,pair<long,long> > >::iterator it = tc->getConstraints()->begin();it != tc->getConstraints()->end();it++){
        pair<long,pair<long,long> > cur = *it;
        long attr = cur.first;
        long tval = cur.second.first;
        long cmp = cur.second.second;

        //special cases, check if an attribute exist or not
        if( tval == -1 && cmp==0 && attributes.find(attr) == attributes.end() )continue;
        if( tval == -1 && cmp==1 && attributes.find(attr) != attributes.end() )continue;

        long mval = attributes[attr];

        if( (cmp == CONSTRAINT_EQUAL && mval != tval ) ||
            (cmp == CONSTRAINT_NOT_EQUAL && mval == tval ) ||
            (cmp == CONSTRAINT_GREATER && mval <= tval ) ||
            (cmp == CONSTRAINT_LESS && mval >= tval )
        ){
            t->compatible[mid] = 0;
            return false;
        }
    }
    t->compatible[mid] = 1;
    return true;
}


void MachineAttributes::setCapacity(float cpu, float ram, float disk){
    this->cpu = cpu;
    this->ram = ram;
    this->disk = disk;
}

float MachineAttributes::getCapCpu(){
    return cpu;
}
float MachineAttributes::getCapRam(){
    return ram;
}
float MachineAttributes::getCapDisk(){
    return disk;
}
