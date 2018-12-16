# Searching-in-Semi-Ordered-Matrix
Given a semi-ordered matrix M of numbers designed a PRAM algorithm for finding (the position of) a given number K in M.
Implemented the parallel algorithm using OpenMP in C++.

Measured the performance for different sizes of M in the range 103 x 103 to 105 x 105. For each size measured the performance for p = 1, 2, .. 2q where p is the number of cores used.
