#include "xllFunctionSupport.h"

#include <boost\algorithm\string.hpp>


bool getPutCall(char *putOrCall, PutCall &type, string &errorMessage)
{
	string optionType(putOrCall);
	boost::to_lower(optionType);
	if ((optionType.compare("c") == 0) || (optionType.compare("call") == 0))
	{
		type = CALL;
	}
	else if ((optionType.compare("p") == 0) || (optionType.compare("put") == 0))
	{
		type = PUT;
	}
	else 
	{
		errorMessage = "Option type must be either (P)ut or (C)all";
		return false;
	}
	return true;
}


/*======================================================================================
returnXloperOnError

=======================================================================================*/
xloper* returnXloperOnError(string errorMessage)
{
    cpp_xloper outputMatrix(1, 1);
    outputMatrix.SetArrayElement(0, 0, (char*)errorMessage.c_str());
    return outputMatrix.ExtractXloper(false);
}

/*======================================================================================
returnXloper

=======================================================================================*/
xloper* returnXloper(double returnValue)
{
	cpp_xloper outputMatrix(1, 1);
	outputMatrix.SetArrayElement(0, 0, returnValue);
	return outputMatrix.ExtractXloper(false);
}

/*======================================================================================
constructVector

Take and *xl_array and turn it into a vector and detail about the success (or not) of
this opperation
=======================================================================================*/
bool constructVector(xl_array* xlArray, vector<double> &outputVector, string &errorMessage)
{
    bool successful = true;
    errorMessage = "";
    WORD xlArray_rows, xlArray_columns;
    cpp_xloper cppXloper(xlArray);
    cppXloper.GetArraySize(xlArray_rows, xlArray_columns);
    if (!((xlArray_columns == 1) || (xlArray_rows == 1)))
    {
        successful = false;
        errorMessage = "Input not a vector";
        return false;
    }
    double rate;
    outputVector = vector<double>();
	if (xlArray_columns == 1)
	{
		for (WORD i = 0; i < xlArray_rows; ++i)
		{
			cppXloper.GetArrayElement(i, 0, rate);
			outputVector.push_back(rate);
		}
	}
	else
	{
		for (WORD i = 0; i < xlArray_columns; ++i)
		{
			cppXloper.GetArrayElement(0, i, rate);
			outputVector.push_back(rate);
		}
	}
    return successful;
}

/*======================================================================================
extractDataFromSurface

Take and *xl_array and turn it into a vector or vectors, creating detail about the success
(or not) of this opperation.
=======================================================================================*/
bool extractDataFromSurface(
    xl_array* surfaceInput,
	bool transpose,
    vector<vector<double>> &data,
    string &errorMessage)
{
    bool successful = true;
    errorMessage = "No error";

    WORD xlArray_rows, xlArray_columns;
    cpp_xloper cppXloper(surfaceInput);
    cppXloper.GetArraySize(xlArray_rows, xlArray_columns);
    if ((xlArray_columns < 2) || (xlArray_rows < 2))
    {
        successful = false;
        errorMessage = "Input surface must contain at least 2 rows and 2 columns";
        data.clear();
        return successful;
    }
    double value;
	vector<double> zeroVector;
	data.clear();
	if (transpose)
	{
		zeroVector = vector<double>(xlArray_rows, 0);
		for (WORD i = 0; i < xlArray_columns; ++i)
		{
			data.push_back(zeroVector);
			for (WORD j = 0; j < xlArray_rows; ++j)
			{
				if (!cppXloper.GetArrayElement(j, i, value))
				{
					successful = false;
					errorMessage = "At least one data point is not a numeric value";
					return successful;
				}
				data[i][j] = (value);
			}
		}
	}
	else
	{
		zeroVector = vector<double>(xlArray_columns, 0);
		for (WORD i = 0; i < xlArray_rows; ++i)
		{
			data.push_back(zeroVector);
			for (WORD j = 0; j < xlArray_columns; ++j)
			{
				if (!cppXloper.GetArrayElement(i, j, value))
				{
					successful = false;
					errorMessage = "At least one data point is not a numeric value";
					return successful;
				}
				data[i][j] = (value);
			}
		}
	}
    return successful;
}

/*======================================================================================
extractDataFromSurface

Same as previous function but make allowance for column and row headings
=======================================================================================*/
bool extractDataFromSurface(
    xl_array* surfaceInput,
    vector<double> &columnHeadings,
    vector<double> &rowHeadings,
    vector<vector<double>> &data,
    string &errorMessage)
{
    bool successful = true;
    errorMessage = "No error";

    WORD xlArray_rows, xlArray_columns;
    cpp_xloper cppXloper(surfaceInput);
    cppXloper.GetArraySize(xlArray_rows, xlArray_columns);
    if ((xlArray_columns < 2) || (xlArray_rows < 2)) 
    {
        successful = false;
        errorMessage = "Input surface must contain at least 2 rows and 2 columns";
        columnHeadings = vector<double>(1, 0);
        rowHeadings = vector<double>(1, 0);
        data.clear();
        data.push_back(columnHeadings);
        return successful;
    }
    double rate;
    rowHeadings.clear();
    for (WORD i = 1; i < xlArray_rows; ++i)
    {
        cppXloper.GetArrayElement(i, 0, rate);
        rowHeadings.push_back(rate);
    }

    columnHeadings = vector<double>(xlArray_columns - 1, 0);
    rowHeadings = vector<double>(xlArray_rows - 1, 0);
    data.clear();
    for (size_t i = 1; i < xlArray_rows; ++i)
    {
        data.push_back(columnHeadings);
    }

    for (WORD i = 1; i < xlArray_rows; ++i)
    {
        if (!cppXloper.GetArrayElement(i, 0, rate)) 
        {
            successful = false;
            errorMessage = "At least one row heading is not a numeric value";
            return successful;
        }
        rowHeadings[i - 1] = (rate);
    }
    for (WORD i = 1; i < xlArray_columns; ++i)
    {
        if (!cppXloper.GetArrayElement(0, i, rate)) 
        {
            successful = false;
            errorMessage = "At least one column heading is not a numeric value";
            return successful;
        }
        columnHeadings[i - 1] = rate;
    }
    for (WORD i = 1; i < xlArray_rows; ++i)
    {
        for (WORD j = 1; j < xlArray_columns; ++j)
        {
            if (!cppXloper.GetArrayElement(i, j, rate)) 
            {
                successful = false;
                errorMessage = "At least one data point is not a numeric value";
                return successful;
            }
            data[i - 1][j - 1] = (rate);
        }
    }
    return successful;
}


