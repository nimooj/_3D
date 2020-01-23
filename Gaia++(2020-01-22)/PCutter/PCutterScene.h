#ifndef _PCUTTER_SCENE_H
#define _PCUTTER_SCENE_H

class PCutterScene : public GScene
{
public:
	// 생성자 및 소멸자
	PCutterScene();
	PCutterScene(const PCutterScene &cpy);
	virtual ~PCutterScene();

	// 대입연산자
	PCutterScene &operator =(const PCutterScene &rhs);

	// 가상함수 중복
	virtual void InitScene();
	virtual void Render();
	virtual void RenderFloor();
	virtual void MouseDownL(int x, int y) {}
	virtual GTransf GetTransf() {return GTransf();}
	virtual double GetEditScale() {return 0.01;}

public:
	// 데이터 멤버
	int m_Width;
	int m_Height;
};

#endif // _PCUTTER_SCENE_H