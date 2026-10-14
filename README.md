# Word hash tables

Two C++ programs count words in `LittleWomen.txt` with chained hash tables and write one CSV per table size.

`main.cpp` hashes each word as `h = h * 31 + ch` into tables of size 500, 1000, 2000, 5000, and 10000. Each bucket is a chain. For every word it records how many times it appears and how many steps the chain walks take.

`experiment.cpp` uses the same tables. After a bucket records more than 5 collisions it switches to a second hash (`h = h * 33 + ch`, starting from 5381) and rehashes. It then prints the collision count for every bucket.

## Run

Both programs open `LittleWomen.txt` from the working directory. The copy in this repo is `output/LittleWomen.txt`.

```
g++ -std=c++17 -o main main.cpp
cp output/LittleWomen.txt .
./main
```

```
g++ -std=c++17 -o experiment experiment.cpp
./experiment
```

Each run writes `WordStats_TableSize_<size>.csv` in the working directory. Columns are `Word`, `Occurrences`, and `Steps`.
