## OpenFHE + embind

OpenFHE to wasm

### build

```
mkdir build
cp index.html build/
cp test.js build/
cp embind_build.sh build/
cd build
sh embind_build.sh
```


### run html + js

```
python -m http.server 8000
```

then open localhost:8000 and developer console
