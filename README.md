# ForkerRenderer: CPU-Based Forward/Deferred Rasterizer, A Tiny OpenGL 🐼

Implement a CPU-based software rasterizer that mimics OpenGL behavior without using any third-party libraries, but use [spdlog](https://github.com/gabime/spdlog)
for logging though :)

![](https://raw.githubusercontent.com/junhaowww/StorageBaseWithoutCatNotice/main/ForkerRendererPic/ForkerRenderer_Head_3.jpg)

![](https://raw.githubusercontent.com/junhaowww/StorageBaseWithoutCatNotice/main/ForkerRendererPic/ForkerRenderer_Head_4_1.jpg)

## Building & Usage 🔨

```sh
# Logging
brew install spdlog  # for macOS

# Clone
git clone https://github.com/junhaowww/ForkerRenderer.git
cd ForkerRenderer 

# Compile
mkdir build && cd build
cmake .. && make

# Usage: <scene filename>
./ForkerRenderer ../scenes/test.scene
```

## Future Development 🥺

- Physically-Based Rendering (PBR)
  - Improvement
- Global Illumination
  - Screen Space Reflection (SSR) aka. Realtime Ray Tracing

## Features ⭐

![](https://raw.githubusercontent.com/junhaowww/StorageBaseWithoutCatNotice/main/ForkerRendererPic/ForkerRenderer_Deferred_2.jpg)

- [x] Rasterization
  - [x] Bresenham's Line Algorithm (used and removed)
  - [x] Bounding Box Method (currently used)
- [x] Rendering Methods
  - [x] Forward Rendering
  - [x] Deferred Rendering
    - G-Buffers: depth, world position, normal, albedo, etc
    - Geometry Pass
    - Lighting Pass

![](https://raw.githubusercontent.com/junhaowww/StorageBaseWithoutCatNotice/main/ForkerRendererPic/ForkerRenderer_PBR_2.jpg)

- [x] Shading
  - [x] Depth Shading
  - [x] Geometry Shading (for deferred rendering)
  - [x] Blinn-Phong Shading
  - [x] PBR Material and Shading
    - Cook-Torrance reflectance equation
    - D: Trowbridge-Reitz GGX
    - F: Fresnel-Schlick approximation
    - G: Smith's method with Schlick-GGX

![](https://bloggg-1254259681.cos.na-siliconvalley.myqcloud.com/bkh9v.png)

- [x] Parsing `*.obj` / `*.mtl`
    - `g` defines mesh name; `usemtl` defines m_Material name (comes in order)
    - [x] Support `Ka`, `Kd`, `Ks`, `Ke`, `map_Kd`, `map_Ks`, `map_Ke`, `map_Bump`, `map_Ao`, `map_Pr`, `map_Pm`
    - [x] Position vertex normalization
    - [x] Auto triangulation
- [x] Parsing scene files `*.scene`

```shell
# Test Scene
screen 1280 800
# forward/deferred rendering
mode deferred
ssaa off 2
shadow on
# Light (type: point/dir, position, color)
light point 2 5 5 1 1 1
# Camera (type: persp/ortho, position, lookAt)
camera persp -1 1 1 0 0 -1
# Models (filepath, position, rotate_y, uniform scale)
model obj/plane/plane.obj false false 0 -1 -1 0 3
# Mary
model obj/mary/mary.obj true true 0.05 0 -1 -10 1
```

- [x] Geometry Template Class
    - Vector: `Vector2f`, `Vector3f`, `Vector4f`, `Vector4i`, ...
    - Matrix: `Matrix3f`, `Matrix4f`, `Matrix3x4f`, ...
    - [x] Support glm-style vector swizzling, e.g. `v.xyz`, `v.xy`
- [x] Light: Point / Directional
  - AreaLight is defined by `AREA_LIGHT_SIZE` in shadow mapping)
- [x] Texture Mapping: 
  - Diffuse / Specular / Normal / Emissive
  - Roughness / Metalness / Ambient Occlusion
- [x] Texture Wrapping: NoWrap / ClampToEdge / Repeat / MirroredRepeat `Texture::WrapMode`
- [x] Texture Filtering: Nearest / Linear (Bilinear) `Texture::FilterMode`

![](https://raw.githubusercontent.com/junhaowww/StorageBaseWithoutCatNotice/main/ForkerRendererPic/ForkerRenderer_Wrap.jpeg)

- [x] Normal Transformation: TBN Matrix
  - Generate and average m_Tangents for each vertex when loading the model
- [x] Camera: Orthographic / Perspective Projection
- [x] Perspective Correct Interpolation (PCI) `#define PERSPECTIVE_CORRECT_INTERPOLATION`

![](https://raw.githubusercontent.com/junhaowww/StorageBaseWithoutCatNotice/main/ForkerRendererPic/ForkerRenderer_Shadow.jpg)

- [x] Shadow Effect
  - [x] Hard Shadow: Shadow Mapping (set `shadow on` in `test.scene` and comment out below macros)
  - [x] Soft Shadow:
    - [x] Percentage-Closer Filtering (PCF) `#define SOFT_SHADOW_PCF`
    - [x] Percentage-Closer Soft Shadow (PCSS) `#define SOFT_SHADOW_PCSS`
```cpp
#ifdef SOFT_SHADOW_PCF
    // Percentage-Closer Filtering (PCF)
    visibility = PCF(uShadowBuffer, shadowCoord, bias, PCF_FILTER_SIZE);
#elif defined(SOFT_SHADOW_PCSS)
    // Percentage-Closer Soft Shadow (PCSS)
    visibility = PCSS(uShadowBuffer, shadowCoord, bias);
#else
    // Hard Shadow
    visibility = HardShadow(uShadowBuffer, shadowCoord, bias);
#endif
```
- [x] Anti-Aliasing (AA)
  - [x] SSAA: Super Sampling Anti-Aliasing (set `ssaa on` in `test.scene`)
  - [ ] MSAA

![](https://raw.githubusercontent.com/junhaowww/StorageBaseWithoutCatNotice/main/ForkerRendererPic/ForkerRenderer_SSAO_2.jpg)

- [x] Global Illuminations
  - [x] Screen Space Ambient Occlusion (SSAO) `ssao on`
    - Range Check (courtesy of [John Chapman](http://john-chapman-graphics.blogspot.com/)
    - Noise Reduction: simple 9x9 blur, two-pass gaussian blur
  - [ ] Screen Space Reflection (SSR)
    - Aka. Screen Space Ray Tracing

## Gallery 🖼️

Apex Horizon (also [Dr. Mary Somers](https://www.ea.com/games/apex-legends/about/characters/horizon)) [Squral]

<img src="https://raw.githubusercontent.com/junhaowww/StorageBaseWithoutCatNotice/main/ForkerRendererPic/ForkerRenderer_Horizon.jpg" width="600">

Mary [TAs from [GAMES202: Real-time High Quality Rendering](https://sites.cs.ucsb.edu/~lingqi/teaching/games202.html)]

<img src="https://raw.githubusercontent.com/junhaowww/StorageBaseWithoutCatNotice/main/ForkerRendererPic/ForkerRenderer_Gallery_5.jpg" width="600">

Horizon Meets Mary

<img src="https://raw.githubusercontent.com/junhaowww/StorageBaseWithoutCatNotice/main/ForkerRendererPic/ForkerRenderer_Horizon_Meets_Mary_2.jpg" width="600">

Sci-Fi Welding Vehicle [Berk Gedik]

<img src="https://raw.githubusercontent.com/junhaowww/StorageBaseWithoutCatNotice/main/ForkerRendererPic/ForkerRenderer_Gallery_2.jpg" width="600">

Cafe Menu Chalkboard [Naiyararahman]

<img src="https://raw.githubusercontent.com/junhaowww/StorageBaseWithoutCatNotice/main/ForkerRendererPic/ForkerRenderer_Chalkboard.jpg" width="600">

Gossblade Greatsword (Monster Hunter Rise) [taj_tajima]

<img src="https://raw.githubusercontent.com/junhaowww/StorageBaseWithoutCatNotice/main/ForkerRendererPic/ForkerRenderer_GreatSword.jpg" width="600">

Backpack [Berk Gedik]

<img src="https://raw.githubusercontent.com/junhaowww/StorageBaseWithoutCatNotice/main/ForkerRendererPic/ForkerRenderer_Gallery_3.jpg" width="400">

African Head [Vidar Rapp]

<img src="https://raw.githubusercontent.com/junhaowww/StorageBaseWithoutCatNotice/main/ForkerRendererPic/ForkerRenderer_Gallery_4.jpg" width="400">


## Structure 📁

About **5,500** lines of code:

- Rendering: `render.h/cpp` (rendering functions), `forkergl.h/cpp` (rasterization)
- Buffer: `buffer.h/cpp` (`Buffer1f`, `Buffer3f`)
- Shader: `shader.h`, `phongshader.h`, `pbrshader.h`, `depthshader.h`, `gshader.h`
- Shadow: `shadow.h/cpp`
- Scene: `scene.h/cpp`, `scenes/test.scene`
- Model: `model.h/cpp`, `mesh.h/cpp`, `material.h`, `pbrmaterial.h`, `texture.h`
- Camera: `camera.h/cpp`
- Light: `light.h`
- Color: `color.h`
- Geometry: `geometry.h/cpp`
- Utility: `tgaimage.h/cpp`, `output.h/cpp`, `check.h`, `utility.h`, `constant.h`, `stringprint.h` (PBRT-v3)

## Console Output 📜

```console
$ ./ForkerRenderer scenes/test.scene
[info] Scene File: 'scenes/test.scene'
[info]   [Mode] Forward Rendering
[info]   [Screen] 1280 x 800
[info]   [Point Light] position: [ 2.00000000, 5.00000000, 5.00000000 ], color: [ 1.00000000, 1.00000000, 1.00000000 ]
[info]   [Camera] position: [ -1.00000000, 1.00000000, 1.00000000 ], lookAt: [ 0.00000000, 0.00000000, -1.00000000 ]
[info]   [Model] 'obj/diablo_pose/diablo_pose.obj'
[info]      v# 2519, f# 5022, vt# 3263, vn# 2519, tg# 2519, mesh# 1, mtl# 1 | normalized[o] generateTangent[o], flipTexCoordY[o]
[info]      [Diablo_Pose] f# 5022 | PBR[x] map_Kd[o] map_Ks[o] map_Ke[x] map_Bump[o] | Ka(0.10, 0.10, 0.10), Kd(0.81, 0.81, 0.81), Ks(0.20, 0.20, 0.20)
[info]   [Config] SSAA(x2)[off] shadow[on] SSAO[off]
[info] ------------------------------------------------------------
[info] Time Used: 0.714729 Seconds (Scene Loaded)

[info] Shadow Pass:
[info]   [Status] Enabled
[info]   [Diablo_Pose] Time Used: 0.106988 Seconds
[info] ------------------------------------------------------------
[info] Time Used: 0.858756 Seconds (Shadow Pass)

[info] Forward Pass (Blinn-Phong):
[info]   [Diablo_Pose] Time Used: 3.92819 Seconds
[info] ------------------------------------------------------------
[info] Time Used: 3.97755 Seconds (Forward Pass)

[info] Anti-Aliasing (SSAA):
[info]   [Status] Disabled
[info] ------------------------------------------------------------
[info] Time Used: 4.87400e-06 Seconds (Anti-Aliasing)

[info] Output TGA File: 'output/framebuffer.tga'
[info] Output TGA File: 'output/shadowmap.tga'
[info] Output TGA File: 'output/zbuffer.tga'
```

## Reference 📚

Started by following the basic workflow in _ssloy/TinyRenderer_, improved code style (e.g. `geometry.h`) when referring
to other great resources such as _mmp/pbrt-v3 & v4_, and resolved tons of issues with the help of the Internet. Thank those authors and creators who wrote the articles and built the models!

Rendering:

- [ssloy/TinyRenderer](https://github.com/ssloy/tinyrenderer)
- [mmp/pbrt-v3 & v4](https://github.com/mmp/)
- [lingqi/GAMES101](https://sites.cs.ucsb.edu/~lingqi/teaching/games101.html)
- [lingqi/GAMES202](https://sites.cs.ucsb.edu/~lingqi/teaching/games202.html)
- [Scratchapixel](https://www.scratchapixel.com/)
- [JoeyDeVries/LearnOpenGL](https://learnopengl.com/)
- [OpenGL Projection Matrix](http://www.songho.ca/opengl/gl_projectionmatrix.html)
- [The Normal Matrix](http://www.lighthouse3d.com/tutorials/glsl-12-tutorial/the-normal-matrix/)
- [Perspectively Correct Texture Mapping And color Interpolation](http://gamma.cs.unc.edu/courses/graphics-s09/Zaferakis/hw6/)
- [Converting quadrilaterals in an OBJ file into triangles?](https://stackoverflow.com/questions/23723993/converting-quadriladerals-in-an-obj-file-into-triangles)
- [OBJ FILE FORMAT](https://www.cs.cmu.edu/~mbz/personal/graphics/obj.html)
- [Object Files (.obj)](http://paulbourke.net/dataformats/obj/)
- More and more articles...

Model Credits:

- [Apex Horizon](https://ch.3dexport.com/3dmodel-horizon-apex-legends-321212.htm) by Squral
- [Sci-Fi Welding Vehicle](https://sketchfab.com/3d-models/sci-fi-welding-vehicle-40a215bf2244434a96f79684160e0e9f) by Berk Gedik
- [Backpack](https://sketchfab.com/3d-models/survival-guitar-backpack-low-poly-799f8c4511f84fab8c3f12887f7e6b36) by Berk Gedik
- [African Head](https://github.com/ssloy/tinyrenderer/) by Vidar Rapp
- [Diablo Pose](https://github.com/ssloy/tinyrenderer/) by Samuel (arshlevon) Sharit
- Mary by TAs in [GAMES202: Real-time High Quality Rendering](https://sites.cs.ucsb.edu/~lingqi/teaching/games202.html)
- [Cafe Menu Chalkboard](https://www.renderhub.com/naiyararahman/chalkboard) by Naiyararahman
- [Gossblade Greatsword](https://sketchfab.com/3d-models/gossblade-greatsword-e01b657292e749e69a953c9115ec115b) by taj_tajima
- Brickwall, Plane, Catbox (for debugging) by MYSELF! :)

