import matplotlib.pyplot as plt
import numpy as np
import os

folder = 'Images'
filenames = [f for f in os.listdir(folder) if f.endswith('.txt')]

for f in filenames:
    # Load the array from the text file
    array = np.loadtxt(os.path.join(folder, f))

    # Display the image
    plt.imshow(array, cmap='gray', aspect='auto')
    plt.tight_layout()
    plt.axis('off')
    plt.rcParams['figure.autolayout'] = True
    os.remove(os.path.join(folder, f))
    f = f.replace('.txt', '.png')
    plt.savefig(os.path.join(folder, f))