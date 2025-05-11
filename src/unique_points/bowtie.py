import random
import numpy as np
import matplotlib.pyplot as plt
from scipy.spatial import ConvexHull, Delaunay


#initializing boundary
length = 50
width = 10

hypo_side = 40


#----------------------editable variables are above ↑ ---------------------------------------
height = 2*hypo_side + width
long = 2*hypo_side + length


boundary = [(0,0), (0,height), (hypo_side, hypo_side), (hypo_side, hypo_side + width), (hypo_side + length, hypo_side), (hypo_side + length, hypo_side + width), (long, 0), (long, height)]
boundary_np = np.array(boundary)

boundary_hull = ConvexHull(boundary_np)
boundary_delaunay = Delaunay(boundary_np[boundary_hull.vertices])

#initializing variables
directions = [(1,0),(-1,0),(0,1),(0,-1)]
while (True):
    machine = (random.randint(0,int(long)+1), random.randint(0,int(height)+1))

    if (machine[0] <= hypo_side):
        if (machine[1] >= machine[0] and machine[1] <= height - machine[0]):
            break
    elif (machine[0] > hypo_side and machine[0] <= hypo_side + length):
        if (machine[1] >= hypo_side and machine[1] <= hypo_side + width):
            break
    elif (machine[0] > hypo_side + length and machine[0] <= long):
        if (machine[1] >= long-machine[0] and machine[1] <= machine[0] + width - length):
            break


path = [machine]
walk_count = 0

def totalPoints():
    total = 0
    for i in range(int(long) + 1):
        for j in range(int(height) +1):
            if (i>= 0 and i <= hypo_side):
                if (j >= i and j <= (height - i)):
                    total += 1
            elif (i > hypo_side and i <= (hypo_side + length)):
                if (j >= hypo_side and j <= (hypo_side + width)):
                    total += 1
            elif (i > (hypo_side + length) and i <= long):
                if (j >= (long-i) and j <= (i + width - length)):
                    total += 1
    return total


def RandomWalk (machine, walk_count):
    tempo = []
    for dx, dy in directions:
        newx = machine[0] + dx
        newy = machine[1] + dy

        if (newx >= 0 and newx <= hypo_side):
            if (newy >= newx and newy <= (height - newx)):
                tempo.append((newx,newy))
        elif (newx > hypo_side and newx <= (hypo_side + length)):
            if (newy >= hypo_side and newy <= (hypo_side + width)):
                tempo.append((newx,newy))
        elif (newx > (hypo_side + length) and newx <= long):
            if (newy >= (long-newx) and newy <= (newx + width - length)):
                tempo.append((newx,newy))

    machine = random.choice(tempo)
    if machine not in path:
        walk_count += 1

    path.append(machine)
    return machine, walk_count


if __name__ == "__main__" :
    totalpoints = totalPoints()
    while (totalpoints//2 != walk_count):
        machine, walk_count = RandomWalk(machine, walk_count)

    #-------------------------------------------------outputs--------------------------------------------------
    print("Total points: ", totalpoints)
    print("Machine walked: ", len(path)-1)

    #ploting for boundary
    plt.scatter(boundary_np[:,0], boundary_np[:,1], color='red', s = 10)
    for simplex in boundary_hull.simplices:
        plt.plot(boundary_np[simplex, 0], boundary_np[simplex, 1], 'red')

    #ploting for machine path
    path_np = np.array(path)
    plt.scatter(path_np[:,0], path_np[:,1], color = 'orange', s = 10)
    
    #generate graph
    plt.title("Random Walk Machine")
    plt.grid(True)
    plt.show()