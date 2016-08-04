#include "Node.h"

#include <memory>

Node::Node(int              state,
		double              pot,
		std::vector<int>    boardCards,
		Player				botPlayer,
		Player				oppPlayer,
		int                 playerTurn,
		std::shared_ptr<Node> parent) :
	game(state,
			pot,
			boardCards,
			botPlayer,
			oppPlayer,
			playerTurn),
	parent(parent) { }

	// Action function implementation
	std::shared_ptr<Node> Node::fold() {
		//create child foldNode
		std::shared_ptr<Node> foldNode(new Node(*this));
		foldNode->isTerminal = true;
		foldChild = foldNode;
		return foldNode;
	}

std::shared_ptr<Node> Node::call(double callAmount) { //remove call amount later
	//creates a temporary playerlist and updates the player's potinvestment and chip count
	if (game.getPlayerTurn() == 0){
		Player tempPlayer = game.getBotPlayer();
		tempPlayer.setPotInvestment(callAmount + tempPlayer.getPotInvestment());
		tempPlayer.setChips(tempPlayer.getChips() - callAmount);
		auto callNode = std::make_shared<Node>(game.getState() + 1,
			game.getPot() + callAmount,
			game.getBoardCards(),
			tempPlayer,
			game.getOppPlayer(),
			game.getPlayerTurn() + 1,
			std::shared_ptr<Node> (this) );
		callChild = callNode;
	} else {
		Player tempPlayer = game.getOppPlayer();
		tempPlayer.setPotInvestment(callAmount + tempPlayer.getPotInvestment());
		tempPlayer.setChips(tempPlayer.getChips() - callAmount);
		auto callNode = std::make_shared<Node>( game.getState() + 1,
			game.getPot() + callAmount,
			game.getBoardCards(),
			game.getBotPlayer(),
			tempPlayer,
			game.getPlayerTurn() + 1,
			std::shared_ptr<Node> (this) );
		callChild = callNode;
	}
	return callChild;
}

std::shared_ptr<Node> Node::raise(double raiseAmount) {
	if (game.getPlayerTurn() == 0){
		Player tempPlayer = game.getBotPlayer();
		tempPlayer.setPotInvestment(raiseAmount + tempPlayer.getPotInvestment());
		tempPlayer.setChips(tempPlayer.getChips() - raiseAmount);
		auto raiseNode = std::make_shared<Node>( game.getState(),
			game.getPot() + raiseAmount,
			game.getBoardCards(),
			tempPlayer,
			game.getOppPlayer(),
			game.getPlayerTurn() + 1,
			std::shared_ptr<Node> (this) );
		(*raiseNode).addCurrentRaise(raiseAmount);
		raiseChild = raiseNode;
	} else {
		Player tempPlayer = game.getOppPlayer();
		tempPlayer.setPotInvestment(raiseAmount + tempPlayer.getPotInvestment());
		tempPlayer.setChips(tempPlayer.getChips() - raiseAmount);
		auto raiseNode = std::make_shared<Node>( game.getState(),
			game.getPot() + raiseAmount,
			game.getBoardCards(),
			game.getBotPlayer(),
			tempPlayer,
			game.getPlayerTurn() + 1,
			std::shared_ptr<Node> (this) );
		(*raiseNode).addCurrentRaise(raiseAmount);
		raiseChild = raiseNode;
	}
	
	return raiseChild;
}
