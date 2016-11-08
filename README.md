# Pooker: An Increasingly Less Terrible Poker AI
* Fully functional two-player No-Limit Texas Hold'em game built ground up in C++

Take a look at the main function!
```cpp
#include "Nodes/Node.cpp"

int main() {
	Node::playGame();
	
	return 0;
}
```

That's it! Each component of the code is broken up into its own class or method, making our code modular to a fault. When we were first starting out, we didn't know what kinds of functionality we might need to add, so we designed our structures to be as extensible as possible.

## Game Structure Classes
These classes are used to record values important to a poker game. Cards were represented using unique integers.

The header files offer a clean, concise view of the class members and interface, but here are the important snippets.
1. Player.h / .cpp
```cpp
double chips;
std::vector<int> holeCards;
```
2. GameObject.h / .cpp
```cpp
Player botPlayer;
Player oppPlayer;
int playerTurn;
std::vector<int> boardCards;
double pot;
```
3. Decision.h / .cpp
```cpp
class Decision {
	public:
    	int action;
        double raiseAmount;
};
```
## The Nodes
We had to code our game implementation so that it'd work smoothly with a Monte Carlo tree search (MCTS). To build the game tree, we decided to create an abstract base class (Node) and two derived classes (ChoiceNode and OpponentNode).


### Abstract Node class
###### The Action Functions
```cpp
virtual void call() = 0;
virtual void raise(double) = 0;
virtual void fold() = 0;
```
These functions move the game forward by creating a child node with the new game state and adding it to the tree. The functions are implemented separately in ChoiceNode(CN) and OpponentNode(ON) to allow potential future specialization, say if we wanted the bot's action functions to start the Monte Carlo

##### Tree linkage
```cpp
Node* parent;
std::unique_ptr<Node> foldChild;
std::unique_ptr<Node> callChild;
std::unique_ptr<Node> raiseChild;
```

In order to avoid memory leaks, we handled memory allocation using std::unique_ptr. A root node is created, which has ultimate ownership of all the child nodes. 

The parent member is a raw pointer because deallocation of the tree is done from the top down; the child nodes should be able to observe and access the parent, but not deallocate it. This will never leak because a parent node is never created from a child.

Note: Originally we had this as a std::shared_ptr, but ran into issues with circular depencies.

##### Copy Constructor and Assignment Operator
When the Monte Carlo simulation is run, we don't want the simulated Nodes to persist on the actual game tree, so we need to run the MCTS on a copy. The default constructs would result in a disastrous member-by-member shallow copy.
```cpp
Node::Node(const Node& obj) :
	foldChild( new Node(obj.pot, obj.playerTurn ... ) ),
    raiseChild( new Node(obj.pot, obj.playerTurn ... ) ),
    callChild( new Node(obj.pot, obj.playerTurn ... ) ) { }
    
Node& Node::operator= (const Node& rhs) {
	// check for self assignment
    if (&rhs == this)
    	return *this;
        
    foldChild.reset( new Node(obj.pot, obj.playerTurn ...) );
    // ...
}
```

## Game Loop
The game structure classes and Nodes provide the infrastructure for the game to be played. Now we need a game loop.

```cpp
// Initializes both players with inital chips and repeatedly
// calls playRound() while both players still have chips
static void Node::playGame();

// Handles playthrough of a single hand, plays the appropriate action
// returned by playTurn, deals boardCards and evaluates showdowns
static void Node::playRound(Player& botPlayer, Player& oppPlayer);

// Returns an int representing which action to play, action is chosen
// with the makeDecision() function
int Node::playTurn(std::vector<int> deck);

// specifies method of making decision, i.e., taking user-input or
// using Monte Carlo Tree Search
// Implemented differently for ChoiceNode vs OpponentNode
virtual Decision makeDecision(std::vector<int> deck) = 0;
```

We designed the game loop to work separately from the makeDecision() function. This way the implementation of makeDecision could be abstracted and we'd be able to switch between different decision strategies without modifying anything except for the makeDecision() function.
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
