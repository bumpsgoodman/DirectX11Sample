#pragma once

#define SAFE_DELETE(p)       { delete (p); (p) = nullptr; }
#define SAFE_DELETE_ARRAY(p) { delete[] (p); (p) = nullptr; }
#define SAFE_RELEASE(p)      { if ((p)) { (p)->Release(); (p) = nullptr; } }