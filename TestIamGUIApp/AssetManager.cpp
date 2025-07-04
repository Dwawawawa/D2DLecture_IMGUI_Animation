#include "pch.h"
#include "AnimationClip.h"
#include "AsepriteParser.h"
#include "D2DRender.h"
#include "AssetManager.h"
#include <fstream>
#include "json.hpp"              //nlohmann의 JsonParser

bool AssetManager::LoadTexture(sample::D2DRenderer* device, const std::wstring& key, const std::filesystem::path& filePath)
{
    // 확장자 제거한 파일명만 추출하려면
    std::wstring filenameWithoutExt = filePath.stem().wstring();
    // 어떤 키를 사용할지 선택
    std::wstring actualKey = filenameWithoutExt; // 또는 filename 또는 key

    // 이미 로드된 텍스처가 있는지 확인
    auto it = m_textures.find(actualKey);
    if(it != m_textures.end())
    {
        return true; // 이미 로드됨
    }
    
    // 파일 존재 확인
    if(!std::filesystem::exists(filePath))
    {
        return false;
    }

    try
    {
        // ComPtr로 직접 생성
        Microsoft::WRL::ComPtr<ID2D1Bitmap1> texture;
        // 새 텍스처 생성
        ID2D1Bitmap1* rawTexture = nullptr;

        device->CreateBitmapFromFile(filePath.c_str(), rawTexture);
        
        texture.Attach(rawTexture);

        // 맵에 저장
        m_textures[actualKey] = std::move(texture);
        return true;
    }
    catch (const std::exception& e)
    {
        return false;
    }

}

bool AssetManager::LoadAseprite(sample::D2DRenderer* device, const std::wstring& key, const std::filesystem::path& filePath)
{
    // 파일명만 추출 (확장자 제거)
    std::wstring filenameWithoutExt = filePath.stem().wstring();
    std::wstring actualKey = filenameWithoutExt;

    // 이미 로드된 애니메이션이 있는지 확인
    auto it1 = m_clipsMap.find(actualKey);
    if (it1 != m_clipsMap.end())
    {
        return true; // 이미 로드됨
    }

    // 파일 존재 확인
    if (!std::filesystem::exists(filePath))
    {
        return false;
    }

    try
    {
        // JSON 파일 읽기
        std::ifstream file(filePath);
        if (!file.is_open())
        {
            return false;
        }

        // JSON 파싱
        nlohmann::json jsonData;
        file >> jsonData;
        file.close();

        // 스프라이트 시트 이미지 파일 경로 찾기
        std::filesystem::path imageDir = filePath.parent_path();
        std::filesystem::path pngPath = imageDir / (filenameWithoutExt + L".png");

        if (!std::filesystem::exists(pngPath))
        {
            std::cout << "이미지 파일 없음: " << filenameWithoutExt.c_str() << ".png" << std::endl;
            return false;
        }

        // 텍스처 로드 (없으면 자동으로 로드)
        Microsoft::WRL::ComPtr<ID2D1Bitmap1> spriteSheet;
        auto it2 = m_textures.find(actualKey);
        if (it2 != m_textures.end())
        {
            // 이미 로드된 텍스처 사용
            spriteSheet = it2->second;
        }
        else
        {
            // 텍스처 자동 로드
            if (!LoadTexture(device, actualKey, pngPath))
            {
                std::cout << "텍스처 로드 실패: " << filenameWithoutExt.c_str() << ".png" << std::endl;
                return false;
            }
            spriteSheet = m_textures[actualKey];
        }

        // 애니메이션 클립 맵에 저장 (AnimationClips는 vector 타입)
        AnimationClips clips;
        
        std::vector<Frame> allFrames;

        // JSON에서 프레임 데이터 파싱
        if (jsonData.contains("frames") && jsonData["frames"].is_array())
        {
            for (const auto& frameData : jsonData["frames"])
            {
                if (frameData.contains("frame") && frameData.contains("duration"))
                {
                    // 프레임 영역 정보 파싱
                    auto frameRect = frameData["frame"];
                    if (frameRect.contains("x") && frameRect.contains("y") &&
                        frameRect.contains("w") && frameRect.contains("h"))
                    {
                        Frame frame;
                        frame.srcRect.left = frameRect["x"].get<UINT32>();
                        frame.srcRect.top = frameRect["y"].get<UINT32>();
                        frame.srcRect.right = frame.srcRect.left + frameRect["w"].get<UINT32>();
                        frame.srcRect.bottom = frame.srcRect.top + frameRect["h"].get<UINT32>();

                        // duration은 밀리초 단위이므로 초로 변환
                        float durationMs = frameData["duration"].get<float>();
                        frame.duration = durationMs / 1000.0f; // 밀리초 → 초

                        allFrames.push_back(frame);
                    }
                }
            }
        }

        // 2. frameTags에 따라 AnimationClip 분할
        if (jsonData.contains("meta") && jsonData["meta"].contains("frameTags"))
        {
            const auto& tags = jsonData["meta"]["frameTags"];
            for (const auto& tag : tags)
            {
                if (tag.contains("name") && tag.contains("from") && tag.contains("to"))
                {
                    std::string tagName = tag["name"].get<std::string>();
                    int from = tag["from"].get<int>();
                    int to = tag["to"].get<int>();

                    // 클립 생성
                    AnimationClip clip;
                    clip.SetBitmap(spriteSheet);

                    for (int i = from; i <= to && i < allFrames.size(); ++i)
                    {
                        clip.AddFrame(allFrames[i]);
                    }

                    if (!clip.GetFrames().empty())
                    {
                        clips.emplace_back(tagName, std::move(clip));
                    }
                }
            }
        }
        else
        {
            std::cout << "frameTags 없음" << std::endl;
            return false;
        }


        std::cout << "애니메이션 로드 성공 (태그 수: " << clips.size() << ")\n";
        // 저장
        m_clipsMap[actualKey] = std::move(clips);
        


        return true;
    }
    catch (const std::exception& e)
    {
        std::cout << "LoadAseprite 예외: " << e.what() << std::endl;
        return false;
    }
}

