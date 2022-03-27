#include "MarchingSquares.h"
#include <cerrno>
#include <cmath>

Function::Function(float(*func)(float, float), float xmin, float ymin, float xmax, float ymax)
    :m_Function(func)
{
    SetDimensions(xmin, ymin, xmax, ymax);
    ComparisonState* binaryImage = ConvertToBinaryImage();
    unsigned char* cellScores = ComputeCellScores(binaryImage);
    m_Vertices = ComputeVertices(cellScores);
    m_Indices = ComputeIndices(getIndexCount());
    delete[] binaryImage;
    delete[] cellScores;
}

Function::~Function()
{
    delete m_Vertices;
    delete[] m_Indices;
}

void Function::RecomputeVertices()
{
    delete m_Vertices;
    delete[] m_Indices;
    ComparisonState* binaryImage = ConvertToBinaryImage();
    unsigned char* cellScores = ComputeCellScores(binaryImage);
    m_Vertices = ComputeVertices(cellScores);
    m_Indices = ComputeIndices(getIndexCount());
    delete[] binaryImage;
    delete[] cellScores;
}

void Function::SetDimensions(float xmin, float ymin, float xmax, float ymax)
{
    m_Xmin = xmin;
    m_Ymin = ymin;
    m_Xmax = xmax;
    m_Ymax = ymax;

    m_StepSizeX = (float)(m_Xmax - m_Xmin) / STEP_COUNT;
    m_StepSizeY = (float)(m_Ymax - m_Ymin) / STEP_COUNT;
}

ComparisonState* Function::ConvertToBinaryImage()
{
    ComparisonState* result = new ComparisonState[ARRAY_SIZE * ARRAY_SIZE];

    for(int indy = 0; indy < ARRAY_SIZE; ++indy)
    {
        float y = indy * m_StepSizeY + m_Ymin;
        for(int indx = 0; indx < ARRAY_SIZE; ++indx)
        {
            float x = indx * m_StepSizeX + m_Xmin; 
            float funcValue = m_Function(x, y);
#ifndef __FAST_MATH__
            if (funcValue != funcValue) // This only works when the IEEE-standard is upheld
                result[indy * ARRAY_SIZE + indx] = COMP_UNDEFINED;
#else
            if (isnan(funcValue)) // This function is apparently very expensive
                result[indy * ARRAY_SIZE + indx] = COMP_UNDEFINED;
#endif
            else if (funcValue < 0)
                result[indy * ARRAY_SIZE + indx] = COMP_TRUE;
            else
                result[indy * ARRAY_SIZE + indx] = COMP_FALSE;
        }
    }
    return result;
}

unsigned char* Function::ComputeCellScores(ComparisonState* binaryImage)
{
    unsigned char* result = new unsigned char[ARRAY_SIZE_1 * ARRAY_SIZE_1];
    for(int y = 0; y < ARRAY_SIZE_1; ++y)
    {
        for(int x = 0; x < ARRAY_SIZE_1; ++x)
        {
            unsigned char score = 0;

            // Since UNDEFINED equals 16, it will automatically go into
            // the default case at the switch
            score += 8 * binaryImage[y*ARRAY_SIZE+x];
            score += 1 * binaryImage[(y+1)*ARRAY_SIZE+x];
            score += 4 * binaryImage[y*ARRAY_SIZE+x+1];
            score += 2 * binaryImage[(y+1)*ARRAY_SIZE+x+1];
            result[y*ARRAY_SIZE_1+x] = score;
        }
    }

    return result;
}

unsigned int* Function::ComputeIndices(unsigned int size)
{
    unsigned int* result = new unsigned int[size];
    for (unsigned int i = 0; i < size; ++i)
    {
        result[i] = i;
    }

    return result;
}

float Function::LinearInterpolateGetT(float f1, float f2)
{
    return -f1 / (f2 - f1);
}

Point Function::LinearInterpolate(int arrx1, int arry1, int arrx2, int arry2)
{
    float funcx1 = arrx1 * m_StepSizeX + m_Xmin;
    float funcx2 = arrx2 * m_StepSizeX + m_Xmin;
    float funcy1 = arry1 * m_StepSizeY + m_Ymin;
    float funcy2 = arry2 * m_StepSizeY + m_Ymin;
    float t = LinearInterpolateGetT(m_Function(funcx1, funcy1), m_Function(funcx2, funcy2));

    float x1 = 2 * (float)arrx1 / ARRAY_SIZE_1 - 1;
    float x2 = 2 * (float)arrx2 / ARRAY_SIZE_1 - 1;    
    float y1 = 2 * (float)arry1 / ARRAY_SIZE_1 - 1;
    float y2 = 2 * (float)arry2 / ARRAY_SIZE_1 - 1;

    float x = (1 - t) * x1 + t * x2;
    float y = (1 - t) * y1 + t * y2;
    return { x, y };
}

std::vector<float>* Function::ComputeVertices(unsigned char* cellScores)
{
    std::vector<float>* vertices = new std::vector<float>();
    for(int y = 0; y < ARRAY_SIZE_1; ++y)
    {
        for(int x = 0; x < ARRAY_SIZE_1; ++x)
        {
            switch(cellScores[y * ARRAY_SIZE_1 + x])
            {
                case 1:
                case 14:
                    LinearInterpolate(x, y, x, y + 1).PushBack(vertices);
                    LinearInterpolate(x, y + 1, x + 1, y + 1).PushBack(vertices);
                    break;
                case 2:
                case 13:
                    LinearInterpolate(x + 1, y, x + 1, y + 1).PushBack(vertices);
                    LinearInterpolate(x, y + 1, x + 1, y + 1).PushBack(vertices);
                    break;
                case 3:
                case 12:
                    LinearInterpolate(x, y, x, y + 1).PushBack(vertices);
                    LinearInterpolate(x + 1, y, x + 1, y + 1).PushBack(vertices);
                    break;
                case 4:
                case 11:
                    LinearInterpolate(x + 1, y, x, y).PushBack(vertices);
                    LinearInterpolate(x + 1, y, x + 1, y + 1).PushBack(vertices);
                    break;
                case 5:
                    LinearInterpolate(x, y, x + 1, y).PushBack(vertices);
                    LinearInterpolate(x, y, x, y + 1).PushBack(vertices);
                    LinearInterpolate(x + 1, y, x + 1, y + 1).PushBack(vertices);
                    LinearInterpolate(x, y + 1, x + 1, y + 1).PushBack(vertices);
                    break;
                case 10:
                    LinearInterpolate(x, y, x + 1, y).PushBack(vertices);
                    LinearInterpolate(x + 1, y, x + 1, y + 1).PushBack(vertices);
                    LinearInterpolate(x, y, x, y + 1).PushBack(vertices);
                    LinearInterpolate(x, y + 1, x + 1, y + 1).PushBack(vertices);
                    break;
                case 6:
                case 9:
                    LinearInterpolate(x, y, x + 1, y).PushBack(vertices);
                    LinearInterpolate(x, y + 1, x + 1, y + 1).PushBack(vertices);
                    break;
                case 7:
                case 8:
                    LinearInterpolate(x, y, x + 1, y).PushBack(vertices);
                    LinearInterpolate(x, y, x, y + 1).PushBack(vertices);
                    break;
                default:
                    break;
            }
        }
    }

    return vertices;
}