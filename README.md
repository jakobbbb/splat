# splat

a simple and not very optimized Gaussian splat renderer

![spat](https://gist.githubusercontent.com/jakobbbb/07b118dc8e9ede626dae1d4283e72360/raw/d86aef155ae754af87464c3ca70205a394cf3f7c/bike.webp)

## building & running

```
mkdir build
cd build
cmake ..
make spalt
./src/splat /path/to/ply
```

## controls

Use `W` `A` `S` `D`, hold down right mouse button to look around.

Press `P` for point rendering mode and `G` to render Gaussians.

Press `Shift` to move faster.  The default movement speed can be adjusted with the mouse wheel and reset with `.`.

Sorting is triggered manually by pressing `C`.
