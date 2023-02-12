class Prio:
    def __init__(self):
        self.o = []
        self.idxs = {}
        self.saved_deps = {}
        self.unmet_deps = {}

    def find_min_and_max_(self, s, deps):
        min_idx = 0
        max_idx = None

        deps_idxs = []
        for d in deps:
            if d in self.idxs:
                deps_idxs.append(self.idxs[d] + 1)
            else:
                if d not in self.unmet_deps:
                    self.unmet_deps[d] = set()
                self.unmet_deps[d].add(s)

        if deps_idxs:
            min_idx = max(deps_idxs)

        unmet_deps_idxs = []
        if s in self.unmet_deps:
            for c in self.unmet_deps[s]:
                unmet_deps_idxs.append(self.idxs[c])

            max_idx = min(unmet_deps_idxs)

        return min_idx, max_idx, unmet_deps_idxs

    def bubble_(self, idx, min_idx):
        while idx < min_idx:
            if self.o[idx] in self.saved_deps[self.o[idx + 1]]:
                print(f"Conflicting dependencies, '{self.o[idx]}' needs to move beyond '{self.o[idx + 1]}', but '{self.o[idx + 1]}' depends on it")
                exit(1)

            self.o[idx], self.o[idx + 1] = self.o[idx + 1], self.o[idx]
            self.idxs[self.o[idx]] -= 1
            self.idxs[self.o[idx + 1]] += 1
            idx += 1

    def add(self, s, deps=None):
        if s in self.o:
            print(f"Already added: '{s}'")
            return

        deps = deps or set()
        if s in deps:
            deps.remove(s)

        min_idx = 0
        self.saved_deps[s] = deps or set()

        if deps is not None:
            min_idx, max_idx, unmet_deps_idxs = self.find_min_and_max_(s, deps)

            if max_idx is not None and max_idx < min_idx:
                if len([v for v in deps if v in self.unmet_deps[s]]) > 0:
                    print(f"Circular dependency: '{s}' depends on {deps}, but {self.unmet_deps[s]} depends on it")
                    exit(1)

                while max_idx < min_idx:
                    bubble_idx = max(idx for idx in unmet_deps_idxs if idx < min_idx)
                    self.bubble_(bubble_idx, min_idx - 1)
                    min_idx, max_idx, unmet_deps_idxs = self.find_min_and_max_(s, deps)

            if unmet_deps_idxs:
                del self.unmet_deps[s]

        self.o.insert(min_idx, s)
        self.idxs[s] = min_idx

        for i in range(min_idx + 1, len(self.o)):
            self.idxs[self.o[i]] += 1


def main():
    p = Prio()

    p.add("a")
    p.add("c", ["b"])
    p.add("d", ["c"])
    p.add("b", ["a"])

    print(p.o, p.idxs)


if __name__ == '__main__':
    main()
