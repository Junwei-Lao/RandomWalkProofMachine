import random
import numpy as np
import matplotlib.pyplot as plt
from scipy.spatial import ConvexHull, Delaunay


#initializing boundary
length = 36.06627841
hypo_side = 4*length

#----------------------editable variables are above ↑ ---------------------------------------
long = 2*hypo_side+length
boundary = [(0,0), (hypo_side,hypo_side), (hypo_side+length, hypo_side), (long, 0)]

boundary_np = np.array(boundary)
boundary_hull = ConvexHull(boundary_np)
boundary_delaunay = Delaunay(boundary_np[boundary_hull.vertices])


#initializing variables
directions = [(1,0),(-1,0),(0,1),(0,-1)]
while (True):
    machine = (random.randint(0,int(long)), random.randint(0,int(hypo_side)))
    tester0 = np.array(machine)
    checker0 = boundary_delaunay.find_simplex(tester0) >= 0
    if (checker0):
        break # making sure the starting point of the machine is in the boundary

path = [machine]
walk_count = 0

def totalPoints():
    total = 0
    for i in range(int(long) + 1):
        for j in range(int(hypo_side) +1):
            tester1 = np.array((i, j))
            checker1 = boundary_delaunay.find_simplex(tester1) >= 0
            if (checker1):
                total += 1
    return total


def RandomWalk (machine, walk_count):
    tempo = []
    for dx, dy in directions:
        newx = machine[0] + dx
        newy = machine[1] + dy
        tester = np.array([(newx, newy)])

        #check if the next step is in the boundary
        checker = boundary_delaunay.find_simplex(tester) >= 0

        if (checker[0]):
            tempo.append((newx,newy))

    machine = random.choice(tempo)
    if machine not in path:
        walk_count += 1

    path.append(machine)
    return machine, walk_count


if __name__ == "__main__" :
    totalpoints = totalPoints()
    while (totalpoints // 2 != walk_count):
        machine, walk_count = RandomWalk(machine, walk_count)

    #-------------------------------------------------outputs--------------------------------------------------
    print("Total points: ", totalpoints)
    print("Machine walked: ", len(path)-1)

    #ploting for boundary
    plt.scatter(boundary_np[:,0], boundary_np[:,1], color='black', s = 10)
    for simplex in boundary_hull.simplices:
        plt.plot(boundary_np[simplex, 0], boundary_np[simplex, 1], 'red')

    #ploting for machine path
    path_np = np.array(path)
    plt.scatter(path_np[:,0], path_np[:,1], color = 'orange', s = 10)
    
    #generate graph
    plt.title("Random Walk Machine")
    plt.grid(True)
    plt.show()