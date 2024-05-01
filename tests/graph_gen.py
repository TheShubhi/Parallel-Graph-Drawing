import networkx as nx
from pathlib import Path


def sierpinski_helper(levels, v1, v2, v3):
    if levels <= 1:
        return [(v1, v2), (v2, v3), (v1, v3)]

    v4 = sierpinski_helper.c
    sierpinski_helper.c += 1
    v5 = sierpinski_helper.c
    sierpinski_helper.c += 1
    v6 = sierpinski_helper.c
    sierpinski_helper.c += 1

    edges = sierpinski_helper(levels - 1, v1, v4, v5)
    edges += sierpinski_helper(levels - 1, v2, v5, v6)
    edges += sierpinski_helper(levels - 1, v3, v4, v6)
    return edges


def gen_sierpinski(levels):
    sierpinski_helper.c = 3
    return nx.Graph(sierpinski_helper(levels, 0, 1, 2))


def write_graph(filename, graph: nx.Graph):
    p = Path(__file__).with_name(filename)
    with p.open('w') as file:
        file.write(f"{graph.number_of_nodes()} {graph.number_of_edges()}\n")
        for edge in graph.edges():
            file.write(f"{edge[0]} {edge[1]}\n")

# G = nx.generators.barabasi_albert_graph(20, 3)
# for i in range(10):
#     G.add_edge(random.randrange(40), random.randrange(40))

G = gen_sierpinski(4)
write_graph("sierpinski_4.txt", G)