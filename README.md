
# Glim

Glim is an imperative programming language with functional elements.\
It was developed from scratch using C and employs techniques like recursive descent parsing, a stack-based virtual machine, and automatic memory management via reference counting.

> **Note:** This project is in the early stages of development and is not yet suitable for production use.

## What does the code look like?

```rs
let fib = n => n <= 1 ? n : fib(n - 1) + fib(n - 2);
fib(10)
```

## Key Features

Glim offers the following features:

- Cross-Platform
- Modern C-like syntax
- Dynamic Typing
- Mutable Value Semantics
- Automatic Reference Counting

## Building the project

This project uses [CMake](https://cmake.org) for building. A build script is provided for convenience. To build the project, simply run the following command:

```
./build.sh
```

## Running tests

To run the tests:

```
./test.sh
```

## Cleaning up

To clean the build artifacts, run:

```
./clean.sh
```

## Grammar specification

You can find the current grammar definition in the [docs/glim.ebnf](docs/glim.ebnf) file.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
