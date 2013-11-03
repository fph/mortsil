#include "mort.h"

#include "z-virt.h"
#include "z-util.h"

double* make_p(int length){
  double* p;
  int i;
  C_MAKE(p,length,double);
  for(i=0;i<length;i++){
    p[i]=0.0;
  }
  return p;
}

odds* make_empty_odds(int length){
  odds* o;
  MAKE(o, odds);
  o->length=length;
  o->p=make_p(length);
  return o;
}

odds* make_zero_odds(){
  odds* o=make_empty_odds(1);
  o->p[0]=1.;
  return o;
}

void set_odds(odds* o, double* p, int length){
 o->length=length;
 KILL(o->p);
 o->p=p;
}

void kill_odds(odds* r){
  KILL(r->p);
  KILL(r);
}

void add_scaled_odds(odds* o1, double constant, odds* o2){
  if(o1==NULL){
    //TODO
  }
  
}

void add_throw(odds* o, int n, int sides){
  int i=0;
  int result;

  if(n==0) return;
  if(n<0 || sides<=0) quit("Invalid parameter in add_throw");
  
  /*recursively add new throws*/
  int newlength=o->length+sides;
  double *newp=make_p(newlength);
  for(result=1;result<=sides;result++){
    for(i=0; i < o->length; i++){
      newp[i+result]+= 1.0/sides * o->p[i];
    }
  }
  set_odds(o,newp,newlength);
  add_throw(o,n-1,sides);
}

void add_variable_throw(odds* o, odds* n, int sides){
  int k;
  for(k=0;k<n->length;k++){
    //TODO
  }
}

void add_const(odds* o, int n){
  add_throw(o,n,1);
}

void print_odds(odds* o){
  int i;
  for(i=0;i<o->length;i++){
    printf("%2d: %g\n",i,o->p[i]);
  }
}

void min_twice(odds* o){
  double *newp=make_p(o->length);
  int i,j;
  for(i=0;i<o->length;i++){
    for(j=0;j<o->length;j++){
      newp[MIN(i,j)]+=o->p[i]*o->p[j];
    }
  }
  set_odds(o,newp,o->length);
}

odds* odds_difference_capped(odds *o1, odds* o2){
  int i,j;
  odds *diff=make_empty_odds(o1->length);
  for(i=0;i<o1->length;i++){
    for(j=0;j<o2->length;j++){
      diff->p[MAX(0,i-j)]+= o1->p[i]*o2->p[j];
    }
  }
  return diff;
}

odds* hit_roll_odds(int att, int evn, const monster_type *m_ptr1, const monster_type *m_ptr2){
  if(m_ptr2!=PLAYER){
    quit("Errouneous use of hit_roll_odds - it's supposed to work only for hits to the player");  
  }
  odds* oatt=make_zero_odds();
  odds* oevn=make_zero_odds();
  add_throw(oatt,1,20);    
  add_throw(oevn,1,20);
  if(p_ptr->cursed){
    min_twice(oevn);
  }
  add_const(oatt,att);
  add_const(oevn,evn);
  odds* diff=odds_difference_capped(oatt,oevn);
  kill_odds(oatt);
  kill_odds(oevn);
  return diff;
}

odds* dam_roll_normal(odds* hit_result, int dd, int ds, const monster_race *r_ptr, int elem_bonus_dice, int effect, int no_crit){
  const int MAX_DICE=40; //assume can't have more than this number of dice
  odds* o=make_empty_odds(MAX_DICE);
  int i;
  
  for(i=0;i<hit_result->length;i++){
    if(i==0 && effect){
      o->p[0]+=hit_result->p[i];  //miss==0 rolls
    }
    int rolls=dd+elem_bonus_dice+(no_crit)?0:crit_bonus(i, 20 * dd, r_ptr, S_MEL, FALSE);
    o->p[rolls]+=hit_result->p[i];
  }
  return o;  
}




