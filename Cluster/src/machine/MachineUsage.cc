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

#include "MachineUsage.h"

MachineUsage::MachineUsage() {
    cpu = 0;
    ram = 0;
    freeable = 0;
    freeable2 = 0;
}

MachineUsage::~MachineUsage() {
}

float MachineUsage::getRam(){
    return ram;
}

float MachineUsage::getRamFreeable(){
    return freeable;
}
float MachineUsage::getRamFreeable2(){
    return freeable2;
}


float MachineUsage::getCpu(){
    return cpu;
}


void MachineUsage::setUsage(float ram, float cpu, float freeable, float freeable2){
    this->ram = ram;
    this->cpu = cpu;
    this->freeable = freeable;
    this->freeable2 = freeable2;
}
