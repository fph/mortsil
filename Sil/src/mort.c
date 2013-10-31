#include "mort.h"

#include "z-virt.h"
#include "z-util.h"

odds* make_odds(){
  odds* o;
  MAKE(o,odds);
  o->length=1;
  C_MAKE(o->p,1,double);
  o->p[0]=1;
  return o;
}

void kill_odds(odds* r){
  KILL(r->p);
  KILL(r);
}

void add_throw(odds* o, int n, int sides){
  int i=0;
  int result;

  if(n==0) return;
  if(n<0 || sides<=0) quit("Invalid parameter in add_throw");
  
  /*recursively add new throws*/
  double *newp;
  int newlength=o->length+sides;
  C_MAKE(newp,newlength,double);
  for(i=0;i<newlength;i++){
    newp[i]=0.0;
  }
  for(result=1;result<=n;result++){
    for(i=0; i < o->length; i++){
      newp[i+result]+= 1.0/sides * o->p[i];
    }
  }
  KILL(o->p);
  o->p=newp;
  o->length=newlength;
  add_throw(o,n-1,sides);
}

void add_const(odds* o, int n){
  add_throw(o,n,1);
}


