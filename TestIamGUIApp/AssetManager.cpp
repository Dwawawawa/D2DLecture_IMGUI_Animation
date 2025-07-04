#include "pch.h"
#include "AnimationClip.h"
#include "AsepriteParser.h"
#include "D2DRender.h"
#include "AssetManager.h"
#include <fstream>
#include "json.hpp"              //nlohmann�� JsonParser

bool AssetManager::LoadTexture(sample::D2DRenderer* device, const std::wstring& key, const std::filesystem::path& filePath)
{
    // Ȯ���� ������ ���ϸ� �����Ϸ���
    std::wstring filenameWithoutExt = filePath.stem().wstring();
    // � Ű�� ������� ����
    std::wstring actualKey = filenameWithoutExt; // �Ǵ� filename �Ǵ� key

    // �̹� �ε�� �ؽ�ó�� �ִ��� Ȯ��
    auto it = m_textures.find(actualKey);
    if(it != m_textures.end())
    {
        return true; // �̹� �ε��
    }
    
    // ���� ���� Ȯ��
    if(!std::filesystem::exists(filePath))
    {
        return false;
    }

    try
    {
        // ComPtr�� ���� ����
        Microsoft::WRL::ComPtr<ID2D1Bitmap1> texture;
        // �� �ؽ�ó ����
        ID2D1Bitmap1* rawTexture = nullptr;

        device->CreateBitmapFromFile(filePath.c_str(), rawTexture);
        
        texture.Attach(rawTexture);

        // �ʿ� ����
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
    // ���ϸ� ���� (Ȯ���� ����)
    std::wstring filenameWithoutExt = filePath.stem().wstring();
    std::wstring actualKey = filenameWithoutExt;

    // �̹� �ε�� �ִϸ��̼��� �ִ��� Ȯ��
    auto it1 = m_clipsMap.find(actualKey);
    if (it1 != m_clipsMap.end())
    {
        return true; // �̹� �ε��
    }

    // ���� ���� Ȯ��
    if (!std::filesystem::exists(filePath))
    {
        return false;
    }

    try
    {
        // JSON ���� �б�
        std::ifstream file(filePath);
        if (!file.is_open())
        {
            return false;
        }

        // JSON �Ľ�
        nlohmann::json jsonData;
        file >> jsonData;
        file.close();

        // ��������Ʈ ��Ʈ �̹��� ���� ��� ã��
        std::filesystem::path imageDir = filePath.parent_path();
        std::filesystem::path pngPath = imageDir / (filenameWithoutExt + L".png");

        if (!std::filesystem::exists(pngPath))
        {
            std::cout << "�̹��� ���� ����: " << filenameWithoutExt.c_str() << ".png" << std::endl;
            return false;
        }

        // �ؽ�ó �ε� (������ �ڵ����� �ε�)
        Microsoft::WRL::ComPtr<ID2D1Bitmap1> spriteSheet;
        auto it2 = m_textures.find(actualKey);
        if (it2 != m_textures.end())
        {
            // �̹� �ε�� �ؽ�ó ���
            spriteSheet = it2->second;
        }
        else
        {
            // �ؽ�ó �ڵ� �ε�
            if (!LoadTexture(device, actualKey, pngPath))
            {
                std::cout << "�ؽ�ó �ε� ����: " << filenameWithoutExt.c_str() << ".png" << std::endl;
                return false;
            }
            spriteSheet = m_textures[actualKey];
        }

        // �ִϸ��̼� Ŭ�� �ʿ� ���� (AnimationClips�� vector Ÿ��)
        AnimationClips clips;
        
        std::vector<Frame> allFrames;

        // JSON���� ������ ������ �Ľ�
        if (jsonData.contains("frames") && jsonData["frames"].is_array())
        {
            for (const auto& frameData : jsonData["frames"])
            {
                if (frameData.contains("frame") && frameData.contains("duration"))
                {
                    // ������ ���� ���� �Ľ�
                    auto frameRect = frameData["frame"];
                    if (frameRect.contains("x") && frameRect.contains("y") &&
                        frameRect.contains("w") && frameRect.contains("h"))
                    {
                        Frame frame;
                        frame.srcRect.left = frameRect["x"].get<UINT32>();
                        frame.srcRect.top = frameRect["y"].get<UINT32>();
                        frame.srcRect.right = frame.srcRect.left + frameRect["w"].get<UINT32>();
                        frame.srcRect.bottom = frame.srcRect.top + frameRect["h"].get<UINT32>();

                        // duration�� �и��� �����̹Ƿ� �ʷ� ��ȯ
                        float durationMs = frameData["duration"].get<float>();
                        frame.duration = durationMs / 1000.0f; // �и��� �� ��

                        allFrames.push_back(frame);
                    }
                }
            }
        }

        // 2. frameTags�� ���� AnimationClip ����
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

                    // Ŭ�� ����
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
            std::cout << "frameTags ����" << std::endl;
            return false;
        }


        std::cout << "�ִϸ��̼� �ε� ���� (�±� ��: " << clips.size() << ")\n";
        // ����
        m_clipsMap[actualKey] = std::move(clips);
        


        return true;
    }
    catch (const std::exception& e)
    {
        std::cout << "LoadAseprite ����: " << e.what() << std::endl;
        return false;
    }
}

// AssetManager.cpp�� �߰��� ����
ID2D1Bitmap1* AssetManager::GetTexture(const std::wstring& key)
{
    auto it = m_textures.find(key);
    if (it != m_textures.end())
    {
        // ���
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
        return it->second; // ��ü AnimationClips ��ȯ
    }

    // �� AnimationClips ��ȯ (static���� �� ���� ����)
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
