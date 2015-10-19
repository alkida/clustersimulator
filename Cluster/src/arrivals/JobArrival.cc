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

#include "JobArrival.h"
#include "Net.h"
#include "basemsg_m.h"
#include <fstream>
#include "TaskConstraintsDistr.h"

JobArrival::JobArrival() {

}

/*
 * class destructor
 * deletion of TaskInfo objects
 * */
JobArrival::~JobArrival() {
    while(!taskinfo.empty()){
        TaskInfo *t = taskinfo.back();
        taskinfo.pop_back();
        delete t;
    }
}

/*
 * class initializer, called by omnet
 * information about tasks and jobs (and its constraints) is loaded
 * parameters from configuration file is loaded
 */
void JobArrival::initialize()
{
    scheduleAt(1,new cMessage("newjob"));
    taskinfo.clear();
    cluster  = simulation.getModuleByPath("Cluster");
    time_max = cluster->par("time_max");
    loadTaskInfo();
    ti_pos =0 ;
    initialtasks = cluster->par("distr_initialtasks");
    prob_longrunning = cluster->par("distr_prob_longrunning");
    prob_abnormalend = cluster->par("distr_prob_abnormalend");
    tcd = new TaskConstraintsDistr(arrival_real==0,(double)cluster->par("distr_task_prob_constraint"),(double)cluster->par("distr_machine_prob_sat_constraint"));
}

/*
 * Messages can be
 * newjob : self message
 *          action: generate a new job and send it to the scheduler
 * */
void JobArrival::handleMessage(cMessage *msg)
{
    //new job when using real google traces
    if (strcmp (msg->getName(), "newjob")==0 && arrival_real ){
        int n=0;
        long jid = taskinfo[ti_pos]->jid;
        //how many tasks there are for this job
        while( ti_pos + n < taskinfo.size() && taskinfo[ti_pos+n]->jid == jid )n++;

        Job *job = new Job(jid,n);
        long t = taskinfo[ti_pos]->t;
        //loading all its tasks
        for(int i=0;i<n;i++){
            TaskInfo ti = *taskinfo[ti_pos+i];
            bool found = false;
            for(int j=0;j<i&&!found;j++){
                if( taskinfo[ti_pos+j]->tid == taskinfo[ti_pos+i]->tid )found = true;
            }
            //if there is the same task twice, treat it as a different job event
            if(found)continue;
            //load constraints and duration
            TaskConstraints *tc = tcd->constraintsForTask(ti.jid,ti.tid);
            long duration = ti.duration;
            //avoid time outside the trace (google uses 2^64-1 for these events)
            if( duration > 30L*24*3600*1000000 )duration = 30L*24*3600*1000000;
            Task *t = new Task(ti.t/1000000,ti.jid,ti.tid,ti.sched,ti.prio,ti.cpu,ti.ram,ti.disk,duration/1000000.0,ti.realcpu,ti.realram,ti.realdisk,ti.disktime,ti.e,tc,ti.constr_diff);
            job->getTasks()->push_back(t);
       }

        //send the job to the scheduler
        BaseMsg *bmsg = new BaseMsg("newjob");
        bmsg->setDst(ID_SCHEDULER);
        bmsg->setSrc(ID_JOBARRIVAL);
        cMsgPar *par = new cMsgPar("job");
        par->setPointerValue(job);
        bmsg->addPar(par);
        send(bmsg,"gate$o");

        //jump to next job
        ti_pos += n;
        if( simTime() > time_max || ti_pos >= taskinfo.size() )throw cTerminationException("END");
        //schedule next arrival event
        scheduleAt(taskinfo[ti_pos]->t/1000000.0L,new cMessage("newjob"));
    }

    //new job when using synthesized traces
    if (strcmp (msg->getName(), "newjob")==0 && !arrival_real ){
        //random number of tasks
        int n = taskperjob();
        long jid = ++curjid;
        Job *job = new Job(jid,n);
        for(int i=0;i<n;i++){
                TaskConstraints *tc = tcd->constraintsForTask(jid,i);
                int e;
                long duration;
                //long running tasks are started when the simulation starts, these tasks stays running for the entire simulation
                if( started < initialtasks && uniform(0,1) < prob_longrunning ){
                    e=5;
                    duration=30L*24*3600;
                }else{
                    //a task can terminate normally, or killed by the user
                    e = uniform(0,1)<prob_abnormalend?4:5;
                    if( e == 4) duration = task_duration_end();
                    else duration = task_duration_kill();
                }
                //random task and cpu
                float cpu = task_cpu();
                float ram = task_ram();
                //a task can have the antiaffinity constraint, tasks of the same job with this constraint must be executed on different machines
                bool diffconstr = uniform(0,1) < cluster->par("distr_task_prob_antiaffinity").doubleValue();
                Task *t = new Task((long)simTime().dbl(),jid,i,0,task_prio(),cpu,ram,0,duration,cpu/2,ram/2,0,0,e,tc,diffconstr);
                job->getTasks()->push_back(t);
        }
        //send the task to the scheduler
        BaseMsg *bmsg = new BaseMsg("newjob");
        bmsg->setDst(ID_SCHEDULER);
        bmsg->setSrc(ID_JOBARRIVAL);
        cMsgPar *par = new cMsgPar("job");
        par->setPointerValue(job);
        bmsg->addPar(par);
        send(bmsg,"gate$o");
        started += n;
        int next = started < initialtasks? 0 : job_arrival();
        //schedule next event
        scheduleAt(simTime()+next,new cMessage("newjob"));
        if( simTime() > time_max )throw cTerminationException("END");
    }
    delete msg;
}

