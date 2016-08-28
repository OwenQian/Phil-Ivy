#include "Node.h"
#include "../Config.h"

#include <memory>
#include <cassert>
#include <algorithm>	//std::min/max
#include <iostream>
#include <cmath>        //for sqrt

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
    Node()
{
    // share the same parent
    parent = obj.parent;

    foldChild.reset(new Node);
    foldChild->expectedValue = obj.foldChild->expectedValue;
    foldChild->visitCount = obj.foldChild->visitCount;

    raiseChild.reset(new Node);
    raiseChild->expectedValue = obj.raiseChild->expectedValue;
    raiseChild->visitCount = obj.raiseChild->visitCount;

    callChild.reset(new Node);
    callChild->expectedValue = obj.callChild->expectedValue;
    callChild->visitCount = obj.callChild->visitCount;
}

// Destructor
Node::~Node() {
    foldChild.reset();
    raiseChild.reset();
    callChild.reset();
}

// Assignment operaor
Node& Node::operator=(const Node& rhs) {
    // self-assignment check
    if (&rhs == this)
        return *this;
    delete this;
    parent = rhs.parent;
    foldChild.reset(new Node);
    foldChild->expectedValue = rhs.foldChild->expectedValue;
    foldChild->visitCount = rhs.foldChild->visitCount;

    raiseChild.reset(new Node);
    raiseChild->expectedValue = rhs.raiseChild->expectedValue;
    raiseChild->visitCount = rhs.raiseChild->visitCount;

    callChild.reset(new Node);
    callChild->expectedValue = rhs.callChild->expectedValue;
    callChild->visitCount = rhs.callChild->visitCount;
    
    game = rhs.game;
    visitCount = rhs.visitCount;
    expectedValue = rhs.expectedValue;
    currentRaise = rhs.currentRaise;
    isFolded = rhs.isFolded;
    isAllIn = rhs.isAllIn;
    firstAction = rhs.firstAction;

    return *this;
}

std::unique_ptr<Node>& Node::fold() {
    foldChild.reset(new Node(*this));
    foldChild->isFolded=true;
    foldChild->visitCount = 0;
    return foldChild;
}

std::unique_ptr<Node>& Node::call() {
    
}

std::shared_ptr<Node> Node::doCall() {
	//creates a temporary playerlist and updates the player's potinvestment and chip count
	if (game.getPlayerTurn() == 0) {
		Player tempPlayer = game.getBotPlayer();
		
		tempPlayer.setChips(tempPlayer.getChips() - (currentRaise - tempPlayer.getPotInvestment()) );
		tempPlayer.setPotInvestment(currentRaise);
		bool tempAllIn = false;
		if (game.getBotPlayer().getChips() + game.getBotPlayer().getPotInvestment() <= currentRaise ||
				game.getOppPlayer().getChips() + game.getOppPlayer().getPotInvestment() <= currentRaise) {
			tempAllIn = true;
		}
		auto callNode = std::make_shared<Node>(game.getState() + !getIsFirst(), //only advances state if not first action taken that stage
			initialChips * 2 - tempPlayer.getChips() - game.getOppPlayer().getChips(),
			game.getBoardCards(),
			tempPlayer,
			game.getOppPlayer(),
			!(game.getPlayerTurn()),
			shared_from_this() );
		callNode->setIsAllIn(tempAllIn);
        // if first Action, preserve currentRaise and potInv, else reset to 0
        callNode->setCurrentRaise(getIsFirst() * currentRaise);
        callNode->getGame().getBotPlayer().setPotInvestment(getIsFirst() * 
                callNode->getGame().getBotPlayer().getPotInvestment());
        callNode->getGame().getOppPlayer().setPotInvestment(getIsFirst() * 
                callNode->getGame().getOppPlayer().getPotInvestment());
		callChild = callNode;
		callChild = callNode;
	} else {
		Player tempPlayer = game.getOppPlayer();
		tempPlayer.setChips(tempPlayer.getChips() - (currentRaise - tempPlayer.getPotInvestment()));
		tempPlayer.setPotInvestment(currentRaise);
		bool tempAllIn = false;
		if (game.getBotPlayer().getChips() + game.getBotPlayer().getPotInvestment() <= currentRaise ||
				game.getOppPlayer().getChips() + game.getOppPlayer().getPotInvestment() <= currentRaise) {
			tempAllIn = true;
		}
		auto callNode = std::make_shared<Node>( game.getState() + !getIsFirst(),
			initialChips * 2 - tempPlayer.getChips() - game.getBotPlayer().getChips(),
			game.getBoardCards(),
			game.getBotPlayer(),
			tempPlayer,
			!(game.getPlayerTurn()),
			shared_from_this() );	
		callNode->setIsAllIn(tempAllIn);
        // if first Action, preserve currentRaise and potInv, else reset to 0
        callNode->setCurrentRaise(getIsFirst() * currentRaise);
        callNode->getGame().getBotPlayer().setPotInvestment(getIsFirst() * 
                callNode->getGame().getBotPlayer().getPotInvestment());
        callNode->getGame().getOppPlayer().setPotInvestment(getIsFirst() * 
                callNode->getGame().getOppPlayer().getPotInvestment());
		callChild = callNode;
	}
	callChild->setIsFirst(false);
	return callChild;
}

