#include <memory>
#include <typeinfo>
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
    botExpectedValue(0.0),
    oppExpectedValue(0.0),
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
    botExpectedValue(0.0),
    oppExpectedValue(0.0),
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
            obj.parent)
    {
        visitCount = obj.visitCount;
        botExpectedValue = obj.botExpectedValue;
        oppExpectedValue = obj.oppExpectedValue;
        currentRaise = obj.currentRaise;
        isFolded = obj.isFolded;
        isAllIn = obj.isAllIn;
        firstAction = obj.firstAction;
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
    botExpectedValue = rhs.botExpectedValue;
    oppExpectedValue = rhs.oppExpectedValue;
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

	if (smallBlindPosition == 0) {
		root.reset( new ChoiceNode(currentStage, bigBlind + smallBlind, std::vector<int>(),
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
		root.reset( new OpponentNode(currentStage, bigBlind + smallBlind, std::vector<int>(),
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
    std::unique_ptr<Node> copyNode;
    if (getGame().getPlayerTurn() == 0) {
        copyNode.reset(new ChoiceNode(*this));
    } else {
        copyNode.reset(new OpponentNode(*this));
    }

    time_t startTime;
    time(&startTime);
    while (time(0) - startTime < maxSeconds) {
        copyNode->runSelection(deck);
    }

    std::cout << "visitCount: " << copyNode->visitCount;

    std::cout << "\n\n@@callVisit: " << copyNode->callChild->visitCount;
    std::cout << "\n bot callScore: " << copyNode->callChild->getBotExpectedValue();
    std::cout << "\n opp callScore: " << copyNode->callChild->getOppExpectedValue();

    std::cout << "\n\n@@raiseVisit: " << copyNode->raiseChild->visitCount;
    std::cout << "\n bot raiseScore: " << copyNode->raiseChild->getBotExpectedValue();
    std::cout << "\n opp raiseScore: " << copyNode->raiseChild->getOppExpectedValue();

    std::cout << "\n\n@@foldVisit: " << copyNode->foldChild->visitCount;
    std::cout << "\n bot foldScore: " << copyNode->foldChild->getBotExpectedValue();
    std::cout << "\n opp foldScore: " << copyNode->foldChild->getOppExpectedValue() << std::endl;

    double maxScore = copyNode->callChild->getBotExpectedValue();
    maxScore = maxScore >= copyNode->raiseChild->getBotExpectedValue() ? maxScore : copyNode->raiseChild->getBotExpectedValue();
    maxScore = maxScore >= copyNode->foldChild->getBotExpectedValue() ? maxScore : copyNode->foldChild->getBotExpectedValue();
    if (maxScore == copyNode->callChild->getBotExpectedValue()) {
        return Action::CALL;
    } else if (maxScore == copyNode->raiseChild->getBotExpectedValue()) {
        // Need to handle how much to raise here
        return Action::RAISE;
    } else {
        return Action::FOLD;
    }
}

void Node::runSelection(std::vector<int> deck) {
    if (isFolded || (game.getState() == static_cast<int>(Stage::SHOWDOWN)) || isAllIn) {
        //std::cout << "botPlayer chips : " << game.getBotPlayer().getChips() << std::endl;
        //std::cout << "oppPlayer chips : " << game.getOppPlayer().getChips() << std::endl;
        //std::cout << 
        backprop(game.getBotPlayer().getChips(), game.getOppPlayer().getChips());
        return;
    }

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
    naiveUCT(selectionScores, game.getPlayerTurn());
    double bestScore = 0;
    
    for (size_t i = 0; i < selectionScores.size(); ++i) {
        bestScore = bestScore > selectionScores[i] ? bestScore : selectionScores[i];
    }
   // std::cout << "selection1: " << selectionScores[0] << std::endl;
//	std::cout << "selection2: " << selectionScores[1] << std::endl;
//	std::cout << "selection3: " << selectionScores[2] << std::endl;
    if (bestScore == selectionScores[0]) {
		callChild->getGame().getBoardCards() = getGame().getBoardCards();
		conditionalDeal(*callChild, getGame().getState(), callChild->getGame().getState(), deck, getGame().getState());
        callChild->runSelection(deck);
    } else if (bestScore == selectionScores[1]) {
		raiseChild->getGame().getBoardCards() = getGame().getBoardCards();
        raiseChild->runSelection(deck);
    } else {
        foldChild->runSelection(deck);
    }
}

void Node::runSimulation(std::vector<int> deck) {
    if (getIsFolded()) {
        allocateChips(!game.getPlayerTurn(), *this);
        backprop(game.getBotPlayer().getChips(), getGame().getOppPlayer().getChips());
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
        getBotExpectedValue() = (getBotExpectedValue() * visitCount + botChips) / (visitCount + 1);
        getOppExpectedValue() = (getOppExpectedValue() * visitCount + oppChips) / (visitCount + 1);
        ++visitCount;

    if (parent != nullptr) {
        parent->backprop(botChips, oppChips);
    }
}

void Node::naiveUCT(std::vector<double>& selectionScores, int playerTurn) {
    assert(selectionScores.size() == 3);
    std::vector<double> explorationTerm(3, 0);

    // use bot or opp EV based on pTurn
    std::vector<double> ambiguousPlayerEV(3, 0);
    if (playerTurn == 0) {
        ambiguousPlayerEV[0] = callChild->getBotExpectedValue();
        ambiguousPlayerEV[1] = raiseChild->getBotExpectedValue();
        ambiguousPlayerEV[2] = foldChild->getBotExpectedValue();
    } else {
        ambiguousPlayerEV[0] = callChild->getOppExpectedValue();
        ambiguousPlayerEV[1] = raiseChild->getOppExpectedValue();
        ambiguousPlayerEV[2] = foldChild->getOppExpectedValue();
    }
    
    // Order here is important; call, raise, fold (CRF)
    // Set the selectionScore and explorationTerm for call
    selectionScores[0] = ambiguousPlayerEV[0];
    explorationTerm[0] = std::sqrt( std::log(double (visitCount)) 
            / double (callChild->visitCount));

    // Set the selectionScore and explorationTerm for raise
    selectionScores[1] = ambiguousPlayerEV[1];
    explorationTerm[1] = std::sqrt( std::log(double (visitCount))
            / double (raiseChild->visitCount));

    // Set the selectionScore and explorationTerm for fold
    selectionScores[2] = ambiguousPlayerEV[2];
    explorationTerm[2] = std::sqrt( std::log( double(visitCount) )
            / double (foldChild->visitCount) );

    for (size_t i = 0; i < selectionScores.size(); ++i) {
        //std::cout << "exploration terms: " << explorationTerm[i] << std::endl;
        explorationTerm[i] *= exploreConst;
        selectionScores[i] += explorationTerm[i]; 
    }
}
