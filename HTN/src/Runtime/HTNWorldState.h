#pragma once

#include "HTNAtom.h"

#include <array>
#include <cassert>
#include <string>
#include <unordered_map>
#include <vector>

class HTNTableBase
{
public:
    virtual ~HTNTableBase() = 0;

    virtual void                              AddEntry(const std::vector<HTNAtom>& inArgs)                        = 0;
    virtual void                              RemoveEntry(const int inIndex)                                      = 0;
    virtual bool                              Check(const int inIndex, const std::vector<HTNAtom*>& inArgs) const = 0;
    virtual unsigned int                      GetNumEntries() const                                               = 0;
    virtual std::vector<std::vector<HTNAtom>> GetEntries() const                                                  = 0;
};

inline HTNTableBase::~HTNTableBase()
{
}

// Contains the array with a list of arguments that corresponds to certain HtnWorldState key.
template<int NumArgs>
class HTNTable final : public HTNTableBase
{
public:
    using HTNEntry = std::array<HTNAtom, NumArgs>;

    // Adds an entry with arguments
    template<typename... Args>
    void AddEntry(const Args&... inArgs);
    void AddEntry(const std::vector<HTNAtom>& inArgs) final;

    void RemoveEntry(const int inIndex) final;

    // Binds the arguments of the entry at the specified index to the unbound arguments passed
    template<typename... Args>
    int Query(const int inIndex, Args&... inArgs) const;

    template<typename... Args>
    bool Check(const int inIndex, Args&... inArgs) const;
    bool Check(const int inIndex, const std::vector<HTNAtom*>& inArgs) const final;

    // Returns the number of entries
    unsigned int GetNumEntries() const final;

    std::vector<std::vector<HTNAtom>> GetEntries() const final;

private:
    // Helper method for fold expression
    template<typename T>
    static void BindArgsUnbound(const HTNEntry& inEntry, T& inArg, int& ioIndex);

    // Helper method for fold expression
    template<typename T>
    static bool CheckArgsBound(const HTNEntry& inEntry, const T& inArg, int& ioIndex);

    std::vector<HTNEntry> mEntries;
};

template<int NumArgs>
template<typename... Args>
inline void HTNTable<NumArgs>::AddEntry(const Args&... inArgs)
{
    static constexpr std::size_t ArgsSize = sizeof...(Args);
    static_assert(ArgsSize == NumArgs);

    // TODO salvarez Fix
    // Check args bound
    for (const HTNEntry& Entry : mEntries)
    {
        int Index = 0;
        if ((CheckArgsBound(Entry, inArgs, Index) && ...))
        {
            return;
        }
    }

    HTNEntry& Entry = mEntries.emplace_back();
    Entry           = {inArgs...};
}

template<int NumArgs>
inline void HTNTable<NumArgs>::AddEntry(const std::vector<HTNAtom>& inArgs)
{
    assert(inArgs.size() == NumArgs);

    // TODO salvarez Put this in a method
    // Check args bound
    for (const HTNEntry& Entry : mEntries)
    {
        bool ContainsEntry = true;
        for (size_t i = 0; i < Entry.size(); ++i)
        {
            const HTNAtom& Arg = inArgs[i];
            if (Arg.IsSet())
            {
                const HTNAtom& EntryArg = Entry[i];
                if (Arg != EntryArg)
                {
                    ContainsEntry = false;
                    break;
                }
            }
        }

        if (ContainsEntry)
        {
            return;
        }
    }

    HTNEntry& Entry = mEntries.emplace_back();
    for (size_t i = 0; i < NumArgs; ++i)
    {
        Entry[i] = inArgs[i];
    }
}

template<int NumArgs>
inline void HTNTable<NumArgs>::RemoveEntry(const int inIndex)
{
    if (inIndex < 0 || inIndex >= mEntries.size())
    {
        return;
    }

    mEntries.erase(mEntries.begin() + inIndex);
}

template<int NumArgs>
template<typename... Args>
inline int HTNTable<NumArgs>::Query(const int inIndex, Args&... inArgs) const
{
    static constexpr std::size_t ArgsSize = sizeof...(Args);
    static_assert(ArgsSize == NumArgs);

    if (inIndex < 0 || inIndex >= mEntries.size())
    {
        return 0;
    }

    const HTNEntry& Entry = mEntries[inIndex];
    int             Index = 0;
    (BindArgsUnbound(Entry, inArgs, Index), ...);

    return 1;
}

