#include "mort.h"

#include <stdio.h>


int main(){
  printf("Running tests...\n");


  odds *r = make_odds();

  if(r->p[0] != 1){
    printf("FAIL\n");
  }

  kill_odds(r);

  printf("Done!\n");
  return 0;
}
