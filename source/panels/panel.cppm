export module atom.editor:panels.panel;

namespace atom::editor
{
    export struct panel
    {
    public:
        panel() {}

    public:
        virtual auto on_imgui_render() -> void = 0;
    };
}
