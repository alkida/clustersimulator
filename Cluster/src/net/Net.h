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

#ifndef NET_H_
#define NET_H_

#define ID_SCHEDULER -2
#define ID_JOBARRIVAL -1
#define ID_MACHINEARRIVAL -3
#define ID_KILLARRIVAL -4


/*
 * Net is an all-to-all connection between all entities
 * a BaseMsg can be sent, specifying source and destination by their id,
 * special IDs are negative values, defined by constants
 * positive IDs are the IDs of the machines of the cluster
 *
 * this net class supports future adding of lag and bandwidth limit
 * it uses a queue for each destination
 * a self message is generated to send a message only when a channel is free
 * */
class Net : public cSimpleModule{
public:
    Net();
    virtual ~Net();
protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
private:
    cQueue **qmachines, *qscheduler;
    cChannel **cmachines, *cscheduler;
    int num_machines;
};

Define_Module(Net);

#endif /* NET_H_ */
