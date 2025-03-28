/*
 * Copyright (C) 2019 Intel Corporation.  All rights reserved.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

/**
 * Following definition from:
 * [Oct 25th, 2022]
 * https://github.com/WebAssembly/wasi-nn/blob/0f77c48ec195748990ff67928a4b3eef5f16c2de/wasi-nn.wit.md
 */

#ifndef WASI_NN_H
#define WASI_NN_H

#include <stdint.h>
#include "wasi_nn_types.h"

/**
 * @brief Load an opaque sequence of bytes to use for inference.
 *
 * @param builder   Model builder.
 * @param encoding  Model encoding.
 * @param target    Execution target.
 * @param g         Graph.
 * @return wasi_nn_error    Execution status.
 */
__attribute__((import_module("wasi_ephemeral_nn")))
wasi_nn_error
load(graph_builder_array *builder, graph_encoding encoding,
     execution_target target, graph *g);

__attribute__((import_module("wasi_ephemeral_nn")))
wasi_nn_error
load_by_name(const char *filename, uint32_t filename_len, graph *g);

__attribute__((import_module("wasi_ephemeral_nn")))
wasi_nn_error
load_by_name_with_config(const char *filename, uint32_t filename_len,
                         const char *config, uint32_t config_size, graph *g);

/**
 * INFERENCE
 *
 */

/**
 * @brief Create an execution instance of a loaded graph.
 *
 * @param g         Graph.
 * @param ctx       Execution context.
 * @return wasi_nn_error    Execution status.
 */
__attribute__((import_module("wasi_ephemeral_nn")))
wasi_nn_error
init_execution_context(graph g, graph_execution_context *ctx);

/**
 * @brief Define the inputs to use for inference.
 *
 * @param ctx       Execution context.
 * @param index     Input tensor index.
 * @param tensor    Input tensor.
 * @return wasi_nn_error    Execution status.
 */
__attribute__((import_module("wasi_ephemeral_nn")))
wasi_nn_error
set_input(graph_execution_context ctx, uint32_t index, tensor *tensor);

/**
 * @brief Compute the inference on the given inputs.
 *
 * @param ctx       Execution context.
 * @return wasi_nn_error    Execution status.
 */
__attribute__((import_module("wasi_ephemeral_nn")))
wasi_nn_error
compute(graph_execution_context ctx) __attribute__((import_module("wasi_ephemeral_nn")));

/**
 * @brief Extract the outputs after inference.
 *
 * @param ctx                   Execution context.
 * @param index                 Output tensor index.
 * @param output_tensor         Buffer where output tensor with index `index` is
 * copied.
 * @param output_tensor_size    Pointer to `output_tensor` maximum size.
 *                              After the function call it is updated with the
 * copied number of bytes.
 * @return wasi_nn_error                Execution status.
 */
__attribute__((import_module("wasi_ephemeral_nn")))
wasi_nn_error
get_output(graph_execution_context ctx, uint32_t index,
           tensor_data output_tensor, uint32_t *output_tensor_size);

#endif
