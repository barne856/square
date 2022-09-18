# LIT - Layer Iterator Rendering Engine (name may change)

```mermaid
classDiagram
    class render_system {
      <<abstract, template>>
      render(dt, entity)
    }

    class controls_system {
      <<abstract, template>>
    on_key(event, entity)
    on_mouse_button(event, entity)
    on_mouse_move(event, entity)
    on_mouse_wheel(event, entity)
    on_resize(event, entity)
    }

    class physics_system {
      <<abstract, template>>
      update(dt, entity)
    }
```

```mermaid
classDiagram
    class object {
      <<abstract>>
      object *parent_object
      child_objects [ ]
      on_load()
      on_unload()
      on_enter()
      on_exit()
      children()
      parent()
      gen_object(args...)
      detach_object(index)
      attach_object(obj)
    }
    class entity {
      <<abstract, template>>
      controls_systems[ ]
      render_systems[ ]
      physics_systems[ ]
      update(dt)
      render(dt)
      on_key(event)
      on_mouse_button(event)
      on_mouse_move(event)
      on_mouse_wheel(event)
      on_resize(event)
      gen_controls_system(args...)
      detach_controls_system(index)
      attach_controls_system(obj)
      gen_physics_system(args...)
      detach_physics_system(index)
      attach_physics_system(obj)
      gen_render_system(args...)
      detach_render_system(index)
      attach_render_system(obj)
    }
    class renderer {
      <<abstract>>
      properties
      load_object(object)
      create_context()
      poll_events()
      destroy_context()
      activate_context()
      swap_buffers()
      update(dt)
      render(dt)
      on_key(event)
      on_mouse_button(event)
      on_mouse_move(event)
      on_mouse_wheel(event)
      on_resize(event)
      run_step()
      exit()
      clear_color_buffer(color)
      other_apis...()
    }

    object <|-- entity
    entity <|-- entity_impl
    component_impl <|-- entity_impl
    object <|-- renderer
    renderer <|-- sdl_gl
    sdl_gl <|-- renderer_impl
```

```mermaid
classDiagram
    class app {
      <<singleton>>
      renderers [ ]
      instance()
      activate_renderer()
      get_renderers()
      gen_renderer(args...)
      detach_renderer(index)
      attach_renderer(obj)
      run()
    }
```

Components:

- meshes
- sounds
- materials
- transform
- camera

Components can have just references to the same data

camera as component for scene

2. camera component, add renderable entities to render system attached to scene
