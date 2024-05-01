
theta = 0.5

def distance(a : tuple[float, float], b : tuple[float, float]) -> float:
    return ((b[0] - a[0]) ** 2 + (b[1] - a[1]) ** 2) ** 0.5

class BH_Tree:
    def __init__(self, nodes: list[tuple[float, float]], lo_x: float, hi_x: float, lo_y: float, hi_y: float) -> None:
        self.count = len(nodes)
        self.width = hi_x - lo_x

        if self.count == 0:
            return
        
        if self.count == 1:
            self.com = nodes[0]
            return

        tot_x = 0
        tot_y = 0
        ne = []
        nw = []
        se = []
        sw = []

        mid_x = (lo_x + hi_x) / 2
        mid_y = (lo_y + hi_y) / 2

        for node in nodes:
            x, y = node
            tot_x += x
            tot_y += y
            if x <= mid_x:
                if y <= mid_y:
                    nw.append(node)
                else:
                    sw.append(node)
            else:
                if y <= mid_y:
                    ne.append(node)
                else:
                    se.append(node)
        
        self.com = (tot_x / self.count, tot_y / self.count)
        self.ne = BH_Tree(ne, mid_x, hi_x, lo_y, mid_y)
        self.nw = BH_Tree(nw, lo_x, mid_x, lo_y, mid_y)
        self.se = BH_Tree(se, mid_x, hi_x, mid_y, hi_y)
        self.sw = BH_Tree(sw, lo_x, mid_x, mid_y, hi_y)

    def query(self, coord: tuple[float, float], k: float):
        if self.count == 0:
            return (0.0, 0.0)
        
        d = distance(coord, self.com)
        if d == 0:
            return (0.0, 0.0)
        
        if self.count == 1 or self.width / d  <= theta:
            x_for = (coord[0] - self.com[0]) * self.count * (k / d) ** 2
            y_for = (coord[1] - self.com[1]) * self.count * (k / d) ** 2
            return (x_for, y_for)

        sex, sey = self.se.query(coord, k) 
        nex, ney = self.ne.query(coord, k) 
        nwx, nwy = self.nw.query(coord, k) 
        swx, swy = self.sw.query(coord, k)
        return (sex + nex + nwx + swx, sey + ney + nwy + swy)