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

#include "TaskConstraintsDistr.h"
#include <fstream>

/*
 * Task constraints can be synthesized or taken from google traces
 * */
TaskConstraintsDistr::TaskConstraintsDistr(bool synth, double probconstr, double probsat) {
    this->synth = synth;
    if( synth ){
        //probability that a task has constraints
        this->probconstr = probconstr;
        //probability that a constraint is satisfable by a machine
        this->probsat = probsat;
        return;
    }
    std::ifstream f(simulation.getModuleByPath("Cluster")->par("tracetaskconstraints").stdstringValue().c_str(),ifstream::in);
    int n=0;
    do{
        long jid,tid;
        long comp, name,val;
        f >> jid; f.ignore();
        f >> tid; f.ignore();
        f >> comp; f.ignore();
        f >> name; f.ignore();
        f >> val;
        if( !f.fail() ){
            n++;
            TaskConstraints *tc;
            if( constraints.find(pair<long,long>(jid,tid)) == constraints.end() ){
                tc = new TaskConstraints();
                constraints[pair<long,long>(jid,tid)] = tc;
            }else{
                tc = constraints[pair<long,long>(jid,tid)];
            }
            tc->addConstraint(name,val,comp);
        }
    }while(!f.fail());
    cout << "Task constraints: "<<n<<endl;
}

TaskConstraintsDistr::~TaskConstraintsDistr() {
}

TaskConstraints* TaskConstraintsDistr::constraintsForTask(long jobid, long taskid){
    if( synth ){
        if( uniform(0,1) < probconstr )return NULL;
        TaskConstraints *tc = new TaskConstraints();
        int n = intuniform(0,999);
        //generate a constraint as an interval, n <= machineattribute <= n+delta , size of delta depends on the probability that a constraint is satisfable
        tc->addConstraint(1,n,3);
        tc->addConstraint(1,n+probsat*1000,2);
        return tc;
    }
    return constraints[pair<long,long>(jobid,taskid)];
}


