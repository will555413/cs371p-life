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
	AbstractCell(char S);
	// in conway/fredkin cell, attempt will look at the neighbors and return true if state change, false otherwise.
	// since this is not a concrete class, always return false
	virtual bool evolve(AbstractCell** const neighbors) = 0;
	// shift the state of this cell
	virtual void shift_state() = 0;
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
	// shift the state of this conway cell
	void shift_state();
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
	// shift the state of this fredkin cell
	void shift_state();
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
	~Cell();
	
	// Cell(AbstractCell* address);
	bool evolve(AbstractCell** const neighbors);
	Cell& operator = (const Cell& rhs);
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
		// bool conway = (type == "ConwayCell");
		// cout<<"Type: "<<type<<endl<<r<<" rows"<<endl<<c<<" columns"<<endl;
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
		print_grid(0);
	}

	// ~Life(){
	// 	for(vector<T*> row:grid)
	// 		for(T* element:row)
	// 			delete element;
	// }

	void run_evolution(int generations){
		// variable that saves the size of the grid
		int r = grid.size(), c = grid[0].size();
		for(; _g<generations; ++_g){
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
							neighbors[3] = grid[i-1][j].get_pointer();
						}
						neighbors[1] = grid[i][j-1].get_pointer();
						if(i+1<r){
							neighbors[2] = grid[i+1][j-1].get_pointer();
							neighbors[4] = grid[i+1][j].get_pointer();
						}
					}
					if(j+1<c){
						if(i-1>=0)
							neighbors[5] = grid[i-1][j+1].get_pointer();
						neighbors[6] = grid[i][j+1].get_pointer();
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

	void print_grid(int gen){
		cout<<"Generation = "<<gen<<", Population = "<<population<<"."<<endl;
		run_evolution(gen);
		for(unsigned int i = 0; i<grid.size(); ++i){
			for(unsigned int j = 0; j<grid[i].size(); ++j){
				cout<<grid[i][j].get_state();
			}
			cout<<endl;
		}
		cout<<endl;
	}
};