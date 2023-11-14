#pragma once

class HTNNodeVisitorContextBase
{
public:
    virtual ~HTNNodeVisitorContextBase() = default;

    int mBaseNumber = 1;
};

class HTNNodeVisitorContext final : public HTNNodeVisitorContextBase
{
public:
    int mDerivedNumber = 2;
    int mOtherDerivedNumber = 3;
};
