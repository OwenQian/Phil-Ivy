# Phil Ivy: An Increasingly Less Terrible Poker AI
* Fully functional two-player No-Limit Texas Hold'em game built ground up in C++

## Running the AI
```
  $ make
  $ ./main
```

That's it!
## Monte Carlo

##### How it works
The Monte Carlo tree search algorithm uses the following four steps

1. Selection
	* Starting from initial Node, choose which child node to explore more deeply, stop if Node is terminal
	* Explore vs. Exploit balanced using Upper Confidence Bound in Trees formula (UCT)
![alt text](https://github.com/OwenQian/Pooker/blob/master/Pictures/naiveUCT.png "naiveUCT formula")
	* wi/ni is the averaged expected value, c is the exploration constant, t is the visit count of the node and ni is the visit count of the considered child node
2. Expansion
	* Create child nodes if they don't already exist
3. Simulation
	* Quickly play out the game to a terminal state (folded or showdown) to find estimate of score
4. Backpropagaion
	* Update parent nodes with results of simulation/selection

##### Our Code
```cpp
// Call runSelection while there's still time left
Action monteCarlo(int maxTime, std::vector<int> deck);

// Expand child nodes if they don't exist and recursively choose child 
// node with highest naiveUCT score until terminal state is reached
void runSelection(std::vector<int> deck);

// When a new node is expanded, call() until a terminal game state is 
// reached. The results of the simulation is used to calculate
// expected value (EV)
void runSimulation(std::vector<int> deck);

// When a terminal state is reached, update the parents visit count
// and expected value
void backprop(double, double);

// Calculates score used by runSelection() to pick next node
void naiveUCT(std::vector<double>&);

```
