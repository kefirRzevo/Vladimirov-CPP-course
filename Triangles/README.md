# Triangles
This is gonna be my solution of 'Triangles cross' task. On the input we have number of triangles in 3d Euclidean  space.

## Build
To build program:
```
        git clone https://github.com/kefirRzevo/Vladimirov-CPP-course.git
        cd Vladimirov-CPP-course/Triangles
        mkdir build
        cd build
        cmake ../
        cmake --build ./
```

## Triangles test
This algorithm erases nodes when they live cache(Frequency Counter is 0 after that).
To run LFU tests:
```
        ./lfu_test
```

## Ideal Cache Tests
This is 2 pass algorithm that works for O(N*log(M)), where N is number of elements need to be put in cache and M is size of cache.
To run Ideal tests:
```
        ./ideal_test
```

---
In Cache.cpp you can see number of hits in both 2 caches.
You can run
```
        ./cache 2 6 1 2 1 2 1 2 (or using stdin)
```
To compare these two algorithms. 
Enjoy.