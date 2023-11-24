// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#define HTN_MAYBE_UNUSED [[maybe_unused]]
#define HTN_NODISCARD [[nodiscard]]

#ifdef HTN_DEBUG
#define HTN_DEBUG_ONLY(...) __VA_ARGS__
#else
#define HTN_DEBUG_ONLY(...)
#endif
