import Square
import pandas
import numpy as np
from concurrent.futures import ThreadPoolExecutor

input_file = "Square.csv"
output_file = "Square_results.csv"

file_rec = pandas.read_csv(input_file)

# Write headers first (if creating new)
with open(output_file, mode='w', newline='') as f:
    f.write("Edge,sqrPoints,mean,Std,middle\n")

for j, (s, points) in enumerate(zip(file_rec["Edge"], file_rec["sqrPoints"])):
    with ThreadPoolExecutor() as executor:
        results = list(executor.map(
            lambda _: Square.mainFunc(s, points),
            range(1000)
        ))

    result_array = np.array(results)
    mean = np.mean(result_array)
    std = np.std(result_array)
    median = np.median(result_array)

    # Append result to CSV immediately
    with open(output_file, mode='a', newline='') as f:
        f.write(f"{s},{points},{mean},{std},{median}\n")

    print(j)