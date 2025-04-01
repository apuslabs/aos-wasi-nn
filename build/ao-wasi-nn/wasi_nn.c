#include "wasi_nn_types.h"
#include "wasi_nn_api.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "wasi_nn.h"

typedef struct GraphBuilder {
    graph_encoding encoding;
    execution_target target;
    const char *config; // optional configuration, nullable
} GraphBuilder;

GraphBuilder
graph_builder_new(graph_encoding en, execution_target target)
{
    GraphBuilder builder = {
        .encoding = en,
        .target = target,
        .config = NULL
    };
    return builder;
}

GraphBuilder*
graph_builder_config(GraphBuilder *builder, const char *config)
{
    builder->config = config;
    return builder;
}

typedef struct Graph {
    graph handle;
    GraphBuilder build_info;
} Graph;

wasi_nn_error
graph_build_from_cache(GraphBuilder *builder, const char *name, Graph *out_graph)
{
    wasi_nn_error err;
    uint32_t name_len = (uint32_t)strlen(name);

    if (builder->config) {
        uint32_t config_len = (uint32_t)strlen(builder->config);
        err = load_by_name_with_config(name, name_len, builder->config, config_len, &out_graph->handle);
    } else {
        err = load_by_name(name, name_len, &out_graph->handle);
    }

    if (err != success) {
        fprintf(stderr, "Failed to build from cache: %d\n", err);
        return err;
    }

    out_graph->build_info = *builder;
    return success;
}

wasi_nn_error
graph_init_execution_context(const Graph *graph, graph_execution_context ctx)
{
    return init_execution_context(graph->handle, &ctx);
}

wasi_nn_error
graph_set_input(graph_execution_context ctx, uint32_t index, const char *data)
{
    uint32_t dimsbuf[1] = { 1 };
    tensor_dimensions_wasm dims;
    dims.buf_offset = (uint32_t)dimsbuf;
    dims.size = 1;

    tensor_wasm tensor;
    tensor.dimensions = dims;
    tensor.type = 4;
    tensor.data_offset = (uint32_t)data;
    tensor.data_size = strlen(data);
    wasi_nn_error err = set_input(ctx, 0, &tensor);

    return err;
}

wasi_nn_error
graph_compute(graph_execution_context ctx)
{
    return compute(ctx);
}

wasi_nn_error
graph_get_output(graph_execution_context ctx, uint32_t index,
                 void *output_data, uint32_t *output_data_size)
{
    return get_output(ctx, index, (tensor_data)output_data, output_data_size);
}

#define MAX_OUTPUT_BUFFER_SIZE (4096 * 6)

wasi_nn_error
set_prompt_input(graph_execution_context ctx, const char* prompt)
{
    uint32_t dims[1] = { 1 };
    return graph_set_input(ctx, 0, prompt);
}

char*
get_result_output(graph_execution_context ctx, uint32_t index)
{
    uint8_t *output_buffer = (uint8_t*)malloc(MAX_OUTPUT_BUFFER_SIZE);
    if (output_buffer == NULL) {
        fprintf(stderr, "malloc failed for output_buffer\n");
        return NULL;
    }
    uint32_t *output_size;
    wasi_nn_error err = graph_get_output(ctx, index, output_buffer, output_size);
    if (err != success) {
        fprintf(stderr, "graph_get_output failed with error: %d\n", err);
        free(output_buffer);
        return NULL;
    }
    return (char*)output_buffer;
}

char*
lib_run_inference(const char* model_path, const char* input_prompt)
{
    wasi_nn_error err;
    GraphBuilder builder = graph_builder_new(ggml, gpu);
    Graph graph;
    err = graph_build_from_cache(&builder, model_path, &graph);
    if (err != success) {
        fprintf(stderr, "graph_build_from_cache failed: %d\n", err);
        return NULL;
    }
    graph_execution_context ctx;
    err = init_execution_context(graph.handle, &ctx);
    if (err != success) {
        fprintf(stderr, "init_execution_context failed: %d\n", err);
        return NULL;
    }
    char full_prompt[4096];
    snprintf(full_prompt, sizeof(full_prompt),
             "<|im_start|>system\nYou are a helpful assistant.<|im_end|>\n\n"
             "<|im_start|>user\n%s<|im_end|>\n<|im_start|>assistant\n",
             input_prompt);
    err = set_prompt_input(ctx, full_prompt);
    if (err != success) {
        fprintf(stderr, "set_prompt_input failed: %d\n", err);
        return NULL;
    }
    err = graph_compute(ctx);
    if (err != success) {
        if (err == context_full) {
            printf("[INFO] context full, reset required.\n");
        } else if (err == prompt_tool_long) {
            printf("[INFO] prompt too long, reset required.\n");
        } else if (err == model_not_found) {
            printf("[INFO] model not found, reset required.\n");
        } else {
            fprintf(stderr, "[ERROR] graph_compute failed: %d\n", err);
            return NULL;
        }
    }
    char *result_string = get_result_output(ctx, 0);
    if (result_string == NULL) {
        fprintf(stderr, "get_result_output failed\n");
        return NULL;
    } else {
        printf("Model inference returned: %s\n", result_string);
    }
    // char *result_string2 = get_result_output(ctx, 1);
    // if (result_string2 == NULL) {
    //     fprintf(stderr, "get_result_output failed\n");
    //     return NULL;
    // } else {
    //     printf("Model inference params returned: %s\n", result_string2);
    // }
    return result_string;
}

int lib_main()
{
    char *prompt = "What is the meaning of life?";
    wasi_nn_error result = run_inference(prompt, strlen(prompt));
    printf("Result: %d\n", result);
    return 0;
}

// uint16_t __wasi_clock_time_get(uint32_t clock_id, uint64_t precision, uint64_t *time) {
//     *time = 0;
//     return 0;
// }