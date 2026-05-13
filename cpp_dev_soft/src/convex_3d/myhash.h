#ifndef MYHASH_H
#define MYHASH_H

struct mytuple_3d
{
    int x;
    int y;
    int z;
    mytuple_3d(int xInput, int yInput, int zInput);
};

struct node
{
    int x;
    int y;
    int z;
    node* nextNode;
    node(int xInput, int yInput, int zInput);
    node(mytuple_3d t);
};

bool comparator(node* first, node* second);

int hashCode(mytuple_3d first, int HashSize);

bool Find_And_Insert_Node(node** hashmap, mytuple_3d t, int HashSize);

#endif // MYHASH_H