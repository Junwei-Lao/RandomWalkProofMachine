import pandas as pd
import numpy as np

side = []
area = []
points = []
a = 2 * np.sqrt(np.pi / np.sqrt(3))
b = np.sqrt(np.pi * np.sqrt(3))

for m in range(10, 300):

    side_length = a*m
    top = b*m
    top_double = top*2

    total = 0

    for i in range(int(side_length) + 1):
        inbound = True
        for j in range(int(side_length) + 1):
            if (i <= side_length/2 and inbound):
                if (j <= np.sqrt(3) * i):
                    total += 1
                else:
                    inbound = False
            elif (i > side_length/2 and inbound):
                if (j <= (-np.sqrt(3) * i) + top_double):
                    total += 1
                else:
                    inbound = False
            else:
                break


    Area = side_length * top / 2
    side.append(a*m)
    area.append(Area)
    points.append(total)
    print(m)


df = pd.DataFrame({
    'Side Length': side,
    'triArea': area,
    'triPoints': points
})

# Save to CSV
df.to_csv('triangle_result.csv', index=False)