/*
 * load task information from google traces
 * */
void JobArrival::loadTaskInfo(){
    arrival_real = cluster->par("arrival_real");
    if( !arrival_real )return;
    FILE *f = fopen(cluster->par("tracetask").stdstringValue().c_str(),"r");
    while( /*taskinfo.size() <8000000*/ 1 ){
        char line[1000];
        long t;
        long jid, tid;
        long e;
        long sched,prio;
        double cpu, ram,disk;
        double realcpu,realram,realdisk,disktime;
        long constr_diff;
        long duration;
        if( fscanf(f,"%[^\n]%*c",line) != 1)break;
        cpu = -1;
        ram = -1;
        int n= sscanf(line,"%ld,%ld,%ld,%ld,%ld,%lf,%lf,%lf,%ld,%ld,%ld,%lf,%lf,%lf,%lf",&t,&jid,&tid,&sched,&prio,&cpu,&ram,&disk,&duration,&e,&constr_diff,&realcpu,&realram,&realdisk,&disktime);
        if( n == 15 ){
            //ignore evited events (also, they shouldn't be on the trace)
            if( e != 2 ){
                TaskInfo *ti = new TaskInfo(t+5000000L,jid,tid,sched,prio,cpu,ram,disk,duration,e,realcpu,realram,realdisk,disktime,constr_diff!=0);
                taskinfo.push_back(ti);
            }
        }else{
            //malformed trace
            cout << taskinfo.size()<<endl;
            exit(1);
            break;
        }
        if( t/1000000 > time_max )break;
    }
    fclose(f);
}


float max(float a, float b){
    return a>b?a:b;
}

int intdistr(float (*f)(float), int a, int b ){
    int p = a;
    float r = uniform(0,1);
    float t = 0;
    float x;

    while( f(p) < r ){
        p++;
        if( p >= b )return intdistr(f,a,b);
    }
    return p;
}

float floatdistr(float (*f)(float), float a, float b ){
    float p = a;
    float r = uniform(0,1);
    float t = 0;
    float x;
    float div = (b-a)/100;

    while( f(p) < r ){
        p += div;
        if( p >= b )return floatdistr(f,a,b);
    }
    return p;
}


//distributions extracted with Alkida Balliu's program
float job_arrival_synth (float x)
{
    return
    0.46459149 - 0.02492888 * x + 0.12348905 * max(x - 0.0764,
        0) - 0.21390105 * max(x - 0.1919062, 0) + 0.2610257 *
        max(x - 0.48204177, 0) - 0.050735938 * max(x - 1.2108221,
        0) - 0.069627897 * max(x - 3.0414171, 0) - 0.013374882 *
        max(x - 7.639618, 0) - 0.011072104 * max(x - 19.189661,
        0) - 0.00088026589 * max(x - 48.201767, 0) + 6.8843759e-06 *
        max(x - 121.07615, 0) - 6.6528718e-07 * max(x - 304.1265,
        0) + 0.00011867527 * max(x - 763.9236, 0)
    ;
}

float task_per_job_synth (float x)
{
    return
    0.73602685 + 0.021067754 * x - 0.012605292 * max(x - 2.0818,
        0) - 0.0013226594 * max(x - 3.6825839, 0) + 0.0015356729 *
        max(x - 6.5142781, 0) - 0.0051482874 * max(x - 11.523382,
        0) - 0.0018376164 * max(x - 20.384196, 0) - 0.00090952322 *
        max(x - 36.058465, 0) - 0.00038477465 * max(x - 63.785341,
        0) - 0.00022501167 * max(x - 112.83258, 0) - 0.00011441863 *
        max(x - 199.59432, 0) - 4.3115821e-05 * max(x - 353.07081,
        0) + 1.2883618e-05 * max(x - 624.56187, 0) - 2.2166652e-05 *
        max(x - 1104.8138, 0) - 2.5512701e-06 * max(x - 1954.3519,
        0) - 6.8132737e-07 * max(x - 3457.1355, 0) - 1.9065147e-07 *
        max(x - 6115.4729, 0) + 0.00010016838 * max(x - 10817.918,
        0)
    ;
}

float task_cpu_synth (float x)
{
    return
    0.0067910046 + 2.3636444 * x - 32.438035 * max(x - 0.005,
        0) + 141.54668 * max(x - 0.0079165056, 0) - 95.941589 *
        max(x - 0.012534212, 0) - 5.5039083 * max(x - 0.019845432,
        0) - 5.4887575 * max(x - 0.031421295, 0) + 3.4738398 *
        max(x - 0.049749372, 0) - 8.0338034 * max(x - 0.078768236,
        0) + 0.16153314 * max(x - 0.12471384, 0) - 0.14239744 *
        max(x - 0.19745956, 0) + 0.0046055954 * max(x - 0.31263794,
        0) - 0.019137935 * max(x - 0.495, 0)
    ;
}


