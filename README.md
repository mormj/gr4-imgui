# gr4-imgui

This is a sandbox/demonstration of GNU Radio with imgui/implot

## Building
```bash
git clone https://github.com/mormj/gr4-imgui
cd gr4-imgui
meson setup build --prefix=$GR_PREFIX --libdir=lib
cd build
ninja 
ninja install
./apps/demo_imgui
```