#include <stdio.h>
#include <string.h>
#include <map>
#include <iterator>
#include <unistd.h>
#include <stdlib.h>

using namespace std;

class TaskInfo{
    public:
    TaskInfo(){
    }
    TaskInfo(long t, long jid, long tid, long sched, long prio, double cpu, double ram, double disk, long duration, long e, bool constr, double realcpu, double realram, double realdisk, double disktime){
        this->t=t;
        this->jid=jid;
        this->tid=tid;
        this->sched=sched;
        this->prio=prio;
        this->cpu=cpu;
        this->ram=ram;
        this->e = e;
        this->duration = duration;
        this->realcpu = realcpu;
        this->realram = realram;
        this->disk = disk;
        this->constr = constr;
        this->realdisk = realdisk;
        this->disktime = disktime;
    }
    long t,jid,tid,sched,prio;
    double cpu,ram,disk;
    long duration,e;
    bool constr;
    double realcpu,realram,realdisk,disktime;
};


int main(){
  FILE *f = fopen("task_trace_temp.csv","r");
  FILE *out = fopen("task_trace_nonevict.csv","w");
  map<pair<long,long>,bool> inserted;
  map<pair<long,long>,TaskInfo> info;
  long t;
  while(1){
    char line[1000];
    long jid, tid;
    long e;
    long sched,prio;
    double cpu, ram,disk;
    long constr;
    double realcpu,realram,realdisk,disktime;
    long duration;
    if( fscanf(f,"%[^\n]%*c",line) != 1)break;
    cpu = -1;
    ram = -1;
    int n= sscanf(line,"%ld,%ld,%ld,%ld,%ld,%lf,%lf,%lf,%ld,%ld,%ld,%lf,%lf,%lf,%lf",&t,&jid,&tid,&sched,&prio,&cpu,&ram,&disk,&duration,&e,&constr,&realcpu,&realram,&realdisk,&disktime);
    pair<long,long> pid = pair<long,long>(jid,tid);
    if( e != 2 && !inserted[pid] )fprintf(out,"%ld,%ld,%ld,%ld,%ld,%lf,%lf,%lf,%ld,%ld,%ld,%lf,%lf,%lf,%lf\n",t,jid,tid,sched,prio,cpu,ram,disk,duration,e,constr,realcpu,realram,realdisk,disktime);
    else{
		if( e == 2 && !inserted[pid] ){
			TaskInfo ti(t,jid,tid,sched,prio,cpu,ram,disk,duration,e,constr!=0,realcpu,realram,realdisk,disktime);
			info[pid] = ti;
			inserted[pid] = true;
		}else if( (e == 3 || e == 4 || e == 5 || e == 6) && inserted[pid] ){
			TaskInfo temp = info[pid];
			long newduration = 0;
			if( e == 4 ) newduration = duration;
			else{
				newduration = t + duration  - temp.t;
			}
			fprintf(out,"%ld,%ld,%ld,%ld,%ld,%lf,%lf,%lf,%ld,%ld,%ld,%lf,%lf,%lf,%lf\n",temp.t,temp.jid,temp.tid,temp.sched,temp.prio,temp.cpu,temp.ram,temp.disk,newduration,e,temp.constr!=0?1L:0L,temp.realcpu,temp.realram,temp.realdisk,temp.disktime);
			inserted[pid] = false;
		}
		
	}
    
  }
  
  int n=0;
  for(map<pair<long,long>,TaskInfo>::iterator it = info.begin();it!=info.end();it++){
	  pair<long,long> pid = it->first;
	  if( inserted[pid] ){
		  TaskInfo temp = info[pid];
		  fprintf(out,"%ld,%ld,%ld,%ld,%ld,%lf,%lf,%lf,%ld,%ld,%ld,%lf,%lf,%lf,%lf\n",temp.t,temp.jid,temp.tid,temp.sched,temp.prio,temp.cpu,temp.ram,temp.disk,t,5L,temp.constr!=0?1L:0L,temp.realcpu,temp.realram,temp.realdisk,temp.disktime);
	  }
  }
  printf("%d\n",n);
  fclose(out);
}
