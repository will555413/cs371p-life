#include "Life.h"

using namespace std;

// ------------
// AbstractCell
// ------------
AbstractCell::AbstractCell(char S)
{
	if(S=='.'||S=='-')
		state = false;
	else
		state = true;
}

// -----------
// Conway Cell
// -----------
Conway::Conway(char S): AbstractCell(S) {}

bool Conway::evolve(AbstractCell** const neighbors){
	int count = 0;
	for(int i = 0; i<=7; ++i){
		if(neighbors[i] != NULL && dynamic_cast<Conway*>(neighbors[i])->state)
			++count;
	}
	assert(count >= 0 && count <= 7);

	if(state && (count < 2 || count > 3))
		return true;
	else if(!state && count == 3)
		return true;
	return false;
}
void Conway::shift_state(){
	state = !state;
}
char Conway::get_state(){
	if(state)
		return '*';
	else
		return '.';
}

// ------------
// Fredkin Cell
// ------------
Fredkin::Fredkin(char S): AbstractCell(S){
	if(S=='+')
		age = 10;
	else if(S <= '9' && S >= '0')
		age = S - '0';
	assert(age >= 0 && age <= 10);
}

bool Fredkin::evolve(AbstractCell** const neighbors){
	int count = 0;
	if(neighbors[1] != NULL && dynamic_cast<Fredkin*>(neighbors[1])->state)
		++count;
	if(neighbors[3] != NULL && dynamic_cast<Fredkin*>(neighbors[3])->state)
		++count;
	if(neighbors[4] != NULL && dynamic_cast<Fredkin*>(neighbors[4])->state)
		++count;
	if(neighbors[6] != NULL && dynamic_cast<Fredkin*>(neighbors[6])->state)
		++count;
	assert(count >= 0 && count <= 4);

	if(state && (count == 0 || count == 2 || count == 4)){
		age = 0;
		return true;
	}
	else if(!state && (count == 1 || count == 3)){
		assert(age == 0);
		return true;
	}
	if(state){
		assert(age >= 0);
		++age;
	}
	return false;
}
void Fredkin::shift_state(){
	state = !state;
}
char Fredkin::get_state(){
	if(state)
		if(age >= 10)
			return '+';
		else
			return (char)(age+'0');
	else
		return '-';
}