template<int NumArgs>
template<typename... Args>
inline bool HTNTable<NumArgs>::Check(const int inIndex, Args&... inArgs) const
{
    static constexpr std::size_t ArgsSize = sizeof...(Args);
    static_assert(ArgsSize == NumArgs);

    if (inIndex < 0 || inIndex >= mEntries.size())
    {
        return false;
    }

    const HTNEntry& Entry  = mEntries[inIndex];
    int             Index  = 0;
    const bool      Result = (CheckArgsBound(Entry, inArgs, Index) && ...);
    if (!Result)
    {
        return false;
    }

    Index = 0;
    (BindArgsUnbound(Entry, inArgs, Index), ...);

    return true;
}

template<int NumArgs>
inline bool HTNTable<NumArgs>::Check(const int inIndex, const std::vector<HTNAtom*>& inArgs) const
{
    assert(NumArgs == inArgs.size());

    if (inIndex < 0 || inIndex >= mEntries.size())
    {
        return false;
    }

    // TODO salvarez Put this in a method
    // Check args bound
    const HTNEntry& Entry = mEntries[inIndex];
    for (size_t i = 0; i < Entry.size(); ++i)
    {
        HTNAtom* Arg = inArgs[i];
        if (Arg->IsSet())
        {
            const HTNAtom& EntryArg = Entry[i];
            if (*Arg != EntryArg)
            {
                return false;
            }
        }
    }

    // TODO salvarez Put this in a method
    // Bind args unbound
    for (size_t i = 0; i < Entry.size(); ++i)
    {
        HTNAtom*       Arg      = inArgs[i];
        const HTNAtom& EntryArg = Entry[i];
        if (!Arg->IsSet())
        {
            *Arg = EntryArg;
        }
    }

    return true;
}

template<int NumArgs>
inline unsigned int HTNTable<NumArgs>::GetNumEntries() const
{
    return static_cast<unsigned int>(mEntries.size());
}

template<int NumArgs>
inline std::vector<std::vector<HTNAtom>> HTNTable<NumArgs>::GetEntries() const
{
    std::vector<std::vector<HTNAtom>> Entries;
    for (const HTNEntry& Entry : mEntries)
    {
        std::vector<HTNAtom> Arguments;
        for (const HTNAtom& Argument : Entry)
        {
            Arguments.emplace_back(Argument);
        }

        Entries.emplace_back(Arguments);
    }

    return Entries;
}

template<int NumArgs>
template<typename T>
inline void HTNTable<NumArgs>::BindArgsUnbound(const HTNEntry& inEntry, T& inArg, int& ioIndex)
{
    if (!inArg.IsSet())
    {
        inArg = inEntry[ioIndex];
    }

    ++ioIndex;
}

template<int NumArgs>
template<typename T>
inline bool HTNTable<NumArgs>::CheckArgsBound(const HTNEntry& inEntry, const T& inArg, int& ioIndex)
{
    bool Result = true;

    if (inArg.IsSet())
    {
        Result = (inEntry[ioIndex] == inArg);
    }

    ++ioIndex;

    return Result;
}

static constexpr int MaxNumArgs = 8;

// World state for the HTN planner.
// Data structure includes a map of <sKey, Array<HTNTable>>. Each key can contain multiple tables, each table has a specific number of arguments.
// There can't be two tables with the same number of arguments for the same key.
class HTNWorldState
{
public:
    using HTNFact = std::array<HTNTableBase*, MaxNumArgs + 1>;

    // Writes a fact with n arguments
    template<typename... Args>
    void AddFact(const char* inKey, const Args&... inArgs);
    void AddFact(const char* inKey, const std::vector<HTNAtom>& inArgs);

    void RemoveFact(const char* inKey, const int inArgsSize, const int inIndex);
    void RemoveAllFacts();

    // It checks if there is an existing entry for the inKey + arguments.
    // If all the arguments are binded then the result is like a binary operation, that query results in true or false.
    // If not all the arguments are binded then the result might return more than 1.
    // Return the number of possible results in the database.
    template<typename... Args>
    int Query(const char* inKey, Args&... inArgs) const;

    // It checks if there is an existing entry for the inKey + arguments.
    // If all the arguments are binded then the result is like a binary operation, that query results in true or false.
    // If not all the arguments are binded then we need to bind it according to the row that correspond to inIndex, the result will always be 0 or 1.
    // Return either 0 or 1 because we are pointing to a specific row within a table.
    template<typename... Args>
    int QueryIndex(const char* inKey, const int inIndex, Args&... inArgs) const;

