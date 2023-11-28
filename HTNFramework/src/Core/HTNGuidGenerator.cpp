// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Core/HTNGuidGenerator.h"

uint32 HTNGuidGenerator::GenerateGUID()
{
    static uint32 mGUID = 0;
    return ++mGUID;
}
