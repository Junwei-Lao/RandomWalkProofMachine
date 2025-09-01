#include <iostream>
#include <random>
#include <vector>

//------------------------------------------------------------structure------------------------------------------------------------------------//
struct tuple{
    int x;
    int y;
    
    tuple(int xInput, int yInput) {
        this->x = xInput;
        this->y = yInput;
    }
};

struct node{
    int x;
    int y;
    node* nextNode;

    node(int xInput, int yInput) {
        this->x = xInput;
        this->y = yInput;
        this->nextNode = nullptr;
    }

    node(tuple t) {
        this->x = t.x;
        this->y = t.y;
        this->nextNode = nullptr;
    }

    void setNext(node* nextNode) {
        this->nextNode = nextNode;
    }

};


//--------------------------------------------------------------functions------------------------------------------------------------------------//
bool comparator(node* first, node* second) {
    return (first->x == second->x && first->y == second->y);
}

int hashCode (tuple first) {
    return first.x + first.y;
}

bool Find_And_Insert_Node(node** hashmap, tuple t) {
    int code = hashCode(t);
    if (hashmap[code] == nullptr) {
        hashmap[code] = new node(t);
        return true;
    } else {
        node* tempoNode = hashmap[code];
        node* newNode = new node(t);

        while (true) {
            if (comparator(tempoNode, newNode)) {
                return false;
            }

            if (tempoNode->nextNode != nullptr) {
                tempoNode = tempoNode->nextNode;
            } else {
                tempoNode->nextNode = newNode;
                return true;
            }
        }
    }
}


void printMap(node** hashmap, int size) {
    for (int i = 0; i < size; i++) {
        node* tempoNode = hashmap[i];
        while (tempoNode != nullptr) {
            std::cout << "(" << tempoNode->x << ", " << tempoNode->y << ")" << " -> ";
            tempoNode = tempoNode->nextNode;
        }
        std::cout << std::endl;
    }
}

//--------------------------------------------------------------constant variables-------------------------------------------------------------//
const double hypo_side = 124.712699;
const double length = 155.8908737;
const double width = 31.17817474;
const int totalPoints = 43961;
const int totalPoints_half = totalPoints / 2;

const tuple directions[4] {tuple(-1, 0), tuple(1, 0), tuple(0, -1), tuple(0, 1)};
const double Long = 2 * hypo_side + length;
const double Height = 2 * hypo_side + width;
const double gate = 0.25 * length + hypo_side;
const double gate2 = 0.5 * length + hypo_side;
const double gate3 = 0.75 * length + hypo_side;
const int hashSize = Long + Height + 2;   //to avoid weird segmentation fault



