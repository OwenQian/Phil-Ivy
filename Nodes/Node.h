#ifndef NODE_H
#define NODE_H

#include "../GameObject.h"
#include "../Player.h"

#include <vector>
#include <memory>

class Node: public GameObject {
	protected:
		//static const int EV_Const = 1;
		std::shared_ptr<Node> foldChild;
		std::shared_ptr<Node> callChild;
		std::shared_ptr<Node> raiseChild;
		std::shared_ptr<Node> const parent; //the parent of this node in the game tree
		int visitCount;	                    //number of times a node has been visited/simulated on
		double expectedValue; 	            //the expected value derived from choosing this node action, updated through backprop
		bool isTerminal;	        //whether or not Node is terminal
		double currentRaise;                   //the current raise

	public:
		// Member-accessibility functions

		// Getters
		std::shared_ptr<Node> getFoldChild() const;
		std::shared_ptr<Node> getCallChild() const;
		std::shared_ptr<Node> getRaiseChild() const;
		std::shared_ptr<Node> getParent() const;
		int getVisitCount() const;
		double getExpectedValue() const;
		bool getTerminalStatus() const;
		double getCurrentRaise() const;

		// Setters
		void setCurrentRaise(double);

		// Constructors
		Node();
		Node(	int						state,
				double					pot,
				std::vector<int>		boardCards,
				std::vector<Player>		playerList,
				int						playerTurn);

		// Action functions
		virtual std::shared_ptr<Node> fold(); 
		virtual std::shared_ptr<Node> raise(double raiseAmount);
		virtual std::shared_ptr<Node> call(double callAmount);
};

// ##################### Start of implementation ######################

// Constructor implementation

Node::Node(	int								state,
			double							pot,
			std::vector<int>				boardCards,
			std::vector<Player> 			playerList,
			int								playerTurn) :
	GameObject(	state,
				pot,
				boardCards,
				playerList,
				playerTurn) { }


// Member accessibility functions

// Getters
std::shared_ptr<Node> Node::getCallChild() const { return callChild; }
std::shared_ptr<Node> Node::getRaiseChild() const { return raiseChild; }
std::shared_ptr<Node> Node::getFoldChild() const { return foldChild; }
std::shared_ptr<Node> Node::getParent() const { return parent; } 
int Node::getVisitCount() const { return visitCount; } 
double Node::getCurrentRaise() const { return currentRaise; } 
double Node::getExpectedValue() const { return expectedValue; } 
bool Node::getTerminalStatus() const { return isTerminal; }

// Setters
void Node::setCurrentRaise(double amount){ currentRaise = amount; }

// Action function implementation
 std::shared_ptr<Node> Node::fold() {
	std::shared_ptr<Node> foldNode(new Node());   //create child foldNode
	foldNode->isTerminal = true;
	foldChild = foldNode;                 //childList[0] holds the foldNodes
	return foldNode;
}

std::shared_ptr<Node> Node::call(double callAmount) {
	//creates a temporary playerlist and updates the player's potinvestment and chip count
	std::vector<Player>tempPlayerList = getPlayerList();
	// Creating tempPlayer and updating its values
	Player tempPlayer = tempPlayerList[getPlayerTurn()];
	//match raise by subtracting chip amount by raise amount
	//Increase player potinvestment
	tempPlayerList[getPlayerTurn()].setChips(tempPlayer.getChips() - getCurrentRaise());    //update tempPlayerList
	tempPlayerList[getPlayerTurn()].setPotInvestment(tempPlayer.getPotInvestment() + getCurrentRaise());    //update tempPlayerList

	
	// increasing state by 1, increasing pot by raiseAmount,
	// blinds and board cards stay the same, playerList updated 
	auto callNode = std::make_shared<Node>( getState() + 1,
											getPot() + getCurrentRaise(),
											getSmallBlind(),
											getBigBlind(),
											getBoardCards(),
											tempPlayerList);
	callChild = callNode;
	return callNode;
}

std::shared_ptr<Node> Node::raise(double raiseAmount) {
	auto raiseNode = std::make_shared<Node>(state,
											pot + raiseAmount,
											boardCards,
											playerList,
											playerTurn + 1);
	raiseChild = raiseNode;
	return raiseNode;
}

#endif	//Node.h
