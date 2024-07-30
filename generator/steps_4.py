#!/usr/bin/env python3

C = 300000

k = 252

step = set()

cur = 0
l, r = 1, k
while l < r:
    cur += l
    step.add(cur)
    cur += r
    step.add(cur)
    r -= 1
    l += 1
cur += k
step.add(cur)

hole = []
for i in range(1, cur):
    if i not in step:
        hole.append(i)

cur += (C - (cur - len(hole)) * k) // k

print(len(hole), k, cur)
print(*hole)
print(*[i for i in range(1, k + 1)])
