#include "pch.h"
#include "MeshSyncClientBlender.h"
using namespace mu;

using float2a = std::array<float, 2>;
using float3a = std::array<float, 3>;
using float4a = std::array<float, 4>;
using float4x4a = std::array<float, 16>;

inline float2a to_a(const float2& v) { return { v.x, v.y }; }
inline float3a to_a(const float3& v) { return { v.x, v.y, v.z }; }
inline float4a to_a(const float4& v) { return { v.x, v.y, v.z, v.w }; }
inline float4a to_a(const quatf& v) { return { v.x, v.y, v.z, v.w }; }
inline float4x4a to_a(const float4x4& v)
{
    float4x4a ret;
    for (int i = 0; i < 16; ++i) { ret[i] = ((float*)&v)[i]; }
    return ret;
}

inline float2 to_float2(const float2a& v) { return { v[0], v[1] }; }
inline float3 to_float3(const float3a& v) { return { v[0], v[1], v[2] }; }
inline float4 to_float4(const float4a& v) { return { v[0], v[1], v[2], v[3] }; }
inline quatf  to_quatf(const float4a& v) { return { v[0], v[1], v[2], v[3] }; }
inline float4x4 to_float4x4(const float4x4a& v) { float4x4 ret; ret.assign(v.data()); return ret; }

inline float2 to_float2(const py::object& v)
{
    static py::str x = "x", y = "y";
    return { v.attr(x).cast<float>(), v.attr(y).cast<float>() };
}
inline float3 to_float3(const py::object& v)
{
    static py::str x = "x", y = "y", z = "z";
    return { v.attr(x).cast<float>(), v.attr(y).cast<float>(), v.attr(z).cast<float>() };
}
inline float4 to_float4(const py::object& v)
{
    static py::str x = "x", y = "y", z = "z", w = "w";
    return { v.attr(x).cast<float>(), v.attr(y).cast<float>(), v.attr(z).cast<float>(), v.attr(w).cast<float>() };
}
inline quatf  to_quatf(const py::object& v)
{
    static py::str x = "x", y = "y", z = "z", w = "w";
    return { v.attr(x).cast<float>(), v.attr(y).cast<float>(), v.attr(z).cast<float>(), v.attr(w).cast<float>() };
}


