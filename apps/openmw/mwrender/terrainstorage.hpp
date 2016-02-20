#ifndef MWRENDER_TERRAINSTORAGE_H
#define MWRENDER_TERRAINSTORAGE_H

#include <components/esmterrain/storage.hpp>

namespace MWRender
{

    /// @brief Connects the ESM Store used in OpenMW with the ESMTerrain storage.
    class TerrainStorage : public ESMTerrain::Storage
    {
    private:
        virtual const ESM::Land* getLand (int cellX, int cellY);
        virtual const ESM::LandTexture* getLandTexture(int index, short plugin);
    public:

        TerrainStorage(const VFS::Manager* vfs, const std::string& normalMapPattern = "", bool autoUseNormalMaps = false);

        /// Get bounds of the whole terrain in cell units
        virtual void getBounds(float& minX, float& maxX, float& minY, float& maxY);
    };

}


#endif
