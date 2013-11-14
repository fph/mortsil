#include "mort.h"

#include "z-virt.h"
#include "z-util.h"

#include <math.h>

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

void check_sum(odds* o){
  double sum=0;
  int i;
  for(i=0;i<o->length;i++){
    sum+=o->p[i];
  }
  if(fabs(sum-1.)>1e-8){
    quit("wrong odds sum");
  }
}

void negate(odds* o){
	if(o->length != 2)
		quit("error: called negate() on a non-boolean odds");
	
	double tmp = o->p[1];
	o->p[1] = o->p[0];
	o->p[0] = tmp;
}

void add_scaled_odds(odds* o1, double constant, odds* o2){
  int i;
  if(o1==NULL){
    o1=make_empty_odds(o2->length);
  }
  if(o1->length < o2->length){ /*must reallocate o1 to extend it*/
    double* newp=make_p(o2->length);
    for(i=0;i<o1->length;i++){
      newp[i]=o1->p[i];
    }
    set_odds(o1,newp,o2->length);
  }
  
  for(i=0;i<o2->length;i++){
    o1->p[i]+= constant * o2->p[i];
  }
  //no check_sum here
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
  check_sum(o);
}

void compress_odds(odds* o){
  int i;
  for(i=o->length-1;i>=1;i--){
    if(o->p[i]!=0.){
      break;
    }
  }
  if(i < o->length-1){
    double *newp=make_p(i+1);
    int j;
    for(j=0;j<i+1;j++){
     newp[j]=o->p[j];
    }
    set_odds(o,newp,i+1);
  }
  check_sum(o);
}

odds* make_variable_throw(odds* n, int sides){
  int i;
  odds *o=make_empty_odds((n->length-1)*sides+1);
  o->p[0]=n->p[0]; //we get 0 if we throw 0 dice
  odds *tmp=make_zero_odds();
  for(i=1;i< n->length; i++){ //1 or more
    add_throw(tmp,1,sides);
    add_scaled_odds(o,n->p[i],tmp);
  }
  kill_odds(tmp);
  check_sum(o);
  return o;
}

void add_const(odds* o, int n){
  add_throw(o,n,1);
}

void print_odds(odds* o){
  int i;
  check_sum(o);
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
  check_sum(o);
}

odds* odds_difference_capped(odds *o1, odds* o2){
  int i,j;
  odds *diff=make_empty_odds(o1->length);
  for(i=0;i<o1->length;i++){
    for(j=0;j<o2->length;j++){
      diff->p[MAX(0,i-j)]+= o1->p[i]*o2->p[j];
    }
  }
  check_sum(diff);
  compress_odds(diff);
  return diff;
}

void odds_multiply_percent(odds* o, int percent){
  int i;
  int newlength=((o->length-1)*percent)/100 + 1;
  double *newp=make_p(newlength);
  for(i=0;i<o->length;i++){
    newp[(i*percent)/100] += o->p[i];
  }
  set_odds(o,newp,newlength);
  check_sum(o);
}

void odds_multiply(odds* o, int n){
  int i;
  int newlength=((o->length-1)*n) + 1;
  double *newp=make_p(newlength);
  for(i=0;i<o->length;i++){
    newp[i*n] += o->p[i];
  }
  set_odds(o,newp,newlength);
  check_sum(o);
}

void odds_divide(odds* o, int d){
  int i;
  int newlength=((o->length-1)/d) + 1;
  double *newp=make_p(newlength);
  for(i=0;i<o->length;i++){
    newp[i/d] += o->p[i];
  }
  set_odds(o,newp,newlength);
  check_sum(o);
}

void odds_chance(odds* o, odds* chance){
	int i;
	if(chance->length != 2)
		quit("wrong odds length");
	
	for(i=0;i<o->length; i++){
		o->p[i] *= chance->p[1];
	}
	o->p[0] += chance->p[0];
	
	check_sum(o);
}

