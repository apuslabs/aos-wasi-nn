import { test } from "node:test";
import * as assert from "node:assert";
import AoLoader from "@permaweb/ao-loader";
import { Model, wasm, options, msg, env } from "./utils.js";

const modelPath = `/home/jax/work/WasmEdge-WASINN-examples/wasmedge-ggml/qwen/src/qwen1_5-0_5b-chat-q2_k.gguf`
const prompt = `Hello, how are you?`;

test("Execute aos-wasi-nn", async () => {
  // Initialize AoLoader with the wasm and options
  const handle = await AoLoader(wasm("aos-wasi-nn"), options);
  let result = await handle(
    null,
    msg(`
      local nn = require("wasinn")
      io.stderr:write([[Loading model...\n]])
      local result = nn.run_inference("${modelPath}", "${prompt}")
      print(result)
      return "OK"
    `),
    env
  );
  console.log(result.Error);
  console.log("\n\n", result.Output.data);

  // Assert that execution times are collected
  assert.notEqual(result.Output.data, undefined);
});
