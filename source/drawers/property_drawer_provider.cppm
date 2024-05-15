export module atom.editor:drawers.property_drawer_provider;

import atom.engine;
import :drawers.property_drawer;
import :drawers.transform_component_drawer;
import :drawers.sprite_component_drawer;
import :drawers.camera_component_drawer;

namespace atom::editor
{
    export class property_drawer_provider
    {
    public:
        static auto get(engine::entity_component* component) -> property_drawer*
        {
            if (dynamic_cast<engine::transform_component*>(component) != nullptr)
            {
                return &_transform_component_drawer;
            }

            if (dynamic_cast<engine::sprite_component*>(component) != nullptr)
            {
                return &_sprite_component_drawer;
            }

            if (dynamic_cast<engine::camera_component*>(component) != nullptr)
            {
                return &_camera_component_drawer;
            }

            return nullptr;
        }

    private:
        static inline transform_component_drawer _transform_component_drawer;
        static inline sprite_component_drawer _sprite_component_drawer;
        static inline camera_component_drawer _camera_component_drawer;
    };
}
