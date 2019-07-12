# matrix_pthread
Check performance and processor affinity with multiple threads.

As only using multiple threads don't necessarily means _improve performance_, this naive test was made to see this behavior.

Using a dummy task as multiply to matrices (by triple for approach), where each thread must cope with a couple of rows (_row split_ rather than _col split_ due cache affinity), we can see that more thread don't mean _less time_ as it may seem (in a first naive glance).

When aiming to achieve _less time_, other approaches must be taken into account, as CPU affinity.

# Authors
Luiz Eduardo Pizzinatto

# Execution
Compile as
```
g++ matrix.cc -o matrix
```

Execute as
```
./matrix <matrix_size> <num_iterations> <num_threads>
./matrix 500 10 4
```


# Examples
```
[matrix]$ ./matrix 500 10 1

 Avg time for [500 x 500] matrix multiplication in 10 iterations (each one with 1 threads) is: 0.79802010000000001000 seconds
```

```
[matrix]$ ./matrix 500 10 2

 Avg time for [500 x 500] matrix multiplication in 10 iterations (each one with 2 threads) is: 0.82497299999999995634 seconds
```

```
[matrix]$ ./matrix 500 10 4

 Avg time for [500 x 500] matrix multiplication in 10 iterations (each one with 4 threads) is: 1.47443509999999999849 seconds
```

Specifically for this problems, there's no gain in _time_ using more threads.

# Infos
CPU time measure was made with `clock()` calls.
