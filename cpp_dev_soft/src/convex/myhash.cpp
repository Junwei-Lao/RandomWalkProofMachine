#include "myhash.h"
#include <cstdint>

mytuple::mytuple(int xInput, int yInput) {
    x = xInput;
    y = yInput;
}

node::node(int xInput, int yInput) {
    x = xInput;
    y = yInput;
    nextNode = nullptr;
}

node::node(mytuple t) {
    x = t.x;
    y = t.y;
    nextNode = nullptr;
}

bool comparator(node* first, node* second) {
    return (first->x == second->x && first->y == second->y);
}

int hashCode(mytuple first, int HashSize) {
    uint64_t hx = static_cast<uint64_t>(static_cast<uint32_t>(first.x));
    uint64_t hy = static_cast<uint64_t>(static_cast<uint32_t>(first.y));
    return (hx << 32 | hy) % HashSize;
}

bool Find_And_Insert_Node(node** hashmap, mytuple t, int HashSize) {
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
            if (tempoNode->nextNode != nullptr)
                tempoNode = tempoNode->nextNode;
            else {
                tempoNode->nextNode = newNode;
                return true;
            }
        }
    }
}
