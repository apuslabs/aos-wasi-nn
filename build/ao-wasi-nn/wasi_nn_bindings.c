#include <stdlib.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "wasi_nn.h"

extern lua_State *wasm_lua_state;

// Wrapper function for run_inference
static int l_wasi_nn_run_inference(lua_State *L) {
    const char *model_path = luaL_checkstring(L, 1);
    const char *input_prompt = luaL_checkstring(L, 2);
    
    char *result = run_inference(model_path, input_prompt);
    
    if (result != NULL) {
        lua_pushstring(L, result);
        free(result); // Free the allocated memory for result
        return 1;
    } else {
        lua_pushnil(L);
        lua_pushstring(L, "Inference failed");
        return 2;
    }
}

// Register the module
int luaopen_wasinn(lua_State *L) {
    static const luaL_Reg wasi_nn_funcs[] = {
        {"run_inference", l_wasi_nn_run_inference},
        {NULL, NULL}  // Sentinel to indicate end of array
    };

    luaL_newlib(L, wasi_nn_funcs); // Create a new table and push the library function
    return 1;
}
