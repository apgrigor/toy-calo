# Calorimeter toy model

I wrote this code as part of the Computational Physics course at the National and Kapodistrian University of Athens.

In `src/model` is the simplistic toy model of the calorimeter.

In `src/analysis` are attempts to discriminate between e/γ objects and hadrons.

This project uses [xmake](https://xmake.io/#/) as the build system, [clang](https://clang.llvm.org/) as the compiler (though this is trivial to change) and relies on [CERN ROOT](https://root.cern/) to be built. The build script `xmake.lua` can only work on Unix systems and I've only ran it on Linux.

To build it, simply run
```
xmake
```
in the project directory. Then, to run the toy model:
```
xmake run toy
```
Or the analysis
```
xmake run an cuts
```
for a simple cut-based discrimination, and
```
xmake run an bdts
```
for a BDT-based discrimination.

Various Python scripts were made for auxillary plots etc., in `scripts`.

You can compile the presentation using [typst](https://github.com/typst/typst) like so:
```
cd doc
typst compile presentation.typ --root ..
```
