# Engine

### How It Looks Like

![ScreenShot](https://github.com/furkansarihan/Engine/blob/master/en-ss.png)

### Must Watch It In Action

https://www.youtube.com/watch?v=N4iF1Vxs5t4


Engine is a 3D game engine.
  
  - Immediate mod graphical user interface
  - Level desing tool

### Scene File

  - Engine scenes can be saved as json format, example scene which includes an object;
```json
{
  "version": "0.0.1",
  "engine_mode": 0,
  "gameobjects": [
    {
      "type": "model",
      "name": "none",
      "has_collider": true,
      "collider_type": 0,
      "mass": 1,
      "scale_factor": [
        1,
        -1,
        15
      ],
      "position": [
        0,
        18,
        0
      ],
      "rotation": [
        0,
        0,
        0
      ],
      "model_path": "assets/models/primitive/cube.obj",
      "shader_type": "textured",
      "custom_vertex_shader_path": "none",
      "custom_fragment_shader_path": "none",
      "light_type": "none",
      "color": [
        0.30000001192092896,
        0.1599999964237213,
        1,
        1
      ]
    }
  ]
}
```

### Included Libraries

Engine uses open source libraries to look good.

* assimp - The Open-Asset-Importer-Lib - http://www.assimp.org/
* bullet - Real-Time Physics Simulation - https://pybullet.org
* glew - The OpenGL Extension Wrangler Library - http://glew.sourceforge.net/
* glfw - A simple API for creating windows, contexts and surfaces, receiving input and events. - https://www.glfw.org/
* glm - OpenGL Mathematics  - https://glm.g-truc.net/0.9.9/index.html
* dear imgui - Bloat-free Immediate Mode Graphical User interface - https://github.com/ocornut/imgui

### Installation Dependencies
- Working Windows 10 and Visual Studio 2017
- Just open Engine.sln

**Free Software**