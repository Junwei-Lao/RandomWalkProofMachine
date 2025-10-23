#ifndef MYHASH_H
#define MYHASH_H

struct tuple
{
    int x;
    int y;
    tuple(int xInput, int yInput);
};

struct node
{
    int x;
    int y;
    node* nextNode;
    node(int xInput, int yInput);
    node(tuple t);
};

bool comparator(node* first, node* second);

int hashCode(tuple first, int HashSize);

bool Find_And_Insert_Node(node** hashmap, tuple t, int HashSize);

#endif // MYHASH_H