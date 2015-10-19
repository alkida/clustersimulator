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
  f = fopen("ME1.csv","r");
  FILE *fout = fopen("ME2.csv","w");
  while(1){
    long p0;
    long p1,p2;
    double p3,p4;
    int n = fscanf(f,"%ld,%ld,%ld,%lf,%lf",&p0,&p1,&p2,&p3,&p4);
//cout << n << endl;
    if(n<3)break;
    if(n==5)fprintf(fout,"%ld,%ld,%ld,%lf,%lf\n",p0,mav[p1],p2,p3,p4);
    else fscanf(f,",,");
  }
  fclose(f);
  fclose(fout);
}

