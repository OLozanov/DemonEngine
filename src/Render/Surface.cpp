#include "Surface.h"
#include "Resources/Model.h"

#include <random>

#undef min
#undef max

namespace Render
{

bool Surface::isDegenerateTriangle(const vec3& a, const vec3& b, const vec3& c)
{
    if ((a - b).length() < math::eps) return true;
    if ((a - c).length() < math::eps) return true;
    if ((b - c).length() < math::eps) return true;

    return false;
}

Surface::Surface(const vec3& pos,
                 Material* mat,
                 long xsize,
                 long ysize,
                 const std::vector<Vertex>& vertices)
{
    m_material = mat;

    calculateBBox(vertices);

    size_t size = (xsize - 1) * (ysize - 1) * 6;
    std::vector<uint16_t> indices(size);

    int p = 0;

    for (uint16_t k = 0; k < xsize - 1; k++)
    {
        for (uint16_t i = 0; i < ysize - 1; i++)
        {
            uint16_t v = i * xsize + k;

            //
            if (!isDegenerateTriangle(vertices[v].position, vertices[v + 1].position, vertices[v + xsize].position))
            {
                indices[p] = v;
                p++;

                indices[p] = v + 1;
                p++;

                indices[p] = v + xsize;
                p++;
            }

            if (!isDegenerateTriangle(vertices[v + xsize].position, vertices[v + 1].position, vertices[v + xsize + 1].position))
            {
                indices[p] = v + xsize;
                p++;

                indices[p] = v + 1;
                p++;

                indices[p] = v + xsize + 1;
                p++;
            }
        }
    }

    m_vertexBuffer.setData(vertices.data(), vertices.size());
    m_indexBuffer.setData(indices.data(), size);

    m_displayElement.material = mat;
    m_displayElement.offset = 0;
    m_displayElement.vertexnum = size;

    m_displayData.emplace_back(DisplayBlock::display_regular,
                               &m_mat,
                               m_vertexBuffer,
                               m_indexBuffer,
                               &m_displayElement);

    m_mat = mat4::Translate(pos);
}

Surface::Surface(const vec3& pos,
                 Material* mat,
                 long res,
                 const std::vector<Vertex>& vertices)
{
	m_material = mat;

	calculateBBox(vertices);

	int tri1 = (res) * (res + 1) / 2;
	int tri2 = (res - 1) * (res) / 2;

	int trinum = tri1 + tri2;

	std::vector<uint16_t> indices(trinum * 3);

	int p = 0;
	int v = 0;

	int num = res - 1;

	for (int k = 0; k < res; k++, num--)
	{
		int vnext = v + num + 1;

		for (int i = 0; i < num; i++)
		{
			int v1 = v + i;
			int v2 = vnext + i;

			indices[p] = v1;
			p++;

			indices[p] = v2;
			p++;

			indices[p] = v1 + 1;
			p++;
		}

		v = vnext;
	}

	v = 0;
	num = res - 1;

	for (int k = 0; k < res - 1; k++, num--)
	{
		int vnext = v + num + 1;

		for (int i = 0; i < num - 1; i++)
		{
			int v1 = v + i;
			int v2 = vnext + i;

			indices[p] = v2;
			p++;

			indices[p] = v2 + 1;
			p++;

			indices[p] = v1 + 1;
			p++;
		}

		v = vnext;
	}

    m_vertexBuffer.setData(vertices.data(), vertices.size());
    m_indexBuffer.setData(indices.data(), trinum * 3);

    m_displayElement.material = mat;
    m_displayElement.offset = 0;
    m_displayElement.vertexnum = trinum * 3;

    m_displayData.emplace_back(DisplayBlock::display_regular,
                               &m_mat,
                               m_vertexBuffer,
                               m_indexBuffer,
                               &m_displayElement);

    m_mat = mat4::Translate(pos);

	DisplayObject::m_mat = mat4::Translate(pos);
}

LayeredSurface::LayeredSurface(const vec3& pos,
                               Material* mat,
                               long xsize,
                               long ysize,
                               const std::vector<Vertex>& vertices,
                               const std::vector<SurfaceLayer>& layers,
                               const std::vector<Render::SurfaceLayerDetails>& layerDetails)
{
    m_material = mat;

    m_parameters.baseMaterial = m_material->id;
    m_parameters.layernum = layers.size();
    m_parameters.width = xsize;
    m_parameters.height = ysize;

    std::vector<float> mask;

    m_layers.resize(layers.size());

    for (size_t i = 0; i < layers.size(); i++)
    {
        m_layers[i].reset(layers[i].material);

        m_parameters.layers[i * 2] = layers[i].material->id;
        m_parameters.layers[i * 2 + 1] = 0;

        mask.insert(mask.end(), layers[i].mask.begin(), layers[i].mask.end());
    }

    if (!layers.empty()) m_maskBuffer.setData(mask.data(), mask.size());

    calculateBBox(vertices);

    size_t size = (xsize - 1) * (ysize - 1) * 6;
    std::vector<uint16_t> indices(size);

    int p = 0;

    for (uint16_t k = 0; k < xsize - 1; k++)
    {
        for (uint16_t i = 0; i < ysize - 1; i++)
        {
            uint16_t v = i * xsize + k;

            //
            //if (!isDegenerateTriangle(vertices[v].position, vertices[v + 1].position, vertices[v + xsize].position))
            {
                indices[p] = v;
                p++;

                indices[p] = v + 1;
                p++;

                indices[p] = v + xsize;
                p++;
            }

            //if (!isDegenerateTriangle(vertices[v + xsize].position, vertices[v + 1].position, vertices[v + xsize + 1].position))
            {
                indices[p] = v + xsize;
                p++;

                indices[p] = v + 1;
                p++;

                indices[p] = v + xsize + 1;
                p++;
            }
        }
    }

    m_vertexBuffer.setData(vertices.data(), vertices.size());
    m_indexBuffer.setData(indices.data(), size);

    m_displayElement.material = mat;
    m_displayElement.offset = 0;
    m_displayElement.vertexnum = size;

    m_displayData.emplace_back(layers.empty() ? DisplayBlock::display_regular : DisplayBlock::display_layered,
                               &m_mat,
                               m_vertexBuffer,
                               m_indexBuffer,
                               &m_displayElement);

    if (!layers.empty())
    {
        m_displayData.back().layersData = m_maskBuffer;
        m_displayData.back().parameters = &m_parameters.baseMaterial;
    }

    m_mat = mat4::Translate(pos);

    vec3 detailSize = {};

    for (const SurfaceLayerDetails& layer : layerDetails)
        generateDetailInstances(layer, xsize, ysize, vertices, layers, detailSize);

    if (!layerDetails.empty())
    {
        m_bbox.min.x -= detailSize.x;
        m_bbox.max.x += detailSize.x;

        m_bbox.min.y -= detailSize.y;
        m_bbox.max.y += detailSize.y;

        m_bbox.min.z -= detailSize.z;
        m_bbox.max.z += detailSize.z;
    }

    return;
}

void Surface::calculateBBox(const std::vector<Vertex>& vertices)
{
    m_bbox.min = vertices[0].position;
    m_bbox.max = vertices[0].position;

    for (int i = 1; i < vertices.size(); i++)
    {
        const vec3& vert = vertices[i].position;

        m_bbox.min.x = std::min(m_bbox.min.x, vert.x);
        m_bbox.min.y = std::min(m_bbox.min.y, vert.y);
        m_bbox.min.z = std::min(m_bbox.min.z, vert.z);

        m_bbox.max.x = std::max(m_bbox.max.x, vert.x);
        m_bbox.max.y = std::max(m_bbox.max.y, vert.y);
        m_bbox.max.z = std::max(m_bbox.max.z, vert.z);
    }
}

void LayeredSurface::generateDetailInstances(const SurfaceLayerDetails& layerDetails, 
                                             uint32_t xsize, uint32_t ysize, 
                                             const std::vector<Vertex>& verts,
                                             const std::vector<SurfaceLayer>& layers,
                                             vec3& maxsize)
{
    std::mt19937 randomGenerator;
    randomGenerator.seed(1729);

    std::uniform_real_distribution<float> distribution(-0.25f, 0.25f);

    std::vector<vec4> points;

    for (int k = 0; k < xsize - 1; k++)
    {
        for (int i = 0; i < ysize - 1; i++)
        {
            int v = i * xsize + k;

            vec3 x = verts[v + 1].position - verts[v].position;
            vec3 y = verts[v + xsize].position - verts[v].position;

            auto occlusionAlpha = [&layers, &layerDetails, v, xsize](float x, float y) -> float
            {
                float alpha = 0.0f;

                for (size_t l = layerDetails.layer; l < layers.size(); l++)
                {
                    float a = layers[l].mask[v];
                    float b = layers[l].mask[v + 1];
                    float c = layers[l].mask[v + xsize];
                    float d = layers[l].mask[v + xsize + 1];

                    float a1 = a * (1.0 - x) + b * x;
                    float a2 = c * (1.0 - x) + d * x;

                    alpha += a1 * (1.0 - y) + a2 * y;
                }

                return std::min(alpha, 1.0f);
            };

            auto layerAlpha = [&layers, &layerDetails, v, xsize](float x, float y) -> float
            {
                if (layerDetails.layer == 0) return 1.0f;

                size_t l = layerDetails.layer - 1;

                float a = layers[l].mask[v];
                float b = layers[l].mask[v + 1];
                float c = layers[l].mask[v + xsize];
                float d = layers[l].mask[v + xsize + 1];

                float a1 = a * (1.0 - x) + b * x;
                float a2 = c * (1.0 - x) + d * x;

                float alpha = a1 * (1.0 - y) + a2 * y;

                return alpha;
            };

            int densx = std::max(1.0f, x.length() * layerDetails.density);
            int densy = std::max(1.0f, y.length() * layerDetails.density);
            
            float granx = 1.0f / densx;
            float grany = 1.0f / densy;

            for (int l = 0; l < densx; l++)
            {
                for (int m = 0; m < densy; m++)
                {
                    float xdev = distribution(randomGenerator);
                    float ydev = distribution(randomGenerator);

                    float xpos = granx * (l + 0.5f + xdev);
                    float ypos = grany * (m + 0.5f + ydev);

                    float alpha = layerAlpha(xpos, ypos) - occlusionAlpha(xpos, ypos);

                    if (alpha < 0.3f) continue;

                    vec3 pt = verts[v].position + x * xpos + y * ypos;

                    points.push_back(vec4(pt, alpha));
                }
            }

            //vec3 pt = verts[v].position + x * (0.5f + xdev) + y * (0.5f + ydev)
            //points.push_back(pt);
        }
    }

    DetailInstanceData& data = m_detailInstanceData.emplace_back();

    data.model.reset(ResourceManager::GetModel(layerDetails.model));
    data.instanceBuffer.setData(points.data(), points.size());

    const BBox& bbox = data.model->bbox();

    vec3 detailSize = { fabs(bbox.min.x) + fabs(bbox.max.x), 
                        fabs(bbox.min.y) + fabs(bbox.max.y), 
                        fabs(bbox.min.z) + fabs(bbox.max.z) };

    if (detailSize.x > maxsize.x) maxsize.x = detailSize.x;
    if (detailSize.y > maxsize.y) maxsize.y = detailSize.y;
    if (detailSize.z > maxsize.z) maxsize.z = detailSize.z;

    const DisplayData& displayData = data.model->meshes()[0];

    m_instanceData.push_back({ &m_mat, 
                               displayData.material.get(), 
                               data.model->vertexBuffer(), 
                               data.model->indexBuffer(), 
                               data.instanceBuffer, 
                               displayData.vertexnum, 
                               data.instanceBuffer.size() });
}

} //namespace Render