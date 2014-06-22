![](https://travis-ci.org/StuartHa/Calculator-Scanner.svg?branch=master)

My attempt at implementing an ad-hoc scanner for a calculator language outlined in Chapter 2 of Programming Language Pragmatics (By Michael Scott)

Features
---
+ Support for arbitrarily long lines
+ Small memory footprint -- only keeps single line in memory
+ When unrecognized token encountered, line and column number are printed
+ Unit tests run on pushes via [Travis](https://travis-ci.org/StuartHa/Calculator-Scanner) and Google C++ testing framework 

TODO
---
+ Add better test coverage
+ Integrate preprocessor into compiler pipeline
