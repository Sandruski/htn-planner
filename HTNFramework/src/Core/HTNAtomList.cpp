// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com Jose Antonio Escribano joseantonioescribanoayllon@gmail.com

#include "Core/HTNAtomList.h"

#include "Core/HTNAtomNode.h"

HTNAtomList::HTNAtomList(const std::initializer_list<HTNAtom>& inElements)
{
    for (const HTNAtom& Element : inElements)
    {
        PushBack(Element);
    }
}

HTNAtomList::HTNAtomList(const HTNAtomList& inOther)
{
    for (const HTNAtomNode* CurrentNode = inOther.mHeadNode; CurrentNode; CurrentNode = CurrentNode->GetNextNode())
    {
        PushBack(CurrentNode->GetData());
    }
}

HTNAtomList::~HTNAtomList()
{
    for (const HTNAtomNode* CurrentNode = mHeadNode; CurrentNode;)
    {
        const HTNAtomNode* NextNode = CurrentNode->GetNextNode();
        delete CurrentNode;
        CurrentNode = NextNode;
    }
}

bool HTNAtomList::operator==(const HTNAtomList& inOther) const
{
    const HTNAtomNode* ThisCurrentNode  = mHeadNode;
    const HTNAtomNode* OtherCurrentNode = inOther.mHeadNode;
    while (ThisCurrentNode && OtherCurrentNode)
    {
        if (ThisCurrentNode->GetData() != OtherCurrentNode->GetData())
        {
            return false;
        }

        ThisCurrentNode  = ThisCurrentNode->GetNextNode();
        OtherCurrentNode = OtherCurrentNode->GetNextNode();
    }

    return !ThisCurrentNode && !OtherCurrentNode;
}

void HTNAtomList::PushBack(const HTNAtom& inValue)
{
    HTNAtomNode* Node = new HTNAtomNode(inValue);

    if (!mHeadNode)
    {
        mHeadNode = Node;
        return;
    }

    HTNAtomNode* CurrentNode = mHeadNode;
    while (CurrentNode->HasNextNode())
    {
        CurrentNode = CurrentNode->GetNextNodeMutable();
    }

    CurrentNode->SetNextNode(Node);
}

const HTNAtom& HTNAtomList::Get(const uint32 inIndex) const
{
    const HTNAtomNode* CurrentNode = mHeadNode;
    for (uint32 i = 0; i < inIndex; ++i)
    {
        CurrentNode = CurrentNode->GetNextNode();
    }

    return CurrentNode->GetData();
}

const HTNAtom* HTNAtomList::Find(const uint32 inIndex) const
{
    if (!mHeadNode)
    {
        return nullptr;
    }

    const HTNAtomNode* CurrentNode = mHeadNode;
    for (uint32 i = 0; i < inIndex; ++i)
    {
        if (!CurrentNode->HasNextNode())
        {
            return nullptr;
        }

        CurrentNode = CurrentNode->GetNextNode();
    }

    return &CurrentNode->GetData();
}

uint32 HTNAtomList::GetSize() const
{
    uint32 Size = 0;

    for (const HTNAtomNode* CurrentNode = mHeadNode; CurrentNode; CurrentNode = CurrentNode->GetNextNode())
    {
        ++Size;
    }

    return Size;
}

bool HTNAtomList::IsEmpty() const
{
    const uint32 Size = GetSize();
    return 0 == Size;
}

std::string HTNAtomList::ToString(const bool inShouldDoubleQuoteString) const
{
    std::string String = "(";

    for (const HTNAtomNode* CurrentNode = mHeadNode; CurrentNode; CurrentNode = CurrentNode->GetNextNode())
    {
        String.append(std::format("{} ", CurrentNode->GetData().ToString(inShouldDoubleQuoteString)));
    }

    // Remove last whitespace
    const size Index = String.find_last_of(" ");
    if (Index != std::string::npos)
    {
        String.erase(Index);
    }

    String.append(")");

    return String;
}
