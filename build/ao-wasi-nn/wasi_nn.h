#ifndef WASI_NN_API_H
#define WASI_NN_API_H

#include "wasi_nn.h"
#include "wasi_nn_types.h"
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Graph builder structure for configuring neural network graph
 */
typedef struct GraphBuilder GraphBuilder;

/**
 * @brief Graph structure containing handle and build info
 */
typedef struct Graph Graph;

/**
 * @brief Create a new graph builder
 * 
 * @param encoding The graph encoding format
 * @param target The execution target
 * @return GraphBuilder initialized builder
 */
GraphBuilder graph_builder_new(graph_encoding encoding, execution_target target);

/**
 * @brief Set configuration for a graph builder
 * 
 * @param builder Pointer to the builder to configure
 * @param config Configuration string
 * @return GraphBuilder* Pointer to the modified builder
 */
GraphBuilder* graph_builder_config(GraphBuilder *builder, const char *config);

/**
 * @brief Build a graph from cache using a name
 * 
 * @param builder Pointer to the builder with configuration
 * @param name Cache name for the model
 * @param out_graph Pointer to store the built graph
 * @return wasi_nn_error Error code
 */
wasi_nn_error graph_build_from_cache(GraphBuilder *builder, const char *name, Graph *out_graph);

/**
 * @brief Initialize execution context for a graph
 * 
 * @param graph Pointer to the graph
 * @param ctx Execution context to initialize
 * @return wasi_nn_error Error code
 */
wasi_nn_error graph_init_execution_context(const Graph *graph, graph_execution_context ctx);

/**
 * @brief Set input tensor for execution
 * 
 * @param ctx Execution context
 * @param index Input tensor index
 * @param type Tensor data type
 * @param dims Array of dimensions
 * @param dims_count Number of dimensions
 * @param data Pointer to input data
 * @param data_bytes Size of input data in bytes
 * @return wasi_nn_error Error code
 */
wasi_nn_error graph_set_input(graph_execution_context ctx, uint32_t index,
                             tensor_type type, const uint32_t *dims, uint32_t dims_count,
                             const void *data, size_t data_bytes);

/**
 * @brief Compute inference using the execution context
 * 
 * @param ctx Execution context
 * @return wasi_nn_error Error code
 */
wasi_nn_error graph_compute(graph_execution_context ctx);

/**
 * @brief Get output tensor from execution
 * 
 * @param ctx Execution context
 * @param index Output tensor index
 * @param output_data Buffer to store output data
 * @param output_data_size Pointer to size of buffer (in/out parameter)
 * @return wasi_nn_error Error code
 */
wasi_nn_error graph_get_output(graph_execution_context ctx, uint32_t index,
                              void *output_data, uint32_t *output_data_size);

/**
 * @brief Unload a graph and free resources
 * 
 * @param graph Pointer to the graph to unload
 * @return wasi_nn_error Error code
 */
wasi_nn_error graph_unload(Graph *graph);

/**
 * @brief Set a text prompt as input to the model
 * 
 * @param ctx Execution context
 * @param prompt Text prompt
 * @return wasi_nn_error Error code
 */
wasi_nn_error set_prompt_input(graph_execution_context ctx, const char* prompt);

/**
 * @brief Get output as a string
 * 
 * @param ctx Execution context
 * @param index Output tensor index
 * @return char* Result string (caller must free)
 */
char* get_result_output(graph_execution_context ctx, uint32_t index);

/**
 * @brief Run inference on a model with a prompt
 * 
 * @param model_path Path to the model
 * @param input_prompt Input prompt for inference
 * @return char* Result string (caller must free)
 */
char* run_inference(const char* model_path, const char* input_prompt);

#ifdef __cplusplus
}
#endif

#endif /* WASI_NN_API_H */
