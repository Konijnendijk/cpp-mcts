# C++ MCTS

C++ MTCS is a header-only framework for the Monte Carlo Tree Search algorithm. By implementing a
few classes, the MCTS framework makes it possible to use MCTS in a custom game.

## MCTS
Monte Carlo Tree Search is an algorithm for making decisions by searching state
spaces heuristically. For more information on the algorithm consult the following resources:
* Coulom, R. (2007). Efficient selectivity and backup operators in Monte-Carlo tree search.
  Computers and Games (CG 2006) (eds. P. Ciancarini and H.J. van den Herik), Vol. 4630 of
  LNCS, pp. 72–83, Springer-Verlag, Berlin, Germany.
* Nijssen, J.A.M. and Winands, M.H.M (2011). Enhancements for Multi-Player Monte-Carlo
  Tree Search. Computers and Games (CG 2010) (eds. H.J. van den Herik, H. Iida, and A.
  Plaat), Vol. 6515 of LNCS, pp. 238–249, Springer-Verlag, Berlin, Germany.


## Using the framework

In order to use the framework the classes `ExpansionStrategy`, `PlayoutStrategy`, `Scoring`,
`Backpropagation` and `TerminationCheck` need to be implemented. See the documentation for
details on implementing those. After implementing, create an instance of `MCTS` and call
`MCTS::calculateAction()`.

## Documentation

In order to generate the documentation get [Doxygen](http://www.doxygen.org) and run
`doxygen Doxyfile` in the project root directory.

## Prerequisites

* A C++11 compiler
* Qt5 (only when building the samples)

## Building
No building is required for the main library, simply put the `mcts.h` file in your project.
When using CMake you can also put this entire project in your project, call `add_subdirectory`
and add `target_link_libraries(target CPP_MCTS)` to your `CMakeLists.txt`.

To build the samples, make sure the `CPP_MCTS_BUILD_SAMPLES` is `ON` and that you have
installed Qt5. Run CMake in the project root directory
(e.g. `cmake -G"Unix Makefiles" -DCPP_MCTS_BUILD_SAMPLES=ON .`) or out of source
(e.g. `cmake -G"Unix Makefiles" -DCPP_MCTS_BUILD_SAMPLES=ON /path/to/source`).
And run `make` and `make install`.