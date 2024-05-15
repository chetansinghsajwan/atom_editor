export module atom.editor:panels.panel;

namespace atom::editor
{
    export class panel
    {
    public:
        panel() {}

    public:
        virtual auto on_imgui_render() -> void = 0;
    };
}
