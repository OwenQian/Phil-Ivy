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
		
		int getCurrentRaise() const {
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

		Node(	int								state,
				double							pot,
				std::vector<int>				boardCards,
				std::vector<Player> 			playerList,
				int								playerTurn) 
		: GameObject(	int					state,
						double				pot,
						std::vector<int>	boardCards,
						std::vector<Player> playerList,
						int					playerTurn) { }

		// Action function implementation
		virtual std::shared_ptr<Node> fold() {
			std::shared_ptr<Node> foldNode(new Node());   //create child foldNode
			foldNode->isTerminal = true;
			this->childList[0] = foldNode;                 //childList[0] holds the foldNodes
			return foldNode;
		}
		
		virtual std::shared_ptr<Node> call(double callAmount /* = 0 */) {
			//creates a temporary playerlist and updates the player's potinvestment and chip count
			std::vector<Player>tempPlayerList;
			tempPlayerList.reserve(getPlayerList().size());
			//copying the elements of current playerlist into the temp
			std::copy(getPlayerList().begin(), getPlayerList().end(), back_inserter(tempPlayerList));
			// Creating tempPlayer and updating its values
			Player tempPlayer = tempPlayerList[getPlayerTurn()];
			//match raise by subtracting chip amount by raise amount
			tempPlayer.setChips(tempPlayer.getChips() - getCurrentRaise());     
			//Increase player potinvestment
			tempPlayer.setPotInvestment(tempPlayer.getPotInvestment() + getCurrentRaise());    
			tempPlayerList[getPlayerTurn()] = tempPlayer;    //update tempPlayerList
			
			// increasing state by 1, increasing pot by raiseAmount,
			// blinds and board cards stay the same, playerList updated 
			auto callNode = std::make_shared<Node>(getState() + 1,
											getPot() + getCurrentRaise(),
											getSmallBlind(),
											getBigBlind(),
											getBoardCards(),
											tempPlayerList);
			this->childList[1] = foldNode;
			return callNode;
		}

		virtual std::shared_ptr<Node> raise(double raiseAmount /* = 0 */) {
			auto raiseNode = std::make_shared<Node>(int					state,
													double				(pot + raiseAmount),
													std::vector<int>	boardCards,
													std::vector<Player> playerList,
													int					++playerTurn);
		}
};

#endif	//Node.h
