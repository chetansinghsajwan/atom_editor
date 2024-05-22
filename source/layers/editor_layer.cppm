export module atom.editor:layers.editor_layer;

import atom.core;
import atom.logging;
import atom.engine;
import :panels;

namespace atom::editor
{
    class editor_layer: public engine::layer
    {
    public:
        editor_layer()
            : layer("editor")
            , _viewport_size(0, 0)
        {
            _setup_logging();
        }

        ~editor_layer()
        {
            delete _frame_buffer;
            delete _rpg_texture;
            delete _stairs_sprite;
            delete _barrel_sprite;
            delete _stairs_entity;
            delete _entity_explorer_panel;
            delete _inspector_panel;
            delete _logger;
        }

    public:
        virtual auto on_attach() -> void override
        {
            _logger->log_info("editor_layer attached.");

            _setup_window();
            _setup_keyboard();
            _setup_mouse();

            _rpg_texture = engine::texture2d::create(
                "/home/chetan/projects/atom-workspace/atom.editor/assets/textures/rpg-pack.png");

            _stairs_sprite = engine::sprite::make_from_coords(
                _rpg_texture, engine::vec2{ 7, 6 }, engine::vec2{ 128, 128 });
            _barrel_sprite = engine::sprite::make_from_coords(
                _rpg_texture, engine::vec2{ 8, 2 }, engine::vec2{ 128, 128 });
            _tree_sprite = engine::sprite::make_from_coords(
                _rpg_texture, engine::vec2{ 2, 1 }, engine::vec2{ 128, 128 }, engine::vec2{ 1, 2 });

            _frame_buffer = engine::frame_buffer::create({
                .width = 1280,
                .height = 720,
            });

            _scene = new engine::scene();
            _entity_manager = _scene->get_entity_manager();
            _entity_explorer_panel = new entity_explorer_panel(_entity_manager);
            _inspector_panel = new inspector_panel();

            _camera_entity = _entity_manager->create_entity("camera");
            _camera_entity->emplace_component<engine::camera_component>();

            _stairs_entity = _entity_manager->create_entity("stairs");
            _stairs_entity->emplace_component<engine::sprite_component>(engine::color{ 0, 1, 0, 1 });
        }

        virtual auto on_update(engine::time_step delta_time) -> void override
        {
            const engine::frame_buffer_specs& specs = _frame_buffer->get_specs();
            if (_viewport_size.x > 0.0f && _viewport_size.y > 0.0f
                && (specs.width != _viewport_size.x || specs.height != _viewport_size.y))
            {
                _frame_buffer->resize(_viewport_size);
                _scene->on_viewport_resize(_viewport_size);
            }

            engine::renderer_2d::reset_stats();

            _frame_buffer->bind();
            engine::render_command::set_clear_color({ 0.1f, 0.1f, 0.1f, 1 });
            engine::render_command::clear_color();

            _scene->on_update(delta_time);

            _frame_buffer->unbind();
        }

        virtual auto on_imgui_render() -> void override
        {
            static bool is_dockspace_open = true;
            static bool opt_fullscreen_persistant = true;
            bool opt_fullscreen = opt_fullscreen_persistant;
            static engine::ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

            // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
            // because it would be confusing to have two docking targets within each others.
            engine::ImGuiWindowFlags window_flags =
                ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
            if (opt_fullscreen)
            {
                engine::ImGuiViewport* viewport = engine::ImGui::GetMainViewport();
                engine::ImGui::SetNextWindowPos(viewport->Pos);
                engine::ImGui::SetNextWindowSize(viewport->Size);
                engine::ImGui::SetNextWindowViewport(viewport->ID);
                engine::ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
                engine::ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
                window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
                                | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
                window_flags |=
                    ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
            }

            // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
            if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
                window_flags |= ImGuiWindowFlags_NoBackground;

            // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
            // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
            // all active windows docked into it will lose their parent and become undocked.
            // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
            // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
            engine::ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, engine::ImVec2(0.0f, 0.0f));
            engine::ImGui::Begin("DockSpace Demo", &is_dockspace_open, window_flags);
            engine::ImGui::PopStyleVar();

            if (opt_fullscreen)
                engine::ImGui::PopStyleVar(2);

            // DockSpace
            engine::ImGuiIO& io = engine::ImGui::GetIO();
            if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
            {
                engine::ImGuiID dockspace_id = engine::ImGui::GetID("MyDockSpace");
                engine::ImGui::DockSpace(dockspace_id, engine::ImVec2(0.0f, 0.0f), dockspace_flags);
            }

