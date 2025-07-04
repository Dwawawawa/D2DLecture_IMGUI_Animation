#pragma once

#include <unordered_map>
#include <filesystem>
#include <wrl/client.h>
#include "AnimationClip.h"

namespace sample
{
    class D2DRenderer;
}

// �����Դϴ�.
class AssetManager
{
public:
    using AnimationClips = std::vector<std::pair<std::string, AnimationClip>>;
   
    AssetManager() = default;
    ~AssetManager() = default;
    
    bool LoadTexture(sample::D2DRenderer* device, const std::wstring& key, const std::filesystem::path& filePath);

    //LoadAseprite(m_Renderer.get(), keyWide, fullPath);
    bool LoadAseprite(sample::D2DRenderer* device, const std::wstring& key, const std::filesystem::path& filePath);

    // �ؽ�ó �������� �Լ�
    ID2D1Bitmap1* GetTexture(const std::wstring& key);

    // ��� �ؽ�ó Ű ��������
    std::vector<std::wstring> GetAllTextureKeys() const;

    // �ؽ�ó �� ��ü �������� (�б� ����)
    const std::unordered_map<std::wstring, Microsoft::WRL::ComPtr<ID2D1Bitmap1>>& GetTextures() const;

    // �ִϸ��̼� Ŭ�� ��ȸ �Լ�
    const AnimationClips& GetClips(const std::wstring& key) const;
    

    // �ε�� �ִϸ��̼� ��� ��ȸ
    std::vector<std::wstring> GetLoadedAnimationKeys() const;


private:

    std::unordered_map<std::wstring, Microsoft::WRL::ComPtr<ID2D1Bitmap1>> m_textures;
    std::unordered_map<std::wstring, AnimationClips>                       m_clipsMap;
};

