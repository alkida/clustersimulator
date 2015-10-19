#include <stdio.h>
#include <string.h>
#include <map>
#include <iterator>
#include <unistd.h>
#include <stdlib.h>

using namespace std;


int main(){
  map<pair<long,long>,pair<pair<double,double>,pair<double,double> > > usage;
  FILE *f = fopen("usage.csv","r");
  while(1){
    char line[1000];
    if( fscanf(f,"%[^\n]%*c",line) != 1)break;
    long jid,tid;
    double cpu,ram,disk,disktime;
    int n = sscanf(line,"%ld,%ld,%lf,%lf,%lf,%lf",&jid,&tid,&cpu,&ram,&disk,&disktime);
    if( n==6 ){
		usage[pair<long,long>(jid,tid)] = pair<pair<double,double>,pair<double,double> >(pair<double,double>(cpu,ram),pair<double,double>(disk,disktime));
	}
  }
  printf("%ld\n",usage.size());
  
  f = fopen("task_events_durations_sorted.csv","r");
  FILE *out = fopen("task_trace_temp.csv","w");

 
  while(1){
    char line[1000];
    long t;
    long jid, tid;
    long e;
    long sched,prio;
    double cpu, ram,disk;
    long duration;
    long diff;
    if( fscanf(f,"%[^\n]%*c",line) != 1)break;
    char *temp = line;
    cpu = -1;
    ram = -1;
    int n= sscanf(line,"%ld,%ld,%ld,%ld,%ld,%lf,%lf,%lf,%ld,%ld,%ld",&t,&jid,&tid,&sched,&prio,&cpu,&ram,&disk,&duration,&e,&diff);
    if( n == 11){
		pair<pair<double,double>,pair<double,double> > u = usage[pair<long,long>(jid,tid)];
		fprintf(out,"%ld,%ld,%ld,%ld,%ld,%lf,%lf,%lf,%ld,%ld,%ld,%lf,%lf,%lf,%lf\n",t,jid,tid,sched,prio,cpu,ram,disk,duration,e,diff,u.first.first,u.first.second,u.second.first,u.second.second);
	}else break;
  }
  
  fclose(out);
}
