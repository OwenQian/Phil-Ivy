# Pooker: Not so shitty Poker AI
* Fully functional two-player No-Limit Texas Hold'em game built ground up in C++

## Game Structure Classes
These classes are used to record values important to a poker game. Cards were represented using unique integers.

The header files offer a clean, concise view of the class members and interface, but here are the important snippets.
1. Player.h / .cpp
```
double chips;
std::vector<int> holeCards;
```
2. GameObject.h / .cpp
```
Player botPlayer;
Player oppPlayer
int playerTurn;
std::vector<int> boardCards;
double pot;
```
## The Nodes
We had to code our game implementation so that it'd work smoothly with a Monte Carlo tree search (MCTS). To build the game tree, we decided to create an abstract base class (Node) and two derived classes (ChoiceNode and OpponentNode).


### Abstract Node class
###### The Action Functions
```
virtual void call() = 0;
virtual void raise(double) = 0;
virtual void call() = 0;
```
These functions move the game forward by creating a child node with the new game state and adding it to the tree. The functions are implemented separately in ChoiceNode(CN) and OpponentNode(ON) to allow potential future specialization, say if we wanted the bot's action functions to start the Monte Carlo

##### Tree linkage
```
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
```
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

##### Game Loop

