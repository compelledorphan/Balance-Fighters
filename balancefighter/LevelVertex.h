#ifndef LevelVERTEX_H
#define LevelVERTEX_H

struct LevelVertex
{
	LevelVertex(){}
	LevelVertex(float x, float y, float z, float u, float v)
		: posL(x,y,z), tex(u, v){}

	D3DXVECTOR3 posL;
	D3DXVECTOR2 tex;
};

#endif // LevelVERTEX_H