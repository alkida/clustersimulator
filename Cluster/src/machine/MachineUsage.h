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

#ifndef MACHINEUSAGE_H_
#define MACHINEUSAGE_H_


/*
 * class used by the scheduler to keep partial information about the machines
 * this class keeps information about used resources that scheduler thinks each machine has
 * */
class MachineUsage {
public:
    MachineUsage();
    virtual ~MachineUsage();
    float getRam();
    float getCpu();
    float getRamFreeable();
    float getRamFreeable2();
    void setUsage(float ram, float cpu,float freeable,float freeable2);
private:
    float cpu,ram,freeable,freeable2;
};

#endif /* MACHINEUSAGE_H_ */
