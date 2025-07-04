# ����� ��ũ
https://github.com/Dwawawawa/D2DLecture_IMGUI_Animation

# ����  
1. ������ �ִϸ��̼� �� ����ϴ�. 
	1) png�� ��Ʈ������ 
	2) json�� �̿��� �ִϸ��̼� ��� 
2. * ������ �� �ʿ��� ����� �ְ� ������ ���ϴ�.

	
# ���� 1
gpt�� �������� ����� �� �� ġ��� �ϳ� ���� �ɷȴ� �� �����ϴ�. 
Ư�� Ű ���� ���ؼ� ����߽��ϴ�. 
���ϸ��� �̿��� �� ���������� ��� ���� �ִ� ���, stem()�� �̿��� ������ �߽��ϴ�.
(C:\aaa\a.png (X) -> a (O))
���� �����̴���, �����ϱ� ��������ϴ�. 

## ������ �κ�
``` cpp
class AssetManager{
    // ���� + ��� = �����
    using AnimationClips = std::vector<std::pair<std::string, AnimationClip>>;

    // �ٽ� �Լ�
    bool LoadTexture(sample::D2DRenderer* device, const std::wstring& key, const std::filesystem::path& filePath);
    bool LoadAseprite(sample::D2DRenderer* device, const std::wstring& key, const std::filesystem::path& filePath);

    // (�ٽ�) �ִϸ��̼� Ŭ�� ��ȸ �Լ� : ������ ���� ���ϸ� ComPtr�� �Ѿ�� �ʾƼ� ���� �߻�!
    const AnimationClips& GetClips(const std::wstring& key) const;
   
};

```

# ���� 2
- SFM �߰�
- IMGUI png �̸����� 