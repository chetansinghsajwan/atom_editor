export module atom.editor:panels.inspector_panel;

import atom.core;
import atom.engine;
import :drawers;
import :panels.panel;

namespace atom::editor
{
    class inspector_panel: public panel
    {
    public:
        virtual auto on_imgui_render() -> void override
        {
            engine::ImGui::Begin("inspector");

            if (_entity != nullptr)
            {
                string_view entity_name = _entity->get_name();

                // todo: replace this with a atom.core.buffer helper class.
                char buffer[100];
                std::copy(entity_name.get_data(), entity_name.get_data() + entity_name.get_count(),
                    buffer);

                if (engine::ImGui::InputText("name", buffer, sizeof(buffer)))
                {
                    _entity->set_name(string_view::from_cstr(buffer, sizeof(buffer)));
                }

                for (engine::entity_component* component : _entity->get_all_components())
                {
                    property_drawer* drawer = property_drawer_provider::get(component);
                    if (drawer == nullptr)
                        continue;

                    engine::ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, engine::ImVec2{ 4, 4 });
                    bool opened = engine::ImGui::TreeNodeEx(drawer->get_id(),
                        ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap, "%s",
                        drawer->get_property_name().get_data());

                    engine::ImGui::SameLine(engine::ImGui::GetWindowWidth() - 25.0f);
                    if (engine::ImGui::Button("+", engine::ImVec2{ 20, 20 }))
                    {
                        engine::ImGui::OpenPopup("component options");
                    }
                    engine::ImGui::PopStyleVar();

                    bool remove_component = false;
                    if (engine::ImGui::BeginPopup("component options"))
                    {
                        if (engine::ImGui::MenuItem("remove component"))
                            remove_component = true;

                        engine::ImGui::EndPopup();
                    }

                    if (opened)
                    {
                        drawer->set_property(component);
                        drawer->draw();

                        engine::ImGui::TreePop();
                    }

                    if (remove_component)
                    {
                        _entity->remove_component(component);
                    }
                }

                if (engine::ImGui::Button("add component"))
                {
                    engine::ImGui::OpenPopup("add component");
                }

                if (engine::ImGui::BeginPopup("add component"))
                {
                    if (engine::ImGui::MenuItem("camera component"))
                    {
                        _entity->emplace_component<engine::camera_component>();
                        engine::ImGui::CloseCurrentPopup();
                    }

                    if (engine::ImGui::MenuItem("sprite component"))
                    {
                        _entity->emplace_component<engine::sprite_component>();
                        engine::ImGui::CloseCurrentPopup();
                    }

                    engine::ImGui::EndPopup();
                }
            }

            engine::ImGui::End();
        }

        auto set_entity(engine::entity* entity) -> void
        {
            _entity = entity;
        }

    private:
        engine::entity* _entity = nullptr;
    };
}
