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

	public:
		//member-accessibility functions
		std::vector<Node> getChildList() const;
		Node* getParent() const;
		//GameObject getGame() const;
		int getVisitCount() const;
		double getExpectedValue() const;
		bool getTerminalStatus() const;
		
		

		//constructor
		Node();
		
		Node(int state,
				double pot,
				std::vector<int> boardCards,
				std::vector<Player> playerList,
				int playerTurn);

		// Action functions
		std::shared_ptr<Node> fold();
		std::shared_ptr<Node> raise(double raiseAmount);
		std::shared_ptr<Node> call();
		


		//constructor implementation
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
					
		std::shared_ptr<Node> fold() { //not sure where fold/raise/check should be located
			std::shared_ptr<Node> foldNode(new Node)
			foldNode->isTerminal = true;
			this->childList[0] = foldNode;
			return foldNode;
		}

		std::shared_ptr<Node> raise(double raiseAmount) {
			std::shared_ptr<Node> raiseNode(new Node(int state,
					double (pot + raiseAmount),
					std::vector<int> boardCards,
					std::vector<Player> playerList,
					int ++playerTurn));
		}
		
		std::shared_ptr<Node> call(){
			
		}
					
};

#endif	//Node.h
