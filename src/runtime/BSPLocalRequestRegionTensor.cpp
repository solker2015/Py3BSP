/*
 * BSPLocalRequestRegionTensor.cpp
 *
 *  Created on: 2014-8-21
 *      Author: junfeng
 */

#include "BSPLocalRequestRegionTensor.hpp"
#include <iostream>

using namespace BSP;

LocalRequestRegionTensor::LocalRequestRegionTensor(ArrayShape &shape,
        IndexSetRegionTensor &indexSet) :
LocalRequestRegion(shape) {
    if (_numberOfDimensions != indexSet._numberOfDimensions)
        throw EInvalidArgument();

    _dataCount = indexSet.getNumberOfIndices();
    _nRegions = indexSet._numberOfRegions;
    uint64_t indexLength = 1 + _numberOfDimensions;
    for (unsigned iDim = 0; iDim < _numberOfDimensions; iDim++) {
        _nComponentsAlongDim[iDim] = indexSet._numberOfComponentsAlongDim[iDim];
        indexLength += 3 * _nComponentsAlongDim[iDim];
    }
    allocateComponents();
    allocate(indexLength);

    // fill in the _indexList, _lowerIndexList, _upperIndexList, _stepList
    uint64_t dstOffset = _numberOfDimensions + 1;
    _indexList[0] = _dataCount * _numberOfBytesPerElement;
    for (unsigned iDim = 0; iDim < _numberOfDimensions; iDim++) {
        _indexList[iDim + 1] = _nComponentsAlongDim[iDim];
        memcpy(_lowerIndexAlongDim[iDim],
                indexSet._lowerComponentAlongDim[iDim],
                sizeof (uint64_t) * _nComponentsAlongDim[iDim]);
        memcpy(_upperIndexAlongDim[iDim],
                indexSet._upperComponentAlongDim[iDim],
                sizeof (uint64_t) * _nComponentsAlongDim[iDim]);
        memcpy(_stepAlongDim[iDim], indexSet._stepAlongDim[iDim],
                sizeof (int32_t) * _nComponentsAlongDim[iDim]);
        for (unsigned iComponent = 0; iComponent < _nComponentsAlongDim[iDim];
                iComponent++) {
            _indexList[dstOffset + (iComponent << 1) + iComponent] =
                    _lowerIndexAlongDim[iDim][iComponent];
            _indexList[dstOffset + (iComponent << 1) + iComponent + 1] =
                    (_upperIndexAlongDim[iDim][iComponent]
                    - _lowerIndexAlongDim[iDim][iComponent]) / _stepAlongDim[iDim][iComponent] + 1;
            _indexList[dstOffset + (iComponent << 1) + iComponent + 2] = 
                (uint64_t)_stepAlongDim[iDim][iComponent];
        }
        dstOffset += 3 * _nComponentsAlongDim[iDim];
    }
}

LocalRequestRegionTensor::~LocalRequestRegionTensor() {
}


