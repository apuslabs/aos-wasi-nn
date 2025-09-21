# AOS WASI-NN

`aos-wasi-nn` is an `ao` module that brings AI inference capabilities to `ao` processes through the WebAssembly System Interface (WASI) for Neural Networks (wasi-nn).

This project enables loading machine learning models and running inference directly within `ao`, leveraging modules for different backends like `llama.cpp` and `sqlite`.

## Features

-   **WASI-NN on `ao`**: Implements the `wasi-nn` API for `ao` processes.
-   **Multiple Backends**: Includes support for different inference backends.
    -   `ao-llama`: A module for running `llama.cpp` models.
    -   `ao-sqlite`: A module providing SQLite database functionality.
-   **Docker-based Build**: Uses a Docker container for a consistent build environment.
-   **Extensible**: Designed to be extended with new backends and models.

## Building the Project

The project uses a `Makefile` to automate the build process using Docker.

### Prerequisites

-   Docker

### Build Steps

You can build specific modules or all of them.

-   **Build `ao-llama` module**:
    ```sh
    make build-ao-llama
    ```

-   **Build `ao-sqlite` module**:
    ```sh
    make build-sqlite
    ```

-   **Build `ao-wasi-nn` module**:
    ```sh
    make build-aos-wasi-nn
    ```

The build process compiles the necessary C/C++ code into WebAssembly, links it into shared libraries, and finally builds the `ao` process `process.wasm`.

## Testing

The project includes a suite of tests to verify the functionality of the different modules.

To run the tests, you will need Node.js installed. First, install dependencies:

```sh
cd tests
npm install
```

Then, run the tests:

```sh
npm test
```