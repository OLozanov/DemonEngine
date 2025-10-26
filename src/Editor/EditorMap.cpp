#include "Editor.h"
#include "DCM.h"

#include "Objects\ObjectFactory.h"
#include "Objects\Entity.h"
#include "Surfaces\SurfaceFactory.h"

#include "stdio.h"

void Editor::writeMapInfo(FILE* file)
{
    uint16_t len = m_mapInfo.mapSky.length();
    fwrite(&len, sizeof(uint16_t), 1, file);
    fwrite(m_mapInfo.mapSky.c_str(), 1, len, file);
    
    fwrite(&m_mapInfo.enableGlobalLight, sizeof(bool), 1, file);
    fwrite(&m_mapInfo.dirLightDirection, sizeof(vec3), 1, file);
    fwrite(&m_mapInfo.dirLightColor, sizeof(vec3), 1, file);
    fwrite(&m_mapInfo.enableGi, sizeof(bool), 1, file);
    fwrite(&m_mapInfo.giColor, sizeof(vec3), 1, file);
}

void Editor::writeBlocks(FILE* file)
{
    for (const Block* block : m_blocks) block->write(file);

    BlockType terminator = BlockType::Invalid;
    fwrite(&terminator, sizeof(BlockType), 1, file);
}

void Editor::writeObjects(FILE* file)
{
    for (const Object* object : m_objects)
    {
        ObjectType type = object->type();

        if (type != ObjectType::Entity)
        {
            fwrite(&type, sizeof(ObjectType), 1, file);
            object->write(file);
        }
    }

    ObjectType terminator = ObjectType::Invalid;
    fwrite(&terminator, sizeof(ObjectType), 1, file);
}

void Editor::writeEntities(FILE* file)
{
    for (const Object* object : m_objects)
    {
        ObjectType type = object->type();

        if (type == ObjectType::Entity) object->write(file);
    }

    EntityClass terminator = EntityClass::Invalid;
    fwrite(&terminator, sizeof(EntityClass), 1, file);
}

void Editor::writeSurfaces(FILE* file)
{
    for (const Surface* surface : m_surfaces)
    {
        SurfaceType type = surface->type();

        fwrite(&type, sizeof(SurfaceType), 1, file);
        surface->writeEditorInfo(file);
    }

    SurfaceType terminator = SurfaceType::Invalid;
    fwrite(&terminator, sizeof(SurfaceType), 1, file);
}

void Editor::writeGameSurfaces(FILE* file)
{
    for (const Surface* surface : m_surfaces) surface->writeGameInfo(file);

    for (auto& sgroup : m_surfaceGroups) sgroup.second.surfaceGraph->buildGeometry();

    for (const Block* block : m_blocks) block->writeSurfaces(file);

    SurfaceType terminator = SurfaceType::Invalid;
    fwrite(&terminator, sizeof(SurfaceType), 1, file);
}

void Editor::readMapInfo(FILE* file)
{
    uint16_t len;
    fread(&len, sizeof(uint16_t), 1, file);

    m_mapInfo.mapSky.resize(len);
    fread(m_mapInfo.mapSky.data(), 1, len, file);

    fread(&m_mapInfo.enableGlobalLight, sizeof(bool), 1, file);
    fread(&m_mapInfo.dirLightDirection, sizeof(vec3), 1, file);
    fread(&m_mapInfo.dirLightColor, sizeof(vec3), 1, file);
    fread(&m_mapInfo.enableGi, sizeof(bool), 1, file);
    fread(&m_mapInfo.giColor, sizeof(vec3), 1, file);
}

