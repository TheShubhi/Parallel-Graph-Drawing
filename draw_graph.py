import matplotlib.pyplot as plt
import matplotlib.lines as lines
from sys import stdin, stdout
import sys
from datetime import datetime

inp = lambda: int(stdin.readline())
inpf = lambda: float(stdin.readline())
inps = lambda: stdin.readline().strip()

seq = lambda: list(map(int, stdin.readline().strip().split()))
seqf = lambda: list(map(float, stdin.readline().strip().split()))
seqc = lambda: list(stdin.readline().strip())

mul = lambda: map(int, stdin.readline().strip().split())
mulf = lambda: map(float, stdin.readline().strip().split())
muls = lambda: stdin.readline().strip().split()

jn = lambda x, l: x.join(map(str, l))

ceildiv = lambda x, d: -(x // -d)

flush = lambda: stdout.flush()
fastprint = lambda x: stdout.write(str(x))

# Function to draw graph from edge list and vertex coordinates
def draw_graph(edge_list, coordinates, filename: str):
    plt.figure(frameon=True)
    for edge in edge_list:
        i, j = edge
        plt.plot([coordinates[i][0], coordinates[j][0]], [coordinates[i][1], coordinates[j][1]], 'k-', linewidth = 50 / len(coordinates))
    for i, (x, y) in enumerate(coordinates):
        plt.plot(x, y, 'o', markersize=500 / len(coordinates), label=str(i))

    # Show plot
    plt.savefig(filename)

def main():
    # Read input
    [n, m] = seq()
    coords = [tuple(seqf()) for _ in range(n)]
    edges = [tuple(seq()) for _ in range(m)]

    output_filename = f'tests/test-outputs/drawing/{datetime.now()}.png'
    if len(sys.argv) > 1 :
        output_filename = sys.argv[1]

    # Draw the graph
    draw_graph(edges, coords, output_filename)

if __name__ == "__main__":
    main()
