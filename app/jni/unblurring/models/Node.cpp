#include <list>

class Node {
    static const int CHILD_SIZE = 4;

    private:
        Node* rootNode;
        Node* rightTopNode;
        Node* rightBottomNode;
        Node* leftTopNode;
        Node* leftBottomNode;
        uint32_t leftX, rightX, topY, bottomY;
        std::list<Node*> childNodeList;

    public:
        Node() {
            rootNode = NULL;
            rightTopNode = NULL;
            rightBottomNode = NULL;
            leftTopNode = NULL;
            leftBottomNode = NULL;
        }

        void setRightTopNode(Node* childNode);
        void setRightBottomNode(Node* childNode);
        void setLeftTopNode(Node* childNode);
        void setLeftBottomNode(Node* childNode);
        Node* getRightTopNode();
        Node* getRightBottomNode();
        Node* getLeftTopNode();
        Node* getLeftBottomNode();
        void setRootNode(Node* root);
        void setLocation(uint32_t left, uint32_t right, uint32_t top, uint32_t bottom);
        static Node* createRootNode(uint32_t left, uint32_t right, uint32_t top, uint32_t bottom);
        int getWidth();
        int getHeight();
        int getLeft();
        int getRight();
        int getTop();
        int getBottom();
        bool isRenderAvailable();
        int getChildCount();
        std::list<Node*> getChildNodeList();
        bool hasChildren();
};

void Node::setRightTopNode(Node* childNode) {
    rightTopNode = childNode;
}

void Node::setRightBottomNode(Node* childNode) {
    rightBottomNode = childNode;
}

void Node::setLeftTopNode(Node* childNode) {
    leftTopNode = childNode;
}

void Node::setLeftBottomNode(Node* childNode) {
    leftBottomNode = childNode;
}

Node* Node::getRightTopNode() {
   return rightTopNode;
}

Node* Node::getRightBottomNode() {
    return rightBottomNode;
}

Node* Node::getLeftTopNode() {
    return leftTopNode;
}

Node* Node::getLeftBottomNode() {
    return leftBottomNode;
}

int Node::getWidth() {
    return rightX - leftX;
}

int Node::getHeight() {
    return bottomY - topY;
}

int Node::getChildCount() {
    return CHILD_SIZE;
}

int Node::getLeft() {
    return leftX;
}

int Node::getRight() {
    return rightX;
}

int Node::getTop() {
    return topY;
}

int Node::getBottom() {
    return bottomY;
}

bool Node::isRenderAvailable() {
    return getWidth() > 1 && getHeight() > 1;
}

std::list<Node*> Node::getChildNodeList() {
    if (childNodeList.empty()) {
        childNodeList.push_front(rightBottomNode);
        childNodeList.push_front(rightTopNode);
        childNodeList.push_front(leftBottomNode);
        childNodeList.push_front(leftTopNode);
    }
    return childNodeList;
}

bool Node::hasChildren() {
    return getRightTopNode() != NULL
        && getRightBottomNode() != NULL
        && getLeftTopNode() != NULL
        && getLeftBottomNode() != NULL;
}

void Node::setLocation(uint32_t left, uint32_t right, uint32_t top, uint32_t bottom) {
    leftX = left;
    rightX = right;
    topY = top;
    bottomY = bottom;
}

Node* Node::createRootNode(uint32_t left, uint32_t right, uint32_t top, uint32_t bottom) {
    Node* createNode = new Node();
    createNode->setLocation(left, right, top, bottom);

    return createNode;
}