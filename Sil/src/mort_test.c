#include "mort.h"

#include <stdio.h>
#include <math.h>

int main(){
  printf("Running tests...\n");


  odds *r = make_empty_odds();

  if(r->p[0] != 1){
    printf("FAIL\n");
  }
  
  add_throw(r,2,3);

  if(r->p[1] != 0){
    printf("FAIL\n");
  }
  
  if(fabs(r->p[4]-1./3.) > 1e-8){
    printf("FAIL\n");
  }
//  print_odds(r);
  
  min_twice(r);
//  print_odds(r);
  if(fabs(r->p[5]-8./81.)>1e-8){
    printf("FAIL\n");
  }

  kill_odds(r);

  printf("Done!\n");
  return 0;
}
