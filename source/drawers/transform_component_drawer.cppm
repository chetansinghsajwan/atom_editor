export module atom.editor:drawers.transform_component_drawer;

import atom.core;
import atom.engine;
import :drawers.property_drawer;

namespace atom::editor
{
    export class transform_component_drawer: public property_drawer
    {
    public:
        transform_component_drawer() {}

    public:
        virtual auto draw() -> void override
        {
            if (_transform == nullptr)
                return;

            engine::f32vec3 position = _transform->get_position();
            _draw_vec3("position", &position);
            _transform->set_position(position);

            engine::f32vec3 rotation = engine::math::degrees(_transform->get_rotation());
            _draw_vec3("rotation", &rotation);
            _transform->set_rotation(engine::math::radians(rotation));

            engine::f32vec3 scale = _transform->get_scale();
            _draw_vec3("scale", &scale);
            _transform->set_scale(scale);
        }

        virtual auto set_property(void* property) -> void override
        {
            _transform = reinterpret_cast<engine::transform_component*>(property);
        }

        virtual auto get_property_name() -> string_view override
        {
            return "transform component";
        }

        virtual auto get_id() -> void* override
        {
            return (void*)typeid(engine::transform_component).hash_code();
        }

    private:
        auto _draw_vec3(string_view label, engine::f32vec3* values,
            engine::f32vec3 reset_values = engine::f32vec3{ 0 }, f32 column_width = 100) -> void
        {
            engine::ImGui::PushID(label.get_data());

            engine::ImGui::Columns(2);
            engine::ImGui::SetColumnWidth(0, column_width);
            engine::ImGui::Text("%s", label.get_data());
            engine::ImGui::NextColumn();

            engine::ImGui::PushMultiItemsWidths(3, engine::ImGui::CalcItemWidth());
            engine::ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, engine::ImVec2{ 0, 0 });

            f32 line_height =
                engine::GImGui->Font->FontSize + engine::GImGui->Style.FramePadding.y * 2.0f;
            engine::ImVec2 button_size = { line_height + 3.0f, line_height };

            engine::ImGui::PushStyleColor(
                ImGuiCol_Button, engine::ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
            engine::ImGui::PushStyleColor(
                ImGuiCol_ButtonHovered, engine::ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
            engine::ImGui::PushStyleColor(
                ImGuiCol_ButtonActive, engine::ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
            if (engine::ImGui::Button("X", button_size))
            {
                values->x = reset_values.x;
            }
            engine::ImGui::PopStyleColor(3);

            engine::ImGui::SameLine();
            engine::ImGui::DragFloat("##X", &values->x, 0.1f, 0.0f, 0.0f, "%.2f");
            engine::ImGui::PopItemWidth();
            engine::ImGui::SameLine();

            engine::ImGui::PushStyleColor(
                ImGuiCol_Button, engine::ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
            engine::ImGui::PushStyleColor(
                ImGuiCol_ButtonHovered, engine::ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
            engine::ImGui::PushStyleColor(
                ImGuiCol_ButtonActive, engine::ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
            if (engine::ImGui::Button("Y", button_size))
            {
                values->y = reset_values.y;
            }
            engine::ImGui::PopStyleColor(3);

            engine::ImGui::SameLine();
            engine::ImGui::DragFloat("##Y", &values->y, 0.1f, 0.0f, 0.0f, "%.2f");
            engine::ImGui::PopItemWidth();
            engine::ImGui::SameLine();

            engine::ImGui::PushStyleColor(
                ImGuiCol_Button, engine::ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
            engine::ImGui::PushStyleColor(
                ImGuiCol_ButtonHovered, engine::ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
            engine::ImGui::PushStyleColor(
                ImGuiCol_ButtonActive, engine::ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
            if (engine::ImGui::Button("Z", button_size))
            {
                values->z = reset_values.z;
            }
            engine::ImGui::PopStyleColor(3);

            engine::ImGui::SameLine();
            engine::ImGui::DragFloat("##Z", &values->z, 0.1f, 0.0f, 0.0f, "%.2f");
            engine::ImGui::PopItemWidth();

            engine::ImGui::PopStyleVar();

            engine::ImGui::Columns(1);

            engine::ImGui::PopID();
        }

    private:
        engine::transform_component* _transform;
    };
}
