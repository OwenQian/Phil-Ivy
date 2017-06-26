#include <memory>
#include <iostream>
#include <cassert>

#include "ChoiceNode.h"
#include "OpponentNode.h"
#include "../Config.h"
#include "../Action.h"
#include "../GameUtilities/Decision.h"
#include "../GameUtilities/GameUtilities.h"

OpponentNode::OpponentNode() :
    Node::Node() { }

OpponentNode::OpponentNode(
        Stage state,
        double pot,
        std::vector<int> boardCards,
        Player botPlayer,
        Player oppPlayer,
        int playerTurn,
        Node* const parent) :
    Node::Node(state,
            pot,
            boardCards,
            botPlayer,
            oppPlayer,
            playerTurn,
            parent) { }

OpponentNode::OpponentNode(const OpponentNode& obj) :
    Node(obj) { }

OpponentNode::OpponentNode(const Node& obj) :
    Node(obj) { }

Decision OpponentNode::makeDecision(std::vector<int> deck) {
    std::cout << "Enter Action opp: Call(0), Raise(1), Fold(2)" << std::endl;
    Decision decision;
    int temp;
    std::cin >> temp;
    decision.action = static_cast<Action>(temp);
    if (decision.action == Action::RAISE) {
        if (game.getBotPlayer().getChips() + game.getBotPlayer().getPotInvestment() <= currentRaise ||
                game.getOppPlayer().getChips() + game.getOppPlayer().getPotInvestment() <= currentRaise) {
            decision.raiseAmount = 1;
        } else {
            std::cout << "Enter Raise amount" << std::endl;
            double amount;
            std::cin >> amount;
            decision.raiseAmount = amount;
        }
    }
    return decision;
}
