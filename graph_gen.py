import networkx as nx
import sys


def gen_sierpinski(levels, v1, v2, v3):
    if levels <= 1:
        return [(v1, v2), (v2, v3), (v1, v3)]

    v4 = gen_sierpinski.c
    gen_sierpinski.c += 1
    v5 = gen_sierpinski.c
    gen_sierpinski.c += 1
    v6 = gen_sierpinski.c
    gen_sierpinski.c += 1

    edges = gen_sierpinski(levels - 1, v1, v4, v5)
    edges += gen_sierpinski(levels - 1, v2, v5, v6)
    edges += gen_sierpinski(levels - 1, v3, v4, v6)
    return edges


def sierpinski(levels):
    gen_sierpinski.c = 3
    return gen_sierpinski(levels, 0, 1, 2)


G: nx.Graph
G = nx.Graph(sierpinski(4))
# G = nx.generators.barabasi_albert_graph(20, 3)
# for i in range(10):
#     G.add_edge(random.randrange(40), random.randrange(40))


# Write the graph to stdout
sys.stdout.write(f"{G.number_of_nodes()} {G.number_of_edges()}\n")
for edge in G.edges():
    sys.stdout.write(f"{edge[0]} {edge[1]}\n")
