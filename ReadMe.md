# 깃허브 링크
https://github.com/Dwawawawa/D2DLecture_IMGUI_Animation

# 과제  
1. 제시한 애니메이션 뷰어를 만듭니다. 
	1) png를 비트맵으로 
	2) json을 이용한 애니매이션 출력 
2. * 본인이 더 필요한 기능을 넣고 개선해 봅니다.

	
# 과제 1
gpt를 걱정말고 쓰라고 한 것 치고는 꽤나 오래 걸렸던 것 같습니다. 
특히 키 값에 대해서 고생했습니다. 
파일명을 이용할 때 폴더구조를 모두 갖고 있는 경우, stem()을 이용해 지워야 했습니다.
(C:\aaa\a.png (X) -> a (O))
쉬운 개념이더라도, 적용하기 어려웠습니다. 

## 수정한 부분
``` cpp
class AssetManager{
    // 벡터 + 페어 = 어려움
    using AnimationClips = std::vector<std::pair<std::string, AnimationClip>>;

    // 핵심 함수
    bool LoadTexture(sample::D2DRenderer* device, const std::wstring& key, const std::filesystem::path& filePath);
    bool LoadAseprite(sample::D2DRenderer* device, const std::wstring& key, const std::filesystem::path& filePath);

    // (핵심) 애니메이션 클립 조회 함수 : 참조로 리턴 안하면 ComPtr이 넘어가지 않아서 오류 발생!
    const AnimationClips& GetClips(const std::wstring& key) const;
   
};

```

# 과제 2
- SFM 추가
- IMGUI png 미리보기 