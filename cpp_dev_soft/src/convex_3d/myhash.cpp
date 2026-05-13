#include "myhash.h"
#include <cstdint>

mytuple_3d::mytuple_3d(int xInput, int yInput, int zInput) {
    x = xInput;
    y = yInput;
    z = zInput;
}

node::node(int xInput, int yInput, int zInput) {
    x = xInput;
    y = yInput;
    z = zInput;
    nextNode = nullptr;
}

node::node(mytuple_3d t) {
    x = t.x;
    y = t.y;
    z = t.z;
    nextNode = nullptr;
}

bool comparator(node* first, node* second) {
    return (first->x == second->x && first->y == second->y && first->z == second->z);
}

int hashCode(mytuple_3d first, int HashSize) {
    uint64_t hx = static_cast<uint64_t>(static_cast<uint32_t>(first.x));
    uint64_t hy = static_cast<uint64_t>(static_cast<uint32_t>(first.y));
    uint64_t hz = static_cast<uint64_t>(static_cast<uint32_t>(first.z));
    return (hx << 32 | hy << 16 | hz) % HashSize;
}

bool Find_And_Insert_Node(node** hashmap, mytuple_3d t, int HashSize) {
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
