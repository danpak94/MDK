#ifndef __mdkKNNReconstructionSparseEncoder_h
#define __mdkKNNReconstructionSparseEncoder_h

#include <string>

#include "mdkFeatureSparseEncoder.h"


namespace mdk
{

template<typename ElementType>
class KNNReconstructionSparseEncoder : public FeatureSparseEncoder<ElementType>
{

private: 

    int64 m_MaxNumberOfNeighbours;

public:

    KNNReconstructionSparseEncoder();

    ~KNNReconstructionSparseEncoder();

    //-----------------------------------------

    void Clear();

    bool SetMaxNumberOfNeighbours(int64 MaxNumberOfNeighbours);

protected:

    bool GenerateCode();

private:
//deleted:
    KNNReconstructionSparseEncoder(const KNNReconstructionSparseEncoder&) = delete;

    void operator=(const KNNReconstructionSparseEncoder&) = delete;

};

}

#include "mdkKNNReconstructionSparseEncoder.hpp"

#endif