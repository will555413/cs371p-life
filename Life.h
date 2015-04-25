#include <cassert>	// assert
#include <string>	// string
#include <vector>	// vector
#include <iostream> // endl, istream, ostream
#include <sstream>  // istringstream
#include <cstdlib>	// rand

using namespace std;

class AbstractCell{
protected:
	bool state;
public:
	// initialize state as true/alive or false/dead depending on the input S
	// In fact, as long as the input is not '-' or '.', it will be consider as alive
	AbstractCell(char S);
	// in conway/fredkin cell, attempt will look at the neighbors and return true if state change, false otherwise.
	// since this is not a concrete class, always return false
	virtual bool evolve(AbstractCell** const neighbors) = 0;
	// shift the state of this cell
	void shift_state();
	// this shouldn't ever be called. therefore return N as a error flag
	virtual char get_state() = 0;

	bool isAlive();

	AbstractCell* get_pointer();
};

class ConwayCell: public AbstractCell{
public:
	// Although Conway has no extra members to initialized compared to its parent class AbstractCell,
	// it's easier to differentiate two type of initialization (Conway/Fredkin) by having a dummy constructor that simply calls the parent constructor.
	ConwayCell(char S);
	// return true if state has changed, false otherwise 
	bool evolve(AbstractCell** const neighbors);
	// print corresponding symbol of the current state: * if alive, . if dead
	char get_state();
};

class FredkinCell: public AbstractCell{
private:
	// record the age of this Fredkin cell
	int age = 0;
public:
	// initialize the state AND the age as 0
	FredkinCell(char S);
	// fredkin cell evolution not only need to check its state shifting, it also need to reset/increment the age accordingly
	// return true if state has changed, false otherwise 
	bool evolve(AbstractCell** const neighbors);
	// print corresponding symbol of the current state: a number if alive, - if dead
	char get_state();
};

// Class cell would be structurally identical to AbstractCell with following differences
// has a non-virtual evolve method.
// Knowing that my world will be either consist of AbstractCell pointers or Cell pointers,
// letting cell have a concrete evolve will ensure that every call through Cell* will call Cell::evolve.
// That allow me to put extra code on the Cell class.
// And then I'll dynamic_cast(this) into a Fradkin/Conway pointer depending on the situation

// update: class Cell is a handle class
class Cell{
private:
	AbstractCell* _p;
public:
	Cell(char S);
	Cell(const Cell& rhs);
	~Cell();
	
	// Cell(AbstractCell* address);
	bool evolve(AbstractCell** const neighbors);
	// Cell& operator = (const Cell& rhs);
	void shift_state();
	char get_state();
	bool isAlive();
	AbstractCell* get_pointer();
};

template<typename T>
class Life{
private:
	int population = 0, _g=0;
	vector<vector<T>> grid;
public:
	Life(istream& input){
		int r, c;
		string type;// decides what type of cell we'll be incorporating
		input>>type;
		input>>r>>c;
		for(int i = 0; i<r; ++i){
			grid.push_back(vector<T>());
			for(int j = 0; j<c; ++j){
				char temp;
				input>>temp;

				T cell(temp);
				grid[i].push_back(cell);
				if(cell.isAlive())
					++population;
			}
		}
	}

private:
	// evolve until the current generation count, _g, reached the specified generation, specific_g
	// in this method, I will supply all neighboring cells to the evolving cell, and, 
	// depending on the type of the evolving cell, some neighbors' state will be ignore (i.e. Fredkin cell ignores the diagnols)
	// the 8 neighboring cells will be pointed by an array of AbstractCell*, and their corresponding index looks like:
	// 0 1 2
	// 3 e 4
	// 5 6 7
	// e being the evolving cell.
	void run_evolution(int specific_g){
		// variable that saves the size of the grid
		int r = grid.size(), c = grid[0].size();
		for(; _g<specific_g; ++_g){
			// buffer that records all Cells that needs to shift its state after each iteration
			// this allow me to iterate through the grid only once and gather all the changes need to be done without changing the state of each cell
			vector<AbstractCell*> buffer;
			// iterate through the grid
			for(int i = 0; i < r; ++i){
				for(int j = 0; j < c; ++j){
					// construct a array of 8 pointers that'll later be used by the AbstractCell/Cell class to determine the state of the neighbors
					AbstractCell* neighbors[8] = {NULL};
					if(j-1>=0){
						if(i-1>=0){
							neighbors[0] = grid[i-1][j-1].get_pointer();
						}
						neighbors[3] = grid[i][j-1].get_pointer();
						if(i+1<r){
							neighbors[5] = grid[i+1][j-1].get_pointer();
						}
					}
					if(i-1>=0)
						neighbors[1] = grid[i-1][j].get_pointer();
					if(i+1<r)
						neighbors[6] = grid[i+1][j].get_pointer();
					if(j+1<c){
						if(i-1>=0)
							neighbors[2] = grid[i-1][j+1].get_pointer();
						neighbors[4] = grid[i][j+1].get_pointer();
						if(i+1<r)
							neighbors[7] = grid[i+1][j+1].get_pointer();
					}

					// if evolve() return true, which means the cell shifts its state, push it onto the buffer
					if(grid[i][j].evolve(neighbors))
						buffer.push_back(grid[i][j].get_pointer());
				}
			}
			// for each cell pointed by the buffer, shift its state.
			for(AbstractCell* to_change : buffer){
				to_change->shift_state();
				if(to_change->isAlive())
					++population;
				else
					--population;
			}
		}
	}

public:
	// print grid of the generation gen
	void print_grid(int gen, ostream& out){
		run_evolution(gen);
		out<<"Generation = "<<gen<<", Population = "<<population<<"."<<endl;
		for(unsigned int i = 0; i<grid.size(); ++i){
			for(unsigned int j = 0; j<grid[i].size(); ++j){
				out<<grid[i][j].get_state();
			}
			out<<endl;
		}
		out<<endl;
	}
};