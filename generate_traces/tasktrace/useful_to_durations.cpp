#include <stdio.h>
#include <string.h>
#include <map>
#include <iterator>
#include <unistd.h>
#include <stdlib.h>

using namespace std;

class Info{
	public:
	Info(){
	}
	Info(long t, long jid, long tid, long sched, long prio, int missing, double cpu, double ram, double disk, bool different){
		this->t=this->t2=t;
		this->jid=jid;
		this->tid=tid;
		this->sched=sched;
		this->prio=prio;
		this->missing=missing;
		this->cpu=cpu;
		this->ram=ram;
		this->disk = disk;
		this->different = different;
	}
	long t,t2,jid,tid,sched,prio;
	int missing;
	double cpu,ram,disk;
	bool different;
};

int main(){
  FILE *f = fopen("task_events_useful.csv","r");
  FILE *out = fopen("task_events_durations.csv","w");
  map<pair<long,long>,bool> inserted;
  map<pair<long,long>,Info> info;
  long t;
  while(1){
    char line[1000];
    long jid, tid;
    long e;
    long sched,prio;
    double cpu, ram,disk;
    long different;
    if( fscanf(f,"%[^\n]%*c",line) != 1)break;
    char *temp = line;
    cpu = -1;
    ram = -1;
    disk = -1;
    different = 0;
    int n= sscanf(line,"%ld,%ld,%ld,%ld,%ld,%ld,%lf,%lf,%lf,%ld",&t,&jid,&tid,&e,&sched,&prio,&cpu,&ram,&disk,&different);
    if( n >= 6){
		if( t > 0 )t -= 600000000;
		pair<long,long> pid = pair<long,long>(jid,tid);
		if( !inserted[pid] ){
			info[pid] = Info(t,jid,tid,sched,prio,n!=8,cpu,ram,disk,different!=0);
			inserted[pid] = true;
		}else{
			Info temp = info[pid];
			if( temp.missing && n==10 ){ temp.cpu = cpu; temp.ram = ram; temp.disk = disk; temp.different = different!=0; if( e == 1)temp.t2 = t; info[pid] = temp;  }
			if( e == 2 || e == 3 || e==4 || e == 5 || e==6 ){
				fprintf(out,"%ld,%ld,%ld,%ld,%ld,%lf,%lf,%lf,%ld,%ld,%ld\n",temp.t,temp.jid,temp.tid,temp.sched,temp.prio,temp.cpu,temp.ram,temp.disk,t-(e!=4?temp.t:temp.t2),e,temp.different?1L:0L);
				inserted[pid] = false;
			}
		}
	}else break;
		
  }
  for(map<pair<long,long>,Info>::iterator it = info.begin();it!=info.end();it++){
	  pair<long,long> pid = it->first;
	  Info temp = it->second;
	  if( inserted[pid] ){
		  fprintf(out,"%ld,%ld,%ld,%ld,%ld,%lf,%lf,%lf,%ld,%ld,%ld\n",temp.t,temp.jid,temp.tid,temp.sched,temp.prio,temp.cpu,temp.ram,temp.disk,t-temp.t2,4L,temp.different?1L:0L);
	  }
  }
  
  fclose(out);
}
