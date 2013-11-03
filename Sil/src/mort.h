#ifndef INCLUDED_MORT_H
#define INCLUDED_MORT_H

#include "angband.h"
//#include "defines.h"
//#include "externs.h"

/*
 * structure to keep probabilities of drawing nonnegative numbers (pdf)
 * p[i] contains the probability of rolling a total of i, i>=0
 */

struct odds
{
  int length;
  double *p;
};

typedef struct odds odds;

/* functions to manage odds (i.e., discrete pdf's over [0,length-1]) */
/* all returned results have to be freed with kill_odds*/
odds* make_empty_odds(); /*return an "empty throw" of 0d1*/
double* make_p(int length); /*allocates a double[length] and sets it to 0.0*/ 
void set_odds(odds* o, double *p, int length); /*set the content of o to the given pdf and length p. Will take ownership of the pointer p*/
void add_throw(odds* o, int n, int sides); /*adds to the current odds n dice with a given number of sides*/
void add_variable_throw(odds* o, odds *n, int sides); /*adds to the current odds n dice with a given number of sides, where n varies according to other odds*/
void add_const(odds* o, int n);
void kill_odds(odds*); /*frees memory*/
void min_twice(odds*); /*set odds to the pdf of min(a,b), for (a,b) independent and distributed as odds*/

void print_odds(odds*);

odds* odds_difference_capped(odds* o1, odds* o2); /*returns pdf of MAX(0,o1-o2) */

/*simulates the result of hit_roll. Returns 0 for any result <0*/
/*the logic must match hit_roll(...) in cmd1.c, in case it gets changed */
odds* hit_roll_odds(int att, int evn, const monster_type *m_ptr1, const monster_type *m_ptr2); 

/*simulates the result of dam_roll and return odds of possible answers.*/
/*logic must match make_attack_normal in cmd1.c */
/*passes all the parameters that we need. The r_ptr is the one passed to crit_bonus */
/*there is a switch so that if !effect then we always hit - not sure if this is actually used anywhere */
odds* dam_roll_normal(odds* hit_result, int dd, int ds, const monster_race *r_ptr, elem_bonus_dice, int effect);

/*TODO: simulate make_attack_normal, make_attack_ranged from melee1.c */


#endif /*INCLUDED_MORT_H*/
