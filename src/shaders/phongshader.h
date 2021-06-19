//
// Created by Junhao Wang (@Forkercat) on 2021/5/5.
//

#pragma once

#include "shader.h"

struct BlinnPhongShader : public Shader
{
    // Interpolated Variables
    Matrix3x3f vPositionCorrectedWS;  // world space
    Matrix2x3f vTexCoordCorrected;
    Matrix3x3f vNormalCorrectedWS;
    Matrix3x3f vTangentCorrectedWS;

    // Vertex (out) -> Fragment (in)
    Matrix3x3f v2fPositionsWS;
    Matrix2x3f v2fTexCoords;
    Vector3f   v2fOneOverWs;

    // Uniform Variables
    Matrix4x4f uModelMatrix;
    Matrix4x4f uViewMatrix;
    Matrix4x4f uProjectionMatrix;
    Matrix3x3f uNormalMatrix;
    PointLight uPointLight;
    Point3f    uEyePos;

    // For Shadow Pass
    Buffer     uShadowBuffer;
    Matrix4x4f uLightSpaceMatrix;
    Matrix3x3f vPositionLightSpaceNDC;

    // Vertex Shader
    Point4f ProcessVertex(int faceIdx, int vertIdx) override
    {
        // MVP
        Point4f positionWS = uModelMatrix * Point4f(mesh->Vert(faceIdx, vertIdx), 1.f);
        Point4f positionVS = uViewMatrix * positionWS;
        Point4f positionCS = uProjectionMatrix * positionVS;

        // TexCoord, Normal, Tangent
        Vector2f texCoord = mesh->TexCoord(faceIdx, vertIdx);
        Vector3f normalWS = uNormalMatrix * mesh->Normal(faceIdx, vertIdx);
        Vector3f tangentWS;
        if (mesh->GetModel()->HasTangents())
        {
            tangentWS = uNormalMatrix * mesh->Tangent(faceIdx, vertIdx);
        }

        // Vertex -> Fragment
        v2fPositionsWS.SetCol(vertIdx, positionWS.xyz);
        v2fTexCoords.SetCol(vertIdx, texCoord);

        // Shadow Mapping
#ifdef SHADOW_PASS
        Point4f positionLightSpaceNDC = uLightSpaceMatrix * positionWS;
        positionLightSpaceNDC /= positionLightSpaceNDC.w;
#endif

        // PCI
#ifdef PERSPECTIVE_CORRECT_INTERPOLATION
        Float oneOverW = 1.f / positionCS.w;
        v2fOneOverWs[vertIdx] = oneOverW;  // 1/w for 3 vertices
        positionWS *= oneOverW;
        texCoord *= oneOverW;
        normalWS *= oneOverW;
        if (mesh->GetModel()->HasTangents()) tangentWS *= oneOverW;
#ifdef SHADOW_PASS
        positionLightSpaceNDC *= oneOverW;
#endif  // SHADOW_PASS
#endif
        // Varying
        vTexCoordCorrected.SetCol(vertIdx, texCoord);
        vPositionCorrectedWS.SetCol(vertIdx, positionWS.xyz);
        vNormalCorrectedWS.SetCol(vertIdx, normalWS);
        if (mesh->GetModel()->HasTangents())
            vTangentCorrectedWS.SetCol(vertIdx, tangentWS);
#ifdef SHADOW_PASS
        vPositionLightSpaceNDC.SetCol(vertIdx, positionLightSpaceNDC.xyz);
#endif

        // Perspective Division
        Point4f positionNDC = positionCS / positionCS.w;
        return positionNDC;
    }

    /////////////////////////////////////////////////////////////////////////////////

