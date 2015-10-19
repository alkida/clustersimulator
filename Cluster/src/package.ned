package cluster;

@license(LGPL);

simple MachineArrival{
    parameters:
        
    gates:
        inout gate;
}

simple JobArrival{
    parameters:
        
    gates:
        inout gate;
}

simple Machine{
    parameters:
    	int id;
    gates:
        inout gate;
}

simple Scheduler{
    gates:
    	inout gate;
}

simple Net{    
    gates:
        inout machines[];
        inout scheduler;
        inout jobarrival;
        inout machinearrival;
}


network Cluster{
    int num_machines = default(11000);
    int arrival_real = default(1);
    volatile int distr_job_arrival @unit(s) = default(-1s);
    volatile int distr_taskperjob = default(-1);
    volatile double distr_task_cpu = default(-1);
    volatile double distr_task_ram = default(-1);
    volatile int distr_task_prio = default(-1);
    volatile int distr_task_duration_end @unit(s) = default(-1s);
    volatile int distr_task_duration_kill @unit(s) = default(-1s);
    volatile int distr_machine_down @unit(s) = default(-1s);
    volatile int distr_machine_downtime @unit(s) = default(-1s);
    double distr_task_prob_antiaffinity = default(0.07);
    double distr_task_prob_constraint = default(0.06);
    double distr_machine_prob_sat_constraint = default(0.45);
    double distr_machine_cpu = default(-1);
    double distr_machine_ram = default(-1);
    int distr_initialtasks = default(144000);
    double distr_prob_longrunning = default(0.83);
    double ram_sched = default(0.489);
	double ram_machine = default(0.489);
	double time_max = default(150000);
	double distr_prob_abnormalend = default(0.592);
    string tracetask = default("/home/me/task_trace.csv");
    string tracetaskconstraints = default("/home/me/TC.csv");
    string tracemachineattributes = default("/home/me/MA.csv");
    string tracemachineevents = default("/home/me/ME.csv");

    submodules:
        
        net : Net;
        scheduler : Scheduler;
        jobarrival: JobArrival;
        machinearrival: MachineArrival;
        machines[num_machines] : Machine{
            id = index;
        }

    connections:
        for i = 0..num_machines-1 {
            machines[i].gate <--> ned.DatarateChannel <--> net.machines++; 
        }
        net.scheduler <--> ned.DatarateChannel <-->  scheduler.gate;
        net.jobarrival <--> jobarrival.gate;
        net.machinearrival <--> machinearrival.gate;
}

