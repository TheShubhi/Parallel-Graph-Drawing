import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import sys

def fruchterman_reingold(edge_list, width, height, iterations=1000, temp_init = 1, cooling_rate = 0.99):
    num_vertices = max(max(edge) for edge in edge_list) + 1
    vertex_coordinates = np.random.rand(num_vertices, 2) * np.array([width, height])
    
    k = 0.2 * np.sqrt(width * height / num_vertices)  # Optimal distance between vertices
    t = temp_init

    for _ in range(iterations):
        displacement = np.zeros_like(vertex_coordinates)
        
        # Calculate repulsive forces
        for i in range(num_vertices):
            for j in range(i+1, num_vertices):
                delta_pos = vertex_coordinates[j] - vertex_coordinates[i]
                distance = np.linalg.norm(delta_pos)
                if distance != 0:
                    displacement[i] -= delta_pos * (k / distance)**2
                    displacement[j] += delta_pos * (k / distance)**2
        
        # Calculate attractive forces
        for i, j in edge_list:
            delta_pos = vertex_coordinates[j] - vertex_coordinates[i]
            distance = np.linalg.norm(delta_pos)
            if distance != 0:
                displacement[i] += delta_pos * distance / k
                displacement[j] -= delta_pos * distance / k
        
        # Update vertex positions
        displacement_norm = np.linalg.norm(displacement, axis=1)
        displacement_norm[displacement_norm == 0] = 1
        vertex_coordinates += displacement * np.minimum(1, t / displacement_norm)[:, np.newaxis]
        
        # Ensure vertices are within the plotting area
        vertex_coordinates = np.clip(vertex_coordinates, 0, np.array([width, height]))
    
        # Update cooling
        t *= cooling_rate

    return vertex_coordinates

# Example edge list
# edge_list = [(0, 1), (0, 2), (1, 2), (1, 3), (2, 3), (3, 4), (4, 5), (5, 0)]
seq = lambda: list(map(int, sys.stdin.readline().strip().split()))
[n, m] = seq()
edge_list = [seq() for _ in range(m)]
    

# Width and height of the plotting area
width = 1
height = 1
for x in fruchterman_reingold(edge_list, width, height):
    print(*x)
exit()

# Create figure and axis
fig, ax = plt.subplots()

# Initialize plot elements
lines = []
for edge in edge_list:
    lines.append(ax.plot([], [], 'k-', zorder=1, linewidth=0.5)[0])
points = ax.scatter([], [], s=3000 / max(len(edge_list), 1), zorder=2)  # Scale point size based on the number of edges

# Set plot limits and remove axes
ax.set_xlim(0, width)
ax.set_ylim(0, height)
ax.axis('off')

# Generate random colors
num_vertices = max(max(edge) for edge in edge_list) + 1

# Update function for animation
def update(frame):
    for line, edge in zip(lines, edge_list):
        i, j = edge
        line.set_data([frame[i][0], frame[j][0]], [frame[i][1], frame[j][1]])
    points.set_offsets(frame)
    return lines + [points]

# Create animation
ani = FuncAnimation(fig, update, frames=fruchterman_reingold(edge_list, width, height), interval=50, blit=True)

plt.tight_layout(pad=0)  # Remove padding

plt.show()
