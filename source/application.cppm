export module atom.editor:application;

import atom.core;
import atom.logging;
import atom.engine;
import :layers;

namespace atom::editor
{
    class editor_application: public engine::application
    {
    public:
        editor_application()
            : application("hazel-editor")
        {
            _layer = new editor_layer();
            push_layer(_layer);
        }

    private:
        editor_layer* _layer;
    };
}

namespace atom::engine
{
    extern "C++" application* create_application()
    {
        return new editor::editor_application();
    }
}