// raiseAmount means amount raising to, NOT raising by
std::shared_ptr<Node> Node::doRaise(double raiseAmount) {
    if (raiseAmount < bigBlind || raiseAmount < 2*currentRaise) {
        raiseAmount = bigBlind > (2*currentRaise) ? bigBlind : (2*currentRaise);
    } 
	// if raise all-in (or more) create AllInNode, handled by call
	if (game.getBotPlayer().getChips() <= currentRaise ||
				game.getOppPlayer().getChips() <= currentRaise) {
			return doCall();
		}
	if (raiseAmount >= game.getBotPlayer().getChips() + game.getBotPlayer().getPotInvestment() ||
			raiseAmount >= game.getOppPlayer().getChips() + game.getOppPlayer().getPotInvestment() ) {
		std::cout << "Raising All-In" << std::endl;

		// set raiseAmount to lesser of chip amounts
		raiseAmount = std::min(game.getBotPlayer().getChips() + game.getBotPlayer().getPotInvestment(),
				game.getOppPlayer().getChips() + game.getOppPlayer().getPotInvestment());
	}
	setIsFirst(false);
	if (game.getPlayerTurn() == 0) {
		Player tempPlayer = game.getBotPlayer();
		tempPlayer.setChips(tempPlayer.getChips() - (raiseAmount - tempPlayer.getPotInvestment()) );
		tempPlayer.setPotInvestment(raiseAmount);
		auto raiseNode = std::make_shared<Node>( game.getState(),
			initialChips * 2 - tempPlayer.getChips() - game.getOppPlayer().getChips(),
			game.getBoardCards(),
			tempPlayer,
			game.getOppPlayer(),
			!(game.getPlayerTurn()),
			shared_from_this() );
		raiseNode->setCurrentRaise(raiseAmount);
		raiseChild = raiseNode;
	} else {
		Player tempPlayer = game.getOppPlayer();
		tempPlayer.setChips(tempPlayer.getChips() - (raiseAmount - tempPlayer.getPotInvestment()));
		tempPlayer.setPotInvestment(raiseAmount);
		auto raiseNode = std::make_shared<Node>( game.getState(),
			initialChips * 2 - tempPlayer.getChips() - game.getBotPlayer().getChips(),
			game.getBoardCards(),
			game.getBotPlayer(),
			tempPlayer,
			!(game.getPlayerTurn()),
			shared_from_this());
		raiseNode->setCurrentRaise(raiseAmount);
		raiseChild = raiseNode;
	}
	
	return raiseChild;
}

void Node::naiveUCT(std::vector<double>& selectionScores, double exploreConst) {
    assert(selectionScores.size() == 3);
    std::vector<double> explorationTerm;
    explorationTerm.resize(3);
    int childVisitSum = 0;
    //std::cout << "foldCount: " << this->getFoldChild()->getVisitCount();
    //std::cout << "\ncallCount: " << this->getCallChild()->getVisitCount();
    //std::cout << "\nraiseCount: " << this->getRaiseChild()->getVisitCount() << std::endl;
    childVisitSum += this->getCallChild()->getVisitCount();
    childVisitSum += this->getFoldChild()->getVisitCount();
    childVisitSum += this->getRaiseChild()->getVisitCount();

    // Order here is important; call, raise, fold (CRF)
    // Set the selectionScore and explorationTerm for call
    selectionScores[0] = this->getCallChild()->getExpectedValue();
    explorationTerm[0] = std::sqrt(
            std::log(double (childVisitSum)) / double (this->getCallChild()->getVisitCount()) );

    // Set the selectionScore and explorationTerm for raise
    selectionScores[1] = this->getRaiseChild()->getExpectedValue();
    explorationTerm[1] = std::sqrt(
            std::log(double (childVisitSum)) / double (this->getRaiseChild()->getVisitCount()) );

    // Set the selectionScore and explorationTerm for fold
    selectionScores[2] = this->getFoldChild()->getExpectedValue();
    explorationTerm[2] = std::sqrt(
            std::log(double (childVisitSum)) / double (this->getFoldChild()->getVisitCount()) );

    for (size_t i = 0; i < selectionScores.size(); ++i) {
        explorationTerm[i] *= exploreConst;
        selectionScores[i] += explorationTerm[i]; 
    }
}
