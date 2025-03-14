import random
import numpy as np
import matplotlib.pyplot as plt
from scipy.spatial import ConvexHull, Delaunay

# The error between path area / boundary area and 1/2
error = 0.01

#initializing boundary
side_length = 100

#----------------------editable variables are above ↑ ---------------------------------------
top = np.sqrt(3) * (side_length/2)

boundary = [(0,0), (side_length, 0), (side_length/2, top)]
boundary_np = np.array(boundary)
boundary_hull = ConvexHull(boundary_np)
boundary_delaunay = Delaunay(boundary_np[boundary_hull.vertices])
area_b = boundary_hull.volume

#initializing variables
directions = [(1,0),(-1,0),(0,1),(0,-1)]
while (True):
    machine = (random.randint(0,int(side_length)), random.randint(0,int(top)))
    tester0 = np.array(machine)
    checker0 = boundary_delaunay.find_simplex(tester0) >= 0
    if (checker0):
        break # making sure the starting point of the machine is in the boundary

path = [machine]
walk_count = 0

def RandomWalk (machine):
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
    path.append(machine)
    return machine


if __name__ == "__main__" :
    while (True):
        machine = RandomWalk(machine)
        walk_count += 1
        path_np = np.array(path)
        try:
            hull = ConvexHull(path_np)
            area = hull.volume
            if ((abs(area/area_b - 0.5)) <= error):
                break

            if (walk_count >= 100000):
                raise ValueError("Maybe causing an infinite loop")
        except:
            continue

    #-------------------------------------------------outputs--------------------------------------------------
    print("Total steps: ", walk_count)
    print("Boundary area: ", area_b)
    print("Walking machine path area: ", area)

    #ploting for boundary
    plt.scatter(boundary_np[:,0], boundary_np[:,1], color='black', s = 10)
    for simplex in boundary_hull.simplices:
        plt.plot(boundary_np[simplex, 0], boundary_np[simplex, 1], 'red')

    #ploting for machine path
    plt.scatter(path_np[:,0], path_np[:,1], color = 'orange', s = 10)
    for simplex in hull.simplices:
        plt.plot(path_np[simplex, 0], path_np[simplex, 1], 'green')
    
    #generate graph
    plt.title("Random Walk Machine")
    plt.grid(True)
    plt.show()