import pandas as pd
import numpy as np

l = []
w = []
hypo = []

area = []
points = []
a = np.sqrt(np.pi / 45)

for m in range(10, 300):

    width = a*m
    length = 5 * width
    hypo_side = 4 * width

    height = 2*hypo_side + width
    long = 2*hypo_side + length

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


    Area = 45 * width**2
    w.append(width.item())
    l.append(length.item())
    hypo.append(hypo_side.item())

    #area.append(Area)
    points.append(total)
    print(m)

print(hypo)
print('\n')
print(w)
print('\n')
print(l)
print('\n')
print(points)
'''
df = pd.DataFrame({
    'Width': w,
    'Length': l,
    'Hypo_side': hypo,
    'Area': area,
    'Points': points
})

# Save to CSV
df.to_csv('bowtie_result.csv', index=False)
'''