            if (engine::ImGui::BeginMenuBar())
            {
                if (engine::ImGui::BeginMenu("File"))
                {
                    // Disabling fullscreen would allow the window to be moved to the front of other windows,
                    // which we can't undo at the moment without finer window depth/z control.
                    //engine::ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

                    if (engine::ImGui::MenuItem("Exit"))
                        engine::application::get()->stop();

                    engine::ImGui::EndMenu();
                }

                engine::ImGui::EndMenuBar();
            }

            {
                _entity_explorer_panel->on_imgui_render();
                engine::entity* entity = _entity_explorer_panel->get_selected_entity();

                _inspector_panel->set_entity(entity);
                _inspector_panel->on_imgui_render();
            }

            {
                engine::ImGui::Begin("settings");

                engine::renderer_2d::statistics stats = engine::renderer_2d::get_stats();
                engine::ImGui::Text("-- Renderer2D Stats -----------------------------------");
                engine::ImGui::Text("Draw Calls: %d", stats.draw_calls);
                engine::ImGui::Text("Quads: %d", stats.quad_count);
                engine::ImGui::Text("Vertices: %d", stats.get_total_vertex_count());
                engine::ImGui::Text("Indices: %d", stats.get_total_index_count());

                engine::ImGui::End();

                engine::ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, engine::ImVec2{ 0, 0 });
                engine::ImGui::Begin("viewport");

                engine::ImVec2 imgui_viewport_size = engine::ImGui::GetContentRegionAvail();
                _viewport_size = engine::vec2{ imgui_viewport_size.x, imgui_viewport_size.y };

                _is_viewport_focused = engine::ImGui::IsWindowFocused();
                _is_viewport_hovered = engine::ImGui::IsWindowHovered();

                void* imgui_texture_renderer_id =
                    reinterpret_cast<void*>(_frame_buffer->get_color_attachment_renderer_id());
                engine::ImGui::Image(imgui_texture_renderer_id,
                    engine::ImVec2(_viewport_size.x, _viewport_size.y), engine::ImVec2{ 0, 1 },
                    engine::ImVec2{ 1, 0 });

                engine::ImGui::End();
                engine::ImGui::PopStyleVar();
            }

            engine::ImGui::End();
        }

    private:
        auto _setup_logging() -> void
        {
            _logger =
                logging::logger_manager::create_logger({ .name = "hazel-editor" }).get_value();
        }

        auto _setup_window() -> void
        {
            _window = engine::window_manager::get_windows().get_mut_front();
            _logger->log_info("using window '{}'.", _window->get_name());

            contract_debug_asserts(_window != nullptr);
        }

        auto _setup_keyboard() -> void
        {
            for (engine::input_device* device : engine::input_manager::get_devices())
            {
                if (device->get_type() == engine::input_device_type::keyboard)
                {
                    _keyboard = reinterpret_cast<engine::keyboard*>(device);
                    _logger->log_info("using keyboard '{}'.", _keyboard->get_name());
                    break;
                }
            }

            contract_debug_asserts(_keyboard != nullptr);
        }

        auto _setup_mouse() -> void
        {
            for (engine::input_device* device : engine::input_manager::get_devices())
            {
                if (device->get_type() == engine::input_device_type::mouse)
                {
                    _mouse = reinterpret_cast<engine::mouse*>(device);
                    _logger->log_info("using mouse '{}'.", _mouse->get_name());
                    break;
                }
            }

            contract_debug_asserts(_mouse != nullptr);
        }

    private:
        logging::logger* _logger;
        engine::entity* _camera_entity;
        engine::texture2d* _rpg_texture;
        engine::sprite* _stairs_sprite;
        engine::entity* _stairs_entity;
        engine::sprite* _barrel_sprite;
        engine::sprite* _tree_sprite;
        engine::window* _window;
        engine::keyboard* _keyboard;
        engine::mouse* _mouse;
        engine::scene* _scene;
        engine::entity_manager* _entity_manager;

        engine::frame_buffer* _frame_buffer;
        engine::vec2 _viewport_size = engine::vec2{ 0, 0 };
        bool _is_viewport_focused = false;
        bool _is_viewport_hovered = false;

        entity_explorer_panel* _entity_explorer_panel;
        inspector_panel* _inspector_panel;
    };
}
