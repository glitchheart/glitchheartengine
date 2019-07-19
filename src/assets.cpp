
namespace assets
{
    static void load_assets(WorkQueue* queue, void* data)
    {
        AssetState* asset_state = (AssetState*)data;
        assert(asset_state);
        asset_state->on_load_assets();

        asset_state->load_mode = AssetLoadMode::GAME_ASSETS_LOADED;
    }

    static void on_assets_loaded(WorkQueue* queue, void* data)
    {
        AssetState* asset_state = (AssetState*)data;
        assert(asset_state);
        asset_state->on_assets_loaded();

        asset_state->load_mode = AssetLoadMode::FINISHED;
    }
}
