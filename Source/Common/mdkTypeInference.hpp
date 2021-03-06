﻿#pragma once


namespace mdk
{

template<typename ScalarType>
inline
ScalarTypeEnum GetScalarType(const ScalarType& Scalar)
{
	String TypeName(typeid(Scalar).name());

    if (TypeName == "double")
    {
        return ScalarTypeEnum::DOUBLE;
    }
    else if (TypeName == "float")
    {
        return ScalarTypeEnum::FLOAT;
    }
    else if (TypeName == "signed char")
    {
        return ScalarTypeEnum::INT8;
    }
    else if (TypeName == "short")
    {
        return ScalarTypeEnum::INT16;
    }
    else if (TypeName == "int")
    {
        return ScalarTypeEnum::INT32;
    }
    else if (TypeName == "__int64") // OS is Windows
    {
        return ScalarTypeEnum::INT64;
    }
    else if (TypeName == "unsigned char")
    {
        return ScalarTypeEnum::UINT8;
    }
    else if (TypeName == "unsigned short")
    {
        return ScalarTypeEnum::UINT16;
    }
    else if (TypeName == "unsigned int")
    {
        return ScalarTypeEnum::UINT32;
    }
    else if (TypeName == "unsigned __int64") // OS is Windows
    {
        return ScalarTypeEnum::UINT64;
    }

    return ScalarTypeEnum::UNKNOWN;
}


template<typename ScalarType>
inline
String GetScalarTypeName(const ScalarType& Scalar)
{
    String OutputName;

    String TypeName(typeid(Scalar).name());

    if (TypeName == "double")
    {
		OutputName = "double";
    }
    else if (TypeName == "float")
    {
		OutputName = "float";
    }
    else if (TypeName == "signed char")
    {
		OutputName = "int8";
    }
    else if (TypeName == "short")
    {
		OutputName = "int16";
    }
    else if (TypeName == "int")
    {
		OutputName = "int32";
    }
    else if (TypeName == "__int64") // OS is Windows
    {
		OutputName = "int64";
    }
    else if (TypeName == "unsigned char")
    {
		OutputName = "uint8";
    }
    else if (TypeName == "unsigned short")
    {
		OutputName = "uint16";
    }
    else if (TypeName == "unsigned int")
    {
		OutputName = "uint32";
    }
    else if (TypeName == "unsigned __int64") // OS is Windows
    {
		OutputName = "uint64";
    }
    else
    {
		OutputName = "unknown";
    }

	return OutputName;
}


template<typename ScalarType>
inline
int_max GetByteCountOfScalar(const ScalarType& Scalar)
{
    MDK_Error("Unknown ScalarType @ GetByteCountOfScalar(ScalarType Scalar)")
    return 0;
}

inline int_max GetByteCountOfScalar(const double&)
{
    return sizeof(double);
}

inline int_max GetByteCountOfScalar(const float&)
{
    return sizeof(float);
}

inline int_max GetByteCountOfScalar(const char&)
{
    return sizeof(char);
}

inline int_max GetByteCountOfScalar(const short&)
{
    return sizeof(short);
}

inline int_max GetByteCountOfScalar(const int&)
{
    return sizeof(int);
}

inline int_max GetByteCountOfScalar(const long&)
{
    return sizeof(long);
}


inline int_max GetByteCountOfScalar(const long long&)
{
    return sizeof(long long);
}


inline int_max GetByteCountOfScalar(const unsigned char&)
{
    return sizeof(unsigned char);
}

inline int_max GetByteCountOfScalar(const unsigned short&)
{
    return sizeof(unsigned short);
}

inline int_max GetByteCountOfScalar(const unsigned int&)
{
    return sizeof(unsigned int);
}

inline int_max GetByteCountOfScalar(const unsigned long&)
{
    return sizeof(unsigned long);
}

inline int_max GetByteCountOfScalar(const unsigned long long&)
{
    return sizeof(unsigned long long);
}

inline int_max GetByteCountOfScalar(const bool&)
{
    return sizeof(bool);
}

inline int_max GetByteCountOfScalar(const wchar_t&)
{
    return sizeof(wchar_t);
}

// unsigned short 
//inline int_max GetByteCountOfScalar(const char16_t&)
//{
//    return sizeof(char16_t);
//}

// unsigned int
//inline int_max GetByteCountOfScalar(const char32_t&)
//{
//   return sizeof(char32_t);
//}

// unsigned long long
//inline int_max GetByteCountOfScalar(const size_t&)
//{
//    return sizeof(size_t);
//}

//-------------------------------------------------------------//

template<typename TemplateClassType>
String GetTemplateClassName(const TemplateClassType& TemplateClassObject)
{
    String ObjectName = typeid(TemplateClassObject).name();

    int_max Index_start = 0;
    int_max Index_end = 0;

    for (int_max k = 0; k < ObjectName.GetLength(); ++k)
    {
        if (ObjectName[k] == ' ')
        {
            Index_start = k + 1;
        }
        else if (ObjectName[k] == '<')
        {
            Index_end = k - 1;
        }
    }

    auto Length = Index_end - Index_start + 1;

    String TemplateClassName;
    TemplateClassName.Resize(Length);

    for (int_max k = 0; k < Length; ++k)
    {
        TemplateClassName[k] = ObjectName[Index_start + k];
    }

    return TemplateClassName;
}

}//end namespace mdk

