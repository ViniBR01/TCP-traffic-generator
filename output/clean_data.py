# Clean outliers based on the z-score: 
# delete all values that are more than 3 stDev away from mean

import pandas as pd
from scipy import stats
import numpy as np

data_in = [0, 1, 2, 3, 4, 5, 67, 6, 7, 8, 9, 10000]
filename = 'download-times-DL-10000ms-10kB-1617663388.txt'
# data_in = pd.read_csv(filename, header = None)
print(data_in)

def clean_data(data_in):
    data_out = np.array(data_in)
    z = np.abs(stats.zscore(data_out))
    z_th = 3
    while(np.max(z) > z_th):
        data_out = data_out[z<z_th]
        #print(data_out)
        z = np.abs(stats.zscore(data_out))
    return data_out

print(clean_data(data_in))
