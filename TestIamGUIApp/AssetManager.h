#pragma once

#include <unordered_map>
#include <filesystem>
#include <wrl/client.h>
#include "AnimationClip.h"

namespace sample
{
    class D2DRenderer;
}

// 예시입니다.
class AssetManager
{
public:
    using AnimationClips = std::vector<std::pair<std::string, AnimationClip>>;
   
    AssetManager() = default;
    ~AssetManager() = default;
    
    bool LoadTexture(sample::D2DRenderer* device, const std::wstring& key, const std::filesystem::path& filePath);

    //LoadAseprite(m_Renderer.get(), keyWide, fullPath);
    bool LoadAseprite(sample::D2DRenderer* device, const std::wstring& key, const std::filesystem::path& filePath);

    // 텍스처 가져오기 함수
    ID2D1Bitmap1* GetTexture(const std::wstring& key);

    // 모든 텍스처 키 가져오기
    std::vector<std::wstring> GetAllTextureKeys() const;

    // 텍스처 맵 전체 가져오기 (읽기 전용)
    const std::unordered_map<std::wstring, Microsoft::WRL::ComPtr<ID2D1Bitmap1>>& GetTextures() const;

    // 애니메이션 클립 조회 함수
    const AnimationClips& GetClips(const std::wstring& key) const;
    

    // 로드된 애니메이션 목록 조회
    std::vector<std::wstring> GetLoadedAnimationKeys() const;


private:

    std::unordered_map<std::wstring, Microsoft::WRL::ComPtr<ID2D1Bitmap1>> m_textures;
    std::unordered_map<std::wstring, AnimationClips>                       m_clipsMap;
};

