#include <memory>
#include <utility>
#include <cassert>
#include <iostream>
#include <chrono>   // Monte carlo shit
#include <cmath>    // naiveUCT log and sqrt

#include "Node.h"
#include "ChoiceNode.h"
#include "OpponentNode.h"
#include "../Config.h"
#include "../Stage.h"
#include "../GameUtilities/GameUtilities.h"
#include "../handEval/helper.h"

Node::Node() :
    parent(nullptr),
    game(0, 0.0, std::vector<int>(), Player(), Player(), 0),
    visitCount(0),
    expectedValue(0.0),
    currentRaise(0.0),
    isFolded(false),
    isAllIn(false),
    firstAction(false),
    foldChild(nullptr),
    callChild(nullptr),
    raiseChild(nullptr) { }
	
Node::Node(int              state,
		double              pot,
		std::vector<int>    boardCards,
		Player				botPlayer,
		Player				oppPlayer,
		int                 playerTurn,
		Node*               parent) :
	parent(parent),
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
    firstAction(false),
    foldChild(nullptr),
    callChild(nullptr),
    raiseChild(nullptr) { }

// Copy Constructor
Node::Node(const Node& obj) :
    Node(obj.game.getState(),
            obj.game.getPot(),
            obj.game.getBoardCards(),
            obj.game.getBotPlayer(),
            obj.game.getOppPlayer(),
            obj.game.getPlayerTurn(),
            obj.parent) {
        isAllIn = obj.isAllIn;
    }

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
    Node* currentNode;
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
    currentNode = root.get();
	while(!currentNode->getIsAllIn() && !currentNode->getIsFolded()
		&& (currentNode->getGame().getState() != static_cast<int>(Stage::SHOWDOWN)) ){
			std::cout << "is this first?: " << currentNode->getIsFirst() << std::endl;
            switch(currentNode->playTurn(deck)) {
                case 0:
                    currentNode = currentNode->callChild.get();
                    break;
                case 1:
                    currentNode = currentNode->raiseChild.get();
                    break;
                case 2:
                    currentNode = currentNode->foldChild.get();
                    break;
            }
			if (currentNode->getGame().getState() != currentStage){
				std::vector<int> updateBoard = currentNode->getGame().getBoardCards();
				std::vector<int> newCards = deal(deck, currentStage);
				currentStage++;
				//adding current board cards to newly dealt cards
				assert(newCards.size() <= 3);
				for (auto i = newCards.begin(); i != newCards.end(); ++i){
					updateBoard.push_back(*i);
				}
				currentNode->setIsFirst(true);
			}
	}
	if (currentNode->getIsFolded()){
		if (currentNode->getGame().getPlayerTurn() == 0) {
			allocateChips(1, (*currentNode));
		} else {
			allocateChips(0, (*currentNode));
		}
	}
	if (currentNode->getGame().getState() == static_cast<int>(Stage::SHOWDOWN) ){
		int winner = showdown(currentNode->getGame().getBotPlayer().getHoleCards(),
				currentNode->getGame().getOppPlayer().getHoleCards(),
				currentNode->getGame().getBoardCards());
		allocateChips(winner, (*currentNode));
	}
	if (currentNode->getIsAllIn()){
		for (int i = currentNode->getGame().getState() - 1; i < static_cast<int>(Stage::SHOWDOWN); ++i) {
				std::vector<int> updateBoard = currentNode->getGame().getBoardCards();
				std::vector<int> newCards = deal(deck, i);
				//adding current board cards to newly dealt cards
				assert(newCards.size() <= 3);
				for (auto j = newCards.begin(); j != newCards.end(); ++j){
					updateBoard.push_back(*j);
				}
				assert(updateBoard.size() <=5);
				currentNode->getGame().setBoardCards(updateBoard);
				}
                printBoardCards(currentNode->getGame().getBoardCards());
				std::cout << "botCards: " << hexToCard(currentNode->getGame().getBotPlayer().getHoleCards()[0]) << " " << hexToCard(currentNode->getGame().getBotPlayer().getHoleCards()[1]);
				std::cout << "\noppCards: " << hexToCard(currentNode->getGame().getOppPlayer().getHoleCards()[0]) << " " << hexToCard(currentNode->getGame().getOppPlayer().getHoleCards()[1]);
				int winner = showdown(currentNode->getGame().getBotPlayer().getHoleCards(),
						currentNode->getGame().getOppPlayer().getHoleCards(),
						currentNode->getGame().getBoardCards());
				allocateChips(winner, (*currentNode));
				std::cout << "\nWinner: " << winner << std::endl;
	}
    botPlayer = currentNode->getGame().getBotPlayer();
    oppPlayer = currentNode->getGame().getOppPlayer();
}

int Node::playTurn(std::vector<int> deck) {
    assert(!isFolded);
    Decision decision = makeDecision(deck);
    switch(decision.action) {
        case Action::CALL: {
                               call();
                               return 0;
                               break;
                           }
        case Action::RAISE: {
                                raise(decision.raiseAmount);
                                return 1;
                                break;
                            }
        case Action::FOLD: {
                               fold();
                               return 2;
                               break;
                           }
        default:
                           std::cout << "Invalid decision" << std::endl;
    }
}