    // Fragment Shader
    bool ProcessFragment(const Vector3f& baryCoord, Color3& gl_Color) override
    {
        std::shared_ptr<const Material> material = mesh->GetMaterial();

        // Interpolation
        Point3f  positionWS = vPositionCorrectedWS * baryCoord;
        Vector2f texCoord = vTexCoordCorrected * baryCoord;
        Vector3f normalWS = vNormalCorrectedWS * baryCoord;

        // PCI
#ifdef PERSPECTIVE_CORRECT_INTERPOLATION
        Float w = 1.f / Dot(v2fOneOverWs, baryCoord);
        positionWS *= w;
        texCoord *= w;
        normalWS *= w;
#endif
        Vector3f N = Normalize(normalWS);  // World Space
        Vector3f normal;

        if (mesh->GetModel()->HasTangents() && material->HasNormalMap())
        {
            Vector3f tangentWS = vTangentCorrectedWS * baryCoord;
#ifdef PERSPECTIVE_CORRECT_INTERPOLATION
            tangentWS *= w;
#endif
            Vector3f T = Normalize(tangentWS + Vector3f(0.001));  // avoid zero division
            // Normal (TBN Matrix)
            T = Normalize(T - Dot(T, N) * N);
            Vector3f   B = Normalize(Cross(N, T));
            Matrix3x3f TbnMatrix;
            TbnMatrix.SetCol(0, T).SetCol(1, B).SetCol(2, N);
            Vector3f sampledNormal = material->normalMap->Sample(texCoord);
            sampledNormal = Normalize(sampledNormal * 2.f - Vector3f(1.f));  // remap
            normal = Normalize(TbnMatrix * sampledNormal);  // World Space
        }
        else
        {
            normal = N;
        }

        // normal = normal * 0.5f + Vector3f(0.5f);
        // gl_Color = normal;
        // return false;

        // Directions
        Vector3f lightDir = Normalize(uPointLight.position - positionWS);
        Vector3f viewDir = Normalize(uEyePos - positionWS);
        Vector3f halfwayDir = Normalize(lightDir + viewDir);

        // Shadow Mapping
        Float visibility = 0.f;
#ifdef SHADOW_PASS
        Point3f positionLightSpaceNDC = vPositionLightSpaceNDC * baryCoord;
#ifdef PERSPECTIVE_CORRECT_INTERPOLATION
        positionLightSpaceNDC *= w;
#endif
        visibility = calculateShadowVisibility(uShadowBuffer, positionLightSpaceNDC,
                                               normal, lightDir);
#endif

        // Blinn-Phong Shading
        gl_Color = calculateLight(lightDir, halfwayDir, normal, texCoord, visibility);

        return false;  // do not discard
    }

private:
    Color3 calculateLight(const Vector3f& lightDir, const Vector3f& halfwayDir,
                          const Vector3f& normal, const Vector2f& texCoord,
                          Float visibility)
    {
        std::shared_ptr<const Material> material = mesh->GetMaterial();
        std::shared_ptr<const Texture>  diffuseMap = material->diffuseMap;
        std::shared_ptr<const Texture>  specularMap = material->specularMap;

        Color3 lightColor = uPointLight.color;

        // Diffuse
        Float diff = Max(0.f, Dot(lightDir, normal));

        // Specular
        Float spec = Max(0.f, Dot(halfwayDir, normal));
        if (material->HasSpecularMap())
        {
            Float specularity = specularMap->SampleFloat(texCoord);  // type 1
            // Float specularity = specularMap->SampleFloat(texCoord) * 255.f;  // type 2
            // spec *= specularity;  // type 1 - intensity
            spec = std::pow(spec, specularity + 5);  // type 2 - shininess
        }
        else
        {
            spec = std::pow(spec, 1.0);
        }

        // Color of Shading Component
        Color3 ambient, diffuse, specular;
        if (material->HasDiffuseMap())
        {
            Color3 diffuseColor = diffuseMap->Sample(texCoord);
            ambient = material->ka * diffuseColor;
            diffuse = diff * diffuseColor;
        }
        else
        {
            ambient = material->ka;  // white
            diffuse = diff * material->kd;
        }
        specular = material->ks * spec;

        ambient = ambient * lightColor;
        diffuse = diffuse * lightColor;
        specular = specular * lightColor;

        // Shadow Mapping
#ifdef SHADOW_PASS
        Float shadowIntensity = 0.6f;
        Float shadow = (1 - visibility) * shadowIntensity;
        visibility = 1 - shadow;
        diffuse *= visibility;
        specular *= visibility;
#endif

        // Combine
        Color3 color = ambient + diffuse + specular;
        return Clamp01(color);
    }
};