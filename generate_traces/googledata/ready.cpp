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
	Info(long t, long jid, long tid, long sched, long prio, int missing, double cpu, double ram){
		this->t=t;
		this->jid=jid;
		this->tid=tid;
		this->sched=sched;
		this->prio=prio;
		this->missing=missing;
		this->cpu=cpu;
		this->ram=ram;
	}
	long t,jid,tid,sched,prio;
	int missing;
	double cpu,ram;
};

int main(){
  FILE *f = fopen("task_events.csv","r");
  map<pair<long,long>,bool> inserted;
  map<pair<long,long>,Info> info;
  long t;
      map<pair<long,long>,long> ready;
      map<pair<long,long>,int> isrunning;
      
  long tprint = 0;
  long waitsum = 0;
  long waitn = 0;
  long waitmax = 0;
  long nevict=0,nkill=0,nended=0,nfail=0;
  long running = 0;
  long waitsched[4] = {0};
  long waitschedn[4] = {0};
  long waitprio[13] = {0};
  long waitprion[13] = {0};
  long schedmax=0,priomax=0;
  int ctsready[13] = {0};
  int ctsrunning[13] = {0};
  int ctsevict[13] = {0};
  int ctsnew[13]={0};
  int tot =0;
  int started = 0;
  while(1){
    char line[1000];
    long jid, tid;
    long e;
    long sched,prio;
    double cpu, ram;
    if( fscanf(f,"%[^\n]%*c",line) != 1)break;
    char *temp = line;
    cpu = -1;
    ram = -1;
    //int n= sscanf(line,"%ld,%ld,%ld,%ld,%ld,%ld,%lf,%lf",&t,&jid,&tid,&e,&sched,&prio,&cpu,&ram);
    for(int i=1;i<=12;i++){
      if( i==1 )sscanf(temp,"%ld",&t);
      if( i==3 )sscanf(temp,"%ld",&jid);
      if( i==4 )sscanf(temp,"%ld",&tid);
      if( i==6 )sscanf(temp,"%ld",&e);
      if( i==8 )sscanf(temp,"%ld",&sched);
      if( i==9 )sscanf(temp,"%ld",&prio);
      
      temp=strstr(temp,",")+1;
    }
if( tprint < t ){
	long sum = nevict+nkill+nended;
	/*printf("Maxwait: %ld sched: %ld prio: %ld\n",waitmax,schedmax,priomax);
	printf("Sched: ");
	for(int i=0;i<4;i++)if( waitschedn[i] ) printf("%ld ",waitsched[i]/waitschedn[i]); else printf("0 "); printf("\n");
	for(int i=0;i<4;i++)printf("%ld ",waitsched[i]);printf("\n");
	printf("Prio: ");
	for(int i=0;i<13;i++)if( waitprio[i] ) printf("%ld ",waitprio[i]/waitprion[i]); else printf("0 "); printf("\n");
	for(int i=0;i<13;i++)printf("%ld ",waitprio[i]);printf("\n");
	printf("%ld %ld %ld\n",t/1000000,ready.size(),waitsum/waitn);
	if(sum>0)printf("Ended: %ld |Killed: %ld |Evicted: %ld\n\n",100*nended/sum,100*nkill/sum,100*nevict/sum);*/
	printf("Time/Ready/N.Evict/N.End: %ld %ld %ld %ld %ld %d %ld %ld\n",(t-600000000)/1000000,ready.size(),nevict,nended,running,tot,nkill,nfail);
	printf("Evict:   ");for(int i=0;i<13;i++)printf("%d ",ctsevict[i]);printf("\n");
	printf("New:   ");for(int i=0;i<13;i++)printf("%d ",ctsnew[i]);printf("\n");
	printf("Started: %d\n",started);
	/*if( t > 37800000000L && t < 39600000000L ){
		printf("Ready:   ");for(int i=0;i<13;i++)printf("%d ",ctsready[i]);printf("\n");
		printf("Running: ");for(int i=0;i<13;i++)printf("%d ",ctsrunning[i]);printf("\n");
		printf("Evict:   ");for(int i=0;i<13;i++)printf("%d ",ctsevict[i]);printf("\n");
	}*/
	//if(t<4000){
		nended=0;
		nkill=0;
		nevict=0;
		for(int i=0;i<13;i++)ctsevict[i]=0;
		for(int i=0;i<13;i++)ctsnew[i]=0;
	//}
	tprint += 450*1000000L;	
}
//    if( n >= 6){
//if(t>0)printf("%ld %ld %ld %ld\n",t,jid,tid,e);
		pair<long,long> pid = pair<long,long>(jid,tid);
		if( e == 0 ){
			started++;
			tot++;
			//info[pid] = Info(t,jid,tid,sched,prio,n!=8,cpu,ram);
			//inserted[pid] = true;
			//nready++;
			ready[pid] = t+1;
			ctsready[prio]++;
			ctsnew[prio]++;
			//printf("%ld\n",ready.size());
			//printf("%ld\n",ready.size());
			/*if( t > 37800000000L && t < 39600000000L ){
				printf("TEST new %ld %ld %ld %ld\n",jid,tid,sched,prio);
				printf("TEST rq %ld\n",ready.size());
			}*/
		}else{
			if(e==2){nevict++;
				ctsevict[prio]++;
			}else if(e==3)nfail++;
			else if(e==4)nended++;
			else if(e==5)nkill++;

			if( e == 1 ){
				running++;
				ctsrunning[prio]++;
				isrunning[pid] = 1;
			}else if( e > 1 && isrunning[pid] == 1 ){
				running--;
				isrunning[pid] = 0;
				ctsrunning[prio]--;
			}
			
			if( e > 1 )tot--;
			
			if( ready[pid] != 0 )ctsready[prio]--;
		//if(e==1 ){
		    long tstart = ready[pid];
		    if( tstart>0 && e==1){
				long twait = (t-1  - tstart)/1000000;
				waitschedn[sched]++;
				waitsched[sched] += twait;
				waitprion[prio]++;
				waitprio[prio]+= twait;
				waitsum += twait;
				waitn++;
				if( twait > waitmax ){
					waitmax = twait;
					schedmax=sched;
					priomax=prio;
				}
			}
			//if( t > 39600000000L )exit(1);
			ready.erase(pid);
			/*if( t > 37800000000L && t < 39600000000L ){
				printf("TEST erase %ld %ld %ld %ld %ld\n",e,jid,tid,sched,prio);
				printf("TEST rq %ld\n",ready.size());
			}*/
			//printf("%ld\n",ready.size());
			//&& inserted[pid] ){
			//inserted[pid] = false;
			//Info temp = info[pid];
			//if( temp.missing && n==8 ){ temp.cpu = cpu; temp.ram = ram; info[pid] = temp; }
			//if( e == 2 || e == 3 || e==4 || e == 5 || e==6 ){
				//fprintf(out,"%ld,%ld,%ld,%ld,%ld,%lf,%lf,%ld,%ld\n",temp.t,temp.jid,temp.tid,temp.sched,temp.prio,temp.cpu,temp.ram,t-temp.t,e);
			//	inserted[pid] = false;
			//}
			//nready--;
			//printf("%d\n",nready);
			//printf("%ld\n",(t-temp.t)/1000000);
		}
	//}else break;
		
  }
  
}
