emcmake cmake .. -DEMSCRIPTEN=true -DBUILD_STATIC=true
emmake make -j
emcc -Wall -O3 --bind lib/libOPENFHEpke_static.a --bind lib/libOPENFHEcore_static.a -o "lib/pke_wasm.js" -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -s ENVIRONMENT="web" # for index.html
#emcc -Wall -O3 --bind lib/libOPENFHEpke_static.a --bind lib/libOPENFHEcore_static.a -o "lib/pke_wasm.js" -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -s ENVIRONMENT="web" -s SINGLE_FILE=1 -s MODULARIZE=1 -s EXPORT_ES6=1 # for react to read



