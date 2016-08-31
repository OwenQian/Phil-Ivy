#include "Node.h"
#include "ChoiceNode.h"
#include "OpponentNode.h"
#include "../Config.h"

#include <memory>
#include <utility>
#include <cassert>
#include <iostream>

Node::Node() :
    parent(nullptr),
    foldChild(nullptr),
    callChild(nullptr),
    raiseChild(nullptr),
    game(0, 0.0, std::vector<int>(), Player(), Player(), 0),
    visitCount(0),
    expectedValue(0.0),
    currentRaise(0.0),
    isFolded(false),
    isAllIn(false),
    firstAction(true) { }

Node::Node(int              state,
		double              pot,
		std::vector<int>    boardCards,
		Player				botPlayer,
		Player				oppPlayer,
		int                 playerTurn,
		Node* parent) :
	parent(parent),
    foldChild(nullptr),
    callChild(nullptr),
    raiseChild(nullptr),
	game(state,
			pot,
			boardCards,
			botPlayer,
			oppPlayer,
			playerTurn),
    visitCount(0),
    expectedValue(0.0),
    currentRaise(0.0),
    isFolded(false),
    isAllIn(false) { }

// Copy Constructor
Node::Node(const Node& obj) :
    Node(obj.game.getState(),
            obj.game.getPot(),
            obj.game.getBoardCards(),
            obj.game.getBotPlayer(),
            obj.game.getOppPlayer(),
            obj.game.getPlayerTurn(),
            obj.parent) { }

// Destructor
Node::~Node() {
    foldChild.reset();
    raiseChild.reset();
    callChild.reset();
}

// Assignment operaor
Node& Node::operator= (const Node& rhs) {
    // self-assignment check
    if (&rhs == this)
        return *this;
    
    parent = rhs.parent;

    game = rhs.game;
    visitCount = rhs.visitCount;
    expectedValue = rhs.expectedValue;
    currentRaise = rhs.currentRaise;
    isFolded = rhs.isFolded;
    isAllIn = rhs.isAllIn;
    firstAction = rhs.firstAction;

    foldChild.reset();
    callChild.reset();
    raiseChild.reset();

    return *this;
}

