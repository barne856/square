# LIT - Layer Iterator Rendering Engine (name may change)

```mermaid
classDiagram
    class render_system {
      <<abstract, template>>
      render(dt, entity)
    }

    class controls_system {
      <<abstract, template>>
    on_key(input, entity)
    on_mouse_button(input, entity)
    on_mouse_move(input, entity)
    on_mouse_wheel(input, entity)
    on_resize(input, entity)
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
      on_key(input)
      on_mouse_button(input)
      on_mouse_move(input)
      on_mouse_wheel(input)
      on_resize(input)
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
      static load_object(object)
      init()
      init_renderer()
      startup()
      start_renderer()
      shutdown()
      end_renderer()
    }

    object <|-- entity
    entity <|-- entity_impl
    component_impl <|-- entity_impl
    object <|-- renderer
    renderer <|-- gl
    gl <|-- app_impl



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
