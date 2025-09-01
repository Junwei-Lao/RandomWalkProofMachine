#include <iostream>
#include <random>
#include <vector>
#include <iomanip>

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
                delete newNode; 
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
const int totalPoints_half = totalPoints / 2; //for random walk
const int sample_size = 1500; //for random walk

const tuple directions[4] {tuple(-1, 0), tuple(1, 0), tuple(0, -1), tuple(0, 1)};
const double Long = 2 * hypo_side + length;
const double Height = 2 * hypo_side + width;
const double gate = 0.25 * length + hypo_side;
const double gate2 = 0.5 * length + hypo_side;
const double gate3 = 0.75 * length + hypo_side; //for random walk
const int hashSize = Long + Height + 2;   //to avoid weird segmentation fault

int valueList[sample_size] = {};

int gateValueListLeft[sample_size] = {};
int gateCrossFromLeft = 0;
int gateValueListRight[sample_size] = {};
int gateCrossFromRight = 0;
int overallGateList[sample_size] = {};

int gateValueListLeft2[sample_size] = {};
int gateValueListRight2[sample_size] = {};

int gateValueListLeft3[sample_size] = {};
int gateValueListRight3[sample_size] = {};


//--------------------------------------------------------------main function-----------------------------------------------------------------//
int main() {
    
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<> randomX(0, Long+1);
    std::uniform_int_distribution<> randomY(0, Height+1);
    
    for (int overall_count = 0; overall_count < sample_size; overall_count++) {
        //------------------------------------------initialize-------------------------------------------//
        tuple machine(0, 0);
        int gateCrossedCount = 0;
        int gateCrossedCount2 = 0;
        int gateCrossedCount3 = 0;

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


        std::cout << "\rSample " << overall_count << ": Total walk time: " << walk_count << std::flush;

        /*
        std::cout << "Total walk time: " << walk_count << std::endl;
        //std::cout << "Unique points: " << unique_points << std::endl;
        std::cout << "Left Trapezoid points: " << leftTrapozoidPoints << std::endl;
        std::cout << "Middle Rectangle points: " << middleRectanglePoints << std::endl;
        std::cout << "Right Trapezoid points: " << rightTrapozoidPoints << std::endl;
        std::cout << "Gate Left points: " << GateLeftPoints << std::endl;
        std::cout << "Gate Right points: " << GateRightPoints << std::endl;
        std::cout << "Gate crossed count: " << gateCrossedCount << std::endl;
        */

        valueList[overall_count] = walk_count;

        if (leftTrapozoidPoints > rightTrapozoidPoints) {
            gateValueListLeft[gateCrossFromLeft] = gateCrossedCount;
            gateValueListLeft2[gateCrossFromLeft] = gateCrossedCount2;
            gateValueListLeft3[gateCrossFromLeft] = gateCrossedCount3;
            gateCrossFromLeft += 1;
        } else if (leftTrapozoidPoints < rightTrapozoidPoints) {
            gateValueListRight[gateCrossFromRight] = gateCrossedCount;
            gateValueListRight2[gateCrossFromRight] = gateCrossedCount2;
            gateValueListRight3[gateCrossFromRight] = gateCrossedCount3;
            gateCrossFromRight += 1;
        }

        //overallGateList[overall_count] = gateCrossedCount;

        //To clear the memory
        for (int i = 0; i < hashSize; i++) {
            node* tempoNode = path[i];
            while (tempoNode != nullptr) {
                node* temp = tempoNode;
                tempoNode = tempoNode->nextNode;
                delete temp;
            }
        }
    }


    // ----------------------------------------------------------Overall output-------------------------------------------------//
    double sumMean = 0;
    //double sumGateMean = 0;
    double sumGateLeftMean = 0;
    double sumGateLeftMean2 = 0;
    double sumGateLeftMean3 = 0;

    double sumGateRightMean = 0;
    double sumGateRightMean2 = 0;
    double sumGateRightMean3 = 0;

    for (int i = 0; i < sample_size; i++) {
        sumMean += valueList[i];
        //sumGateMean += overallGateList[i];
    }
    for (int i = 0; i < gateCrossFromLeft; i++) {
        sumGateLeftMean += gateValueListLeft[i];
        sumGateLeftMean2 += gateValueListLeft2[i];
        sumGateLeftMean3 += gateValueListLeft3[i];
    }
    for (int i = 0; i < gateCrossFromRight; i++) {
        sumGateRightMean += gateValueListRight[i];
        sumGateRightMean2 += gateValueListRight2[i];
        sumGateRightMean3 += gateValueListRight3[i];
    }
    
    
    std::cout << "\rAverage walk time: " << std::fixed << std::setprecision(4) << sumMean / sample_size << std::flush;
/*
    std::cout << std::endl;
    //std::cout << "Average gate crossed count: " << sumGateMean / sample_size << std::endl;

    std::cout << "Average gate crossed count while left trapozoid dominate: " << sumGateLeftMean / gateCrossFromLeft << std::endl;
    std::cout << "Average gate crossed count while left trapozoid dominate (gate 2): " << sumGateLeftMean2 / gateCrossFromLeft << std::endl;
    std::cout << "Average gate crossed count while left trapozoid dominate (gate 3): " << sumGateLeftMean3 / gateCrossFromLeft << std::endl;

    std::cout << std::endl;

    std::cout << "Average gate crossed count while right trapozoid dominate: " << sumGateRightMean / gateCrossFromRight << std::endl;
    std::cout << "Average gate crossed count while right trapozoid dominate (gate 2): " << sumGateRightMean2 / gateCrossFromRight << std::endl;
    std::cout << "Average gate crossed count while right trapozoid dominate (gate 3): " << sumGateRightMean3 / gateCrossFromRight << std::endl;
    */

    // Average walk time, gate1 count (Left), gate2 count (Left), gate3 count (Left), gate1 count (Right), gate2 count (Right), gate3 count (Right)
    std::cout << ""<< std::endl;


    
    return 0;
}