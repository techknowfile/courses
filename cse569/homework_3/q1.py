import numpy as np
from scipy.stats import ttest_ind

def main():
    sample_sizes = [30, 150, 600]
    
    for n in sample_sizes:
        D_1 = np.random.normal(1, 1, n)
        D_2 = np.random.normal(1.5, 1, n)
        
        print(n, ttest_ind(D_1, D_2))
        

if __name__ == '__main__':
    main()
