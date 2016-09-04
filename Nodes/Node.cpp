#include <memory>
#include <utility>
#include <cassert>
#include <iostream>
#include <chrono>   // Monte carlo shit

#include "Node.h"
#include "ChoiceNode.h"
#include "OpponentNode.h"
#include "../Config.h"
#include "../Stage.h"
#include "../GameUtilities/GameUtilities.h"
#include "../handEval/helper.h"

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
    firstAction(false) { }
	
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
    isAllIn(false), 
    firstAction(false) { }

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

    return *this;
}

void Node::playGame(){
	Player botPlayer(0, 0, initialChips, 0); //0, 0 represents dummy cards
	Player oppPlayer(0, 0, initialChips, 0);

	while ((botPlayer.getChips() > 0)&& (oppPlayer.getChips() > 0)){ // while both players have chips
		playRound (botPlayer, oppPlayer);
		std::cout << "bot player chips: " << botPlayer.getChips() << std::endl;
		std::cout << "opp player chips: " << oppPlayer.getChips() << std::endl;
		smallBlindPosition = !smallBlindPosition;
        botPlayer.setPotInvestment(0);
        oppPlayer.setPotInvestment(0);
	}
}

void Node::playRound(Player& botPlayer, Player& oppPlayer){
	std::cout << "########################################";
	std::cout << "\nSmall Blind: " << smallBlind << "\nBig Blind : " << bigBlind;
	std::cout << "\nbot player chips: " << botPlayer.getChips();
	std::cout << "\nopp player chips: " << oppPlayer.getChips() << std::endl;

    // creating the deck
	std::vector<int> deck;
	init_deck(deck);

    // dealing player hole cards
	botPlayer.setHoleCards(deal(deck, static_cast<int>(Stage::HOLECARDS) ));
	oppPlayer.setHoleCards(deal(deck, static_cast<int>(Stage::HOLECARDS) ));
    
    std::cout << "Bot Cards: " << hexToCard(botPlayer.getHoleCards()[0]) << " " << hexToCard(botPlayer.getHoleCards()[1]);
    std::cout << "\nOpp Cards: " << hexToCard(oppPlayer.getHoleCards()[0]) << " " << hexToCard(oppPlayer.getHoleCards()[1]) << std::endl;

	int currentStage = 1;       // preflop 
	
	std::unique_ptr<Node> root;
	std::cout << "smallblindposition: " << smallBlindPosition << std::endl;
    // if smallBlindPosition == 0, node should be a ChoiceNode
	assert(smallBlindPosition == 0 || smallBlindPosition == 1);
	if (smallBlindPosition == 0){
		root.reset( new ChoiceNode(1, bigBlind + smallBlind, std::vector<int>(),
				botPlayer, oppPlayer, smallBlindPosition, nullptr));
		// if the smallBlind puts the bot allIn
		if (smallBlind >= botPlayer.getChips()){
			root->getGame().getOppPlayer().setChips(oppPlayer.getChips() - botPlayer.getChips());
			root->getGame().getBotPlayer().setChips(0);
			root->getGame().getBotPlayer().setPotInvestment(botPlayer.getChips());
			root->getGame().getOppPlayer().setPotInvestment(botPlayer.getChips());
        // if the bigBlind puts the opp alIn
		} else if (bigBlind >= oppPlayer.getChips()) {
			double lesserAmount = smallBlind < oppPlayer.getChips() ? smallBlind : oppPlayer.getChips();
			root->getGame().getBotPlayer().setChips(botPlayer.getChips() - lesserAmount);
			root->getGame().getOppPlayer().setChips(0);
			root->getGame().getBotPlayer().setPotInvestment(lesserAmount);
			root->getGame().getOppPlayer().setPotInvestment(oppPlayer.getChips());
			root->setCurrentRaise(lesserAmount);
		} else {
			// subtract blinds from chip count
			root->getGame().getBotPlayer().setChips(botPlayer.getChips() - smallBlind);
			root->getGame().getOppPlayer().setChips(oppPlayer.getChips() - bigBlind);
			// add blinds to potInvestment
			root->getGame().getBotPlayer().setPotInvestment(smallBlind);
			root->getGame().getOppPlayer().setPotInvestment(bigBlind);
			root->setCurrentRaise(bigBlind);
		}
	} else {
		root.reset( new OpponentNode(1, bigBlind + smallBlind, std::vector<int>(),
				botPlayer, oppPlayer, smallBlindPosition, nullptr) );
		// if the smallBlind puts the opp allIn
		if (smallBlind >= oppPlayer.getChips()){
			root->getGame().getBotPlayer().setChips(botPlayer.getChips() - oppPlayer.getChips());
			root->getGame().getOppPlayer().setChips(0);
			root->getGame().getBotPlayer().setPotInvestment(oppPlayer.getChips());
			root->getGame().getOppPlayer().setPotInvestment(oppPlayer.getChips());
        // if the bigBlind puts the bot alIn
		} else if (bigBlind >= botPlayer.getChips()) {
			double lesserAmount = smallBlind < botPlayer.getChips() ? smallBlind : botPlayer.getChips();
			root->getGame().getOppPlayer().setChips(oppPlayer.getChips() - lesserAmount);
			root->getGame().getBotPlayer().setChips(0);
			root->getGame().getOppPlayer().setPotInvestment(lesserAmount);
			root->getGame().getBotPlayer().setPotInvestment(botPlayer.getChips());
			root->setCurrentRaise(lesserAmount);
		} else {
			// subtract blinds from chip count
			root->getGame().getBotPlayer().setChips(botPlayer.getChips() - bigBlind);
			root->getGame().getOppPlayer().setChips(oppPlayer.getChips() - smallBlind);

			// add blinds to potInvestment
			root->getGame().getBotPlayer().setPotInvestment(bigBlind);
			root->getGame().getOppPlayer().setPotInvestment(smallBlind);
			root->setCurrentRaise(bigBlind);
		}
	} 
	root->setIsFirst(true);
	while(!root->getIsAllIn() && !root->getIsFolded()
		&& (root->getGame().getState() != static_cast<int>(Stage::SHOWDOWN)) ){
			std::cout << "is this first?: " << root->getIsFirst() << std::endl;
			root = std::move(root->playTurn());
			if (root->getGame().getState() != currentStage){
				std::vector<int> updateBoard = root->getGame().getBoardCards();
				std::vector<int> newCards = deal(deck, currentStage);
				currentStage++;
				//adding current board cards to newly dealt cards
				assert(newCards.size() <= 3);
				for (auto i = newCards.begin(); i != newCards.end(); ++i){
					updateBoard.push_back(*i);
				}
				root->setIsFirst(true);
			}
	}
	if (root->getIsFolded()){
		if (root->getGame().getPlayerTurn() == 0) {
			allocateChips(1, (*root));
		} else {
			allocateChips(0, (*root));
		}
	}
	if (root->getGame().getState() == static_cast<int>(Stage::SHOWDOWN) ){
		int winner = showdown(root->getGame().getBotPlayer().getHoleCards(),
				root->getGame().getOppPlayer().getHoleCards(),
				root->getGame().getBoardCards());
		allocateChips(winner, (*root));
	}
	if (root->getIsAllIn()){
		for (int i = root->getGame().getState() - 1; i < static_cast<int>(Stage::SHOWDOWN); ++i) {
				std::vector<int> updateBoard = root->getGame().getBoardCards();
				std::vector<int> newCards = deal(deck, i);
				//adding current board cards to newly dealt cards
				assert(newCards.size() <= 3);
				for (auto j = newCards.begin(); j != newCards.end(); ++j){
					updateBoard.push_back(*j);
				}
				assert(updateBoard.size() <=5);
				root->getGame().setBoardCards(updateBoard);
				}
                printBoardCards(root->getGame().getBoardCards());
				std::cout << "botCards: " << hexToCard(root->getGame().getBotPlayer().getHoleCards()[0]) << " " << hexToCard(root->getGame().getBotPlayer().getHoleCards()[1]);
				std::cout << "\noppCards: " << hexToCard(root->getGame().getOppPlayer().getHoleCards()[0]) << " " << hexToCard(root->getGame().getOppPlayer().getHoleCards()[1]);
				int winner = showdown(root->getGame().getBotPlayer().getHoleCards(),
						root->getGame().getOppPlayer().getHoleCards(),
						root->getGame().getBoardCards());
				allocateChips(winner, (*root));
				std::cout << "\nWinner: " << winner << std::endl;
	}
    botPlayer = root->getGame().getBotPlayer();
    oppPlayer = root->getGame().getOppPlayer();
}

