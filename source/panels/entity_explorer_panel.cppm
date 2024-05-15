export module atom.editor:panels.entity_explorer_panel;

import std;
import atom.engine;
import :panels.panel;

namespace atom::editor
{
    class entity_explorer_panel: public panel
    {
    public:
        entity_explorer_panel(engine::entity_manager* manager)
            : _manager{ manager }
            , panel{}
        {}

        ~entity_explorer_panel() {}

    public:
        virtual auto on_imgui_render() -> void override
        {
            engine::ImGui::Begin("entity heirarchy");

            for (auto entry : _manager->view_all())
            {
                engine::entity* entity = &std::get<1>(entry);

                engine::ImGuiStyle& style = engine::ImGui::GetStyle();
                style.WindowMinSize.x = 370;

                engine::ImGuiTreeNodeFlags flags =
                    ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
                if (_selected_entity == entity)
                {
                    flags |= ImGuiTreeNodeFlags_Selected;
                }

                bool opened = engine::ImGui::TreeNodeEx(
                    (void*)entity->get_id(), flags, "%s", entity->get_name().get_data());
                if (engine::ImGui::IsItemClicked())
                {
                    _selected_entity = entity;
                }

                bool delete_entity = false;
                if (engine::ImGui::BeginPopupContextItem())
                {
                    if (engine::ImGui::MenuItem("delete entity"))
                    {
                        delete_entity = true;
                    }

                    engine::ImGui::EndPopup();
                }

                if (opened)
                {
                    engine::ImGui::TreePop();
                }

                if (delete_entity)
                {
                    _manager->destroy_entity(entity);
                    if (entity == _selected_entity)
                    {
                        _selected_entity = nullptr;
                    }
                }
            }

            if (engine::ImGui::IsMouseDown(0) && engine::ImGui::IsWindowHovered())
            {
                _selected_entity = nullptr;
            }

            if (engine::ImGui::BeginPopupContextWindow(
                    0, ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
            {
                if (engine::ImGui::MenuItem("create entity"))
                {
                    _manager->create_entity("new entity");
                }

                engine::ImGui::EndPopup();
            }

            engine::ImGui::End();
        }

        auto set_selected_entity(engine::entity* entity) -> void
        {
            _selected_entity = entity;
        }

        auto get_selected_entity() -> engine::entity*
        {
            return _selected_entity;
        }

    private:
        engine::entity_manager* _manager = nullptr;
        engine::entity* _selected_entity = nullptr;
    };
}
