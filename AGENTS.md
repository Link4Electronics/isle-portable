# Big-Endian (ppc64be) Support

## Goal
Make isle-portable run on PowerPC64 Big-Endian without breaking x86\_64/arm64.

## Data Format
All game files (.si, .wdb, .dat, .bmp, .flc) are little-endian. The game reads them into `MxU8` buffers and interprets multi-byte fields via pointer casts or `memcpy`. Every such read must go through a byte-swap helper when targeting BE.

## Swap Primitives (LEGO1/omni/include/mxendian.h)
- `EndianReadLE16/32/64` — memcpy + `SDL_Swap*LE` → returns host-order value
- `EndianWriteLE16/32` — `SDL_Swap*LE` + memcpy → writes LE bytes
- `UnalignedRead<T>(ptr)` in `mxutilities.h` — has specializations for `MxU16/S16/U32/S32/F64/F32` that call `EndianReadLE*`
- `ReadLE<T>(LegoStorage*)` in `legostorage.h` — reads `sizeof(T)` bytes via `LegoStorage::Read` + `SDL_Swap*LE`

## Files Modified

### omni (core engine)
| File | Change |
|------|--------|
| `mxendian.h` | Swap helpers using `SDL_Swap16LE/32LE/64LE` |
| `mxutilities.h` | `UnalignedRead<>` specializations; removed duplicate `MxLong` |
| `mxio.cpp` | Swap `ckid`, `cksize`, `fccType` in MMIO Descend/Ascend |
| `mxstreamchunk.cpp` | `ReadChunkHeader` uses `EndianReadLE*` |
| `mxdsbuffer.cpp` | `Append`/`CalcBytesRemaining` swapped sizes; `*IntoFlags` write via `EndianWriteLE16` |
| `mxdsfile.cpp` | `m_header`, `m_lengthInDWords` swapped |
| `mxdiskstreamprovider.cpp` | `IntoType`/`IntoTime` via `EndianReadLE*` |
| `mxbitmap.cpp` | `BITMAPFILEHEADER`/`BITMAPINFOHEADER` fields swapped |
| `flic.cpp`, `flic.h` | `EndianSwapFLICChunk/Header/Frame` helpers; used in decode |
| `mxflcpresenter.cpp` | FLIC header/frame/rect `MxRect32` swapped |
| `mxramstreamprovider.cpp` | `ReadData()` — all raw `*IntoFlags/ObjectId/Time` replaced with `EndianReadLE*`/`EndianWriteLE*` |

### lego (game code)
| File | Change |
|------|--------|
| `legostorage.h` | `ReadLE<T>` template; fixed `ReadS16/U16/S32/U32/Float` + writes |
| `legoimage.cpp` | `width`, `height`, `count` swapped on read; LE write |
| `legotexturepresenter.cpp` | `numTextures` `ReadLE` |
| `legopartpresenter.cpp` | offsets, counts, texture names `ReadLE` |
| `legomodelpresenter.cpp` | version, offsets, counts `ReadLE` |
| `legoanimpresenter.cpp` | magic, bounding radius, center `ReadLE` |
| `legoroi.cpp` | LOD counts, offsets `ReadLE` |
| `legolod.cpp` | flags, mesh/vertex/texcoord counts; float/U32 array swaps |
| `legoanim.cpp` | translation/rotation/scale/time keys; scene/node data |
| `legomesh.cpp` | alpha, texture/material lengths |
| `legosphere.cpp` | radius `ReadLE` |
| `legovertex.cpp` | coordinates `ReadLE` |
| `legotree.cpp` | `numChildren` `ReadLE` |
| `legoanimationmanager.cpp` | objectId, location, direction, up `ReadLE` |
| `legobuildingmanager.cpp` | sound, move, nextVariant `ReadLE` |
| `legoplantmanager.cpp` | sound, move `ReadLE` |
| `legocharactermanager.cpp` | sound, move `ReadLE` |
| `legopathcontroller.cpp` | numT/N/E/L, struct flags, edge flags/lengths, boundary triggers, float vectors |

### modeldb
| File | Change |
|------|--------|
| `modeldb.cpp` | All `MxU32`/`MxS32` after `SDL_ReadIO` |

### CMake
| Change | Description |
|--------|-------------|
| SDL3/iniparser/mbedtls/libwebsockets | `find_package` system first, download only as fallback |
| `SDL3-shared` | Use correct `_sdl3_fetched` variable for install guard |

## Remaining Known Issues

1. **Other Into* callers**: Any code that dereferences `IntoFlags()`, `IntoObjectId()`, `IntoTime()`, or `IntoLength()` on a raw LE buffer without byte-swapping will silently produce wrong values on BE. Check for new/changed callers.
2. **Direct struct overlays**: Any code that casts a `MxU8*` to a packed struct to read multi-byte fields needs per-field swapping (like `BITMAPFILEHEADER` and FLIC).
3. **Write paths**: Any code that *writes* multi-byte values into the game data buffer (e.g., save files) must emit LE bytes.

## Non-Issues
- `SDL_Swap16LE` etc. are no-ops on LE — zero runtime cost on x86/arm.
- All `FOURCC()` comparisons use `UnalignedRead<MxU32>` which swaps.
- `UnalignedRead` uses `memcpy` for safe unaligned access on all architectures.