// AssetManager.cpp에 추가할 구현
ID2D1Bitmap1* AssetManager::GetTexture(const std::wstring& key)
{
    auto it = m_textures.find(key);
    if (it != m_textures.end())
    {
        // 출력
        //int size_needed = WideCharToMultiByte(CP_UTF8, 0, &key[0], (int)key.size(), NULL, 0, NULL, NULL);
        //std::string strTo(size_needed, 0);
        //WideCharToMultiByte(CP_UTF8, 0, &key[0], (int)key.size(), &strTo[0], size_needed, NULL, NULL);
        //std::cout << strTo << std::endl;

        return it->second.Get();
    }
    return nullptr;
}

std::vector<std::wstring> AssetManager::GetAllTextureKeys() const
{
    std::vector<std::wstring> keys;
    keys.reserve(m_textures.size());

    for (const auto& pair : m_textures)
    {
        keys.push_back(pair.first);
    }
    return keys;
}

const std::unordered_map<std::wstring, Microsoft::WRL::ComPtr<ID2D1Bitmap1>>& AssetManager::GetTextures() const
{
    return m_textures;
}

const AnimationClips& AssetManager::GetClips(const std::wstring& key) const
{
    auto it = m_clipsMap.find(key);
    if (it != m_clipsMap.end() && !it->second.empty())
    {
        return it->second; // 전체 AnimationClips 반환
    }

    // 빈 AnimationClips 반환 (static으로 한 번만 생성)
    static const AnimationClips emptyClips;
    return emptyClips;

}

std::vector<std::wstring> AssetManager::GetLoadedAnimationKeys() const
{
    std::vector<std::wstring> keys;
    for (const auto& pair : m_clipsMap)
    {
        keys.push_back(pair.first);
    }
    return keys;   
}
