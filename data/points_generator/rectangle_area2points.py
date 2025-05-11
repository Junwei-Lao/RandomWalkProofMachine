import pandas as pd
import numpy as np

l = []
w = []
area = []
points = []
a = np.sqrt(np.pi / 3)

for m in range(10, 300):
    
    length = 3*a*m
    width = a*m

    total = 0
    for i in range(int(length) + 1):
        for j in range(int(width) +1):
            if (i<= length and j<= width):
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
