#include <stdio.h>
#include <string.h>
#include <iostream>
#include <map>

using namespace std;
int main(){
  FILE *f = fopen("mid.csv","r");
  map<long,long> mav;
  while(1){
    long id;
    long val;
    int n = fscanf(f,"%ld,%ld",&id,&val);
    if( n != 2 )break;
    mav[val] = id;
  }
  fclose(f);
  f = fopen("MA1.csv","r");
  FILE *fout = fopen("MA2.csv","w");
  while(1){
    long p1,p2,p3;
    int n = fscanf(f,"%ld,%ld,%ld",&p1,&p2,&p3);
    if(n!=3)break;
    fprintf(fout,"%ld,%ld,%ld\n",mav[p1],p2,p3);
  }
  fclose(f);
  fclose(fout);
}

