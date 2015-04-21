#include <cassert>	// assert
#include <string>	// string
#include <vector>	// vector
#include <iostream> // endl, istream, ostream
#include <sstream>  // istringstream
#include <cstdlib>	// rand

using namespace std;

class AbstractCell{
public:
	bool state;
public:
	// initialize state as true/alive or false/dead depending on the input S
	AbstractCell(char S);
	// in conway/fredkin cell, attempt will look at the neighbors and return true if state change, false otherwise.
	// since this is not a concrete class, always return false
	virtual bool evolve(AbstractCell** const neighbors) = 0;
	// shift the state of this cell
	virtual void shift_state() = 0;
	// this shouldn't ever be called. therefore return N as a error flag
	virtual char get_state() = 0;
};

// Class cell would be structurally identical to AbstractCell with following differences
// has a non-virtual evolve method.
// Knowing that my world will be either consist of AbstractCell pointers or Cell pointers,
// letting cell have a concrete evolve will ensure that every call through Cell* will call Cell::evolve.
// That allow me to put extra code on the Cell class.
// And then I'll dynamic_cast(this) into a Fradkin/Conway pointer depending on the situation
class Cell{
public:
	bool evolve(AbstractCell** const neighbors);
// private:
// 	AbstractCell* _cell;
// public:
// 	// set _cell to point to conway/fredkin cell
// 	Cell();
// 	// return true if state has changed, false otherwise 
// 	bool evolve(const int&);
// 	// shift the state of this fredkin cell
// 	void shift_state();
// 	// print corresponding symbol of the current state depending on the type of cell
// 	char get_state();
};


class Conway: public AbstractCell{
public:
	// Although Conway has no extra members to initialized compared to its parent class AbstractCell,
	// it's easier to differentiate two type of initialization (Conway/Fredkin) by having a dummy constructor that simply calls the parent constructor.
	Conway(char S);
	// return true if state has changed, false otherwise 
	bool evolve(AbstractCell** const neighbors);
	// shift the state of this conway cell
	void shift_state();
	// print corresponding symbol of the current state: * if alive, . if dead
	char get_state();
};

class Fredkin: public AbstractCell{
private:
	// record the age of this Fredkin cell
	int age = 0;
public:
	// initialize the state AND the age as 0
	Fredkin(char S);
	// fredkin cell evolution not only need to check its state shifting, it also need to reset/increment the age accordingly
	// return true if state has changed, false otherwise 
	bool evolve(AbstractCell** const neighbors);
	// shift the state of this fredkin cell
	void shift_state();
	// print corresponding symbol of the current state: a number if alive, - if dead
	char get_state();
};

template<typename T>
class Life{
private:
	int population = 0;
	vector<vector<T*>> grid;
public:
	Life(istream& input){
		int r, c;
		string type;// decides what type of cell we'll be incorporating
		input>>type;
		input>>r>>c;
		bool conway = (type == "ConwayCell");
		// cout<<"Type: "<<type<<endl<<r<<" rows"<<endl<<c<<" columns"<<endl;
		for(int i = 0; i<r; ++i){
			grid.push_back(vector<T*>());
			for(int j = 0; j<c; ++j){
				char temp;
				input>>temp;
				if(temp != '.' && temp != '-')
					++population;

				if(conway)
					grid[i].push_back(new Conway(temp));
				else
					grid[i].push_back(new Fredkin(temp));
			}
		}	
	}

	void run_evolution(int generations){
		// print initial grid
		print_grid(0);
		// variable that saves the size of the grid
		int r = grid.size(), c = grid[0].size();
		for(int g = 1; g<generations; ++g){
			// buffer that records all Cells that needs to shift its state after each iteration
			// this allow me to iterate through the grid only once and gather all the changes need to be done without changing the state of each cell
			vector<T*> buffer;
			// iterate through the grid
			for(int i = 0; i < r; ++i){
				for(int j = 0; j < c; ++j){
					// construct a array of 8 pointers that'll later be used by the AbstractCell/Cell class to determine the state of the neighbors
					T* neighbors[8] = {NULL};
					if(j-1>=0){
						if(i-1>=0){
							neighbors[0]= grid[i-1][j-1];
							neighbors[3]= grid[i-1][j];
						}
						neighbors[1] = grid[i][j-1];
						if(i+1<r){
							neighbors[2]= grid[i+1][j-1];
							neighbors[4]= grid[i+1][j];
						}
					}
					if(j+1<c){
						if(i-1>=0)
							neighbors[5]= grid[i-1][j+1];
						neighbors[6] = grid[i][j+1];
						if(i+1<r)
							neighbors[7]= grid[i+1][j+1];
					}

					// if evolve() return true, which means the cell shifts its state, push it onto the buffer
					if(grid[i][j]->evolve(neighbors))
						buffer.push_back(grid[i][j]);
				}
			}
			// for each cell pointed by the buffer, shift its state.
			for(T* to_change : buffer){
				to_change->shift_state();
				// if the state after shifting is alive, ++population, otherwise --population
				char temp = to_change->get_state();
				if(temp != '.' && temp != '-')
					++population;
				else
					--population;
			}
			print_grid(g);
		}
	}

	void print_grid(int gen){
		cout<<"Generation = "<<gen<<", Population = "<<population<<"."<<endl;
		for(unsigned int i = 0; i<grid.size(); ++i){
			for(unsigned int j = 0; j<grid[i].size(); ++j){
				cout<<grid[i][j]->get_state();
			}
			cout<<endl;
		}
		cout<<endl;
	}
};