import { test } from "node:test";
import * as assert from "node:assert";
import AoLoader from "@permaweb/ao-loader";
import { wasm, options, msg, env } from "./utils.js";

test("create sqlite db, run insert & select", async () => {
  const handle = await AoLoader(wasm("aos-sqlite"), options);

  const result1 = await handle(
    null,
    msg(`
    local sqlite3 = require("lsqlite3")
  
    db = sqlite3.open_memory()
    
    db:exec[[
      CREATE TABLE test (id INTEGER PRIMARY KEY, content);
      INSERT INTO test VALUES (NULL, 'Hello Lua');
      INSERT INTO test VALUES (NULL, 'Hello Sqlite3');
      INSERT INTO test VALUES (NULL, 'Hello ao!!!');
    ]]
    return "ok"
    `),
    env
  );
  console.log("result1:\n" + result1.Output?.data);
  assert.equal(result1.Output?.data, "ok");

  const result2 = await handle(
    result1.Memory,
    msg(`
  local s = ""

  for row in db:nrows("SELECT * FROM test") do
    s = s .. row.id .. ": " .. row.content .. "\\n"
  end

  return s
  `),
    env
  );
  console.log("\nresult2:\n" + result2.Output?.data);
  assert.equal(
    result2.Output?.data,
    "1: Hello Lua\n2: Hello Sqlite3\n3: Hello ao!!!\n"
  );
});
