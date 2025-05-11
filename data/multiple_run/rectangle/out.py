#import Circle
import Rectangle
import pandas
import numpy as np
from concurrent.futures import ThreadPoolExecutor

#3-1 Rectangle

input_file = "Rectangle.csv"
output_file = "Rectangle_results.csv"

file_rec = pandas.read_csv(input_file)
j = 0
# Write headers first (if creating new)
with open(output_file, mode='w', newline='') as f:
    f.write("Length,width,recPoints,mean,Std,middle\n")

for l, w, points in zip(file_rec["Length"], file_rec["width"], file_rec["recPoints"]):
    with ThreadPoolExecutor() as executor:
        results = list(executor.map(
            lambda _: Rectangle.mainFunc(l, w, points),
            range(1000)
        ))

    result_array = np.array(results)
    mean = np.mean(result_array)
    std = np.std(result_array)
    median = np.median(result_array)

    # Append result to CSV immediately
    with open(output_file, mode='a', newline='') as f:
        f.write(f"{l},{w},{points},{mean},{std},{median}\n")

    print(j)
    j += 1