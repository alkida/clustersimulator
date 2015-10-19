#include <stdio.h>
#include <string.h>
#include <map>
#include <iterator>
#include <unistd.h>
#include <stdlib.h>

using namespace std;


int main(){
  FILE *f = fopen("task_events.csv","r");
  map<pair<long,long>,bool> inserted;
  long t;
  map<pair<long,long>,long> ready;
  map<pair<long,long>,long> start;
  
  long tprint = 18000 * 1000000L;
  long waitsum = 0;
  long waitn = 0;
  long waitmax = 0;
  long nevict=0,nkill=0,nended=0;
  long waitsched[4] = {0};
  long waitschedn[4] = {0};
  long waitprio[13] = {0};
  long waitprion[13] = {0};
  long schedmax=0,priomax=0;
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
    sched = -1;
    prio = -1;
    long constr = -1;
    for(int i=1;i<=13;i++){
      if( i==1 )sscanf(temp,"%ld",&t);
      if( i==3 )sscanf(temp,"%ld",&jid);
      if( i==4 )sscanf(temp,"%ld",&tid);
      if( i==6 )sscanf(temp,"%ld",&e);
      if( i==8 )sscanf(temp,"%ld",&sched);
      if( i==9 )sscanf(temp,"%ld",&prio);
      if( i==13)sscanf(temp,"%ld",&constr);
      temp=strstr(temp,",")+1;
    }
if( tprint < t ){
if( t > 1000000L * 150000 ) return 0;
	long sum = nevict+nkill+nended;
	printf("Maxwait: %ld sched: %ld prio: %ld\n",waitmax,schedmax,priomax);
	printf("Sched: ");
	for(int i=0;i<4;i++)if( waitschedn[i] ) printf("%ld ",waitsched[i]/waitschedn[i]); else printf("0 "); printf("\n");
	for(int i=0;i<4;i++)printf("%ld ",waitsched[i]);printf("\n");
	printf("Prio: ");
	for(int i=0;i<13;i++)if( waitprion[i] ) printf("%ld ",waitprio[i]/waitprion[i]); else printf("0 "); printf("\n");
	for(int i=0;i<13;i++)printf("%ld ",waitprio[i]);printf("\n");
	if(waitn) printf("Time readysize avgwait %ld %ld %ld\n",t/1000000,ready.size(),waitsum/waitn);
	if(sum>0)printf("Ended: %ld |Killed: %ld |Evicted: %ld\n\n",100*nended/sum,100*nkill/sum,100*nevict/sum);
	if(t<4000){
		nended=0;
		nkill=0;
		nevict=0;
	}
	tprint += 450*1000000L;	

}

		pair<long,long> pid = pair<long,long>(jid,tid);
		if( e == 0 ){

			if( ready[pid] == 0 )ready[pid] = t+1;


		}else{
			if( e == 1 ){
				start[pid] = t+1;
			}else if( (e==4 || e == 5) && ready[pid] != 0 && start[pid] != 0){
				long duration = t+1 - start[pid];
				long twait = t+1 - ready[pid] - duration;
				twait /= 1000000;
				if( 0 && constr != 1 ){
					waitschedn[sched]++;
					waitsched[sched] += twait;
					waitprion[prio]++;
					waitprio[prio]+= twait;
					waitsum += twait;
					waitn++;
				}
			}
			
			
			
			if(e==2)nevict++;
			else if(e==4)nended++;
			else if(e==5)nkill++;
		    
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
			ready.erase(pid);
			

		}
		
  }
  
}
