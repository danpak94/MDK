#ifndef __mdkFeatureDictionaryBasedEncoder_h
#define __mdkFeatureDictionaryBasedEncoder_h

#include "mdkProcessObject.h"

namespace mdk
{

template<typename DictionaryType, typename CodeType>
class FeatureDictionaryBasedEncoder : public ProcessObject
{
public:
	typedef typename DictionaryType::ScalarType ScalarType;

protected:
    FeatureDictionaryBasedEncoder() {}
    virtual ~FeatureDictionaryBasedEncoder() {}

public:
    virtual bool IsDenseEncoder() const = 0;

    virtual bool IsSparseEncoder() const = 0;

    virtual void Clear() = 0;
    
	virtual void SetInputFeatureData(const DenseMatrix<ScalarType>* InputFeatureData) = 0;
    
	virtual void SetInputDictionary(const DictionaryType* InputDictionary) = 0;

    virtual void SetMaxNumberOfThread(int_max Number) = 0;

    virtual bool Update() = 0;

	virtual CodeType* GetOutputCode() = 0;

private:
//deleted:
    FeatureDictionaryBasedEncoder(const FeatureDictionaryBasedEncoder&) = delete;
    void operator=(const FeatureDictionaryBasedEncoder&) = delete;
};

}

#endif