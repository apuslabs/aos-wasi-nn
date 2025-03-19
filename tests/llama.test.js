import { test } from "node:test";
import * as assert from "node:assert";
import AoLoader from "@permaweb/ao-loader";
import { Model, wasm, options, msg, env } from "./utils.js";

const prompt = `Hello, how are you?`;

test("Execute aos-llama", async () => {
  // Initialize AoLoader with the wasm and options
  const handle = await AoLoader(wasm("aos-llama"), options);
  let result = await handle(
    null,
    msg(`
      local Llama = require("llama")
      io.stderr:write([[Loading model...\n]])
      local result = Llama.load("/data/${Model}")
      Llama.set_prompt([[${prompt}]])
      io.stderr:write("Prompt set! Running...\\n")
      local str = Llama.run(30)
      print(str)
      return "OK"
    `),
    env
  );
  console.log("\n\n", result.Output.data);

  // Assert that execution times are collected
  assert.notEqual(result.Output.data, undefined);
});
