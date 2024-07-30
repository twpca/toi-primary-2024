#!/usr/bin/env python3

import sys
from typing import List


def generate_fixed_k(k: int, limit_e: int):
    step = set()

    cur = 0
    for i in range(1, k):
        cur += i
        step.add(cur)
        if i != k - 1:
            cur += k
        step.add(cur)

    hole = []
    for i in range(1, cur):
        if i not in step:
            hole.append(i)

    if (cur - len(hole)) * len(step) > limit_e:
        return None

    return cur, step, hole


def main(args: List[str]):
    if len(args) < 2:
        raise ValueError(f"len(args) < 2 {args=}")

    k = 2
    limit_e = int(args[1])
    while True:
        result = generate_fixed_k(k * 2, limit_e)
        if result is None:
            break
        k *= 2

    bit = k // 2
    while bit != 0:
        result = generate_fixed_k(k + bit, limit_e)
        if result is not None:
            k += bit
        bit //= 2

    result = generate_fixed_k(k, limit_e)
    assert result is not None

    cur, step, hole = result

    print(len(hole), k, cur)
    print(*hole)
    print(*[i for i in range(1, k + 1)])


if __name__ == "__main__":
    main(sys.argv)
