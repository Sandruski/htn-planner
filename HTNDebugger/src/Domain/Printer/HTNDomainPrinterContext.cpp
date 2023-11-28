// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com Jose Antonio Escribano joseantonioescribanoayllon@gmail.com

#include "Domain/Printer/HTNDomainPrinterContext.h"

#ifdef HTN_DEBUG_DECOMPOSITION
HTNDomainPrinterContext::HTNDomainPrinterContext(const std::shared_ptr<const HTNDomainNode>& inDomainNode) : mDomainNode(inDomainNode)
{
}
#endif
