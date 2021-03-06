#pragma once

namespace mdk
{
template<typename ScalarType>
DistanceMinimizationBasedShapeAligner<ScalarType>::DistanceMinimizationBasedShapeAligner()
{
	this->Clear();
}

template<typename ScalarType>
DistanceMinimizationBasedShapeAligner<ScalarType>::~DistanceMinimizationBasedShapeAligner()
{
}

template<typename ScalarType>
void DistanceMinimizationBasedShapeAligner<ScalarType>::Clear()
{
	m_InputShapeList = nullptr;	
	m_InputSimilarityTable = nullptr;
	m_Flag_use_SimilarityTransform = false;
	m_MaxNeighbourCount = 5;
	m_MaxIterCount = 10;
	m_InputReferenceShapeIndex = 0;
	m_Flag_ComputeObjectiveFunctionValue = false;
	m_MaxThreadCount = 1;
	m_Flag_Parallel_UpdateTransform = false;
	m_OutputTransformList.Clear();
	if (m_OutputTransformList.IsPureEmpty() == true)
	{
		m_OutputTransformList.Recreate();
	}
	m_OutputShapeList.Clear();
	if (m_OutputShapeList.IsPureEmpty() == true)
	{
		m_OutputShapeList.Recreate();
	}
}

template<typename ScalarType>
bool DistanceMinimizationBasedShapeAligner<ScalarType>::CheckInput()
{
	if (m_InputShapeList == nullptr)
	{
		MDK_Error("InputShapeList is nullptr @ DistanceMinimizationBasedShapeAligner::CheckInput()")
		return false;
	}

	if (m_InputShapeList->GetLength() <= 1)
	{
		MDK_Error("InputShapeList is empty or only 1 shape @ DistanceMinimizationBasedShapeAligner::CheckInput()")
		return false;
	}

	for (int_max k = 0; k < m_InputShapeList->GetLength(); ++k)
	{
		const auto& Shape_k = (*m_InputShapeList)[k];
		if (Shape_k.GetRowCount() != 3 && Shape_k.GetRowCount() != 2)
		{
			MDK_Error("input shape is not in 2D or 3D @ DistanceMinimizationBasedShapeAligner::CheckInput(...)")
			return false;
		}
	}

	if (m_InputSimilarityTable == nullptr)
	{
		MDK_Error("InputSimilarityTable is nullptr @ DistanceMinimizationBasedShapeAligner::CheckInput()")
		return false;
	}

	if (m_InputSimilarityTable->IsEmpty() == true)
	{
		MDK_Error("InputSimilarityTable is empty @ DistanceMinimizationBasedShapeAligner::CheckInput()")
		return false;
	}

	if (m_InputSimilarityTable->GetLength() != m_InputShapeList->GetLength())
	{
		MDK_Error("InputSimilarityTable size not the same InputShapeList size @ DistanceMinimizationBasedShapeAligner::CheckInput()")
		return false;
	}

	//check m_InputSimilarityTable to make sure (*m_InputSimilarityTable)[k] is not empty for all k
	for (int_max k = 0; k < m_InputSimilarityTable->GetLength(); ++k)
	{
		if ((*m_InputSimilarityTable)[k].IsEmpty() == true)
		{
			MDK_Error("InputSimilarityTable[" << std::to_string(k) << "] is empty @ DistanceMinimizationBasedShapeAligner::CheckInput()")
			return false;
		}
	}

	if (m_MaxNeighbourCount <= 0 || m_MaxNeighbourCount > m_InputShapeList->GetLength())
	{
		MDK_Error("MaxNeighbourCount is out of range @ DistanceMinimizationBasedShapeAligner::CheckInput()")
		return false;
	}

	if (m_MaxIterCount < 0)
	{
		MDK_Error("MaxIterCount is out of range @ DistanceMinimizationBasedShapeAligner::CheckInput()")
		return false;
	}

	if (m_InputReferenceShapeIndex < 0 || m_InputReferenceShapeIndex >= m_InputShapeList->GetLength())
	{
		MDK_Error("InputReferenceShapeIndex is out of range @ DistanceMinimizationBasedShapeAligner::CheckInput()")
		return false;
	}

	if (m_MaxThreadCount < 0)
	{
		MDK_Error("MaxThreadCount < 0 @ DistanceMinimizationBasedShapeAligner::CheckInput()")
		return false;
	}

	return true;
}

template<typename ScalarType>
void DistanceMinimizationBasedShapeAligner<ScalarType>::Update()
{
	if (this->CheckInput() == false)
	{
		return;
	}

	m_OutputShapeList.Clear();
	m_OutputShapeList = *m_InputShapeList;//copy

	int_max Dimension = m_OutputShapeList[0].GetRowCount();

	m_OutputTransformList.Clear();
	m_OutputTransformList.Resize(m_OutputShapeList.GetLength());
	for (int_max k = 0; k < m_OutputTransformList.GetLength();++k)
	{
		m_OutputTransformList[k].Scale = 1;		
		m_OutputTransformList[k].Rotation.Resize(Dimension, Dimension);
		m_OutputTransformList[k].Rotation.FixSize();
		m_OutputTransformList[k].Rotation.Fill(0);
		m_OutputTransformList[k].Rotation.FillDiagonal(1);		
		m_OutputTransformList[k].Translation.Resize(Dimension, 1);
		m_OutputTransformList[k].Translation.FixSize();
		m_OutputTransformList[k].Translation.Fill(0);		
	}

	if (m_Flag_ComputeObjectiveFunctionValue == true)
	{
		m_ObjectiveFunctionValue.SetCapacity(100);
		m_ObjectiveFunctionValue.Append(this->ComputeObjectiveFunctionValue());
	}

	this->FindInitialTransform();
	
	if (m_MaxIterCount > 0)
	{
		if (m_MaxThreadCount > 1 && m_Flag_Parallel_UpdateTransform == true)
		{
			this->UpdateTransform_parallel();
		}
		else
		{
			this->UpdateTransform_sequential();
		}
	}

	this->AlignToReferenceShape();
}


template<typename ScalarType>
void DistanceMinimizationBasedShapeAligner<ScalarType>::FindInitialTransform()
{
	int_max ShapeCount = m_InputShapeList->GetLength();
	int_max ShapePairCount = ShapeCount*(ShapeCount - 1) / 2;

	//sort shape-pair similarity from the highest to the lowest ----------------------------------//
	DenseMatrix<int_max> ShapePairList;
	ShapePairList.SetCapacity(2, ShapePairCount);
	// pair_n: (shape_i, shape_j)
	//ShapePairList(0,n) : index of shape_i in m_InputShapeList
	//ShapePairList(1,n) : index of shape_j in m_InputShapeList
	{
		DenseVector<ScalarType> SimilarityOfEachPair;		
		SimilarityOfEachPair.SetCapacity(ShapePairCount);
		for (int_max n = 0; n < ShapeCount -1; ++n)
		{
			for (int_max m = n+1; m < ShapeCount; ++m)
			{
				ShapePairList.AppendCol({ n, m });
				SimilarityOfEachPair.Append((*m_InputSimilarityTable)[n][m]);
			}
		}
		auto PairIndexList_sort = SimilarityOfEachPair.Sort("descend");
		
		DenseMatrix<int_max> ShapePairList_sort;
		ShapePairList_sort.Resize(2, ShapePairCount);
		DenseVector<ScalarType> SimilarityOfEachPair_sort;
		SimilarityOfEachPair_sort.Resize(ShapePairCount);
		for (int_max n = 0; n < ShapePairCount; ++n)
		{
			ShapePairList_sort(0, n) = ShapePairList(0, PairIndexList_sort[n]);
			ShapePairList_sort(1, n) = ShapePairList(1, PairIndexList_sort[n]);
			SimilarityOfEachPair_sort[n] = SimilarityOfEachPair[n];
		}
		ShapePairList = std::move(ShapePairList_sort);
		SimilarityOfEachPair= std::move(SimilarityOfEachPair_sort);
	}
	//--------------------------------------------------------------------------------------------//

	ObjectArray<DenseVector<int_max>> ShapeGroup;
	//ShapeGroup[k]: shape index list of group-k 
	// initialization: each shape is in a different group
	ShapeGroup.Resize(ShapeCount);
	for (int_max n = 0; n < ShapeCount; ++n)
	{
		ShapeGroup[n] = n;
	}

	DenseVector<int_max> GroupIndexOfEachShape;
	//GroupIndexOfEachShape[n]: Group Index of Shape_n, n is Shape Index in m_InputShapeList
	// initialization: each shape is in a different group
	GroupIndexOfEachShape.Resize(ShapeCount);
	for (int_max n = 0; n < ShapeCount; ++n)
	{
		GroupIndexOfEachShape[n] = n;
	}

	for (int_max n = 0; n < ShapePairCount; ++n)
	{
		auto ShapeIndex_a = ShapePairList(0, n);	
		auto ShapeIndex_b = ShapePairList(1, n);

		auto GroupIndex_a = GroupIndexOfEachShape[ShapeIndex_a];
		auto ShapeCount_in_Group_a = ShapeGroup[GroupIndex_a].GetLength();
	
		auto GroupIndex_b = GroupIndexOfEachShape[ShapeIndex_b];
		auto ShapeCount_in_Group_b = ShapeGroup[GroupIndex_b].GetLength();

		if (GroupIndex_a != GroupIndex_b)//Not in the same group -> not aligned
		{
			int_max ShapeIndex_ref = -1;
			int_max ShapeIndex_moving = -1;
			int_max GroupIndex_ref = -1;
			int_max GroupIndex_moving = -1;
			DenseVector<int_max> Group_ref, Group_moving;
			// transform Shape in Group_moving to Group_ref

			if (ShapeCount_in_Group_a >= ShapeCount_in_Group_b)
			{// transform all the shape in group-b to group-a
				ShapeIndex_ref = ShapeIndex_a;
				ShapeIndex_moving = ShapeIndex_b;
				GroupIndex_ref = GroupIndex_a;
				GroupIndex_moving = GroupIndex_b;
				Group_ref = ShapeGroup[GroupIndex_a];
				Group_moving = ShapeGroup[GroupIndex_b];
			}
			else
			{// transform all the shape in group-a to group-b
				ShapeIndex_ref = ShapeIndex_b;
				ShapeIndex_moving = ShapeIndex_a;
				GroupIndex_ref = GroupIndex_b;
				GroupIndex_moving = GroupIndex_a;
				Group_ref = ShapeGroup[GroupIndex_b];
				Group_moving = ShapeGroup[GroupIndex_a];
			}

			// Estimate transform from Shape_moving (X) to Shape_ref (Y):  Y ~ R*s*X+t
			// transfrom every shape in Group_moving (ShapeIndex_moving)
			auto TransformParameter = this->EstimateTransformParameter(m_OutputShapeList[ShapeIndex_moving], m_OutputShapeList[ShapeIndex_ref]);			
			for (int_max k = 0; k < Group_moving.GetLength(); ++k)
			{
				auto& Shape_k = m_OutputShapeList[Group_moving[k]];
				Shape_k = this->TransformShape(Shape_k, TransformParameter);
			}
			//move shape in Group_moving to Group_ref
			//update GroupIndexOfEachShape
			for (int_max k = 0; k < Group_moving.GetLength(); ++k)
			{
				GroupIndexOfEachShape[Group_moving[k]] = GroupIndex_ref;
			}
			// update m_OutputTransformList
			const auto& Rotation = TransformParameter.Rotation;
			const auto& Scale = TransformParameter.Scale;
			const auto& Translation = TransformParameter.Translation;
			for (int_max k = 0; k < Group_moving.GetLength(); ++k)
			{
				m_OutputTransformList[Group_moving[k]].Scale *= Scale;
				auto Rotation_old = m_OutputTransformList[Group_moving[k]].Rotation;
				m_OutputTransformList[Group_moving[k]].Rotation = Rotation*Rotation_old;
				auto Translation_old = m_OutputTransformList[Group_moving[k]].Translation;
				m_OutputTransformList[Group_moving[k]].Translation = Scale*Rotation*Translation_old + Translation;
			}
			//update ShapeGroup
			ShapeGroup[GroupIndex_ref].Append(ShapeGroup[GroupIndex_moving]);
			ShapeGroup[GroupIndex_moving].Clear();
		}
	}

	if (m_Flag_ComputeObjectiveFunctionValue == true)
	{
		m_ObjectiveFunctionValue.Append(this->ComputeObjectiveFunctionValue());
	}
}


template<typename ScalarType>
void DistanceMinimizationBasedShapeAligner<ScalarType>::UpdateTransform_sequential()
{
	for (int_max Iter = 0; Iter < m_MaxIterCount; ++Iter)
	{
		int_max ShapeCount = m_OutputShapeList.GetLength();
		for (int_max k = 0; k < ShapeCount; ++k)
		{
			auto NeighbourShapeIndexList = (*m_InputSimilarityTable)[k].IndexList();
			auto NeighbourSimilarityList = (*m_InputSimilarityTable)[k].ElementList();	

			// exclude self in NeighbourShapeIndexList
			/*
			{
				auto tempIndex = NeighbourShapeIndexList.ExactMatch("first", k);
				if (tempIndex >= 0)
				{
					NeighbourShapeIndexList.Delete(tempIndex);
					NeighbourSimilarityList.Delete(tempIndex);
				}				
			}
			if (NeighbourShapeIndexList.GetLength() > m_MaxNeighbourCount)
			{
				auto RelativeIndexList_sort = NeighbourSimilarityList.Sort("descend");
				RelativeIndexList_sort = RelativeIndexList_sort.GetSubSet(0, m_MaxNeighbourCount - 1);
				NeighbourShapeIndexList = NeighbourShapeIndexList.GetSubSet(RelativeIndexList_sort);
				NeighbourSimilarityList = NeighbourSimilarityList.GetSubSet(RelativeIndexList_sort);
			}
			*/

			// include self in NeighbourShapeIndexList			
			{
				auto tempIndex = NeighbourShapeIndexList.ExactMatch("first", k);
				if (tempIndex < 0)
				{
					NeighbourShapeIndexList.Append(k);
					NeighbourSimilarityList.Append(1);
				}
			}
			if (NeighbourShapeIndexList.GetLength() > m_MaxNeighbourCount + 1)
			{
				auto RelativeIndexList_sort = NeighbourSimilarityList.Sort("descend");
				RelativeIndexList_sort = RelativeIndexList_sort.GetSubSet(0, m_MaxNeighbourCount);
				NeighbourShapeIndexList = NeighbourShapeIndexList.GetSubSet(RelativeIndexList_sort);
				NeighbourSimilarityList = NeighbourSimilarityList.GetSubSet(RelativeIndexList_sort);
			}
			
			//normalize NeighbourSimilarityList
			auto tempSum = NeighbourSimilarityList.Sum();
			auto EPS = std::numeric_limits<ScalarType>::epsilon();
			if (tempSum <= m_MaxNeighbourCount*EPS)
			{
				NeighbourSimilarityList.Fill(1);				
			}
			NeighbourSimilarityList /= ScalarType(NeighbourSimilarityList.GetLength());

			//get mean shape of neighbour
			DenseMatrix<ScalarType> MeanShape;
			MeanShape.Resize(m_OutputShapeList[0].GetSize());
			MeanShape.Fill(0);
			for (int_max n = 0; n < NeighbourShapeIndexList.GetLength(); ++n)
			{
				const auto& Shape_n = m_OutputShapeList[NeighbourShapeIndexList[n]];
				for (int_max m = 0; m < Shape_n.GetElementCount(); ++m)
				{
					MeanShape[m] += NeighbourSimilarityList[n] * Shape_n[m];
				}
			}

			//transfrom from Shape_k (X) to MeanShape (Y): :  Y ~ R*s*X+t
			auto& Shape_k = m_OutputShapeList[k];
			auto TransformParameter = this->EstimateTransformParameter(Shape_k, MeanShape);
			Shape_k = this->TransformShape(Shape_k, TransformParameter);
			// update m_OutputTransformList
			const auto& Rotation = TransformParameter.Rotation;
			const auto& Scale = TransformParameter.Scale;
			const auto& Translation = TransformParameter.Translation;
			m_OutputTransformList[k].Scale *= Scale;
			auto Rotation_old = m_OutputTransformList[k].Rotation;
			m_OutputTransformList[k].Rotation = Rotation*Rotation_old;
			auto Translation_old = m_OutputTransformList[k].Translation;
			m_OutputTransformList[k].Translation = Scale*Rotation*Translation_old + Translation;
		}

		if (m_Flag_ComputeObjectiveFunctionValue == true)
		{
			m_ObjectiveFunctionValue.Append(this->ComputeObjectiveFunctionValue());
		}
	}
}


template<typename ScalarType>
void DistanceMinimizationBasedShapeAligner<ScalarType>::UpdateTransform_parallel()
{
	//copy data
	auto OutputShapeList_updated = m_OutputShapeList;

	for (int_max Iter = 0; Iter < m_MaxIterCount; ++Iter)
	{		
		int_max ShapeCount = m_OutputShapeList.GetLength();
		//for (int_max k = 0; k <= ShapeCount-1; ++k)
		auto TempFunction = [&](int_max k)
		{
			auto NeighbourShapeIndexList = (*m_InputSimilarityTable)[k].IndexList();
			auto NeighbourSimilarityList = (*m_InputSimilarityTable)[k].ElementList();

			// exclude self in NeighbourShapeIndexList
			/*
			{
			auto tempIndex = NeighbourShapeIndexList.ExactMatch("first", k);
			if (tempIndex >= 0)
			{
			NeighbourShapeIndexList.Delete(tempIndex);
			NeighbourSimilarityList.Delete(tempIndex);
			}
			}
			if (NeighbourShapeIndexList.GetLength() > m_MaxNeighbourCount)
			{
			auto RelativeIndexList_sort = NeighbourSimilarityList.Sort("descend");
			RelativeIndexList_sort = RelativeIndexList_sort.GetSubSet(0, m_MaxNeighbourCount - 1);
			NeighbourShapeIndexList = NeighbourShapeIndexList.GetSubSet(RelativeIndexList_sort);
			NeighbourSimilarityList = NeighbourSimilarityList.GetSubSet(RelativeIndexList_sort);
			}
			*/

			// include self in NeighbourShapeIndexList			
			{
				auto tempIndex = NeighbourShapeIndexList.ExactMatch("first", k);
				if (tempIndex < 0)
				{
					NeighbourShapeIndexList.Append(k);
					NeighbourSimilarityList.Append(1);
				}
			}
			if (NeighbourShapeIndexList.GetLength() > m_MaxNeighbourCount + 1)
			{
				auto RelativeIndexList_sort = NeighbourSimilarityList.Sort("descend");
				RelativeIndexList_sort = RelativeIndexList_sort.GetSubSet(0, m_MaxNeighbourCount);
				NeighbourShapeIndexList = NeighbourShapeIndexList.GetSubSet(RelativeIndexList_sort);
				NeighbourSimilarityList = NeighbourSimilarityList.GetSubSet(RelativeIndexList_sort);
			}

			//normalize NeighbourSimilarityList
			auto tempSum = NeighbourSimilarityList.Sum();
			auto EPS = std::numeric_limits<ScalarType>::epsilon();
			if (tempSum <= m_MaxNeighbourCount*EPS)
			{
				NeighbourSimilarityList.Fill(1);
			}
			NeighbourSimilarityList /= ScalarType(NeighbourSimilarityList.GetLength());

			//get mean shape of neighbour
			DenseMatrix<ScalarType> MeanShape;
			MeanShape.Resize(m_OutputShapeList[0].GetSize());
			MeanShape.Fill(0);
			for (int_max n = 0; n < NeighbourShapeIndexList.GetLength(); ++n)
			{
				const auto& Shape_n = m_OutputShapeList[NeighbourShapeIndexList[n]];
				for (int_max m = 0; m < Shape_n.GetElementCount(); ++m)
				{
					MeanShape[m] += NeighbourSimilarityList[n] * Shape_n[m];
				}
			}

			//transfrom from Shape_k (X) to MeanShape (Y): :  Y ~ R*s*X+t
			auto& Shape_k = OutputShapeList_updated[k];
			auto TransformParameter = EstimateTransformParameter(Shape_k, MeanShape);
			Shape_k = this->TransformShape(Shape_k, TransformParameter);
			// update m_OutputTransformList
			const auto& Rotation = TransformParameter.Rotation;
			const auto& Scale = TransformParameter.Scale;
			const auto& Translation = TransformParameter.Translation;
			m_OutputTransformList[k].Scale *= Scale;
			auto Rotation_old = m_OutputTransformList[k].Rotation;
			m_OutputTransformList[k].Rotation = Rotation*Rotation_old;
			auto Translation_old = m_OutputTransformList[k].Translation;
			m_OutputTransformList[k].Translation = Scale*Rotation*Translation_old + Translation;
		};
		ParallelForLoop(TempFunction, 0, ShapeCount - 1, m_MaxThreadCount);

		m_OutputShapeList = OutputShapeList_updated;

		if (m_Flag_ComputeObjectiveFunctionValue == true)
		{
			m_ObjectiveFunctionValue.Append(this->ComputeObjectiveFunctionValue());
		}		
	}
}


template<typename ScalarType>
void DistanceMinimizationBasedShapeAligner<ScalarType>::AlignToReferenceShape()
{
	const auto& Shape_input = (*m_InputShapeList)[m_InputReferenceShapeIndex];
	const auto& Shape_output = m_OutputShapeList[m_InputReferenceShapeIndex];
	int_max ShapeCount = m_InputShapeList->GetLength();

	auto TransformParameter = EstimateTransformParameter(Shape_output, Shape_input);
	const auto& Rotation = TransformParameter.Rotation;
	const auto& Scale = TransformParameter.Scale;
	const auto& Translation = TransformParameter.Translation;

	//for (int_max k = 0; k <= ShapeCount-1; ++k)
	auto TempFunction = [&](int_max k)
	{
		//transfrom from Shape_k (X) to Shape_ref (Y): :  Y ~ R*s*X+t
		auto& Shape_k = m_OutputShapeList[k];
		Shape_k = this->TransformShape(Shape_k, TransformParameter);
		// update m_OutputTransformList
		m_OutputTransformList[k].Scale *= Scale;
		auto Rotation_old = m_OutputTransformList[k].Rotation;
		m_OutputTransformList[k].Rotation = Rotation*Rotation_old;
		auto Translation_old = m_OutputTransformList[k].Translation;
		m_OutputTransformList[k].Translation = Scale*Rotation*Translation_old + Translation;
	};
	ParallelForLoop(TempFunction, 0, ShapeCount - 1, m_MaxThreadCount);

	if (m_Flag_ComputeObjectiveFunctionValue == true)
	{
		m_ObjectiveFunctionValue.Append(this->ComputeObjectiveFunctionValue());
	}
}


template<typename ScalarType>
ScalarType DistanceMinimizationBasedShapeAligner<ScalarType>::ComputeObjectiveFunctionValue()
{
	ScalarType RMSE = 0;
	ScalarType SimilaritySum = 0;
	int_max ShapeCount = m_OutputShapeList.GetLength();
	for (int_max n = 0; n < ShapeCount - 1; ++n)
	{
		auto NeighbourShapeIndexList = (*m_InputSimilarityTable)[n].IndexList();
		auto NeighbourSimilarityList = (*m_InputSimilarityTable)[n].ElementList();
		// exclude self in NeighbourShapeIndexList		
		{
			auto tempIndex = NeighbourShapeIndexList.ExactMatch("first", n);
			if (tempIndex >= 0)
			{
				NeighbourShapeIndexList.Delete(tempIndex);
				NeighbourSimilarityList.Delete(tempIndex);
			}
		}
		if (NeighbourShapeIndexList.GetLength() > m_MaxNeighbourCount)
		{
			auto RelativeIndexList_sort = NeighbourSimilarityList.Sort("descend");
			RelativeIndexList_sort = RelativeIndexList_sort.GetSubSet(0, m_MaxNeighbourCount - 1);
			NeighbourShapeIndexList = NeighbourShapeIndexList.GetSubSet(RelativeIndexList_sort);
			NeighbourSimilarityList = NeighbourSimilarityList.GetSubSet(RelativeIndexList_sort);
		}

		//normalize NeighbourSimilarityList
		auto tempSum = NeighbourSimilarityList.Sum();
		auto EPS = std::numeric_limits<ScalarType>::epsilon();
		if (tempSum <= m_MaxNeighbourCount*EPS)
		{
			NeighbourSimilarityList.Fill(1);
		}
		NeighbourSimilarityList /= ScalarType(NeighbourSimilarityList.GetLength());
		
		const auto& Shape_n = m_OutputShapeList[n];
		for (int_max m = n + 1; m < ShapeCount; ++m)
		{
			const auto& Shape_m = m_OutputShapeList[m];
			auto Similarity_nm = (*m_InputSimilarityTable)[n][m];
			ScalarType MSE_nm = 0;
			for (int_max k = 0; k < Shape_m.GetElementCount(); ++k)
			{
				MSE_nm += (Shape_n[k] - Shape_m[k])*(Shape_n[k] - Shape_m[k]);
			}
			MSE_nm /= ScalarType(Shape_m.GetColCount());
			RMSE += Similarity_nm*MSE_nm;
			SimilaritySum += Similarity_nm;
		}
	}
	RMSE /= ScalarType(SimilaritySum);
	RMSE = std::sqrt(RMSE);
	return RMSE;
}


template<typename ScalarType>
Parameter_Of_SimilarityTransform<ScalarType>
DistanceMinimizationBasedShapeAligner<ScalarType>::EstimateTransformParameter(const DenseMatrix<ScalarType>& SourceShape, const DenseMatrix<ScalarType>& TagetShape)
{
	Parameter_Of_SimilarityTransform<ScalarType> TransformParameter;
	
	if (m_Flag_use_SimilarityTransform == true)
	{
		int_max Dimension = SourceShape.GetRowCount();
		if (Dimension == 3)
		{
			SimilarityTransform3D<ScalarType> Transform;
			if (m_LandmarkOnShape.IsEmpty() == true)
			{
				Transform.SetSourceLandmarkPointSet(&SourceShape);
				Transform.SetTargetLandmarkPointSet(&TagetShape);
				Transform.EstimateParameter();
				TransformParameter.Rotation = Transform.GetRotationMatrix();
				TransformParameter.Scale = Transform.GetScale();
				TransformParameter.Translation = Transform.GetTranslation_After_Scale_Rotation();
			}
			else
			{
				auto Source = SourceShape.GetSubMatrix(ALL, m_LandmarkOnShape);
				auto Target = TagetShape.GetSubMatrix(ALL, m_LandmarkOnShape);
				Transform.SetSourceLandmarkPointSet(&Source);
				Transform.SetTargetLandmarkPointSet(&Target);
				Transform.EstimateParameter();
				TransformParameter.Rotation = Transform.GetRotationMatrix();
				TransformParameter.Scale = Transform.GetScale();
				TransformParameter.Translation = Transform.GetTranslation_After_Scale_Rotation();
			}
		}
		else//Dimension == 2
		{
			SimilarityTransform2D<ScalarType> Transform;
			if (m_LandmarkOnShape.IsEmpty() == true)
			{
				Transform.SetSourceLandmarkPointSet(&SourceShape);
				Transform.SetTargetLandmarkPointSet(&TagetShape);
				Transform.EstimateParameter();
				TransformParameter.Rotation = Transform.GetRotationMatrix();
				TransformParameter.Scale = Transform.GetScale();
				TransformParameter.Translation = Transform.GetTranslation_After_Scale_Rotation();
			}
			else
			{
				auto Source = SourceShape.GetSubMatrix(ALL, m_LandmarkOnShape);
				auto Target = TagetShape.GetSubMatrix(ALL, m_LandmarkOnShape);
				Transform.SetSourceLandmarkPointSet(&Source);
				Transform.SetTargetLandmarkPointSet(&Target);
				Transform.EstimateParameter();
				TransformParameter.Rotation = Transform.GetRotationMatrix();
				TransformParameter.Scale = Transform.GetScale();
				TransformParameter.Translation = Transform.GetTranslation_After_Scale_Rotation();
			}
		}
	}
	else
	{
		int_max Dimension = SourceShape.GetRowCount();
		if (Dimension == 3)
		{
			RigidTransform3D<ScalarType> Transform;
			if (m_LandmarkOnShape.IsEmpty() == true)
			{
				Transform.SetSourceLandmarkPointSet(&SourceShape);
				Transform.SetTargetLandmarkPointSet(&TagetShape);
				Transform.EstimateParameter();
				TransformParameter.Rotation = Transform.GetRotationMatrix();
				TransformParameter.Scale = 1;
				TransformParameter.Translation = Transform.GetTranslation_AfterRotation();
			}
			else
			{
				auto Source = SourceShape.GetSubMatrix(ALL, m_LandmarkOnShape);
				auto Target = TagetShape.GetSubMatrix(ALL, m_LandmarkOnShape);
				Transform.SetSourceLandmarkPointSet(&Source);
				Transform.SetTargetLandmarkPointSet(&Target);
				Transform.EstimateParameter();
				TransformParameter.Rotation = Transform.GetRotationMatrix();
				TransformParameter.Scale = 1;
				TransformParameter.Translation = Transform.GetTranslation_AfterRotation();
			}
		}
		else//Dimension == 2
		{
			RigidTransform2D<ScalarType> Transform;
			if (m_LandmarkOnShape.IsEmpty() == true)
			{
				Transform.SetSourceLandmarkPointSet(&SourceShape);
				Transform.SetTargetLandmarkPointSet(&TagetShape);
				Transform.EstimateParameter();
				TransformParameter.Rotation = Transform.GetRotationMatrix();
				TransformParameter.Scale = 1;
				TransformParameter.Translation = Transform.GetTranslation_AfterRotation();
			}
			else
			{
				auto Source = SourceShape.GetSubMatrix(ALL, m_LandmarkOnShape);
				auto Target = TagetShape.GetSubMatrix(ALL, m_LandmarkOnShape);
				Transform.SetSourceLandmarkPointSet(&Source);
				Transform.SetTargetLandmarkPointSet(&Target);
				Transform.EstimateParameter();
				TransformParameter.Rotation = Transform.GetRotationMatrix();
				TransformParameter.Scale = 1;
				TransformParameter.Translation = Transform.GetTranslation_AfterRotation();
			}
		}
	}

	return TransformParameter;
}


template<typename ScalarType>
DenseMatrix<ScalarType> DistanceMinimizationBasedShapeAligner<ScalarType>::TransformShape(const DenseMatrix<ScalarType>& Shape, const Parameter_Of_SimilarityTransform<ScalarType>& Parameter)
{
	if (m_Flag_use_SimilarityTransform == true)
	{
		int_max Dimension = Shape.GetRowCount();
		if (Dimension == 3)
		{
			SimilarityTransform3D<ScalarType> Transform;
			Transform.SetRotationMatrix(Parameter.Rotation);
			Transform.SetScale(Parameter.Scale);
			Transform.SetTranslation_After_Scale_Rotation(Parameter.Translation);
			return Transform.TransformPoint(Shape);
		}
		else//Dimension == 2
		{
			SimilarityTransform2D<ScalarType> Transform;
			Transform.SetRotationMatrix(Parameter.Rotation);
			Transform.SetScale(Parameter.Scale);
			Transform.SetTranslation_After_Scale_Rotation(Parameter.Translation);
			return Transform.TransformPoint(Shape);
		}

	}
	else
	{
		int_max Dimension = Shape.GetRowCount();
		if (Dimension == 3)
		{
			RigidTransform3D<ScalarType> Transform;
			Transform.SetRotationMatrix(Parameter.Rotation);
			Transform.SetTranslation_AfterRotation(Parameter.Translation);
			return Transform.TransformPoint(Shape);
		}
		else//Dimension == 2
		{
			RigidTransform2D<ScalarType> Transform;
			Transform.SetRotationMatrix(Parameter.Rotation);
			Transform.SetTranslation_AfterRotation(Parameter.Translation);
			return Transform.TransformPoint(Shape);
		}
	}
}

//---------------------------------------------------------- public static function ---------------------------------------------------------------------------//
template<typename ScalarType>
ObjectArray<SparseVector<ScalarType>> DistanceMinimizationBasedShapeAligner<ScalarType>::ComputeShapeSimilarity(const ObjectArray<DenseMatrix<ScalarType>>& ShapeList, const std::string& TransformName, bool Flag_Symmetry, ScalarType SimilarityThreshold, int_max MaxThreadCount)
{
	DenseVector<int_max> Landmark;
	return DistanceMinimizationBasedShapeAligner<ScalarType>::ComputeShapeSimilarity(ShapeList, Landmark, TransformName, Flag_Symmetry, SimilarityThreshold, MaxThreadCount);
}

template<typename ScalarType>
ScalarType ComputeShapeSimilarity(const DenseMatrix<ScalarType>& ShapeA, const DenseMatrix<ScalarType>& ShapeB, std::string& TransformName, bool Flag_Symmetry)
{
	return ComputeSimilarityBetweenShapeWithPointCorrespondence(ShapeA, ShapeB, TransformName, Flag_Symmetry);
}

template<typename ScalarType>
ObjectArray<SparseVector<ScalarType>> DistanceMinimizationBasedShapeAligner<ScalarType>::ComputeShapeSimilarity(const ObjectArray<DenseMatrix<ScalarType>>& ShapeList, const DenseVector<int_max>& Landmark, const std::string& TransformName, bool Flag_Symmetry, ScalarType SimilarityThreshold, int_max MaxThreadCount)
{
	ObjectArray<SparseVector<ScalarType>> SimilarityTable;
	int_max ShapeCount = ShapeList.GetLength();
	if (ShapeCount == 0)
	{
		return SimilarityTable;
	}
	SimilarityTable.Resize(ShapeCount);	

	//for (int_max n = 0; n <= ShapeCount-1; ++n)
	auto TempFunction = [&](int_max n)
	{		
		SimilarityTable[n].Resize(ShapeCount);
		SimilarityTable[n].SetElement(n, 1);
		for (int_max m = n+1; m < ShapeCount; ++m)
		{
			// compute similarity between Shape_n and Shape_m
			auto Similarity = ComputeSimilarityBetweenShapeWithPointCorrespondence(ShapeList[n], ShapeList[m], Landmark, TransformName, Flag_Symmetry);
			if (Similarity > SimilarityThreshold)
			{
				SimilarityTable[n].SetElement(m, Similarity);				
			}
		}
	};
	ParallelForLoop(TempFunction, 0, ShapeCount-1, MaxThreadCount);
	
	for (int_max n = 0; n < ShapeCount; ++n)
	{				
		for (int_max m = n + 1; m < ShapeCount; ++m)
		{
			SimilarityTable[m].SetElement(n, SimilarityTable[n][m]);
		}
	}
	return SimilarityTable;
}

template<typename ScalarType>
ScalarType DistanceMinimizationBasedShapeAligner<ScalarType>::ComputeShapeSimilarity(const DenseMatrix<ScalarType>& ShapeA, const DenseMatrix<ScalarType>& ShapeB, const DenseVector<int_max>& Landmark, const std::string& TransformName, bool Flag_Symmetry)
{
	return ComputeSimilarityBetweenShapeWithPointCorrespondence(ShapeA, ShapeB, Landmark, TransformName, Flag_Symmetry);
}

}//namespace mdk