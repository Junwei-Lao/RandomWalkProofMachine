import bowtie
import pandas
import numpy as np
from concurrent.futures import ThreadPoolExecutor


input_file = "bowtie.csv"
output_file = "bowtie_results.csv"

file_rec = pandas.read_csv(input_file)

# Write headers first (if creating new)
with open(output_file, mode='w', newline='') as f:
    f.write("Width,Length,Hypo_side,bowtiePoints,mean,Std,middle\n")

for j, (w, l, hypo, points) in enumerate(zip(file_rec["Width"], file_rec["Length"], file_rec["Hypo_side"], file_rec["Points"])):
    with ThreadPoolExecutor() as executor:
        results = list(executor.map(
            lambda _: bowtie.mainFunc(w, l, hypo, points),
            range(1000)
        ))

    result_array = np.array(results)
    mean = np.mean(result_array)
    std = np.std(result_array)
    median = np.median(result_array)

    # Append result to CSV immediately
    with open(output_file, mode='a', newline='') as f:
        f.write(f"{w},{l},{hypo},{points},{mean},{std},{median}\n")

    print(j)