//--------------------------------------------------------------main function-----------------------------------------------------------------//
int main() {
    //------------------------------------------initialize-------------------------------------------//
    tuple machine(0, 0);
    int gateCrossedCount = 0;
    int gateCrossedCount2 = 0;
    int gateCrossedCount3 = 0;

    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<> randomX(0, Long);
    std::uniform_int_distribution<> randomY(0, Height);
    while (true) {
        int machineX = randomX(gen);
        int machineY = randomY(gen);

        if (machineX <= hypo_side){
            if (machineY >= machineX and machineY <= Height - machineX){
                machine = tuple(machineX, machineY);
                break;
            }
        } else if (machineX > hypo_side and machineX <= hypo_side + length){
            if (machineY >= hypo_side and machineY <= hypo_side + width){
                machine = tuple(machineX, machineY);
                break;
            }
        } else if (machineX > hypo_side + length and machineX <= Long){
            if (machineY >= Long-machineX and machineY <= machineX + Height - Long){
                machine = tuple(machineX, machineY);
                break;
            }
        }
    }

    int walk_count = 0;
    int unique_points = 1;
    node* path[hashSize] = {};
    Find_And_Insert_Node(path, machine);
    //--------------------------------------------------------------------------------------------------------//

    //------------------------------------------random walk---------------------------------------------------//
    bool nextLeft, nextRight, originLeft, originRight;
    bool keep = false;

    bool nextLeft2, nextRight2, originLeft2, originRight2;
    bool keep2 = false;

    bool nextLeft3, nextRight3, originLeft3, originRight3;
    bool keep3 = false;
    

    while (unique_points < totalPoints_half) {
        std::vector<tuple> DirectionChoice;

        if (!keep) {
            if (machine.x < gate) {
                originLeft = true;
                originRight = false;
            } else if (machine.x > gate) {
                originLeft = false;
                originRight = true;
            } else {
                originLeft = false;
                originRight = false;
            }
        }

        if (!keep2) {
            if (machine.x < gate2) {
                originLeft2 = true;
                originRight2 = false;
            } else if (machine.x > gate2) {
                originLeft2 = false;
                originRight2 = true;
            } else {
                originLeft2 = false;
                originRight2 = false;
            }
        }

        if (!keep3) {
            if (machine.x < gate3) {
                originLeft3 = true;
                originRight3 = false;
            } else if (machine.x > gate3) {
                originLeft3 = false;
                originRight3 = true;
            } else {
                originLeft3 = false;
                originRight3 = false;
            }
        }

        for (int i = 0; i < 4; i++) {
            int newX = machine.x + directions[i].x;
            int newY = machine.y + directions[i].y;

            if (newX >= 0 and newX <= hypo_side){
                if (newY >= newX and newY <= (Height - newX)){
                    DirectionChoice.push_back(tuple(newX, newY));
                }
            }
            else if (newX > hypo_side and newX <= (hypo_side + length)){
                if (newY >= hypo_side and newY <= (hypo_side + width)){
                    DirectionChoice.push_back(tuple(newX, newY));
                }
            }
            else if (newX > (hypo_side + length) && newX <= Long){
                if (newY >= (Long - newX) && newY <= (newX + Height - Long)){
                    DirectionChoice.push_back(tuple(newX, newY));
                }
            }
        }


        if (DirectionChoice.size() <= 0) {
            std::cerr << "Error: No valid direction choices available." << std::endl;
            break;
        } else {
            std::uniform_int_distribution<> randomNext(0, DirectionChoice.size()-1);
            machine = DirectionChoice.at(randomNext(gen));
        }

        //----------------------------------------------------------gate 1-------------------------------------------------//
        if (machine.x < gate) {
            nextLeft = true;
            nextRight = false;
            keep = false;
        } else if (machine.x > gate) {
            nextLeft = false;
            nextRight = true;
            keep = false;
        } else {
            nextLeft = false;
            nextRight = false;
            keep = true;
        }

        if ((nextLeft && originRight) || (nextRight && originLeft)) {
            gateCrossedCount += 1;
            nextLeft = false;
            nextRight = false;
            originLeft = false;
            originRight = false;
            keep = false;
        } 
        //-----------------------------------------------------------------------------------------------------------------//

        //----------------------------------------------------------gate 2-------------------------------------------------//
        if (machine.x < gate2) {
            nextLeft2 = true;
            nextRight2 = false;
            keep2 = false;
        } else if (machine.x > gate2) {
            nextLeft2 = false;
            nextRight2 = true;
            keep2 = false;
        } else {
            nextLeft2 = false;
            nextRight2 = false;
            keep2 = true;
        }
        if ((nextLeft2 && originRight2) || (nextRight2 && originLeft2)) {
            gateCrossedCount2 += 1;
            nextLeft2 = false;
            nextRight2 = false;
            originLeft2 = false;
            originRight2 = false;
            keep2 = false;
        }

        //-----------------------------------------------------------------------------------------------------------------//

        //----------------------------------------------------------gate 3-------------------------------------------------//
        if (machine.x < gate3) {
            nextLeft3 = true;
            nextRight3 = false;
            keep3 = false;
        } else if (machine.x > gate3) {
            nextLeft3 = false;
            nextRight3 = true;
            keep3 = false;
        } else {
            nextLeft3 = false;
            nextRight3 = false;
            keep3 = true;
        }
        if ((nextLeft3 && originRight3) || (nextRight3 && originLeft3)) {
            gateCrossedCount3 += 1;
            nextLeft3 = false;
            nextRight3 = false;
            originLeft3 = false;
            originRight3 = false;
            keep3 = false;
        }
        //-----------------------------------------------------------------------------------------------------------------//

        if (Find_And_Insert_Node(path, machine)) {
            unique_points += 1;
        }
        walk_count += 1;
    }
    //--------------------------------------------------------------------------------------------------------



    //-----------------------------------------------------------count the partial points-------------------------------------------------//
    int leftTrapozoidPoints = 0;
    int rightTrapozoidPoints = 0;
    int middleRectanglePoints = 0;
    int GateLeftPoints = 0;
    int GateRightPoints = 0;
    for (int i = 0; i < hashSize; i++) {
        node* tempoNode = path[i];
        while (tempoNode != nullptr) {
            if (tempoNode->x <= hypo_side){
                leftTrapozoidPoints += 1;
                GateLeftPoints += 1;
            } else if (tempoNode->x > hypo_side and tempoNode->x <= hypo_side + length){
                middleRectanglePoints += 1;
                if (tempoNode->x <= gate){
                    GateLeftPoints += 1;
                } else {
                    GateRightPoints += 1;
                }
            } else if (tempoNode->x > hypo_side + length and tempoNode->x <= Long){
                rightTrapozoidPoints += 1;
                GateRightPoints += 1;
            } else {
                std::cout << "Error: out of bounds" << std::endl;
            }
            tempoNode = tempoNode->nextNode;
        }
    }
    //--------------------------------------------------------------------------------------------------------



    
    std::cout << "Total walk time: " << walk_count << std::endl;
    //std::cout << "Unique points: " << unique_points << std::endl;
    std::cout << "Left Trapezoid points: " << leftTrapozoidPoints << std::endl;
    std::cout << "Middle Rectangle points: " << middleRectanglePoints << std::endl;
    std::cout << "Right Trapezoid points: " << rightTrapozoidPoints << std::endl;
    //std::cout << "Gate Left points: " << GateLeftPoints << std::endl;
    //std::cout << "Gate Right points: " << GateRightPoints << std::endl;
    std::cout << "Gate crossed count: " << gateCrossedCount << std::endl;
    std::cout << "Gate 2 crossed count: " << gateCrossedCount2 << std::endl;
    std::cout << "Gate 3 crossed count: " << gateCrossedCount3 << std::endl;





    //To clear the memory
    for (int i = 0; i < hashSize; i++) {
        node* tempoNode = path[i];
        while (tempoNode != nullptr) {
            node* temp = tempoNode;
            tempoNode = tempoNode->nextNode;
            delete temp;
        }
    }
    return 0;
}