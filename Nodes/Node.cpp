#include "Node.h"

extern const double smallBlind;
extern const double bigBlind;

// Constructor implementation
//Node::Node(Node n) :
//	Node(n.state,
//			n.pot,
//			n.boardCards,
//			n.playerList,
//			n.playerTurn) { }

Node::Node(int              state,
		double              pot,
		std::vector<int>    boardCards,
		std::vector<Player> playerList,
		int                 playerTurn) :
	GameObject(state,
			pot,
			boardCards,
			playerList,
			playerTurn) { }

	// Action function implementation
	std::shared_ptr<Node> Node::fold() {
		//create child foldNode
		std::shared_ptr<Node> foldNode(new Node(*this));
		foldNode->isTerminal = true;
		foldChild = foldNode;
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
			getBoardCards(),
			tempPlayerList,
			getPlayerTurn() + 1);
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
