#define STEP_COUNT 40
#define ARRAY_SIZE STEP_COUNT
#define ARRAY_SIZE_1 (ARRAY_SIZE - 1)

#include <iostream>
#include <vector>
#include "Expression.h"

enum ComparisonState : unsigned char
{
	COMP_FALSE= 0,
	COMP_TRUE = 1,
	COMP_UNDEFINED = 16
};

struct Point
{
	float x, y;

	void PushBack(std::vector<float>* vector)
	{
		vector->push_back(this->x);
		vector->push_back(this->y);
	}
};

class Function
{
private:
	float m_Xmin, m_Ymin;
	float m_Xmax, m_Ymax;
	float m_StepSizeX, m_StepSizeY;

	std::queue<Token> m_Function;
	std::vector<float>* m_Vertices;
	unsigned int* m_Indices;

public:
	Function(std::queue<Token>& func, float xmin = -1, float ymin = -1, float xmax = 1, float ymax = 1);
	~Function();

	float* getVertices() const { return m_Vertices->data(); }
	unsigned int getVertexSize() const { return m_Vertices->size() * sizeof(float); }
	unsigned int* getIndices() const { return m_Indices; }
	unsigned int getIndexCount() const { return m_Vertices->size(); }

	void SetDimensions(float xmin, float ymin, float xmax, float ymax);
	void IncrementDimensions(float dxmin, float dymin, float dxmax, float dymax);
	void RecomputeVertices();

private:
	ComparisonState* ConvertToBinaryImage();
	unsigned char* ComputeCellScores(ComparisonState* binaryImage);
	std::vector<float>* ComputeVertices(unsigned char* cellScores);
	unsigned int* ComputeIndices(unsigned int size);
	inline float LinearInterpolateGetT(float f1, float f2);
	Point LinearInterpolate(int arrx1, int arry1, int arrx2, int arry2);
};