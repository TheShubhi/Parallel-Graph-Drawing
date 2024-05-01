import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

import argparse
import sys

from bh_tree import BH_Tree


def fruchterman_reingold(edge_list, n, max_iters=None, temperature = 1, cooling_rate = None):
    # Initialise parameters
    if max_iters is None:
        max_iters = 5 * n

    target_dist = 0.1 * np.sqrt(1 / n)  # Target distance between connected vertices
    
    # temperature @ max_iters = temperature * (cooling_rate)^max_iters = 0.1 * target_dist
    if cooling_rate is None:
        cooling_rate = ((0.1 * target_dist) / temperature) ** (1 / max_iters)
        
    # Randomly place vertices in [0, 1) x [0, 1)
    vertex_coordinates = np.random.rand(n, 2)
    
    for _ in range(max_iters):
        displacement = np.zeros_like(vertex_coordinates)
        
        tr = BH_Tree(vertex_coordinates, 0, 1, 0, 1)

        # Calculate repulsive forces
        for i in range(n):
            displacement[i] = tr.query(vertex_coordinates[i], target_dist)
        
        # Calculate attractive forces
        for i, j in edge_list:
            delta_pos = vertex_coordinates[j] - vertex_coordinates[i]
            distance = np.linalg.norm(delta_pos)
            if distance != 0:
                displacement[i] += delta_pos * distance / target_dist
                displacement[j] -= delta_pos * distance / target_dist
        
        # Update vertex positions (using temperature)
        displacement_norm = np.linalg.norm(displacement, axis=1)
        displacement_norm[displacement_norm == 0] = 1
        vertex_coordinates += displacement * np.minimum(1, temperature / displacement_norm)[:, np.newaxis]
        
        # Ensure vertices are within the plotting area
        vertex_coordinates = 1 - np.abs(1 - np.abs(vertex_coordinates))
    
        # Update cooling
        temperature *= cooling_rate

        # Break early
        if max(np.linalg.norm(displacement, axis=1)) < 0.1 * target_dist:
            break        

        # Use as generator
        yield vertex_coordinates

    return vertex_coordinates


class GenValue:
    def __init__(self, gen):
        self.gen = gen

    def __iter__(self):
        self.value = yield from self.gen
        return self.value


def animated_force_directed_drawing(edge_list, n):
    coord_gen = GenValue(fruchterman_reingold(edge_list, n))

    # Create figure and axis
    fig, ax = plt.subplots()

    # Initialize plot elements
    lines = []
    for edge in edge_list:
        lines.append(ax.plot([], [], 'k-', zorder=1, linewidth=50 / n)[0])
    points = ax.scatter([], [], s=500 / n, zorder=2)  # Scale point size based on the number of edges

    # Set plot limits and remove axes
    ax.set_xlim(0, 1)
    ax.set_ylim(0, 1)
    ax.axis('off')

    # Update function for animation
    def update(frame):
        for line, edge in zip(lines, edge_list):
            i, j = edge
            line.set_data([frame[i][0], frame[j][0]], [frame[i][1], frame[j][1]])
        points.set_offsets(frame)
        return lines + [points]

    # Create animation
    ani = FuncAnimation(fig, update, frames=coord_gen, interval=50, blit=True)

    plt.tight_layout()  # Remove padding

    plt.show()

    return coord_gen.value


def force_directed_drawing(edge_list, n):
    gen_with_val = GenValue(fruchterman_reingold(edge_list, n))

    # Run through all steps
    for _ in gen_with_val:
        pass

    return gen_with_val.value


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-a", "--animate", help="display animation of graph drawing using matplotlib", action="store_true")
    args = parser.parse_args()
    
    # Read in graph through stdin
    seq = lambda: list(map(int, sys.stdin.readline().strip().split()))
    try:
        [n, m] = seq()
        edge_list = [seq() for _ in range(m)]
    except:
        print("Invalid file format for graph", file=sys.stderr)
        exit(1)

    # Calculate coordinates
    if args.animate:
        coords = animated_force_directed_drawing(edge_list, n)
    else:
        coords = force_directed_drawing(edge_list, n)

    # Output graph using stdout
    print(n, m)
    for x in coords:
        print(*x)
    for e in edge_list:
        print(*e)
