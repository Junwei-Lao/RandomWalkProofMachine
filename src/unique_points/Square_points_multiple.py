import random
import numpy as np
import matplotlib.pyplot as plt
from scipy.spatial import ConvexHull, Delaunay


#the number of times the program will run
times_running = 60

#initializing boundary
length = 50

#histogram bin width
bin_width = 500

#----------------------editable variables are above ↑ ---------------------------------------
width = length
boundary = [(0,0), (0,width), (length, 0), (length, width)]
boundary_np = np.array(boundary)
boundary_hull = ConvexHull(boundary_np)
boundary_delaunay = Delaunay(boundary_np[boundary_hull.vertices])


def totalPoints():
    total = 0
    for i in range(int(length) + 1):
        for j in range(int(width) + 1):
            tester1 = np.array((i, j))
            checker1 = boundary_delaunay.find_simplex(tester1) >= 0
            if (checker1):
                total += 1
    return total

#initializing variables
def initialize():
    while (True):
        machine = (random.randint(0,int(length)), random.randint(0,int(width)))
        tester0 = np.array(machine)
        checker0 = boundary_delaunay.find_simplex(tester0) >= 0
        if (checker0):
            break # making sure the starting point of the machine is in the boundary
    path = [machine]
    return (machine, 0, path)

directions = [(1,0),(-1,0),(0,1),(0,-1)]
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
    
    num_list = []
    totalpoint = totalPoints()
    for i in range(times_running):
        machine, walk_count, path = initialize()
        while (True):
            machine, walk_count = RandomWalk(machine, walk_count)

            if (totalpoint // 2 == walk_count):
                break

        num_list.append(len(path)-1)
        
    #-------------------------------------------------outputs--------------------------------------------------

    #histogram initialize
    num_list_np = np.array(num_list)
    bins = np.arange(0, num_list_np.max() + bin_width, bin_width)
    mean = np.mean(num_list_np)
    var = np.var(num_list_np)

    #generate graph
    plt.hist(num_list_np, bins=bins, edgecolor='black', color='blue', alpha=0.7)
    plt.xlabel('Value Range')
    plt.ylabel('Frequency')
    plt.title(f"Random Walk Machine runs {times_running} times")
    plt.figtext(0.5, 0.01, f"Mean: {mean:.2f}, Variance: {var:.2f}", ha="center", fontsize=11)
    plt.show()