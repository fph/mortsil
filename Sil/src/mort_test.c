#include "mort.h"

#include <stdio.h>


int main(){
  printf("Running tests...\n");


  odds *r = make_odds();

  if(r->p[0] != 1){
    printf("FAIL\n");
  }
  
  add_throw(r,2,3);

  if(r->p[1] != 0){
    printf("FAIL\n");
  }
  
  if(r->p[4] != 1/3){
    printf("FAIL\n");
  }

  kill_odds(r);

  printf("Done!\n");
  return 0;
}
