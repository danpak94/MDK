#ifndef __mdkKMeansDictionaryBuilder_h
#define __mdkKMeansDictionaryBuilder_h

#include <vector>
#include <string>

#include <opencv2/core/core.hpp>

#include "mdkTypeToOpenCVTranslation.h"

#include "mdkFeatureDictionaryBuilder.h"
#include "mdkFeatureDictionaryForSparseCoding.h"

namespace mdk
{

template<typename Element_Type>
class KMeansDictionaryBuilder : public FeatureDictionaryBuilder<Element_Type>
{
public:
	typedef Element_Type ElementType;

public:
    int_max m_ClusterNumber;

    std::string m_KMeansLibraryName;

private:

    const DenseMatrix<ElementType>* m_FeatureData;

    const FeatureDictionaryForSparseCoding<ElementType>* m_InitialDictionary;

    FeatureDictionaryForSparseCoding<ElementType>* m_Dictionary;

    FeatureDictionaryForSparseCoding<ElementType> m_Dictionary_SharedCopy;

public:

    KMeansDictionaryBuilder();

    ~KMeansDictionaryBuilder();

    void Clear();
  
    void SetInputFeatureData(const DenseMatrix<ElementType>* InputFeatureData);

    void SetInitialDictionary(const FeatureDictionary<ElementType>* InitialDictionary);

    void SetOutputDictionary(FeatureDictionaryForSparseCoding<ElementType>* Dictionary);

    bool CheckInput();

    FeatureDictionaryForSparseCoding<ElementType>* GetOutputDictionary();

private:

    void GenerateDictionary();

    void ClearPipelineOutput();

    void UpdatePipelineOutput();

    void KMeansFirstTimeBuild();

    void KMeansFirstTimeBuild_using_OpenCV();

    void KMeansFirstTimeBuild_using_VLFeat();

    void KMeansOnlineUpdate();

private:
    KMeansDictionaryBuilder(const KMeansDictionaryBuilder&) = delete;

    void operator=(const KMeansDictionaryBuilder&) = delete;

    KMeansDictionaryBuilder(KMeansDictionaryBuilder&&) = delete;

    void operator=(KMeansDictionaryBuilder&&) = delete;
};

}// namespace mdk


#include "mdkKMeansDictionaryBuilder.hpp"

#endif