void Editor::readBlocks(FILE* file)
{
    while (true)
    {
        BlockType type;

        fread(&type, sizeof(BlockType), 1, file);

        if (type == BlockType::Invalid) break;

        vec3 pos;

        fread(&pos, sizeof(vec3), 1, file);

        uint16_t surfaceId = -1;

        std::vector<vec3> vertices;
        std::vector<uint16_t> indices;
        std::vector<BlockPolygon> polygons;

        uint32_t vnum;
        uint32_t inum;
        uint32_t pnum;

        fread(&vnum, sizeof(uint32_t), 1, file);
        fread(&inum, sizeof(uint32_t), 1, file);
        fread(&pnum, sizeof(uint32_t), 1, file);

        vertices.resize(vnum);
        indices.resize(inum);
        polygons.resize(pnum);

        fread(vertices.data(), sizeof(vec3), vnum, file);
        fread(indices.data(), sizeof(uint16_t), inum, file);

        //Polygons
        for (unsigned long i = 0; i < polygons.size(); i++)
        {
            BlockPolygon& poly = polygons[i];

            fread(&poly.flags, sizeof(uint8_t), 1, file);

            fread(&poly.smoothGroop, sizeof(uint8_t), 1, file);
            fread(&poly.offset, sizeof(uint16_t), 1, file);
            fread(&poly.vertnum, sizeof(uint16_t), 1, file);

            fread(&poly.s, sizeof(vec3), 1, file);
            fread(&poly.t, sizeof(vec3), 1, file);
            fread(&poly.tcoord, sizeof(vec2), 1, file);

            std::string tname;
            uint16_t tlen;
            fread(&tlen, sizeof(uint16_t), 1, file);
            
            tname.resize(tlen);
            
            fread(tname.data(), 1, tlen, file);

            poly.material = loadMaterial(tname);

            // surface
            uint16_t surfSize;

            fread(&surfSize, sizeof(uint16_t), 1, file);

            if (surfSize)
            {   
                fread(&surfaceId, sizeof(uint16_t), 1, file);

                size_t size = surfSize * surfSize;

                std::vector<TexturedVertex> vertices(size);
                std::vector<vec3> normals(size);

                fread(vertices.data(), sizeof(TexturedVertex), size, file);
                fread(normals.data(), sizeof(vec3), size, file);

                if (type == BlockType::Solid)
                    poly.surface.reset(new EditSurface(&poly, surfSize, vertices, normals));

                uint8_t layers;
                fread(&layers, sizeof(uint8_t), 1, file);

                if (layers > 0)
                {
                    std::vector<float> layerMask(size);

                    for (size_t l = 0; l < layers; l++)
                    {
                        LayerOrientation orientation;
                        Material* material;

                        fread(&orientation, sizeof(uint8_t), 1, file);

                        std::string tname;
                        uint16_t tlen;
                        fread(&tlen, sizeof(uint16_t), 1, file);

                        tname.resize(tlen);

                        fread(tname.data(), 1, tlen, file);

                        fread(layerMask.data(), sizeof(float), size, file);
                    
                        poly.surface->addLayer(loadMaterial(tname), orientation, layerMask);
                    }
                }

                uint8_t layerDetails;
                fread(&layerDetails, sizeof(uint8_t), 1, file);

                if (layerDetails > 0)
                {
                    std::vector<float> layerMask(size);

                    for (size_t l = 0; l < layerDetails; l++)
                    {
                        uint8_t layer;
                        float density;

                        fread(&layer, sizeof(uint8_t), 1, file);
                        fread(&density, sizeof(float), 1, file);

                        std::string model;
                        std::string material;

                        uint16_t strlen;
                        fread(&strlen, sizeof(uint16_t), 1, file);
                        model.resize(strlen);
                        fread(model.data(), 1, strlen, file);

                        fread(&strlen, sizeof(uint16_t), 1, file);
                        material.resize(strlen);
                        fread(material.data(), 1, strlen, file);

                        poly.surface->addDetails(layer, model, material, density);
                    }
                }
            }
        }

        if (type == BlockType::Edit)
        {
            m_editBlock.reset(vertices, indices, polygons);
            m_editBlock.setPos(pos);
        }
        else
        {
            Block* block = new Block(std::move(vertices), std::move(indices), std::move(polygons), type);

            block->setPos(pos);

            block->generatePolygons();
            clipBlock(block);
            block->buildGeometry();
            
            m_blocks.append(block);

            if (surfaceId != -1) m_surfaceGroups[surfaceId].blocks.insert(block);
        }
    }

    // Block surfaces
    for (auto& entry : m_surfaceGroups)
    {
        SurfaceGroup& sgroup = entry.second;

        std::vector<EditSurface*> surfaces;

        for (Block* block : sgroup.blocks) surfaces.insert(surfaces.end(), block->surfaces().begin(), block->surfaces().end());
        
        sgroup.surfaceGraph = std::make_shared<SurfaceGraph>(entry.first, surfaces);
    }
}

void Editor::readObjects(FILE* file)
{
    while (true)
    {
        ObjectType type;

        fread(&type, sizeof(ObjectType), 1, file);
        if (type == ObjectType::Invalid) break;

        Object* object = ObjectFactory::CreateObject(type, file);
        m_objects.append(object);
    }
}

void Editor::readEntities(FILE* file)
{
    while (true)
    {
        EntityClass eclass;

        fread(&eclass, sizeof(EntityClass), 1, file);
        if (eclass == EntityClass::Invalid) break;

        Object* entity = Entity::CreateEntity(eclass, file);
        m_objects.append(entity);
    }
}

void Editor::readSurfaces(FILE* file)
{
    while (true)
    {
        SurfaceType type;

        fread(&type, sizeof(SurfaceType), 1, file);
        if (type == SurfaceType::Invalid) break;

        Surface* surface = SurfaceFactory::CreateSurface(type, file);
        m_surfaces.append(surface);
    }
}