PYBIND11_PLUGIN(MeshSyncClientBlender)
{
    py::module mod("MeshSyncClientBlender", "Python bindings for MeshSync");

#define BindMethod(Name) .def(#Name, &self_t::Name)
#define BindMethod2(Name, ...) .def(#Name, __VA_ARGS__)
#define BindField(Name) .def_readwrite(#Name, &self_t::Name)
#define BindProperty(Name, ...) .def_property(#Name, __VA_ARGS__)
    {
        using self_t = ms::Transform;
        py::class_<ms::Transform, ms::TransformPtr>(mod, "Transform")
            BindField(visible)
            BindField(reference)
            BindProperty(index,
                [](const ms::Transform& self) { return self.index; },
                [](ms::Transform& self, int v) { self.index = v; })
            BindProperty(path,
                [](const ms::Transform& self) { return self.path; },
                [](ms::Transform& self, const std::string& v) { self.path = v; })
            BindProperty(position,
                [](const ms::Transform& self) { return to_a(self.position); },
                [](ms::Transform& self, const float3a& v) { self.position = to_float3(v); })
            BindProperty(scale,
                [](const ms::Transform& self) { return to_a(self.scale); },
                [](ms::Transform& self, const float3a& v) { self.scale = to_float3(v); })
            BindProperty(rotation,
                [](const ms::Transform& self) { return to_a(self.rotation); },
                [](ms::Transform& self, const float4a& v) { self.rotation = to_quatf(v); })
            BindProperty(matrix,
                [](const ms::Transform& self) { return to_a(self.toMatrix()); },
                [](ms::Transform& self, const float4x4a& v) { self.assignMatrix(to_float4x4(v)); })
            BindMethod2(addTranslationKey,
                [](ms::Transform& self, float t, const float3a& v) { self.addTranslationKey(t, to_float3(v)); })
            BindMethod2(addRotationKey,
                [](ms::Transform& self, float t, const float4a& v) { self.addRotationKey(t, to_quatf(v)); })
            BindMethod2(addScaleKey,
                [](ms::Transform& self, float t, const float3a& v) { self.addScaleKey(t, to_float3(v)); })
            ;
    }
    {
        using self_t = ms::Camera;
        py::class_<ms::Camera, ms::CameraPtr, ms::Transform>(mod, "Camera")
            BindField(fov)
            BindField(near_plane)
            BindField(far_plane)
            BindField(vertical_aperture)
            BindField(horizontal_aperture)
            BindField(focal_length)
            BindField(focus_distance)
            BindMethod(addFovKey)
            BindMethod(addNearPlaneKey)
            BindMethod(addFarPlaneKey)
            BindMethod(addVerticalApertureKey)
            BindMethod(addHorizontalApertureKey)
            BindMethod(addFocalLengthKey)
            BindMethod(addFocusDistanceKey)
            ;
    }
    {
        using self_t = ms::Light;
        py::class_<ms::Light, ms::LightPtr, ms::Transform>(mod, "Light")
            BindField(type)
            BindProperty(color,
                [](const ms::Light& self) { return to_a(self.color); },
                [](ms::Light& self, const float4a& v) { self.color = to_float4(v); })
            BindField(intensity)
            BindField(range)
            BindField(spot_angle)
            BindMethod2(addColorKey,
                [](ms::Light& self, float t, const float4a& v) { self.addColorKey(t, to_float4(v)); })
            BindMethod(addIntensityKey)
            BindMethod(addRangeKey)
            BindMethod(addSpotAngleKey)
            ;
    }
    {
        using self_t = ms::Mesh;
        py::class_<ms::Mesh, ms::MeshPtr, ms::Transform>(mod, "Mesh")
            BindProperty(swap_faces,
                [](const ms::Mesh& self) { return (bool)self.refine_settings.flags.swap_faces; },
                [](ms::Mesh& self, bool v) { self.refine_settings.flags.swap_faces = v; })
            BindProperty(mirror_x,
                [](const ms::Mesh& self) { return (bool)self.refine_settings.flags.mirror_x; },
                [](ms::Mesh& self, bool v) { self.refine_settings.flags.mirror_x = v; })
            BindProperty(mirror_y,
                [](const ms::Mesh& self) { return (bool)self.refine_settings.flags.mirror_y; },
                [](ms::Mesh& self, bool v) { self.refine_settings.flags.mirror_y = v; })
            BindProperty(mirror_z,
                [](const ms::Mesh& self) { return (bool)self.refine_settings.flags.mirror_z; },
                [](ms::Mesh& self, bool v) { self.refine_settings.flags.mirror_z = v; })
            BindProperty(mirror_merge,
                [](const ms::Mesh& self) { return (bool)self.refine_settings.flags.mirror_x_weld; },
                [](ms::Mesh& self, bool v) { self.refine_settings.flags.mirror_x_weld = self.refine_settings.flags.mirror_y_weld = self.refine_settings.flags.mirror_z_weld = v; })
            ;
    }
    {
        using self_t = ms::Material;
        py::class_<ms::Material, ms::MaterialPtr>(mod, "Material")
            BindField(id)
            BindField(name)
            BindProperty(color,
                [](const ms::Material& self) { return to_a(self.color); },
                [](ms::Material& self, const float4a& v) { self.color = to_float4(v); })
            ;
    }
    {
        using self_t = ms::Scene;
        py::class_<ms::Scene, ms::ScenePtr>(mod, "Scene")
            BindField(transforms)
            BindField(cameras)
            BindField(lights)
            BindField(meshes)
            BindField(materials)
            ;
    }
    {
        using self_t = msbContext;
        py::class_<msbContext, msbContextPtr>(mod, "Context")
            .def(py::init<>())
            BindMethod(addTransform)
            BindMethod(addCamera)
            BindMethod(addLight)
            BindMethod(addMesh)
            BindMethod(addDeleted)
            BindMethod(addMaterial)
            BindMethod(extractTransformData)
            BindMethod(extractCameraData)
            BindMethod(extractLightData)
            BindMethod(extractMeshData)
            BindMethod(isSending)
            BindMethod(send)
            BindProperty(server_address,
                [](const msbContext& self) { return self.getSettings().client_settings.server; },
                [](msbContext& self, const std::string& v) { self.getSettings().client_settings.server = v; })
            BindProperty(server_port,
                [](const msbContext& self) { return self.getSettings().client_settings.port; },
                [](msbContext& self, uint16_t v) { self.getSettings().client_settings.port = v; })
            BindProperty(scene_name,
                [](const msbContext& self) { return self.getSettings().scene_settings.name; },
                [](msbContext& self, const std::string& v) { self.getSettings().scene_settings.name = v; })
            BindProperty(scale_factor,
                [](const msbContext& self) { return self.getSettings().scene_settings.scale_factor; },
                [](msbContext& self, float v) { self.getSettings().scene_settings.scale_factor = v; })
            BindProperty(handedness,
                [](const msbContext& self) { return (int)self.getSettings().scene_settings.handedness; },
                [](msbContext& self, int v) { (int&)self.getSettings().scene_settings.handedness = v; })
            BindProperty(sync_normals,
                [](const msbContext& self) { return (int)self.getSettings().sync_normals; },
                [](msbContext& self, int v) { (int&)self.getSettings().sync_normals = v; })
            BindProperty(sync_uvs,
                [](const msbContext& self) { return self.getSettings().sync_uvs; },
                [](msbContext& self, bool v) { self.getSettings().sync_uvs = v; })
            BindProperty(sync_colors,
                [](const msbContext& self) { return self.getSettings().sync_colors; },
                [](msbContext& self, bool v) { self.getSettings().sync_colors = v; })
            BindProperty(sync_bones,
                [](const msbContext& self) { return self.getSettings().sync_bones; },
                [](msbContext& self, bool v) { self.getSettings().sync_bones = v; })
            BindProperty(sync_poses,
                [](const msbContext& self) { return self.getSettings().sync_poses; },
                [](msbContext& self, bool v) { self.getSettings().sync_poses = v; })
            BindProperty(sync_blendshapes,
                [](const msbContext& self) { return self.getSettings().sync_blendshapes; },
                [](msbContext& self, bool v) { self.getSettings().sync_blendshapes = v; })
            BindProperty(current_scene,
                [](const msbContext& self) { return self.getCurrentScene(); },
                [](msbContext& self, ms::ScenePtr v) { self.getCurrentScene() = v; })
            ;
    }
#undef BindMethod
#undef BindMethod2
#undef BindField
#undef BindProperty

    return mod.ptr();
}