std::unique_ptr<Node>& Node::playTurn() {
    assert(!isFolded);
    Decision decision = makeDecision();
    switch(decision.action) {
        case Action::CALL: {
                               return call();
                               break;
                           }
        case Action::RAISE: {
                                return raise(decision.raiseAmount);
                                break;
                            }
        case Action::FOLD: {
                               return fold();
                               break;
                           }
        default:
                           std::cout << "Invalid decision" << std::endl;
    }
}

Action Node::monteCarlo(int maxSeconds) {
    time_t startTime;
    time(&startTime);
    std::unique_ptr<Node> copyNode;
    if (getGame().getPlayerTurn() == 0) {
        copyNode.reset(new ChoiceNode(*dynamic_cast<ChoiceNode*>(this)));
    } else {
        copyNode.reset(new OpponentNode(*dynamic_cast<OpponentNode*>(this)));
    }
    while (time(0) - startTime < maxSeconds) {
        copyNode->runSelection();
    }
    double maxScore = copyNode->callChild->getExpectedValue();
    maxScore = maxScore >= copyNode->raiseChild->getExpectedValue() ? maxScore : copyNode->raiseChild->getExpectedValue();
    maxScore = maxScore >= copyNode->foldChild->getExpectedValue() ? maxScore : copyNode->foldChild->getExpectedValue();
    if (maxScore == copyNode->callChild->getExpectedValue()) {
        return Action::CALL;
    } else if (maxScore == copyNode->raiseChild->getExpectedValue()) {
        // Need to handle how much to raise here
        return Action::RAISE;
    } else {
        return Action::FOLD;
    }
}

void Node::runSelection() {
    if (!callChild) {
        (call())->runSimulation();
        return;
    } else if (!raiseChild) {
        (raise(1))->runSimulation();
        return;
    } else if (!foldChild) {
        (fold())->runSimulation();
    }
}

void Node::runSimulation() {
    std::unique_ptr<Node> copyNode;
    if (getGame().getPlayerTurn() == 0) {
        copyNode.reset(new ChoiceNode(*dynamic_cast<ChoiceNode*>(this)));
    } else {
        copyNode.reset(new OpponentNode(*dynamic_cast<OpponentNode*>(this)));
    }

    while (getGame().getState() != static_cast<int>(Stage::SHOWDOWN)) {
        call();
    }

}
