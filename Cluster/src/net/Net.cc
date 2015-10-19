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

#include "Net.h"
#include "basemsg_m.h"

Net::Net() {
    // TODO Auto-generated constructor stub

}

Net::~Net() {
    for(int i=0;i<num_machines;i++){
        delete qmachines[i];
    }
    delete[] qmachines;
    delete qscheduler;
}


void Net::initialize()
{
    cModule *cluster  = simulation.getModuleByPath("Cluster");
    num_machines = cluster->par("num_machines");
    qmachines = new cQueue*[num_machines];
    cmachines = new cChannel*[num_machines];
    for(int i=0;i<num_machines;i++)qmachines[i] = new cQueue();
    for(int i=0;i<num_machines;i++)cmachines[i] = NULL;
    qscheduler = new cQueue();
    cscheduler = NULL;
}

void Net::handleMessage(cMessage *msg)
{
    if( strcmp("freechannel",msg->getName()) ){
        BaseMsg *amsg = check_and_cast<BaseMsg*>(msg);
        int dest = amsg->getDst();
        if( dest != ID_SCHEDULER ){
            if(!cmachines[dest]) cmachines[dest] = gate("machines$o",dest)->getTransmissionChannel();
            if( !cmachines[dest]->isBusy() ){
                send(amsg,"machines$o",dest);
            }else{
                cMessage *fc = new cMessage("freechannel");
                cMsgPar *par = new cMsgPar("dest");
                par->setLongValue(dest);
                fc->addPar(par);
                if( qmachines[dest]->isEmpty() )scheduleAt(cmachines[dest]->getTransmissionFinishTime(),fc);
                qmachines[dest]->insert(msg);
            }
        }else{
            if(!cscheduler)cscheduler = gate("scheduler$o")->getTransmissionChannel();
            if( !cscheduler->isBusy() ){
                send(amsg,"scheduler$o");
            }else{
                cMessage *fc = new cMessage("freechannel");
                cMsgPar *par = new cMsgPar("dest");
                par->setLongValue(dest);
                fc->addPar(par);
                if( qscheduler->isEmpty() )scheduleAt(cscheduler->getTransmissionFinishTime(),fc);
                    qscheduler->insert(msg);
            }
        }
    }else{
        int dest = msg->par(0).longValue();
        cChannel *cdest;
        cQueue *qdest;
        char *sdest;
        if( dest==ID_SCHEDULER ){
            cdest=cscheduler;
            sdest="scheduler$o";
            qdest=qscheduler;
        }else{
            cdest=cmachines[dest];
            sdest="machines$o";
            qdest=qmachines[dest];
        }
        simtime_t t = cdest->getTransmissionFinishTime();
        if( t > simTime() ){
            scheduleAt(t,new cMessage("freechannel",dest));
        }else{
            BaseMsg *p = (BaseMsg*)qdest->pop();
            if( dest==ID_SCHEDULER )
                send(p,sdest);
            else
                send(p,sdest,dest);
            if( !qdest->isEmpty() ){
                simtime_t t = cdest->getTransmissionFinishTime();
                scheduleAt(t,new cMessage("freechannel",dest));
            }
        }
        delete msg;
    }
}
