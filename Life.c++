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

bool AbstractCell::isAlive(){ return state;}

AbstractCell* AbstractCell::get_pointer(){return this;}

// -----------
// Conway Cell
// -----------
ConwayCell::ConwayCell(char S): AbstractCell(S) {}

bool ConwayCell::evolve(AbstractCell** const neighbors){
	int count = 0;
	for(int i = 0; i<=7; ++i){
		if(neighbors[i] != NULL && neighbors[i]->isAlive())
			++count;
	}
	assert(count >= 0 && count <= 7);

	if(state && (count < 2 || count > 3))
		return true;
	else if(!state && count == 3)
		return true;
	return false;
}
void ConwayCell::shift_state(){
	state = !state;
}
char ConwayCell::get_state(){
	if(state)
		return '*';
	else
		return '.';
}

// ------------
// Fredkin Cell
// ------------
FredkinCell::FredkinCell(char S): AbstractCell(S){
	if(S=='+')
		age = 10;
	else if(S <= '9' && S >= '0')
		age = S - '0';
	assert(age >= 0);
}

bool FredkinCell::evolve(AbstractCell** const neighbors){
	int count = 0;
	if(neighbors[1] != NULL && neighbors[1]->isAlive())
		++count;
	if(neighbors[3] != NULL && neighbors[3]->isAlive())
		++count;
	if(neighbors[4] != NULL && neighbors[4]->isAlive())
		++count;
	if(neighbors[6] != NULL && neighbors[6]->isAlive())
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
void FredkinCell::shift_state(){
	state = !state;
}
char FredkinCell::get_state(){
	if(state)
		if(age >= 10)
			return '+';
		else
			return (char)(age+'0');
	else
		return '-';
}

// ----
// Cell
// ----

Cell::Cell(char S){
	if(S == '*'||S == '.')
		_p = new ConwayCell(S);
	else
		_p = new FredkinCell(S);
}

Cell::Cell(const Cell& rhs){
	if(dynamic_cast<ConwayCell*>(rhs._p))
		_p = new ConwayCell(*(dynamic_cast<ConwayCell*>(rhs._p)));
	else
		_p = new FredkinCell(*(dynamic_cast<FredkinCell*>(rhs._p)));
}

Cell::~Cell(){
	delete _p;
}

bool Cell::evolve(AbstractCell** const neighbors){
	char oldstate = _p->get_state();
	// AbstractCell* neighbors[8];
	// for(int i=0; i<8; ++i)
	// 	neighbors[i] = n[i]->_p;
	bool output = _p->evolve(neighbors);
	if(FredkinCell* cell = dynamic_cast<FredkinCell*>(_p)){
		if(oldstate == '1' && cell->get_state()=='2'){
			_p = new ConwayCell('*');
			delete cell;
		}
	}
	return output;
}

// Cell& Cell::operator=(const Cell& rhs){
// 	delete _p;
// 	_p = rhs._p;
// 	return *this;
// }

void Cell::shift_state(){
	_p->shift_state();
}

char Cell::get_state(){
	return _p->get_state();
}

bool Cell::isAlive(){
	return _p->isAlive();
}

AbstractCell* Cell::get_pointer(){
	return _p;
}