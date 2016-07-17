#ifndef NODE_H
#define NODE_H

#include "GameObject.h"
#include "Player.h"

#include <vector>

class Node: GameObject {
	protected:
		static const int EV_Const = 1;
		std::vector<Node*> childList;
		Node* const parent;
		GameObject game;
		int visitCount;
		double expectedValue;
		bool isTerminal = false;	//whether or note Node is terminal
		double callAmount = 0;

	public:
		//member-accessibility functions
		std::vector<Node> getChildList() const;
		Node* getParent() const;
		GameObject getGame() const;
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

		//functions to be implemented differently for ChoiceNode and \
		//OpponentNode
		virtual Node fold() = 0; 
		virtual Node raise(double raiseAmount) = 0;
		virtual Node call(double callAmount = 0.0) = 0;


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
};

#endif	//Node.h
