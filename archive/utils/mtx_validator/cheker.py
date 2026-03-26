import os

from pathlib import Path
from scipy.io import mmread

EPS = 1e-12

def isSymmetricMatrix(path):
    A = mmread(path).tocoo()

    if A.shape[0] != A.shape[1]:
        return False

    A_T = A.T.tocoo()
    if (A != A_T).nnz == 0:
        return True

    edges = {(int(i), int(j)): float(w) for i, j, w in zip(A.row, A.col, A.data)}
    for (u, v), w in edges.items():
        if (v, u) not in edges:
            return False
        if abs(edges[(v, u)] - w) > EPS:
            return False
    return True


def findMTXFilesInDirs(directories):
    mtx_files = []
    for directory in directories:
        path = Path(directory)
        if not path.exists():
            print(f"No such directory: {directory}")
            continue
        for file in path.rglob("*.mtx"):
            mtx_files.append(file)
    return mtx_files


def main():
    with open("mtx_names.txt", "r", encoding="utf-8") as f:
        directories = [line.strip() for line in f if line.strip()]

    bad_matrices = []
    for path in findMTXFilesInDirs(directories):
        print(f"Checking {path} ...", end=" ")
        try:
            if isSymmetricMatrix(path):
                print("OK")
            else:
                print("Incorrect!")
                bad_matrices.append(path)
        except Exception as e:
            print(f"Error: ({e})")
            bad_matrices.append(path)

    print("\n========")
    if bad_matrices:
        print("Bad matrices:")
        for path in bad_matrices:
            print(" -", path)
    else:
        print("All matrices are correct.")

    input("\n\nPress ENTER to exit...")


if __name__ == "__main__":
    main()
