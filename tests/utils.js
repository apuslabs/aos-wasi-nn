import fs from "fs";
import weaveDrive from "./weavedrive.js";
export const Model = "ISrbGzQot05rs_HKC08O_SmkipYQnqgB1yC3mjZZeEo";
export const wasm = (name) => fs.readFileSync(`./${name}.wasm`);
export const options = {
  format: "wasm64-unknown-emscripten-draft_2024_02_15",
  admissableList: [Model],
  WeaveDrive: weaveDrive,
  ARWEAVE: "http://localhost:3001",
  mode: "test",
  blockHeight: 100,
  spawn: {
    Scheduler: "TEST_SCHED_ADDR",
  },
  Process: {
    Id: "AOS",
    Owner: "FOOBAR",
    tags: [{ name: "Extension", value: "Weave-Drive" }],
  },
};
export const env = {
  Process: {
    Id: "1",
    Owner: "FOOBAR",
    Tags: [{ name: "Name", value: "TEST_PROCESS_OWNER" }],
  },
};
export const msg = (Data, Action = "Eval") => ({
  Target: "1",
  From: "FOOBAR",
  Owner: "FOOBAR",
  Module: "FOO",
  Id: "1",
  "Block-Height": "1000",
  Timestamp: Date.now(),
  Tags: [{ name: "Action", value: Action }],
  Data,
});
