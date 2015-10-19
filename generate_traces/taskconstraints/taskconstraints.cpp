#include <stdio.h>
#include <string.h>
#include <iostream>
#include <map>

using namespace std;
int main(){
  FILE *f = fopen("machine_attributes_values.csv","r");
  map<string,long> mav;
  while(1){
    long id;
    char val[100];
    int n = fscanf(f,"%ld,%[^\n]%*c",&id,val);
    if( n != 2 )break;
    string s(val);
    mav[s] = id;
  }
  fclose(f);
  f = fopen("task_constraints2.csv","r");
  FILE *fout = fopen("task_constraints3.csv","w");
  while(1){
    long p1,p2,p3,p4,p5;
    char p6[100];
    int n = fscanf(f,"%ld,%ld,%ld,%ld,%ld,%[^\n]",&p1,&p2,&p3,&p4,&p5,p6);
    if( n < 5 ) break;
    fprintf(fout,"%ld,%ld,%ld,%ld,%ld,",p1,p2,p3,p4,p5);
    if( n==5 )fprintf(fout,"-1\n");
    else if( strlen(p6) <20 )fprintf(fout,"%s\n",p6);
    else{
      string s(p6);
      long id = mav[s];
      fprintf(fout,"%ld\n",id);
    }
  }
  fclose(f);
  fclose(fout);
}
