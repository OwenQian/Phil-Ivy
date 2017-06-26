#ifndef OPPONENT_NODE_H
#define OPPONENT_NODE_H

#include <memory>

#include "Node.h"
#include "../GameObject.h"
#include "../Action.h"
#include "../GameUtilities/Decision.h"

class ChoiceNode;
class OpponentNode: public Node {
  private:
    virtual Decision makeDecision(std::vector<int> deck);
  public:
    // Contructors
    OpponentNode();
    OpponentNode(
        Stage             state,
        double            pot,
        std::vector<int>  boardCards,
        Player            botPlayer,
        Player            oppPlayer,
        int               playerTurn,
        Node*             const parent);
    OpponentNode(const OpponentNode&);
    OpponentNode(const Node&);
    OpponentNode(const NodeParamObject params): Node(params) { };
};

#endif  //OpponentNode.h
