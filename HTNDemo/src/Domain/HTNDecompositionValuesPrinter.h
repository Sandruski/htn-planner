#pragma once

#include "Domain/Interpreter/HTNDecompositionContext.h"

/**
 * Prints the values of a decomposition
 */
class HTNDecompositionValuesPrinter
{
public:
    explicit HTNDecompositionValuesPrinter(const HTNNodeSnapshotDebug& inNodeSnapshotDebug);

    void Print() const;

private:
    const HTNNodeSnapshotDebug& mNodeSnapshotDebug;

    HTNDecompositionContext mDecompositionContext;
};

inline HTNDecompositionValuesPrinter::HTNDecompositionValuesPrinter(const HTNNodeSnapshotDebug& inNodeSnapshotDebug)
    : mNodeSnapshotDebug(inNodeSnapshotDebug)
{
}
