#ifndef NODE_H
#define NODE_H

#include "GameObject.h"
#include "Player.h"

#include <vector>
#include <memory>

class Node: public GameObject {
	protected:
		//static const int EV_Const = 1;
		std::vector<std::shared_ptr<Node> > childList; // 0 = fold, 1 = call, 2 = raise
		std:shared_ptr<Node> const parent; //the parent of this node in the game tree
		//GameObject game; //unsure whether it is better for node to inherit gameObject or to have 
						   //gameObject as a field
		int visitCount;    //number of times a node has been visited/simulated on
		double expectedValue; 	//the expected value derived from choosing this node action, updated through backprop
		bool isTerminal = false;	//whether or not Node is terminal
		int currentRaise; //the current raise

	public:
		// Member-accessibility functions
		std::vector<Node> getChildList() const;
		Node* getParent() const;
		//GameObject getGame() const;
		int getVisitCount() const;
		double getExpectedValue() const;
		bool getTerminalStatus() const;
		
		

		// Constructor
		Node();

		Node(int state,
				double pot,
				std::vector<int> boardCards,
				std::vector<Player> playerList,
				int playerTurn);

		// Action functions
		virtual shared_ptr<Node> fold(); 
		virtual shared_ptr<Node> raise(double raiseAmount);
		virtual shared_ptr<Node> call(double callAmount = 0.0);


		// ##################### Start of implementation ######################

		// Getter and Setter implementation
		std::shared_ptr getParent() {
			return parent;
		}

		int getVisitCount() {
			return visitCount;
		}
		
		int getCurrentRaise(){
			return currentRaise;
		}

		double getExpectedValue() {
			return expectedValue;
		}

		bool getTerminalStatus() {
			return isTerminal;
		}
		
		void setCurrentRaise(double amount){
			currentRaise = amount;
		}

		std::vector<Node> getChildList() {
			return childList;
		}

		// Constructor implementation
		Node() {
			childList.resize(3, NULL);
		}
		Node(int state,
				double pot,
				std::vector<int> boardCards,
				std::vector<Player> playerList,
				int playerTurn): 
			GameObject(int state,
					double pot,
					std::vector<int> boardCards,
					std::vector<Player> playerList,
					int playerTurn) { }

		// Action function implementation
		virtual std::shared_ptr<Node> fold() {
			std::shared_ptr<Node> foldNode(new Node());   //create child foldNode
			foldNode->isTerminal = true;
			this->childList[0] = foldNode;                 //childList[0] holds the foldNodes
			return foldNode;
		}
		
		virtual std::shared_ptr<Node> call() {
			//creates a temporary playerlist and updates the player's potinvestment and chip count
			std::vector<Player>tempPlayerList;
			tempPlayerList.reserve(game.getPlayerList().size());
			//copying the elements of current playerlist into the temp
			std::copy(game.getPlayerList().begin(), game.getPlayerList().end(), back_inserter(tempPlayerList));
			//updating the values
			tempPlayerList[game.getPlayerTurn()].setChips(tempPlayerList[game.getPlayerTurn()].getChips() - currentRaise); // NOT STRAIGHT
			tempPlayerList[game.getPlayerTurn()].setPotInvestment(tempPlayerList[game.getPlayerTurn()].getPotInvestment() + currentRaise); //GAY
			
			std::shared_ptr<Node> callNode(new Node(game.getState() + 1,
					game.getPot() + currentRaise,
					game.getSmallBlind(),
					game.getBigBlind(),
					game.getBoardCards(),
					tempPlayerList));
			
			ChoiceNode* choiceCheck = new ChoiceNode(this, checkGame);
			return choiceCheck;
		}

		virtual std::shared_ptr<Node> raise(double raiseAmount) {
			std::shared_ptr<Node> raiseNode(new Node(int state,
					double (pot + raiseAmount),
					std::vector<int> boardCards,
					std::vector<Player> playerList,
					int ++playerTurn) );
		}
};

#endif	//Node.h