    template<typename... Args>
    bool CheckIndex(const char* inKey, const int inIndex, Args&... inArgs) const;
    bool CheckIndex(const char* inKey, const int inIndex, const std::vector<HTNAtom*>& inArgs) const;

    // Returns the number of HTNTables registered for the fact inKey.
    int GetNumFactTables(const char* inKey) const;

    // Returns the number of HTNTables registered for the fact inKey and the inNumArgs. This function should only return 0 or 1.
    // If it does returns something different then this will cause problems later on during decomposition.
    int GetNumFactTablesByNumArgs(const char* inKey, const int inNumArgs) const;

    unsigned int GetNumFactEntries(const std::string& inKey, const unsigned int inNumArgs) const;

    const std::unordered_map<std::string, HTNFact>& GetFacts() const;

private:
    // Helper method for fold expression
    template<typename T>
    static void CountArgsBound(const T& inArg, int& outNumArgsBound);

    std::unordered_map<std::string, HTNFact> mFacts;
};

template<typename... Args>
inline void HTNWorldState::AddFact(const char* inKey, const Args&... inArgs)
{
    static constexpr std::size_t ArgsSize = sizeof...(Args);
    static_assert(ArgsSize <= MaxNumArgs);

    HTNFact&            Fact  = mFacts[inKey];
    HTNTable<ArgsSize>* Table = static_cast<HTNTable<ArgsSize>*>(Fact[ArgsSize]);
    if (!Table)
    {
        Fact[ArgsSize] = Table = new HTNTable<ArgsSize>();
    }

    Table->AddEntry(inArgs...);
}

template<typename... Args>
inline int HTNWorldState::Query(const char* inKey, Args&... inArgs) const
{
    static constexpr std::size_t ArgsSize = sizeof...(Args);
    static_assert(ArgsSize <= MaxNumArgs);

    int NumArgsBound = 0;
    (CountArgsBound(inArgs, NumArgsBound), ...);
    if (ArgsSize == NumArgsBound)
    {
        // All arguments are bound
        return 1;
    }

    const auto FactIt = mFacts.find(inKey);
    if (FactIt == mFacts.end())
    {
        // Fact not found
        return 0;
    }

    const HTNFact&            Fact  = FactIt->second;
    const HTNTable<ArgsSize>* Table = static_cast<HTNTable<ArgsSize>*>(Fact[ArgsSize]);
    if (!Table)
    {
        // Table not found
        return 0;
    }

    return Table->GetNumEntries();
}

template<typename... Args>
inline int HTNWorldState::QueryIndex(const char* inKey, const int inIndex, Args&... inArgs) const
{
    static constexpr std::size_t ArgsSize = sizeof...(Args);
    static_assert(ArgsSize <= MaxNumArgs);

    int NumArgsBound = 0;
    (CountArgsBound(inArgs, NumArgsBound), ...);
    if (ArgsSize == NumArgsBound)
    {
        // All arguments are bound
        return 1;
    }

    const auto FactIt = mFacts.find(inKey);
    if (FactIt == mFacts.end())
    {
        // Fact not found
        return 0;
    }

    const HTNFact&            Fact  = FactIt->second;
    const HTNTable<ArgsSize>* Table = static_cast<HTNTable<ArgsSize>*>(Fact[ArgsSize]);
    if (!Table)
    {
        // Table not found
        return 0;
    }

    return Table->Query(inIndex, inArgs...);
}

template<typename... Args>
inline bool HTNWorldState::CheckIndex(const char* inKey, const int inIndex, Args&... inArgs) const
{
    static constexpr std::size_t ArgsSize = sizeof...(Args);
    static_assert(ArgsSize <= MaxNumArgs);

    const auto FactIt = mFacts.find(inKey);
    if (FactIt == mFacts.end())
    {
        // Fact not found
        return false;
    }

    const HTNFact&            Fact  = FactIt->second;
    const HTNTable<ArgsSize>* Table = static_cast<HTNTable<ArgsSize>*>(Fact[ArgsSize]);
    if (!Table)
    {
        // Table not found
        return false;
    }

    return Table->Check(inIndex, inArgs...);
}

template<typename T>
inline void HTNWorldState::CountArgsBound(const T& inArg, int& outNumArgsBound)
{
    if (inArg.IsSet())
    {
        ++outNumArgsBound;
    }
}

inline const std::unordered_map<std::string, HTNWorldState::HTNFact>& HTNWorldState::GetFacts() const
{
    return mFacts;
}
