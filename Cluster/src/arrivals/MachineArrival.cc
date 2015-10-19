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

#include "MachineArrival.h"
#include "Net.h"
#include "basemsg_m.h"
#include <fstream>
#include <omnetpp.h>
#include <string>

MachineArrival::MachineArrival() {
}

MachineArrival::~MachineArrival() {
}

/*
 * Initializer, called by omnet
 * information about machine is loaded from google traces
 * */
void MachineArrival::initialize()
{
    cluster  = simulation.getModuleByPath("Cluster");
    loadMachineInfo();
    if( arrival_real )scheduleAt(0,new cMessage("newmachineevent"));
    else{
        scheduleAt(0,new cMessage("newmachineeventsynth"));
        //in case of synthesized events machines must be started
        startMachines();
    }
}

/*
 * Messages can be
 * newmachineevent : self message, new machine event from trace
 *      action : send new event to the interested machine
 * newmachineeventsynth : self message, new machine event synthesized
 *      action : send a down event to a machine, schedule a self message to set the machine up in the future
 * machineupsynth : self message
 *      action : re set the machine up
 * */
void MachineArrival::handleMessage(cMessage *msg)
{
    if (strcmp (msg->getName(), "newmachineevent")==0){
        MachineArrivalInfo *mai = machinearrivalinfo[mai_pos];
        if( mai->e == 0 ){
            //machine is now up, send a message to the machine to wake it up, specifying the amount of ram and cpu
            BaseMsg *bmsg = new BaseMsg("machineup");
            bmsg->setDst(mai->mid);
            bmsg->setSrc(ID_MACHINEARRIVAL);
            cMsgPar *par = new cMsgPar("cpu");
            par->setDoubleValue(mai->cpu);
            bmsg->addPar(par);
            par = new cMsgPar("ram");
            par->setDoubleValue(mai->ram);
            bmsg->addPar(par);
            par = new cMsgPar("disk");
            par->setDoubleValue(1);
            bmsg->addPar(par);
            send(bmsg,"gate$o");
        }else if( mai->e == 1 ){
            //machine is down, send a message to it
            BaseMsg *bmsg = new BaseMsg("machinedown");
            bmsg->setDst(mai->mid);
            bmsg->setSrc(ID_MACHINEARRIVAL);
            send(bmsg,"gate$o");
        }else{ //e == 2
            //machine has now a different amount of cpu and ram
            BaseMsg *bmsg = new BaseMsg("machineupdate");
            bmsg->setDst(mai->mid);
            bmsg->setSrc(ID_MACHINEARRIVAL);
            cMsgPar *par = new cMsgPar("cpu");
            par->setDoubleValue(mai->cpu);
            bmsg->addPar(par);
            par = new cMsgPar("ram");
            par->setDoubleValue(mai->ram);
            bmsg->addPar(par);
            par = new cMsgPar("disk");
            par->setDoubleValue(1);
            bmsg->addPar(par);
            send(bmsg,"gate$o");
        }
        mai_pos++;
        scheduleAt(machinearrivalinfo[mai_pos]->t/1000000.0L,new cMessage("newmachineevent"));
    }

    if (strcmp (msg->getName(), "newmachineeventsynth")==0){
        //put down a random machine
        BaseMsg *bmsg = new BaseMsg("machinedown");
        int mid = intuniform(0,(int)cluster->par("num_machines")-1);
        bmsg->setDst(mid);
        bmsg->setSrc(ID_MACHINEARRIVAL);
        send(bmsg,"gate$o");
        cMessage *msg = new cMessage("machineupsynth");
        cMsgPar *par = new cMsgPar("mid");
        par->setLongValue(mid);
        msg->addPar(par);
        int downtime = machine_downtime();
        int next = machine_down();
        //schedule a self message to wake up the machine
        scheduleAt(simTime()+downtime,msg);
        //and schedule next down event
        scheduleAt(simTime()+next,new cMessage("newmachineeventsynth"));
    }

    if (strcmp (msg->getName(), "machineupsynth")==0){
        int mid = msg->par(0).longValue();
        BaseMsg *bmsg = new BaseMsg("machineup");
        bmsg->setDst(mid);
        bmsg->setSrc(ID_MACHINEARRIVAL);
        cMsgPar *par = new cMsgPar("cpu");
        par->setDoubleValue(machine_cpu());
        bmsg->addPar(par);
        par = new cMsgPar("ram");
        par->setDoubleValue(machine_ram());
        bmsg->addPar(par);
        par = new cMsgPar("disk");
        par->setDoubleValue(1);
        bmsg->addPar(par);
        send(bmsg,"gate$o");
    }

    delete msg;
}

