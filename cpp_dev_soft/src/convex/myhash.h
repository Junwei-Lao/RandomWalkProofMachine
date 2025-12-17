#ifndef MYHASH_H
#define MYHASH_H

struct mytuple
{
    int x;
    int y;
    mytuple(int xInput, int yInput );
};

struct node
{
    int x;
    int y;
    node* nextNode;
    node(int xInput, int yInput);
    node(mytuple t);
};

bool comparator(node* first, node* second);

int hashCode(mytuple first, int HashSize);

bool Find_And_Insert_Node(node** hashmap, mytuple t, int HashSize);

#endif // MYHASH_H