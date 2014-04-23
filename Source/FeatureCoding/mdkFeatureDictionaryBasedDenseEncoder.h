#ifndef __mdkFeatureDictionaryBasedDenseEncoder_h
#define __mdkFeatureDictionaryBasedDenseEncoder_h

#include <string>

#include "mdkFeatureDictionaryBasedEncoder.h"
#include "mdkFeatureDictionaryForDenseCoding.h"

namespace mdk
{

template<typename ElementType>
class FeatureDictionaryBasedDenseEncoder : public FeatureDictionaryBasedEncoder<ElementType>
{
protected:

    // input:

    const DenseMatrix<ElementType>* m_FeatureData;

    // input 

    const FeatureDictionaryForDenseCoding<ElementType>* m_Dictionary;

    // output:

    DenseMatrix<ElementType>* m_Code;

private:
    DenseMatrix<ElementType> m_Code_SharedCopy;

    //about multithreading:

    int_max m_MinNumberOfDataPerThread;

    int_max m_MaxNumberOfThreads;

protected:
    FeatureDictionaryBasedDenseEncoder();
    virtual ~FeatureDictionaryBasedDenseEncoder();

public:
    //-----------------------------------------

    virtual void Clear();
    
    //-----------------------------------------

    void SetInputFeatureData(const DenseMatrix<ElementType>* InputFeatureData);

    void SetInputDictionary(const FeatureDictionaryForDenseCoding<ElementType>* Dictionary);

    void SetOutputCode(DenseMatrix<ElementType>* Code);

    void SetMaxNumberOfThreads(int_max Number);

    void SetMinNumberOfDataPerThread(int_max Number);

    //-----------------------------------------

    virtual bool CheckInput();

    virtual bool Preprocess();

    virtual bool Postprocess();

    virtual bool Update();

    //----------------------------------------------------//

    DenseMatrix<ElementType>* GetOutputCode();

protected:

    int_max GetNumberOfThreadsTobeCreated();

    int_max GetMinNumberOfDataPerThread();

    int_max GetTotalNumberOfInputFeatureDataVectors();

    void SetupDefaultPipelineOutput();

    void UpdatePipelineOutput();

    virtual void GenerateCode_in_a_Thread(int_max IndexOfFeatureVector_start, int_max IndexOfFeatureVector_end);

private:
//deleted:
    FeatureDictionaryBasedDenseEncoder(const FeatureDictionaryBasedDenseEncoder&) = delete;

    void operator=(const FeatureDictionaryBasedDenseEncoder&) = delete;

};

}

#include "mdkFeatureDictionaryBasedDenseEncoder.hpp"

#endif