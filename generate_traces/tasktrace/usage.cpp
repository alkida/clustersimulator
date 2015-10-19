#include <stdio.h>
#include <string.h>
#include <map>
#include <iostream>

using namespace std;

float maxf(float a,float b){
	return a>b?a:b;
}

int main(){
	map<pair<long,long>,bool> inserted;
	map<pair<long,long>,pair<long,pair<pair<double,double>,pair<double,double> > > > usages;
	char name[100];
	for(int i=0;i<10;i++){
		printf("File %d\n",i);
		sprintf(name,"part-%05d-of-00500.csv",i);
		FILE *f = fopen(name,"r");
		while(1){
			char line[1000];
			long t;
			long jid, tid;
			double cpu, ram,disk,disktime;
			
			int ret = fscanf(f,"%[^\n]%*c",line);
			if( ret == 1 ){
				char *temp = line;
				int n=0;
				for(int i=1;i<=13;i++){
				  if( i==1 )n+=sscanf(temp,"%ld",&t);
				  if( i==3 )n+=sscanf(temp,"%ld",&jid);
				  if( i==4 )n+=sscanf(temp,"%ld",&tid);
				  if( i==6)n+=sscanf(temp,"%lf",&cpu);
				  if( i==11)n+=sscanf(temp,"%lf",&ram);
				  if( i==12)n+=sscanf(temp,"%lf",&disktime);
				  if( i== 13)n+=sscanf(temp,"%lf",&disk);
				  temp=strstr(temp,",")+1;
				}
				if( n==7 ){
					pair<long,long> pid = pair<long,long>(jid,tid);
					bool isfirst = !inserted[pid];
					if( isfirst ){
						usages[pid] = pair<long,pair<pair<double,double>,pair<double,double> > >(1,pair<pair<double,double>,pair<double,double> >(pair<double,double>(cpu,ram),pair<double,double>(disk,disktime)));
						inserted[pid] = true;
					}else{
						pair<long,pair<pair<double,double>,pair<double,double> > > t = usages[pid];
						t.first++;
						t.second.first.first += cpu;
						t.second.first.second += ram;//maxf(t.second.first.second,ram);
						t.second.second.first += disk;
						t.second.second.second += disktime;
						usages[pid] = t;
					}
				}
			}else break;
		}
	}
	FILE *f = fopen("result.csv","w");
	for(map<pair<long,long>,pair<long,pair<pair<double,double>,pair<double,double> > > >::iterator it = usages.begin();it != usages.end();it++){
		pair<long,long> pid = it->first;
		pair<long,pair<pair<double,double>,pair<double,double> > > t = it->second;
		fprintf(f,"%ld,%ld,%lf,%lf,%lf,%lf\n",pid.first,pid.second,t.second.first.first/t.first,t.second.first.second/t.first,t.second.second.first/t.first,t.second.second.second/t.first);
	}
	fclose(f);
}