//load machine info from google traces
void MachineArrival::loadMachineInfo(){
    arrival_real = cluster->par("arrival_real");
    if( arrival_real == 0)return;
    std::ifstream f(cluster->par("tracemachineevents").stdstringValue().c_str(),std::ifstream::in);
    do{
        long t;
        long mid;
        long e;
        double cpu, ram;
        f >> t; f.ignore();
        f >> mid; f.ignore();
        f >> e; f.ignore();
        f >> cpu; f.ignore();
        f >> ram;
        if( !f.fail() ){
            MachineArrivalInfo *mai = new MachineArrivalInfo(t,mid,e,cpu,ram);
            machinearrivalinfo.push_back(mai);
        }
    }while(!f.fail());
    std::cout << "Machine arrival: "<<machinearrivalinfo.size()<<endl;
}

//in case of synthesized traces, machines must be started, with random ram and cpu
void MachineArrival::startMachines(){
    int n = cluster->par("num_machines");
    for( int i=0;i<n;i++ ){
        BaseMsg *bmsg = new BaseMsg("machineup");
        bmsg->setDst(i);
        bmsg->setSrc(ID_MACHINEARRIVAL);
        cMsgPar *par = new cMsgPar("cpu");
        par->setDoubleValue(machine_cpu());
        bmsg->addPar(par);
        par = new cMsgPar("ram");
        par->setDoubleValue(machine_ram());
        bmsg->addPar(par);
        par = new cMsgPar("disk");
        par->setDoubleValue(1);
        bmsg->addPar(par);
        send(bmsg,"gate$o");
    }
}

//synthesized distribution extracted with Alkida Balliu's program

int intdistr(float (*f)(float), int a, int b );

float floatdistr(float (*f)(float), float a, float b );

float max(float a, float b);

float machine_down_arrival_synth (float x)
{
    return
    0.027690933 + 0.010656061 * x - 0.0053986241 * max(x -
        2.651, 0) + 0.0079427646 * max(x - 5.2889112, 0) -
        0.0062345577 * max(x - 10.551709, 0) - 0.0030062127 *
        max(x - 21.051322, 0) - 0.0015445397 * max(x - 41.998707,
        0) - 0.00081575547 * max(x - 83.790054, 0) - 0.00052459707 *
        max(x - 167.16641, 0) - 0.00051919455 * max(x - 333.50747,
        0) - 0.00040893378 * max(x - 665.36832, 0) - 0.00013642909 *
        max(x - 1327.4515, 0) - 0.0010529329 * max(x - 2648.349,
        0)
    ;
}

float machine_downtime_synth (float x)
{
    return
    -0.0021932766 + 0.00059666887 * x - 0.00051953414 * max(x -
        1049.361, 0) - 6.1173891e-05 * max(x - 1662.4853, 0) -
        1.0673976e-05 * max(x - 2633.8479, 0) + 9.9351478e-06 *
        max(x - 4172.7617, 0) + 6.6301849e-06 * max(x - 6610.8372,
        0) - 1.6098315e-05 * max(x - 10473.44, 0) - 2.2684245e-06 *
        max(x - 16592.898, 0) - 1.6990986e-06 * max(x - 26287.853,
        0) - 1.1719541e-06 * max(x - 41647.411, 0) + 2.0402986e-07 *
        max(x - 65981.304, 0) - 5.9372462e-07 * max(x - 104533.09,
        0) - 8.5747908e-08 * max(x - 165610.04, 0) - 8.6543553e-08 *
        max(x - 262373.25, 0) - 3.2011587e-08 * max(x - 415673.6,
        0) - 1.8965915e-08 * max(x - 658544.81, 0) - 3.6151531e-08 *
        max(x - 1043321.6, 0)
    ;
}

