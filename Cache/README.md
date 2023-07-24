# Cache
Simple LFU Cache model and Ideal Cache to compare how good it works.

## Build
    git clone https://github.com/kefirRzevo/Vladimirov-CPP-course.git
    cd Vladimirov-CPP-course/Cache
    make
    ./cache

Here in Cache.cpp you can see number of hits in both 2 caches.

## LFU Cache Tests
This algorithm erases nodes when they live cache(Frequency Counter is 0 after that).
To run LFU tests:
        make test_lfu
        ./cache_testLFU

## Ideal Cache Tests
This is 2 pass algorithm that works for O(Nlog(M)), where N is number of elements need to be put in cache and M is size of cache.
To run Ideal tests:
        make test_ideal
        ./cache_testIdeal
