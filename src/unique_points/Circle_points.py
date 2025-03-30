import random
import numpy as np
import matplotlib.pyplot as plt
from scipy.spatial import ConvexHull, Delaunay


#initializing boundary
radius = 20
points_num = 812


#----------------------editable variables are above ↑ ---------------------------------------
angle = 0
angleC = 2*np.pi / points_num
boundary = []
for i in range(points_num):
    boundary.append((radius + radius*np.cos(angle), radius + radius*np.sin(angle)))
    angle += angleC

boundary_np = np.array(boundary)
boundary_hull = ConvexHull(boundary_np)
boundary_delaunay = Delaunay(boundary_np[boundary_hull.vertices])

#initializing variables
directions = [(1,0),(-1,0),(0,1),(0,-1)]
while (True):
    machine = (random.randint(0,int(2*radius)), random.randint(0,int(2*radius)))
    tester0 = np.array(machine)
    checker0 = boundary_delaunay.find_simplex(tester0) >= 0
    if (checker0):
        break # making sure the starting point of the machine is in the boundary

path = [machine] # The path of the machine (recording as 2D points list)
walk_count = 0 # The starting point does not count as a step

def totalPoints(): # The total number of points in the boundary
    total = 0
    for i in range(int(2*radius) + 1):
        for j in range(int(2*radius) + 1):
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