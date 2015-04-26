#include <cassert>	// assert
#include <string>	// string
#include <vector>	// vector
#include <iostream> // endl, istream, ostream
#include <sstream>  // istringstream
#include <cstdlib>	// rand

using namespace std;

class AbstractCell{
protected:
	// -----
	// state
	// -----
	/*
	 * Stored the alive/dead status of the cell.
	 */
	bool state;
public:
	// -------------------------------
	// AbstractCell's char constructor
	// -------------------------------
	/* 
	 * Initialize state as true/alive or false/dead depending on the input S.
	 * In fact, as long as the input is not '-' or '.', it will be consider as alive.
	 */

	AbstractCell(char S);
	// -----------------
	// Abstract evolve()
	// -----------------
	/* 
	 * In Conway/Fredkin cell, evolve will look at the neighbors and return true if state change required, false otherwise.
	 * Since this is not a concrete class, I made this abstract (AKA pure virtual) to force the child class to implement this method.
	 */
	virtual bool evolve(AbstractCell** const neighbors) = 0;

	// -----------
	// shift_state
	// -----------
	/* 
	 * Simply shift the state of this cell.
	 * By separating the state shifting from evolve(), I can iterate through the life grid only once and without making a copy of the grid.
	 * I'll explain this concept in detail in the description of run_evolution() in class Life.
	 */
	void shift_state();

	// ---------
	// get_state
	// ---------
	/*
	 * In Conway/Fredkin cell, get_state will return a char representing the current state for the each corresponding class.
	 * This is again an abstract method, since each child class might represent a live/dead cell differently.
	 */
	virtual char get_state() = 0;

	// -------
	// isAlive
	// -------
	/*
	 * Tells whether a cell is alive or not.
	 * Since that information is simply stored as a bool in state, I could just return the value of state.
	 */
	bool isAlive();
};

class ConwayCell: public AbstractCell{
public:
	// -----------------------------
	// ConwayCell's char constructor 
	// -----------------------------
	/* 
	 * Although Conway has no extra members to initialized, it's easier to differentiate two type of initialization (Conway/Fredkin) by having a dummy constructor that simply calls the parent constructor.
	 */
	ConwayCell(char S);

	// -------------------------------------
	// concrete evolve method for ConwayCell
	// -------------------------------------
	/*
	 * Determine whether the state needs to be change or not using rules for a Conway Cell.
	 * Return true if the state needs to be changed, false otherwise.
	 */
	bool evolve(AbstractCell** const neighbors);

	// ----------------------------------------
	// concrete get_state method for ConwayCell
	// ----------------------------------------
	/*
	 * Print corresponding symbol of the current state: '*' if alive, '.' if dead.
	 */
	char get_state();
};

class FredkinCell: public AbstractCell{
private:
	// ---
	// age
	// ---
	/*
	 * Record the age of this Fredkin cell.
	 */
	int age = 0;
public:
	// ------------------------------
	// FredkinCell's char constructor
	// ------------------------------
	/*
	 * Unlike ConwayCell, FredkinCell has one more member to initialize, its age.
	 * This constructor will initialize the state according to the input AND the age as 0.
	 */
	FredkinCell(char S);

	// --------------------------------------
	// concrete evolve method for FredkinCell
	// --------------------------------------
	/*
	 * A FredkinCell's evolution not only need to check its state shifting, it also need to reset/increment the age accordingly.
	 * Return true if the state needs to changed, false otherwise.
	 */
	bool evolve(AbstractCell** const neighbors);

	// -----------------------------------------
	// concrete get_state method for FredkinCell
	// -----------------------------------------
	/*
	 * Print corresponding symbol of the current state: a number if alive, '-' if dead.
	 */
	char get_state();
};

class Cell{
private:
	// --
	// _p
	// --
	/*
	 * Stored the address of the AbstractCell object Cell handles.
	 */
	AbstractCell* _p;
public:
	// -----------------------
	// Cell's char constructor
	// -----------------------
	/*
	 * Create a ConwayCell/FredkinCell object on heap and saves its address in _p.
	 */
	Cell(char S);
	// -----------------------
	// Cell's copy constructor
	// -----------------------
	/*
	 * Extract the AbstractCell object rhs is handling and duplicate that object on heap.
	 * Stored the address to the duplicate created in _p;
	 */
	Cell(const Cell& rhs);
	// -----------------
	// Cell's destructor
	// -----------------
	/*
	 * Deallocate the space _p pointed to.
	 */
	~Cell();
	
