#pragma once

#define MAYBE_UNUSED [[maybe_unused]]

#ifdef HTN_DEBUG
#define HTN_DEBUG_ONLY(...) __VA_ARGS__
#else
#define HTN_DEBUG_ONLY(...)
#endif
