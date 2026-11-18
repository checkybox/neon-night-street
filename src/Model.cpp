#define TINYOBJLOADER_IMPLEMENTATION
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifdef APIENTRY
#undef APIENTRY
#endif
#endif
#include "Model.h"

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#endif
#include "tiny_obj_loader.h"
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif

#include <iostream>

std::shared_ptr<Mesh> Model::loadOBJ(const std::string& filepath, const glm::vec3& defaultColor, const glm::vec3* overrideBodyColor) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    // Load and auto-triangulate all polygon faces
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str(), nullptr, true);
    if (!ret) {
        std::cerr << "[Model] Failed to load OBJ: " << filepath << " (" << err << ")" << std::endl;
        return nullptr;
    }

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    for (const auto& shape : shapes) {
        size_t index_offset = 0;
        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); ++f) {
            size_t fv = static_cast<size_t>(shape.mesh.num_face_vertices[f]);

            size_t startVertexIdx = vertices.size();
            for (size_t v = 0; v < fv; ++v) {
                tinyobj::index_t idx = shape.mesh.indices[index_offset + v];

                Vertex vertex;
                vertex.position = glm::vec3(
                    attrib.vertices[3 * idx.vertex_index + 0],
                    attrib.vertices[3 * idx.vertex_index + 1],
                    attrib.vertices[3 * idx.vertex_index + 2]
                );

                if (idx.normal_index >= 0) {
                    vertex.normal = glm::vec3(
                        attrib.normals[3 * idx.normal_index + 0],
                        attrib.normals[3 * idx.normal_index + 1],
                        attrib.normals[3 * idx.normal_index + 2]
                    );
                } else {
                    vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
                }

                if (idx.texcoord_index >= 0) {
                    vertex.texCoords = glm::vec2(
                        attrib.texcoords[2 * idx.texcoord_index + 0],
                        attrib.texcoords[2 * idx.texcoord_index + 1]
                    );
                } else {
                    vertex.texCoords = glm::vec2(0.0f, 0.0f);
                }

                if (!attrib.colors.empty() && static_cast<size_t>(3 * idx.vertex_index + 2) < attrib.colors.size()) {
                    vertex.color = glm::vec3(
                        attrib.colors[3 * idx.vertex_index + 0],
                        attrib.colors[3 * idx.vertex_index + 1],
                        attrib.colors[3 * idx.vertex_index + 2]
                    );
                    if (overrideBodyColor != nullptr) {
                        if ((std::abs(vertex.color.r - 0.24f) < 0.05f && std::abs(vertex.color.g - 0.35f) < 0.05f && std::abs(vertex.color.b - 0.26f) < 0.05f) ||
                            (std::abs(vertex.color.r - 0.68f) < 0.05f && std::abs(vertex.color.g - 0.16f) < 0.05f && std::abs(vertex.color.b - 0.14f) < 0.05f)) {
                            vertex.color = *overrideBodyColor;
                        }
                    }
                } else {
                    vertex.color = (overrideBodyColor != nullptr) ? *overrideBodyColor : defaultColor;
                }
                indices.push_back(static_cast<unsigned int>(vertices.size()));
                vertices.push_back(vertex);
            }

            // If face had missing normals, compute face normal from first 3 vertices
            if (fv >= 3 && shape.mesh.indices[index_offset].normal_index < 0) {
                glm::vec3 edge1 = vertices[startVertexIdx + 1].position - vertices[startVertexIdx].position;
                glm::vec3 edge2 = vertices[startVertexIdx + 2].position - vertices[startVertexIdx].position;
                glm::vec3 faceNormal = glm::normalize(glm::cross(edge1, edge2));
                for (size_t v = 0; v < fv; ++v) {
                    vertices[startVertexIdx + v].normal = faceNormal;
                }
            }

            index_offset += fv;
        }
    }

    return std::make_shared<Mesh>(vertices, indices);
}
