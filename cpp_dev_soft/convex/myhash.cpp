#include <iostream>
#include <cstdint>

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

};

bool comparator(node* first, node* second) {
    return (first->x == second->x && first->y == second->y);
}


int hashCode (tuple first, int HashSize) {  //new hash function
    uint64_t hx = (uint64_t)(uint32_t)first.x;
    uint64_t hy = (uint64_t)(uint32_t)first.y;
    return (hx << 32 | hy) % HashSize;
}

bool Find_And_Insert_Node(node** hashmap, tuple t, int HashSize) {
    int code = hashCode(t, HashSize);
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