Action Node::monteCarlo(int maxSeconds, std::vector<int> deck) {
    time_t startTime;
    time(&startTime);
    std::unique_ptr<Node> copyNode;
    if (getGame().getPlayerTurn() == 0) {
        copyNode.reset(new ChoiceNode(*dynamic_cast<ChoiceNode*>(this)));
    } else {
        copyNode.reset(new OpponentNode(*dynamic_cast<OpponentNode*>(this)));
    }
    while (time(0) - startTime < maxSeconds) {
        copyNode->runSelection(deck);
    }
    std::cout << "callScore: " << copyNode->callChild->getExpectedValue();
    std::cout << "\nraiseScore: " << copyNode->raiseChild->getExpectedValue();
    std::cout << "\nfoldScore: " << copyNode->foldChild->getExpectedValue() << std::endl;
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

void Node::runSelection(std::vector<int> deck) {
    if (!callChild) {
        call();
		conditionalDeal(*callChild, getGame().getState(), callChild->getGame().getState(), deck, getGame().getState());
        callChild->runSimulation(deck);
        return;
    } else if (!raiseChild) {
        // TODO use different raise amt?
        raise(1);
        raiseChild->runSimulation(deck);
        return;
    } else if (!foldChild) {
        fold();
        foldChild->runSimulation(deck);
        return;
    }

    std::vector<double> selectionScores {0,0,0};
    naiveUCT(selectionScores);
    double maxScore = 0;
    for (size_t i = 0; i < selectionScores.size(); ++i) {
        maxScore = maxScore > selectionScores[i] ? maxScore : selectionScores[i];
    }
    
    if (maxScore == selectionScores[0]) {
		callChild->getGame().getBoardCards() = getGame().getBoardCards();
		conditionalDeal(*callChild, getGame().getState(), callChild->getGame().getState(), deck, getGame().getState());
        callChild->runSelection(deck);
    } else if (maxScore == selectionScores[1]) {
		raiseChild->getGame().getBoardCards() = getGame().getBoardCards();
        raiseChild->runSelection(deck);
    } else {
        foldChild->runSelection(deck);
    }
}

void Node::runSimulation(std::vector<int> deck) {
    if (getIsFolded()) {
        switch (getGame().getPlayerTurn()) {
            case 0 :
                allocateChips(1, *this);
                break;
            case 1 :
                allocateChips(0, *this);
                break;
        }
        backprop(getGame().getBotPlayer().getChips(), getGame().getOppPlayer().getChips());
        return;
    }

    Node* currentNode = this;
	int prevStage;
    while (currentNode->getGame().getState() != static_cast<int>(Stage::SHOWDOWN)) {
		prevStage = currentNode->getGame().getState();
        currentNode->call();
        currentNode = currentNode->callChild.get();
		conditionalDeal(*currentNode, prevStage, currentNode->getGame().getState(), deck, prevStage);
    }
    int winner = showdown( 
            currentNode->getGame().getBotPlayer().getHoleCards(),
            currentNode->getGame().getOppPlayer().getHoleCards(),
            currentNode->getGame().getBoardCards());
    allocateChips(winner, *currentNode);
    currentNode->backprop(currentNode->getGame().getBotPlayer().getChips(),
            currentNode->getGame().getOppPlayer().getChips());
}

void Node::backprop(double botChips, double oppChips) {
    if (parent != nullptr) {
        if (parent->getGame().getPlayerTurn() == 0) {
            parent->getExpectedValue() = (parent->getExpectedValue() * parent->getVisitCount()
                + botChips) / ++parent->getVisitCount();
        } else {
            parent->getExpectedValue() = (parent->getExpectedValue() * parent->getVisitCount()
                + oppChips) / ++parent->getVisitCount();
        }
        parent->backprop(botChips, oppChips);
    }
}

void Node::naiveUCT(std::vector<double>& selectionScores) {
    assert(selectionScores.size() == 3);
    std::vector<double> explorationTerm;
    explorationTerm.resize(3);
    int childVisitSum = 0;
    childVisitSum += this->callChild->getVisitCount();
    childVisitSum += this->raiseChild->getVisitCount();
    childVisitSum += this->foldChild->getVisitCount();

    // Order here is important; call, raise, fold (CRF)
    // Set the selectionScore and explorationTerm for call
    selectionScores[0] = this->callChild->getExpectedValue();
    explorationTerm[0] = std::sqrt( std::log(double(this->callChild->getVisitCount()) )
            / double (childVisitSum));
    // Set the selectionScore and explorationTerm for raise
    selectionScores[1] = this->raiseChild->getExpectedValue();
    explorationTerm[1] = std::sqrt( std::log(double(this->raiseChild->getVisitCount()) )
            / double (childVisitSum));

    // Set the selectionScore and explorationTerm for fold
    selectionScores[2] = this->foldChild->getExpectedValue();
    explorationTerm[2] = std::sqrt( std::log(double(this->foldChild->getVisitCount()) )
            / double (childVisitSum));

    for (size_t i = 0; i < selectionScores.size(); ++i) {
        explorationTerm[i] *= exploreConst;
        selectionScores[i] += explorationTerm[i]; 
    }
}
