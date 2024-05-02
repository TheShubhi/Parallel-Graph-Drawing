import matplotlib.pyplot as plt
import matplotlib.lines as lines
from sys import stdin, stdout
import sys
from datetime import datetime
from pathlib import Path

seq = lambda: list(map(int, stdin.readline().strip().split()))
seqf = lambda: list(map(float, stdin.readline().strip().split()))

# Function to draw graph from edge list and vertex coordinates
def draw_graph(edge_list, coordinates, filename: str):
    plt.figure(frameon=True)
    for edge in edge_list:
        i, j = edge
        plt.plot([coordinates[i][0], coordinates[j][0]], [coordinates[i][1], coordinates[j][1]], 'k-', linewidth = 50 / len(coordinates))
    for i, (x, y) in enumerate(coordinates):
        plt.plot(x, y, 'o', markersize=500 / len(coordinates), label=str(i))

    plt.gca().axis('off')
    plt.tight_layout()
    # Show plot
    plt.savefig(filename)

if __name__ == "__main__":
    # Read input
    [n, m] = seq()
    coords = [tuple(seqf()) for _ in range(n)]
    edges = [tuple(seq()) for _ in range(m)]

    name = datetime.now()
    if len(sys.argv) > 1 :
        name = sys.argv[1]
        
   
    output_filename = f'{Path(__file__).parent}/tests/drawing/temp-{name}.png'

    # Draw the graph
    draw_graph(edges, coords, output_filename)
