#include "wasi_nn_types.h"
#include "wasi_nn_api.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

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
graph_set_input(graph_execution_context ctx, uint32_t index,
                tensor_type type, const uint32_t *dims, uint32_t dims_count,
                const void *data, size_t data_bytes)
{
    tensor_dimensions dimensions = {
        .buf = (uint32_t *)malloc(sizeof(uint32_t) * dims_count),
        .size = dims_count
    };
    if (!dimensions.buf)
        return too_large;

    memcpy(dimensions.buf, dims, dims_count * sizeof(uint32_t));

    tensor input_tensor = {
        .dimensions = &dimensions,
        .type = type,
        .data = (uint8_t*)data
    };

    wasi_nn_error err = set_input(ctx, index, &input_tensor);

    free(dimensions.buf);
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
    return graph_set_input(ctx, 0, u8, dims, 1, (const uint8_t*)prompt, strlen(prompt));
}

char*
get_result_output(graph_execution_context ctx, uint32_t index)
{
    uint8_t *output_buffer = (uint8_t*)malloc(MAX_OUTPUT_BUFFER_SIZE);
    if (output_buffer == NULL) {
        fprintf(stderr, "malloc failed for output_buffer\n");
        return NULL;
    }
    uint32_t output_size = MAX_OUTPUT_BUFFER_SIZE;
    wasi_nn_error err = graph_get_output(ctx, index, output_buffer, &output_size);
    if (err != success) {
        fprintf(stderr, "graph_get_output failed with error: %d\n", err);
        free(output_buffer);
        return NULL;
    }
    if (output_size >= MAX_OUTPUT_BUFFER_SIZE)
        output_size = MAX_OUTPUT_BUFFER_SIZE - 1;
    output_buffer[output_size] = '\0';
    return (char*)output_buffer;
}

char*
run_inference(const char* model_path, const char* input_prompt)
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
    return result_string;
}