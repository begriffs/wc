### Beating haskell with C

An attemp to measure a simple, a multithreaded, and a unicode-aware version of
wc written in C compared with the optimized Haskell implementation in
[ChrisPenner/wc](https://github.com/ChrisPenner/wc).

Unicode version is forthcoming, but will be a cleaned up version of
https://begriffs.com/posts/2019-05-23-unicode-icu.html#counting-words-and-graphemes

### To run

The fastest version is in threads.c.

```sh
make
./threads N /path/to/file

# where N is the number of threads to use
```
