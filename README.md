# 3d-mimic
> 3D Object Viewer 기능을 시작으로 간단한 애니메이션 재생, 액션 이벤트 생성 및 키보드 핸들러를 제공을 생각 중

## 미리보기
[![3d-mimic demo video](http://img.youtube.com/vi/zP0itxCsay8/0.jpg)](https://www.youtube.com/watch?v=zP0itxCsay8)

> 사진 클릭 시 Youtube 데모 영상 페이지로 넘어갑니다

## Menu bar
### File
#### Open
File dialog에서 선택된 3D object 파일을 불러온다

### Toolbar
Object Move, Rotate 등 간단한 도구 툴바

### Renderer Settings
불러온 3D Object에 대한 렌더링 세팅 값을 설정하는 뷰
GL depth, blend과 같은 기능을 on/off 하거나
camera coordinate 정보나 P-V-M matrix 정보, 다양한 color 값 등을 설정할 수 있게 처리


## 추가 예정
 - 3D object의 material 처리
 - 3D 애니메이션 관련 처리
 - 간단한 액션 이벤트를 연결 처리
