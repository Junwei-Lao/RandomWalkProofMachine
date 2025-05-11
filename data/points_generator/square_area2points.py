import pandas as pd
import numpy as np

l = []
area = []
points = []
a = np.sqrt(np.pi)

for m in range(10, 300):
    
    length = a*m
    width = a*m

    total = 0
    for i in range(int(length) + 1):
        for j in range(int(width) +1):
            if (i<= length and j<= width):
                total += 1


    Area = length * width
    l.append(length)
    area.append(Area)
    points.append(total)
    print(m)


df = pd.DataFrame({
    'Edge-length': l,
    'Area': area,
    'Points': points
})

# Save to CSV
df.to_csv('square_result.csv', index=False)
