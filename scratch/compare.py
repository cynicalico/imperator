import numpy as np
import sys
import pandas as pd
from matplotlib import pyplot as plt


def main():
    col_counts = [int(v) for v in sys.argv[1:]]

    plt.rcParams['figure.autolayout'] = True

    df = pd.read_csv('out.csv')

    max_col = len(df.columns) - 1
    fig, axs = plt.subplots(1, len(col_counts), sharex=True)
    axs = np.array(axs).reshape(-1)

    i = 1
    j = 0
    while i < max_col:
        for m in range(col_counts[j]):
            axs[j].plot(df[df.columns[0]], df[df.columns[i + m]])
        axs[j].set(xlabel=df.columns[0])
        axs[j].legend(df.columns[i:i + col_counts[j]])

        i += col_counts[j]
        j += 1

    plt.show()


if __name__ == '__main__':
    main()
