#include "mort.h"

#include "z-virt.h"

odds* make_odds(){
  odds* r;
  MAKE(r,odds);
  r->length=1;
  C_MAKE(r->p,1,double);
  r->p[0]=1;
  return r;
}

void kill_odds(odds* r){
  KILL(r->p);
  KILL(r);
}

