# Ray Tracer

## Building

Just run `make` to build the renderer.

## Usage

The current input file format is as follows:

```
SPHERE name x y z radius surface\_r surface\_g surface\_b reflection refraction\_index emission\_r emission\_g emission\_b
0 START name
```

So, for example:

```
SPHERE sun 0 20 -30 3 0 0 0 0 0 3 3 3

SPHERE object1 20 0 -40 4 1.00 0.30 0.36 1.0 0.5 0 0 0

0 START sun
0 START object1
```

Note that this imput format is subject to change very, very, very, very, very freely and rapidly.

To use the input, place it in the file in some empty directory, and run the program with that directory as the sole argument (if you place the file in `default/`, you do not need to pass it as an argument).  Then just run the renderer:

```
./renderer > output.ppm
```

## Documentation

The `doc/` directory has a Doxyfile that can be used to generate the documentation for the source.

Well, most of it.  I've yet to figure out how to make Doxygen parse the documentation in the header files for functions.  If you know how, please drop me a line!