void collapse_to_01(odds* o){
	double* newp = make_p(2);
	int i;
	
	newp[0] = o->p[0];
	for(i=1;i<o->length;i++){
		newp[1] += o->p[i];
	}
	set_odds(o,newp,2);
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

odds* damroll_odds(odds* hit_result, int dd, int ds, int weight, const monster_race *r_ptr, int elem_bonus_dice, int effect, int no_crit){
  const int MAX_DICE=40; //assume can't have more than this number of dice
  odds* dice=make_empty_odds(MAX_DICE+1);
  int i;
  
  for(i=0;i<hit_result->length;i++){
    if(i==0 && effect){
      dice->p[0]+=hit_result->p[i];  //miss==0 rolls
    }
    else{
    	int rolls=dd+elem_bonus_dice+((no_crit)?0:crit_bonus(i, weight, r_ptr, S_MEL, FALSE));
    	if(rolls>MAX_DICE){
    		quit("Too many damage rolls");
    	}
    	dice->p[rolls]+=hit_result->p[i];
    }
  }
  compress_odds(dice);
  
  odds* o=make_variable_throw(dice,ds);
  
  return o;  
}

odds* protection_roll_odds(int typ, bool melee)
{
	int i;
	object_type *o_ptr;
	int mult = 1;
	int armour_weight = 0;
	
	// things that always count:
	
	odds *prt=make_zero_odds();
	
	if (singing(SNG_STAYING))
	{
	  add_throw(prt,1,MAX(1, ability_bonus(S_SNG, SNG_STAYING)));
	}
	
	if (p_ptr->active_ability[S_WIL][WIL_HARDINESS])
	{
	  add_throw(prt,1,p_ptr->skill_use[S_WIL] / 6);
	}
	
	// armour:
	
	for (i = INVEN_WIELD; i < INVEN_TOTAL; i++)
	{
		o_ptr = &inventory[i];
		if (i >= INVEN_BODY) armour_weight += o_ptr->weight;

		// fire and cold and generic 'hurt' all check the shield
		if (i == INVEN_ARM)
		{
			if ((typ == GF_HURT) || (typ == GF_FIRE) || (typ == GF_COLD))
			{
				if (p_ptr->active_ability[S_EVN][EVN_BLOCKING] && (!melee || (p_ptr->previous_action[0] == 5)))
				{
					mult = 2;
				}
				if (o_ptr->pd > 0)
				{
				  add_throw(prt,o_ptr->pd * mult, o_ptr->ps);
				}
			}
		}
		
		// also add protection if damage is generic 'hurt' or it is a ring or amulet slot
		else if ((typ == GF_HURT) || (i == INVEN_LEFT) || (i == INVEN_RIGHT) || (i == INVEN_NECK))
		{
			if (o_ptr->ps > 0)
			{
			  add_throw(prt,o_ptr->pd, o_ptr->ps);
			}
		}
	}

	// heavy armour bonus
	if (p_ptr->active_ability[S_EVN][EVN_HEAVY_ARMOUR] && (typ == GF_HURT))
	{
	  add_throw(prt,1, armour_weight / 150);
	}
	
	return prt;
}

void breath_damage_odds(int dd, int ds, int typ, int resistance){
	odds* dam_odds = make_zero_odds();
	add_throw(dam_odds, dd, ds);
	
	if (resistance > 0) odds_divide(dam_odds,resistance);
	else				odds_multiply(dam_odds, (-resistance));
	
	odds* prot_odds = protection_roll_odds(typ, FALSE);
	
	odds* result = odds_difference_capped(dam_odds, prot_odds);
	take_hit_odds(result,"breath attack");
	kill_odds(dam_odds);
	kill_odds(prot_odds);
	kill_odds(result);
}

odds* check_hit_odds(int power){
	odds* o = hit_roll_odds(power, p_ptr->skill_use[S_EVN] + dodging_bonus(), NULL, PLAYER);
	collapse_to_01(o);
	return o;
}

odds* skill_check_odds(monster_type *m_ptr1, int skill, int difficulty, monster_type *m_ptr2){
	// bonuses against your enemy of choice
	if ((m_ptr1 == PLAYER) && (m_ptr2 != NULL)) skill += bane_bonus(m_ptr2);
	if ((m_ptr2 == PLAYER) && (m_ptr1 != NULL)) difficulty += bane_bonus(m_ptr1);
	
	odds* skill_odds = make_zero_odds();add_throw(skill_odds,1,10);add_const(skill_odds,skill);
	odds* difficulty_odds = make_zero_odds();add_throw(difficulty_odds,1,10);add_const(difficulty_odds,difficulty);
	// player curse?
	if (p_ptr->cursed)
	{ 
		if (m_ptr1 == PLAYER) min_twice(skill_odds);
		if (m_ptr2 == PLAYER) min_twice(difficulty_odds);
	}

	odds* result = odds_difference_capped(skill_odds, difficulty_odds);
	kill_odds(skill_odds);
	kill_odds(difficulty_odds);
	return result;
}

odds* saving_throw_odds(monster_type *m_ptr, bool resistance){
		int player_score = p_ptr->skill_use[S_WIL];
	int difficulty;
	
	if (m_ptr == NULL)	difficulty = 10;
	else				difficulty = monster_will(m_ptr);
	
	if (resistance)		difficulty -= 10;
	
	odds* res = skill_check_odds(m_ptr, difficulty, player_score, PLAYER);
	collapse_to_01(res);
	
	return res;
}

odds* allow_player_aux_odds(monster_type *m_ptr, int player_flag)
{
	bool resistance = FALSE;
	
	if (player_flag)
	{ 
		
		// makes things much easier
		resistance = TRUE;
	}
	
	odds* st = saving_throw_odds(m_ptr, resistance);
	negate(st); //don't allow if saving throw passes, right?
	return st;
}

void take_hit_odds(odds* dam, const char* source){
	printf("==Damage from %s:\n", source);
	print_odds(dam);
	
	//for these computation, check the comments in mort.h
	
	double probdeath=0.0;
	int i;
	//we make this cycle from the end since small probabilities are at the end and for better accuracy they have to be summed first
	//moreover, note that damage == chp results in death ,so there is a >= there.
	for(i=dam->length; i >= p_ptr->chp;i--){
		probdeath += dam->p[i];
	}
	p_ptr->loglive.d += log1p(-probdeath);
	printf("Probability of being dead: %g millimort\n", fabs(-expm1(p_ptr->loglive.d)*1.e3)); //fabs() serves the only purpose of converting -0 to 0.
}

