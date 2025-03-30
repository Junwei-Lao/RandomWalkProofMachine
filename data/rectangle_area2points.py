import pandas as pd
import numpy as np
from scipy.spatial import ConvexHull, Delaunay

l = []
w = []
area = []
points = []
a = np.sqrt(np.pi / 3)

for m in range(10, 300):
    
    length = 3*a*m
    width = a*m

    boundary = [(0,0), (0,width), (length, 0), (length, width)]
    boundary_np = np.array(boundary)
    boundary_hull = ConvexHull(boundary_np)
    boundary_delaunay = Delaunay(boundary_np[boundary_hull.vertices])

    total = 0
    for i in range(int(length) + 1):
        for j in range(int(width) +1):
            tester1 = np.array((i, j))
            checker1 = boundary_delaunay.find_simplex(tester1) >= 0
            if (checker1):
                total += 1


    Area = length * width
    l.append(length)
    w.append(width)
    area.append(Area)
    points.append(total)
    print(m)

df = pd.DataFrame({
    'Length': l,
    'width': w,
    'Area': area,
    'Points': points
})

# Save to CSV
df.to_csv('rectangle_result.csv', index=False)
