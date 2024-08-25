#pragma once

#define MPCHAT_EXPORT __attribute__((visibility("default")))

#ifdef __cplusplus
#define MPCHAT_EXPORT_FUNC extern "C" MPCHAT_EXPORT
#else
#define MPCHAT_EXPORT_FUNC MPCHAT_EXPORT
#endif