	// ------
	// evolve
	// ------
	/*
	 * Other than running the regular evolve() on the handled cell, Cell's evolve() also monitor the type of such cell.
	 * If the handled cell is of type FredkinCell and just change from age 1 to 2, deallocate the original cell and redirect _p to a new alive ConwayCell on heap.
	 */
	bool evolve(AbstractCell** const neighbors);

	// ---------
	// operator&
	// ---------
	/*
	 * Rather than returning Cell's address, I overloaded operator& to return _p;
	 */
	AbstractCell* operator & ();

	// -----------
	// shift_state
	// -----------
	/*
	 * Calls handled cell's shift_state method.
	 */
	void shift_state();

	// ---------
	// get_state
	// ---------
	/*
	 * Calls handled cell's get_state method.
	 */
	char get_state();

	// -------
	// isAlive
	// -------
	/*
	 * Calls handled cell's isAlive method.
	 */
	bool isAlive();
};

template<typename T>
class Life{
private:
	// -----------------
	// population and _g
	// -----------------
	/*
	 * Population, as its name suggested, stored the current population in this grid.
	 * _g stored the current generation.
	 */
	int population = 0, _g=0;
	// ----
	// grid
	// ----
	/*
	 * Stored the world as a vector of vectors of user specified type.
	 */
	vector<vector<T>> grid;
public:
	// --------------------------
	// Life's istream constructor
	// --------------------------
	/*
	 * Reads input from user specified istream and builds the grid using that input.
	 */
	Life(istream& input){
		int r, c;
		string type;
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
	// -------------
	// run_evolution
	// -------------
	/* 
	 * Runs evolution until the current generation count, _g, reached the specified generation, specific_g.
	 * In this method, I will supply all neighboring cells to the evolving cell, and, depending on the type of the evolving cell, some neighbors' state will be ignore (i.e. Fredkin cell ignores the diagnols).
	 * The 8 neighboring cells will be pointed by an array of AbstractCell*, and their corresponding index looks like:
	 * 0 1 2
	 * 3 e 4
	 * 5 6 7
	 * e being the evolving cell.
	 * Whenever a cell needs ite state changed, I put the address of that cell into a buffer (I'm using a vector of Abstract*), and, at the very end of an evolution, I shift the state of every cell in such buffer.
	 * By doing so, I no longer need to make a copy of the grid just to look at the state of each cell since it won't change until the end of evolution.
	 */
	void run_evolution(int specific_g){
		int r = grid.size(), c = grid[0].size();
		for(; _g<specific_g; ++_g){
			vector<AbstractCell*> buffer;
			// Iterate through the grid
			for(int i = 0; i < r; ++i){
				for(int j = 0; j < c; ++j){
					// Construct a array of 8 pointers that'll later be used to determine the state of the neighbors.
					AbstractCell* neighbors[8] = {NULL};
					if(j-1>=0){
						if(i-1>=0){
							neighbors[0] = &grid[i-1][j-1];
						}
						neighbors[3] = &grid[i][j-1];
						if(i+1<r){
							neighbors[5] = &grid[i+1][j-1];
						}
					}
					if(i-1>=0)
						neighbors[1] = &grid[i-1][j];
					if(i+1<r)
						neighbors[6] = &grid[i+1][j];
					if(j+1<c){
						if(i-1>=0)
							neighbors[2] = &grid[i-1][j+1];
						neighbors[4] = &grid[i][j+1];
						if(i+1<r)
							neighbors[7] = &grid[i+1][j+1];
					}

					// If evolve() return true, which means the cell needs to shift its state, push it onto the buffer.
					if(grid[i][j].evolve(neighbors))
						buffer.push_back(&grid[i][j]);
				}
			}
			// For each cell pointed by the buffer, shift its state.
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
	// ----------
	// print_grid
	// ----------
	/*
	 * Print grid of the generation gen.
	 */
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