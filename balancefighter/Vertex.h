#ifndef VERTEX_H
#define VERTEX_H

struct Vertex
{
	Vertex(){}
	Vertex(float x, float y, float z, 
		float nx, float ny, float nz, 
		float txU, float txV,
		float _size)
		: posL(x,y,z), directionL(nx,ny,nz), texUV(txU, txV), fSize(_size)
	{
		uiTexIndex = 0;
		fBalanceStatus = 0;
	}

	D3DXVECTOR3 posL;
	D3DXVECTOR3 directionL;
	D3DXVECTOR2 texUV;
	float		fSize;
	unsigned int uiTexIndex;
	float		fBalanceStatus;
	int iFacingDirection;
};

#endif // VERTEX_H

