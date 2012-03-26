#ifndef MISC_H
#define MISC_H

struct Cost
{
    int Gold;
    int Wood;
    int Food;
	
	Cost(int p_gold = 0, int p_wood = 0, int p_food = 0) : Gold(p_gold), Wood(p_wood), Food(p_food) { }
};

#endif // MISC_HL