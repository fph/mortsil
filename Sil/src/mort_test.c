#include "mort.h"

#include <stdio.h>
#include <math.h>

int main(){
  printf("Running tests...\n");


  odds *r = make_zero_odds();

  if(r->p[0] != 1){
    printf("FAIL 1\n");
  }
  
  add_throw(r,2,3);

  if(r->p[1] != 0){
    printf("FAIL 2\n");
  }
  
  if(fabs(r->p[4]-1./3.) > 1e-8){
    printf("FAIL 3\n");
  }
  
  min_twice(r);
  if(fabs(r->p[5]-8./81.)>1e-8){
    printf("FAIL 4\n");
  }

  kill_odds(r);
  
  //tests: cast 0 or 1 or 2 d2
  odds* n=make_empty_odds(3);
  n->p[0]=1./3.;
  n->p[1]=1./3.;
  n->p[2]=1./3.;
  odds* o=make_variable_throw(n,2);
  if(fabs(o->p[0]-1./3.)>1e-8 || fabs(o->p[1]-1./6.)>1e-8 || fabs(o->p[2]-3./12.)>1e-8 ){
    print_odds(o);
    printf("FAIL 5\n");
  }
  
  printf("Done!\n");
  return 0;
}