void Editor::write(const std::string& filename, const EditorInfo& editorInfo)
{
    FILE* file;
    DcmHeader head;

    unsigned long offset;

    std::vector<DcmEntry> entry;
    entry.reserve(10);

    errno_t error = fopen_s(&file, filename.c_str(), "wb");

    if (error) return;
    
    head.signature = DcmSignature;
    head.version = DcmVersion;
    head.entrynum = 1;
    head.eoffset = 0;

    fwrite(&head, sizeof(DcmHeader), 1, file);

    head.eoffset = sizeof(DcmHeader);

    // Editor params
    fwrite(&editorInfo, sizeof(EditorInfo), 1, file);
    offset = ftell(file);
    entry.push_back({ DcmEntryType::entry_editorinfo, offset - head.eoffset, head.eoffset });
    head.eoffset = offset;

    // Map Info
    writeMapInfo(file);
    offset = ftell(file);
    entry.push_back({ DcmEntryType::entry_mapinfo, offset - head.eoffset, head.eoffset });
    head.eoffset = offset;

    // Blocks
    writeBlocks(file);
    offset = ftell(file);
    entry.push_back({ DcmEntryType::entry_blocks, offset - head.eoffset, head.eoffset });
    head.eoffset = offset;

    // Objects
    writeObjects(file);
    offset = ftell(file);
    entry.push_back({ DcmEntryType::entry_objects, offset - head.eoffset, head.eoffset });
    head.eoffset = offset;

    // Entities
    writeEntities(file);
    offset = ftell(file);
    entry.push_back({ DcmEntryType::entry_entities, offset - head.eoffset, head.eoffset });
    head.eoffset = offset;

    // Surfaces
    writeSurfaces(file);
    offset = ftell(file);
    entry.push_back({ DcmEntryType::entry_editor_surfaces, offset - head.eoffset, head.eoffset });
    head.eoffset = offset;

    // Game map
    if (!m_map.empty())
    {
        m_map.generateDisplayData();

        m_map.writeVertices(file);
        offset = ftell(file);
        entry.push_back({ DcmEntryType::entry_geometry, offset - head.eoffset, head.eoffset });
        head.eoffset = offset;

        m_map.writeLeafs(file);
        offset = ftell(file);
        entry.push_back({ DcmEntryType::entry_leafs, offset - head.eoffset, head.eoffset });
        head.eoffset = offset;

        m_map.writeNodes(file);
        offset = ftell(file);
        entry.push_back({ DcmEntryType::entry_nodes, offset - head.eoffset, head.eoffset });
        head.eoffset = offset;

        m_map.writePortals(file);
        offset = ftell(file);
        entry.push_back({ DcmEntryType::entry_portals, offset - head.eoffset, head.eoffset });
        head.eoffset = offset;

        m_map.writeZones(file);
        offset = ftell(file);
        entry.push_back({ DcmEntryType::entry_zoneinfo, offset - head.eoffset, head.eoffset });
        head.eoffset = offset;

        writeGameSurfaces(file);
        offset = ftell(file);
        entry.push_back({ DcmEntryType::entry_surfaces, offset - head.eoffset, head.eoffset });
        head.eoffset = offset;
    }

    fwrite(entry.data(), entry.size(), sizeof(DcmEntry), file);

    //Rewrite header
    head.entrynum = entry.size();
    fseek(file, 0, SEEK_SET);
    fwrite(&head, 1, sizeof(DcmHeader), file);

    fclose(file);
}

void Editor::read(const std::string& filename, EditorInfo& editorInfo)
{
    FILE* file;
    DcmHeader head;

    errno_t error = fopen_s(&file, filename.c_str(), "rb");

    if (error)
        throw std::exception("Can't open file");

    fread(&head, 1, sizeof(DcmHeader), file);

    if (head.signature != DcmSignature) 
        throw std::exception("Not a DCM file");

    if (head.version != DcmVersion)
        throw std::exception("Invalid map version");
        
    fseek(file, head.eoffset, SEEK_SET);

    std::vector<DcmEntry> entry(head.entrynum);

    fread(entry.data(), sizeof(DcmEntry), head.entrynum, file);

    for (int e = 0; e < head.entrynum; e++)
    {
        fseek(file, entry[e].offset, SEEK_SET);

        switch (entry[e].type)
        {
        case DcmEntryType::entry_editorinfo:
            fread(&editorInfo, sizeof(EditorInfo), 1, file);
        break;
        
        case DcmEntryType::entry_mapinfo:
            readMapInfo(file);
        break;

        case DcmEntryType::entry_blocks:
            readBlocks(file);
        break;

        case DcmEntryType::entry_leafs:
        case DcmEntryType::entry_nodes:
        case DcmEntryType::entry_portals:
        case DcmEntryType::entry_zoneinfo:
        break;

        case DcmEntryType::entry_objects:
            readObjects(file);
        break;

        case DcmEntryType::entry_entities:
            readEntities(file);
        break;

        case DcmEntryType::entry_editor_surfaces:
            readSurfaces(file);
        break;
        }
    }

    fclose(file);
}