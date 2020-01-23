#ifndef _PCUTTER_SCENE_H
#define _PCUTTER_SCENE_H

class PCutterScene : public GScene
{
public:
	// ������ �� �Ҹ���
	PCutterScene();
	PCutterScene(const PCutterScene &cpy);
	virtual ~PCutterScene();

	// ���Կ�����
	PCutterScene &operator =(const PCutterScene &rhs);

	// �����Լ� �ߺ�
	virtual void InitScene();
	virtual void Render();
	virtual void RenderFloor();
	virtual void MouseDownL(int x, int y) {}
	virtual GTransf GetTransf() {return GTransf();}
	virtual double GetEditScale() {return 0.01;}

public:
	// ������ ���
	int m_Width;
	int m_Height;
};

#endif // _PCUTTER_SCENE_H