float machine_cpu_synth (float x)
{
    return
    0.009858139 + 1.1792716e-12 * x - 1.3543439e-12 * max(x -
        0.2575, 0) + 3.5426209e-13 * max(x - 0.26454302, 0) +
        2.1001172e-13 * max(x - 0.27177867, 0) - 9.2722546e-12 *
        max(x - 0.27921223, 0) + 9.533611e-11 * max(x - 0.2868491,
        0) - 1.2062738e-09 * max(x - 0.29469486, 0) + 1.0957234e-08 *
        max(x - 0.30275521, 0) - 5.6563704e-08 * max(x -
        0.31103602, 0) + 1.9106051e-07 * max(x - 0.31954333,
        0) - 4.9583356e-07 * max(x - 0.32828333, 0) + 1.2950737e-06 *
        max(x - 0.33726237, 0) - 4.6482207e-06 * max(x -
        0.34648701, 0) + 1.8423555e-05 * max(x - 0.35596395,
        0) - 5.9896229e-05 * max(x - 0.36570011, 0) + 0.00019064872 *
        max(x - 0.37570256, 0) - 0.00068616277 * max(x -
        0.38597859, 0) + 0.0024747339 * max(x - 0.39653569,
        0) - 0.01069368 * max(x - 0.40738154, 0) + 0.042369368 *
        max(x - 0.41852405, 0) - 0.19077768 * max(x - 0.42997131,
        0) + 0.67960878 * max(x - 0.44173168, 0) - 2.4794047 *
        max(x - 0.45381371, 0) + 8.991925 * max(x - 0.4662262,
        0) - 27.949359 * max(x - 0.4789782, 0) + 101.55517 *
        max(x - 0.49207897, 0) - 76.73348 * max(x - 0.50553808,
        0) - 4.6766714 * max(x - 0.51936531, 0) + 0.95256291 *
        max(x - 0.53357074, 0) - 0.23168013 * max(x - 0.54816471,
        0) + 0.061337278 * max(x - 0.56315784, 0) - 0.016015378 *
        max(x - 0.57856106, 0) + 0.0038768878 * max(x - 0.59438558,
        0) - 0.00086508548 * max(x - 0.61064293, 0) + 0.00020240402 *
        max(x - 0.62734494, 0) - 5.4065248e-05 * max(x -
        0.64450377, 0) + 1.4312367e-05 * max(x - 0.66213193,
        0) - 3.4919135e-06 * max(x - 0.68024224, 0) + 9.0540544e-07 *
        max(x - 0.6988479, 0) - 2.3449699e-07 * max(x - 0.71796244,
        0) + 6.0066626e-08 * max(x - 0.73759981, 0) - 1.4332706e-08 *
        max(x - 0.75777428, 0) + 3.7714173e-09 * max(x -
        0.77850055, 0) - 1.0565323e-09 * max(x - 0.79979372,
        0) + 2.859288e-10 * max(x - 0.82166929, 0) - 7.463207e-11 *
        max(x - 0.84414319, 0) + 1.9541789e-11 * max(x -
        0.86723179, 0) - 5.1567091e-12 * max(x - 0.89095189,
        0) + 1.3053286e-12 * max(x - 0.91532077, 0) - 4.784512e-13 *
        max(x - 0.94035618, 0) + 2.5321283e-13 * max(x -
        0.96607635, 0) + 8.4528866 * max(x - 0.9925, 0)
    ;
}
float machine_ram_synth (float x)
{
    return
    0.00037276227 + 0.00090680995 * x - 0.0014279249 * max(x -
        0.03181915, 0) + 0.0018922918 * max(x - 0.037803594,
        0) - 0.0052864761 * max(x - 0.044913573, 0) + 0.016155698 *
        max(x - 0.053360774, 0) - 0.022299958 * max(x - 0.063396698,
        0) + 0.043054136 * max(x - 0.075320146, 0) - 0.12920571 *
        max(x - 0.089486119, 0) + 0.38088753 * max(x - 0.10631638,
        0) - 0.48349633 * max(x - 0.12631203, 0) + 0.96191847 *
        max(x - 0.15006839, 0) - 3.0115191 * max(x - 0.17829277,
        0) + 8.8403163 * max(x - 0.21182551, 0) - 3.7698047 *
        max(x - 0.25166497, 0) - 2.8367241 * max(x - 0.29899731,
        0) - 1.6227833 * max(x - 0.35523177, 0) + 6.734766 *
        max(x - 0.42204263, 0) - 2.2083832 * max(x - 0.50141906,
        0) - 3.4237194 * max(x - 0.59572435, 0) + 1.3148088 *
        max(x - 0.70776628, 0) - 0.93747508 * max(x - 0.8408807,
        0) + 74.045282 * max(x - 0.99903085, 0)
    ;


}

double MachineArrival::machine_cpu(){
    double n = cluster->par("distr_machine_cpu");
    if( n < 0 )n = floatdistr(machine_cpu_synth,0.25,1);
    return n;
}

double MachineArrival::machine_ram(){
    double n = cluster->par("distr_machine_ram");
    if( n < 0 )n = floatdistr(machine_ram_synth,0.03085,1);
    return n;
}



int MachineArrival::machine_down(){
    int n = cluster->par("distr_machine_down");
    if( n < 0 ){
        n = intdistr(machine_down_arrival_synth,0,2651);
    }
    return n;
}

int MachineArrival::machine_downtime(){
    int n = cluster->par("distr_machine_downtime");
    if( n < 0 ){
        n = intdistr(machine_downtime_synth,5,1044366);
    }
    return n;
}
