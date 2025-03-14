import random
import numpy as np
import matplotlib.pyplot as plt
from scipy.spatial import ConvexHull, Delaunay

# The error between path area / boundary area and 1/2
error = 0.01

#the number of times the program will run
times_running = 150

#initializing boundary
length = 50

#----------------------editable variables are above ↑ ---------------------------------------
width = length
boundary = [(0,0), (0,width), (length, 0), (length, width)]
boundary_np = np.array(boundary)
boundary_hull = ConvexHull(boundary_np)
boundary_delaunay = Delaunay(boundary_np[boundary_hull.vertices])
area_b = boundary_hull.volume

#initializing variables
def initialize():
    machine = (random.randint(0,int(length)), random.randint(0,int(width)))
    path = [machine]
    walk_count = 0
    return (machine, walk_count, path)

directions = [(1,0),(-1,0),(0,1),(0,-1)]
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
    return machine

if __name__ == "__main__" :
    machine, walk_count, path = initialize()
    num_list = []
    for i in range(times_running):
        while (True):
            machine = RandomWalk(machine)
            path.append(machine)
            walk_count += 1

            #area calculate
            path_np = np.array(path)
            try:
                hull = ConvexHull(path_np)
                area = hull.volume
                if ((abs(area/area_b - 0.5)) <= error):
                    break

                if (walk_count >= 100000):
                    print("Maybe causing an infinite loop")
                    break
            except:
                continue

        num_list.append(walk_count)
        machine, walk_count, path = initialize()
        
    #-------------------------------------------------outputs--------------------------------------------------

    #histogram initialize
    num_list_np = np.array(num_list)
    bin_width = 500
    bins = np.arange(0, num_list_np.max() + bin_width, bin_width)

    #generate graph
    plt.hist(num_list_np, bins=bins, edgecolor='black', color='blue', alpha=0.7)
    plt.xlabel('Value Range')
    plt.ylabel('Frequency')
    plt.title(f"Random Walk Machine runs {times_running} times")
    plt.show()