float task_ram_synth (float x)
{
    return
    0.00017335445 + 18.407742 * x + 56.173695 * max(x - 0.009551,
        0) - 76.155353 * max(x - 0.015122109, 0) + 19.73037 *
        max(x - 0.023942852, 0) - 13.746385 * max(x - 0.037908745,
        0) - 3.0648806 * max(x - 0.060020958, 0) - 0.98953612 *
        max(x - 0.09503125, 0) - 0.31631796 * max(x - 0.15046308,
        0) - 0.032945024 * max(x - 0.23822837, 0) - 0.0056118668 *
        max(x - 0.37718725, 0) - 0.00082848887 * max(x -
        0.59720099, 0) + 0.00071339139 * max(x - 0.945549,
        0)
    ;
}

float task_prio_synth (float x)
{
    return
    0.25456033 - 1.6213369e-13 * x + 0.96499546 * max(x -
        1, 0) - 3.5383167 * max(x - 1.5, 0) + 7.2952243 * max(x -
        2, 0) - 10.388187 * max(x - 2.5, 0) + 10.388321 * max(x -
        3, 0) - 7.420345 * max(x - 3.5, 0) + 5.1061602 * max(x -
        4, 0) - 2.9429314 * max(x - 4.5, 0) + 0.58859446 *
        max(x - 5, 0) - 0.0028034289 * max(x - 5.5, 0) -
        0.054564789 * max(x - 6, 0) + 0.021187062 * max(x -
        6.5, 0) - 0.063561187 * max(x - 7, 0) + 0.12712237 *
        max(x - 7.5, 0) - 0.13790592 * max(x - 8, 0) + 0.10451809 *
        max(x - 8.5, 0) - 0.042511036 * max(x - 9, 0) - 0.0068711314 *
        max(x - 9.5, 0) + 0.0024130049 * max(x - 10, 0)
    ;

}

float task_duration_end_synth (float x)
{
    return
    -0.0069158329 + 0.00053276278 * x + 0.0015543325 * max(x -
        17.8521, 0) - 0.00099924904 * max(x - 39.365942, 0) +
        0.00067504588 * max(x - 86.806447, 0) - 0.00087737081 *
        max(x - 191.41824, 0) - 0.00049481979 * max(x - 422.09932,
        0) - 0.00027819304 * max(x - 930.77774, 0) - 7.8430005e-05 *
        max(x - 2052.4724, 0) - 1.9606496e-05 * max(x - 4525.9386,
        0) - 1.2586155e-05 * max(x - 9980.2172, 0) - 1.8555226e-06 *
        max(x - 22007.531, 0) + 0.00011352672 * max(x - 48529.148,
        0)
    ;
}

float task_duration_kill_synth (float x)
{
    return
    4.8215535e-05 + 0.0014105646 * x - 0.0044290086 * max(x -
        6.2924, 0) + 0.01444519 * max(x - 15.805636, 0) - 0.0099776489 *
        max(x - 39.701566, 0) - 0.00071020941 * max(x - 99.724827,
        0) - 0.00047005285 * max(x - 250.49494, 0) - 0.00019081386 *
        max(x - 629.20854, 0) - 2.1470848e-05 * max(x - 1580.4846,
        0) - 3.1324982e-05 * max(x - 3969.9581, 0) - 2.121656e-05 *
        max(x - 9971.9841, 0) - 3.208142e-06 * max(x - 25048.241,
        0) - 0.00078508278 * max(x - 62917.708, 0)
    ;
}


int JobArrival::job_arrival(){
    int n = cluster->par("distr_job_arrival");
    if( n < 0 )n = intdistr(job_arrival_synth,0,764);
    return n;
}

int JobArrival::taskperjob(){
    int n = cluster->par("distr_taskperjob");
    if( n < 0 )n = intdistr(task_per_job_synth,1,10819);
    return n;
}

double JobArrival::task_cpu(){
    double n = cluster->par("distr_task_cpu");
    if( n < 0 )n = floatdistr(task_cpu_synth,0,0.5);
    return n;
}

double JobArrival::task_ram(){
    double n = cluster->par("distr_task_ram");
    if( n < 0 )n = floatdistr(task_ram_synth,0,0.9551);
    return n;
}

int JobArrival::task_prio(){
    int n = cluster->par("distr_task_prio");
    if( n < 0 )n = intdistr(task_prio_synth,0,11);
    return n;
}

int JobArrival::task_duration_end(){
    int n = cluster->par("distr_task_duration_end");
    if( n < 0 )n = intdistr(task_duration_end_synth,13,48534);
    return n;
}

int JobArrival::task_duration_kill(){
    int n = cluster->par("distr_task_duration_kill");
    if( n < 0 )n = intdistr(task_duration_kill_synth,0,62924);
    return n;
}

